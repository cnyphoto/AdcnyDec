#pragma once

#ifdef ADCNYFOO_EXPORTS
#define ADCNYFOO_API __declspec(dllexport)
#else
#define ADCNYFOO_API __declspec(dllimport)
#endif

#include <CameraApt.h>

class __declspec(dllexport) FolderWare :public CameraApt
{
public:
	void specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f);
	void stop();
	bool exied;
};

