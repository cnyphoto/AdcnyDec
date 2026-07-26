#pragma once
#include <vector>
#include <Target.h>
#include <imgOp.h>
#include <Yolotrt.h>
#include <condition_variable>

#ifdef ADCNYYOO_EXPORTS
#define ADCNYYOO_API __declspec(dllexport)
#else
#define ADCNYYOO_API __declspec(dllimport)
#endif

class __declspec(dllexport) InferStore
{

public:
    struct  procDt {
        cv::Mat img;
        std::vector<std::vector<int>> bboxs;
        long long coild;
        int modetype;
        int saveType;
        int imgNum;
    };

    InferStore();
    ~InferStore();
    void infer(cv::Mat, std::vector<std::vector<int>> bboxs, long long coild, int modetype,int imgNum,int saveType);
    void inferForeverToStr(const char* sqlbse, std::function<void(std::vector<std::string>)> f);
    void setStart(bool b);
    void setMods(std::vector<modpar> mods, std::string folder);

    void nmsforiou(std::vector<Detection>& res, std::vector<Detection> dets, std::vector<Detection> olddecs, float nms_thresh = 0.5) 
    {
        std::sort(dets.begin(), dets.end(), CompareContours());
        for (size_t m = 0; m < dets.size(); ++m) {
            auto& item = dets[m];
            res.push_back(item);
            for (size_t n = 0; n < olddecs.size(); ++n) {
                cv::Rect rect1(static_cast<int>(item.bbox[0]), static_cast<int>(item.bbox[1]), static_cast<int>(item.bbox[2]), static_cast<int>(item.bbox[3]));
                cv::Rect rect2(static_cast<int>(olddecs[n].bbox[0]), static_cast<int>(olddecs[n].bbox[1]), static_cast<int>(olddecs[n].bbox[2]), static_cast<int>(olddecs[n].bbox[3]));
                cv::Rect intersection = rect1 & rect2;
                if (!intersection.empty()) {
                    olddecs.erase(olddecs.begin() + n);
                    --n;
                }
            }
        }
        for (auto& item : olddecs)
        {
            res.push_back(item);
        }
    }

private:
    std::string  sqlip;
    std::queue<procDt> procDt_queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool start;
    std::vector<YoloTrt*> yolotrts;
    int decw;
    int dech;
    bool trained;
    std::unique_ptr<imgOp> imgop;
};

