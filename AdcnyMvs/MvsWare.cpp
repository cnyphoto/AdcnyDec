#include "pch.h"
#include "MvsWare.h"


void MvsWare::specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f)
{
    mvs = std::make_unique<MvsStore>(spar.scoilid - 1);
    //mvs->drivesNum = spar.scoilid-1;
	mvs->camId = spar.CamId;
    mvs->run(f);
}

void MvsWare::stop()
{
    mvs->stopCam();
}

