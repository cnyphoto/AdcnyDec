#include "pch.h"
#include "BslWare.h"


void BslWare::specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f)
{
    this->Bsl = std::make_unique<BslStore>();
    Bsl->drivesNum = spar.scoilid - 1;
    Bsl->run(f);
}

void BslWare::stop()
{
    this->Bsl->stopCam();
}