#pragma once

#ifdef ADCNYFVO_EXPORTS
#define ADCNYFVO_API __declspec(dllexport)
#else
#define ADCNYFVO_API __declspec(dllimport)
#endif

#include <CameraApt.h>


class __declspec(dllexport) VideoReadWare :public CameraApt
{
public:
	void specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f);
	void stop();
	bool exied;
};

