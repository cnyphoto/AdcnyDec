#include "pch.h"
#include "EdgCalTcm.h"

std::vector<int> EdgCalTcm::getBigBox(cv::Mat binary, int areaValue)
{
	std::vector<int> box(5);

	cv::Mat labels, stats, centroids;

	int n = connectedComponentsWithStats(binary, labels, stats, centroids, 8, CV_32S);

	for (size_t i = 0; i < n; i++)
	{
		if (i == 0)
		{
			continue;
		}
		int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
		int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
		int x = stats.at<int>(i, cv::CC_STAT_LEFT);
		int y = stats.at<int>(i, cv::CC_STAT_TOP);
		int a = stats.at<int>(i, cv::CC_STAT_AREA);
		int wxh = w * h;

		if (wxh > areaValue && wxh > box[0] * box[1])
		{
			box = { w, h, x, y, a };
		}
	}
	return box;
}



EdgCalTcm::EdgCalTcm(int imW, int calSize, int amplif)
{
	this->imgW = imW;
	this->CalSize = calSize;
	this->amplify = amplif;
}

int EdgCalTcm::edgeTodoe(cv::Mat imgz, int th, int rl)
{
	cv::Rect r(rl == 0 ? 0 : imgz.cols - 800, imgz.rows * 0.3, 800, 20);
	cv::Mat imga1 = imgz(r).clone();
	cv::threshold(imga1, imga1, th, 255, cv::THRESH_BINARY);
	auto box = this->getBigBox(imga1, 3200);
	int xxx = rl == 0 ? box[2] : box[0];
	return xxx;
}





