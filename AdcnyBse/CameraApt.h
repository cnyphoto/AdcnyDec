#pragma once
#include <functional>
#include <Target.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>

#ifdef ADCNYBSE_EXPORTS
#define ADCNYBSE_API __declspec(dllexport)
#else
#define ADCNYBSE_API __declspec(dllimport)
#endif

class __declspec(dllexport) CameraApt
{
public:
	virtual void specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f) = 0;
	virtual void stop() = 0;
	virtual ~CameraApt() = default;
	bool exied;
};

