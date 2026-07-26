#include "pch.h"
#include "InferStore.h"
#include <Mtx.h>


InferStore::InferStore()
{
	this->dech = 1500;
	this->decw = 2048;
	this->trained = false;
	start = false;
}

InferStore::~InferStore()
{
	
}


void InferStore::infer(cv::Mat img, std::vector<std::vector<int>> bboxs, long long coild, int modetype, int imgNum, int saveType)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	this->procDt_queue.push(procDt{img,bboxs,coild,modetype,saveType,imgNum});
	this->cv.notify_one();
}


void InferStore::inferForeverToStr(std::function<void(std::vector<std::string>)> f)
{
    while (true)
    {
        procDt pd;
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            this->cv.wait(lock, [this] {
                return !this->procDt_queue.empty() || !this->start;
            });
            if (!this->start) break;
            pd = this->procDt_queue.front();
            this->procDt_queue.pop();
        }

        int gridw = pd.img.cols / this->decw;
        int gridh = pd.img.rows / this->dech;

        std::vector<std::string> strarr;

        std::map<int, std::vector<std::vector<int>>> grouped = { {0,{}} };
        
        if (pd.saveType < 2)
        {
            for (size_t i = 2; i < pd.bboxs.size(); i++)
            {
                auto elem = pd.bboxs.at(i);
                int ww = (elem.at(1) + elem.at(3) / 2) / this->decw;
                int hh = (elem.at(2) + elem.at(4) / 2) / this->dech;
                int gridnum = (hh) * pd.img.cols / this->decw + ww;

                elem.push_back(gridnum);
                grouped[elem[8]].push_back(elem);
            }
        }

        std::vector<Detection> decall(0);
        std::vector<Detection> decolds(0);

        for (auto& elem : grouped) {

            for (auto& it : elem.second)
            {
                Detection old;
                old.class_id = it[7];
                old.conf = 0.1f;
                old.bbox[0] = static_cast<float>(it[1]);
                old.bbox[1] = static_cast<float>(it[2]);
                old.bbox[2] = static_cast<float>(it[3]);
                old.bbox[3] = static_cast<float>(it[4]);
                decolds.push_back(old);
            }

            cv::Rect roi((elem.first % gridw) * this->decw, (elem.first / gridw) * this->dech, this->decw, this->dech);

            cv::Mat imm = pd.img(roi);

            if (imm.channels() == 1) {
                cv::cvtColor(imm, imm, cv::COLOR_GRAY2RGB);
            }

            for (auto& yolotrt : this->yolotrts)
            {
                yolotrt->Infer(imm.cols, imm.rows, imm.channels(), imm.data);

                for (auto& res : yolotrt->boxes)
                {
                    cv::Rect r = yolotrt->scale_boxes(imm, res.bbox);
                    res.bbox[0] = r.x + (elem.first % gridw) * this->decw;
                    res.bbox[1] = r.y + (elem.first / gridw) * this->decw;
                    res.bbox[2] = static_cast<float>(r.width);
                    res.bbox[3] = static_cast<float>(r.height);
                    res.class_id = yolotrt->getBigClsId(res.class_id);
                    decall.push_back(res);
                }
            }
        }

        std::vector<Detection> okboxes(0);

        if (this->trained && decolds.size() > 0)
        {
            nmsforiou(okboxes, decall, decolds, 0.01f);
        }
        else {
            okboxes = decall;
        }

        for (auto& ele : okboxes)
        {
            char buf[128] = { 0 };
            sprintf_s(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%lld,%d",
                0,
                pd.bboxs[2][0],
                static_cast<int>(ele.bbox[0]),
                static_cast<int>(ele.bbox[1]),
                static_cast<int>(ele.bbox[2]),
                static_cast<int>(ele.bbox[3]),
                pd.bboxs[2][5],
                pd.bboxs[2][6],
                ele.class_id,
                pd.coild,
                pd.modetype);
            strarr.push_back(buf);
        }

        if (strarr.size() > 0)
        {
            f(strarr);
            if (pd.saveType > 1)
            {
                this->imgop->saveImg(pd.imgNum, pd.img, pd.coild, pd.modetype, 2);
            }
        }
        else {
            if (pd.saveType == 3)
            {
                this->imgop->saveImg(pd.imgNum, pd.img, pd.coild, pd.modetype, 3);
            }
        }
    }
}

void InferStore::setStart(bool b)
{
	{
		std::lock_guard<std::mutex> lock(this->mtx);
		this->start = b;
	}
	this->cv.notify_all();
}

void InferStore::setMods(std::vector<modpar> mods, std::string folder)
{
	this->decw = mods[0].modw;
	this->dech = mods[0].modh;

	if (!folder.empty())
	{
		this->imgop = std::make_unique<imgOp>(folder);
	}

	this->trained = mods.at(0).trained;

	for (size_t i = 0; i < mods.size(); i++)
	{
		YoloTrt* yo = new YoloTrt();
		yo->init(mods[i].modpath,mods[i].slot, mods[i].conf, mods[i].name);
		this->yolotrts.push_back(yo);
	}
}


