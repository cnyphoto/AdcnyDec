#pragma once

#ifdef ADCNYMVS_EXPORTS
#define ADCNYMVS_API __declspec(dllexport)
#else
#define ADCNYMVS_API __declspec(dllimport)
#endif

#include <memory>
#include <CameraApt.h>
#include "MvsStore.h"

class __declspec(dllexport) MvsWare :public CameraApt
{
public:
	void specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f);
	void stop();
	bool exied;
private:
	std::unique_ptr<MvsStore> mvs;
};

