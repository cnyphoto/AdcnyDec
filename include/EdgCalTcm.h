#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>

class EdgCalTcm
{
public:
	EdgCalTcm(int imW, int calSize, int amplif);
	int edgeTodoe(cv::Mat imgz, int th, int rl);
private:
	std::vector<int> getBigBox(cv::Mat binary, int areaValue);
	int	imgW;
	int CalSize;
	int amplify;
};
