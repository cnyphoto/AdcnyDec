#pragma once

#ifdef ADCNYBSL_EXPORTS
#define ADCNYBSL_API __declspec(dllexport)
#else
#define ADCNYBSL_API __declspec(dllimport)
#endif

#include <CameraApt.h>
#include "Bslstore.h"


class __declspec(dllexport) BslWare :public CameraApt
{
public:
	void specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f);
	void stop();
	bool exied;
private:
	BslStore* Bsl;
};
