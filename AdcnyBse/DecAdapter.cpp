#include "pch.h"
#include "DecAdapter.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>


DecAdapter::DecAdapter(CameraApt* cameraApt):cameraApt(cameraApt)
{
	
}

void DecAdapter::requestPhoto(const SorcePar& spar, std::function<void(cv::Mat data, int)> f)
{
	this->cameraApt->specificRequest(spar,f);
}

void DecAdapter::stopPhoto()
{
	this->cameraApt->stop();
}
