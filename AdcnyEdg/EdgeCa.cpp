#include "pch.h"
#include "EdgeCa.h"

std::vector<int> EdgeCa::getBigBox(cv::Mat binary, int areaValue, bool r)
{
	std::vector<int> box(5);

	cv::Mat labels, stats, centroids;

	int n = cv::connectedComponentsWithStats(binary, labels, stats, centroids, 8, CV_32S);

	for (int i = 0; i < n; i++)
	{
		if (i == 0)
		{
			continue;
		}
		size_t w = stats.at<int>(i, cv::CC_STAT_WIDTH);
		size_t h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
		size_t x = stats.at<int>(i, cv::CC_STAT_LEFT);
		size_t y = stats.at<int>(i, cv::CC_STAT_TOP);
		size_t a = stats.at<int>(i, cv::CC_STAT_AREA);
		size_t wxh = w * h;

		if (a > areaValue && (r ? x > box[2]:w < box[2]))
		{
			box = { static_cast<int>(w), static_cast<int>(h), static_cast<int>(x), static_cast<int>(y), static_cast<int>(a) };
		}
	}
	return box;
}


cv::Mat EdgeCa::getdiffNewRow(cv::Mat img, int n, int amplify)
{
	cv::Mat diffRow(img.rows, img.cols / n, CV_8UC1);

	for (int x = 1; x < img.cols / n; ++x)
	{
		cv::Mat diffRowxx;
		cv::Rect roi(x, 0, 1, img.rows);
		// 提取当前行和上一行的像素
		cv::Mat prevcol = img.col(x * n - n);
		cv::Mat currcol = img.col(x * n);

		// 计算两行的差值
		cv::absdiff(prevcol, currcol, diffRowxx);
		diffRowxx = diffRowxx * amplify;

		// 可以在这里添加你想要进行的处理，例如统计非零像素的个数等
		// 例如统计非零像素的个数
		// int nonZeroCount = cv::countNonZero(diffRow);

		// 输出差值图片
		//cv::convertScaleAbs(diffRowxx, diffRowxx, 1 / 256.0);
		diffRowxx.copyTo(diffRow(roi));
	}

	cv::Rect roi(0, 0, img.cols / n, img.rows);
	return diffRow(roi);
}

EdgeCa::EdgeCa(int imgW, int CalSize, int amplify) :imgW(imgW), CalSize(CalSize), amplify(amplify)
{

}

int EdgeCa::edgeTodo(cv::Mat mat, bool lefted, int yStart)
{
	std::vector<int> box1;

	int x = 0;

	cv::Rect r(x, yStart, mat.cols, 20);

	cv::Mat gray = mat(r);

	cv::Mat mat2 = getdiffNewRow(gray, this->CalSize, this->amplify);

	cv::threshold(mat2, mat2, 50, 255, cv::THRESH_BINARY);

	box1 = getBigBox(mat2, 15, lefted);

	return  lefted ? box1[2] * this->CalSize + 2 : (imgW - (box1[2] * this->CalSize + 0)) - 2;
}