#pragma once

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

// Include file to use pylon universal instant camera parameters.
#include <pylon/BaslerUniversalInstantCamera.h>


#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <functional>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>


// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using pylon universal instant camera parameters.
using namespace Basler_UniversalCameraParams;

// Namespace for using cout.
using namespace std;


class BslStore
{
public:
    BslStore();
    ~BslStore();

	//std::function<void(cv::Mat, int)> boundFunction;

	void run(std::function<void(cv::Mat, int)> f);
    
	void stopCam();

	int drivesNum;

private:
	bool stoped;
};



