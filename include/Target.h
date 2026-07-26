#pragma once
#include <functional>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>

#ifdef ADCNYBSE_EXPORTS
#define ADCNYBSE_API __declspec(dllexport)
#else
#define ADCNYBSE_API __declspec(dllimport)
#endif

typedef struct modpar {
    int id;
    int name;
    std::string modpath;
    int slot;
    float conf;
    bool trained;
	int modh;
	int modw;
}modpar;

typedef struct SorcePar {
    int id;
    std::string imgRoot;
    int scoilid;
    int filepolling;
    std::string fileType;
    std::string savePath;
    int ascNum;
    std::vector<modpar> mods;
    bool openAied;
    int imgW;
    int imgH;
    int rl;
    int du;
    int Acquisition;
    int colorNum;
    std::string webSockectIpPort;
    bool closed;
    bool dataEned;
    bool tested;
    bool trained;
    std::string CamId;
}SorcePar;

class __declspec(dllexport) Target
{
public:
	virtual void requestPhoto(const SorcePar& spar, std::function<void(cv::Mat, int)> f) = 0;
	virtual ~Target() = default;
};
