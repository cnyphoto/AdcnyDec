#pragma once
#ifdef ADCNYVDO_EXPORTS
#define ADCNYVDO_API __declspec(dllexport)
#else
#define ADCNYVDO_API __declspec(dllimport)
#endif

#include <CameraApt.h>


class __declspec(dllexport) VdoWare :public CameraApt
{
public:
	void specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f);
	void stop();
	bool exied;
};

