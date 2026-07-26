#pragma once
#include <CameraApt.h>
#include <Target.h>
#include <functional>

#ifdef ADCNYBSE_EXPORTS
#define ADCNYBSE_API __declspec(dllexport)
#else
#define ADCNYBSE_API __declspec(dllimport)
#endif

class __declspec(dllexport) DecAdapter : public Target
{
	
public:
	DecAdapter(CameraApt* cameraApt);
	void requestPhoto(const SorcePar& spar, std::function<void(cv::Mat, int)> f);
	void stopPhoto();
	CameraApt* cameraApt;
};

