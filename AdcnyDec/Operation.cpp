#include "Operation.h"
#include "Factory.h"
#include <memory>
#include <DecAdapter.h>
#include <ImgCal.h>

Operation::Operation(DbToCode &dt, const SorcePar &spar)
{
	this->edgeInt = 0.0f;
	this->spar = spar;
	coild = 0;
	this->startNum = 0;
	if (spar.tested)
		cameraApt = createCamera("folder");
	target = std::make_unique<DecAdapter>(cameraApt.get());
	this->dtcode = &dt;
	pool = std::make_unique<AccLibary>(this->spar.ascNum * 3);
	wsoc = std::make_unique<Wsoc>(dt, 0, 0, *cameraApt, 0);
}

Operation::~Operation() {}

void Operation::star(bool c, int a, char *v[])
{
	if (a > 4)
		this->startNum = atoi(v[4]);
	coild = dtcode->combCoilData(c, a, v);
	wsoc->setCoil(coild);
	int re = 0;
	pool->enqueue([this, re]() -> int
				  {this->runinfer();return 0; });
	pool->enqueue([this, re]() -> int
				  {this->runCom(re);return 0; });
	for (size_t i = 0; i < this->spar.ascNum; i++)
	{
		auto img = createDetector("edgesin");
		img->imgW = spar.imgW;
		img->imgH = spar.imgH;
		img->colorNum = spar.colorNum;
		img->scoilid = spar.scoilid;
		img->du = spar.du;
		img->rl = spar.rl;
		img->coilnum = coild;
		img->imgop = std::make_unique<imgOp>(spar.savePath);
		ccklist.emplace_back(std::move(img));
	}
	target->requestPhoto(spar, [this](Mat d, int b)
						 { this->disOutImg(b, d); });
}

void Operation::runDetection(int n) const
{
	auto s = std::chrono::high_resolution_clock::now();
	auto &ic = ccklist.at(n % this->spar.ascNum);
	wsoc->setImgNum(n);
	if (spar.closed)
		return;
	if (spar.dataEned)
		ic->img = ic->imgop->enhancedPhotoForMultiply(ic->img);
	if (spar.Acquisition < 0)
	{
		ic->imgop->saveImg(n, ic->img, coild, spar.scoilid, 0);
		return;
	}
	auto decs = ic->getBboxs(n);
	if (decs.size() > 1)
		wsoc->setBoundary(decs.at(1).at(0));
	if (spar.Acquisition < 2 && (decs.size() > 2 || n == 0))
	{
		if (spar.openAied)
		{
			cv::Rect r(0, 0, decs[1][0] - 30, ic->img.rows);
			cv::Mat m;
			ic->img.copyTo(m);
			m(r).setTo(cv::Scalar(0));
			dtcode->toInfer(m, decs, coild, spar.scoilid, n, 0);
		}
		else
			dtcode->combDecData(decs, coild, spar.scoilid);
		ic->imgop->saveImg(n, ic->img, coild, spar.scoilid, 0);
	}
	else if (spar.Acquisition > 1 && spar.openAied)
	{
		cv::Mat m;
		ic->img.copyTo(m);
		dtcode->toInfer(m, decs, coild, spar.scoilid, n, spar.Acquisition);
	}
	if (spar.Acquisition < 1 && decs.size() < 3)
		ic->imgop->saveImg(n, ic->img, coild, spar.scoilid, 0);
	auto ms = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - s)).count();
	std::cout << decs.size() << " ===" << ms << "mm;#" << n << std::endl;
}
void Operation::runCom(int n) const { wsoc->runSoc(std::to_string(spar.scoilid), spar.webSockectIpPort); }
void Operation::runinfer() const
{
	if (spar.openAied)
		dtcode->startInfer(spar.mods, spar.Acquisition > 1 ? spar.savePath : "");
}
void Operation::disOutImg(int r, const Mat d) const
{
	pool->enqueue([this, r, d]() -> int
				  {
		ccklist[(r+startNum)%spar.ascNum]->img = d;
		runDetection(r+startNum); return 0; });
}