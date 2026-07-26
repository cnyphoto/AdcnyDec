#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>



class EdgeCal
{
public:
	EdgeCal(int imgW, int CalSize, int amplify);
	std::vector<int> getBigBox(cv::Mat binary, int areaValue, bool r);
	int edgeTodo(cv::Mat mat, bool lefted, int yStart);
private:
	cv::Mat getdiffNewRow(cv::Mat img, int n, int amplify);
	int	imgW;
	int CalSize;
	int amplify;
};

