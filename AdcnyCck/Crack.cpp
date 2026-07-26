#include "pch.h"
#include "Crack.h"
#include <corecrt_io.h>


#include <numeric>

Crack::Crack()
{
	this->edgeCal = nullptr;
}

Crack::~Crack()
{

}

std::vector<int> changeZqImg(cv::Mat& src) {

	std::vector<int> listforRow;

	for (int i = 0; i < src.rows; i++) {

		uchar* src_row_ptr = src.ptr<uchar>(i);

		for (int j = src.cols - 2; j >= 0; j--) {

			int xa = std::abs(src_row_ptr[j] - src_row_ptr[j + 1]);
			auto vc = static_cast<double>(std::abs(xa)) / src_row_ptr[j + 1];
			if (vc > 0.30)
			{
				listforRow.push_back(j);
				
				break;
			}
		}
	}

	return listforRow;
	
}



std::vector<int> getEdgeInfo(cv::Mat mm,int jw,int xbz,int num,int edgeValue)
{

	double xx = 0;
	int cc = 0;

	for (int i = 0; i < mm.rows / 1 - 5; i++)
	{
		cv::Rect roi(0, i * 1, jw, 1);

		cv::Mat im = mm(roi);
		cv::Scalar my_mean;
		my_mean = cv::sum(im);

		bool b = false;

		b = std::abs(my_mean[0] - xx) / my_mean[0] > 0.045;

		if (i > 0 && b && (i - cc) > 5)
		{
			std::cout << xbz << "-------" << "xb"  << std::endl;

			return { 0,xbz,(int)i,20,5,num,edgeValue,-1 };
			cc = i;
		}

		xx = my_mean[0];
	}

	return {};
}




std::vector<std::vector<int>> Crack::getBboxs(int num)
{
	std::vector<std::vector<int>> bboxs;

	bboxs.push_back({ 6000 });

	int edgeValue;
	cv::Mat imga=this->img;


	if (this->edgeCal == nullptr)
	{
		this->edgeCal = new EdgeCal(this->img.rows, 4, 3);
	}

	int starty1 = this->img.rows / 3;
	int edgeNum1 = this->edgeCal->edgeTodo(imga, true, starty1);
	int edgeNum2 = this->edgeCal->edgeTodo(imga, true, starty1 * 2);

	edgeValue = edgeNum1 > edgeNum2 ? edgeNum1 : edgeNum2;


	int bb = this->du == 1 ? (this->rl == 0 ? 10 : 10) : (this->rl == 0 ? 10 : 10);

	int xbz = edgeValue + bb;

	bboxs.push_back({ edgeValue });


	std::vector<int> list;

	int calsize = imga.rows / 10;

	for (int i = 1; i < imga.rows / calsize - 2; i++)
	{
		cv::Rect roi(edgeValue - 20, i * calsize, 40, calsize);

		cv::Mat src = imga(roi).clone();
		auto listforRow = changeZqImg(src);

		if (listforRow.size() > 0)
		{

			int sum = std::accumulate(listforRow.begin(), listforRow.end(), 0);

			// ����ƽ��ֵ
			int average = static_cast<int>(static_cast<double>(sum) / listforRow.size()) + (i < 3 ? 3 : 2);

			auto cnt1 = std::count_if(listforRow.begin(), listforRow.end(), [average](int x) {
				return x > average;
				});

			if (cnt1 > 1)
			{
				list.push_back(i);
				bboxs.push_back({ 0,edgeValue - 5,(int)i * calsize,15,calsize,num,edgeValue,9009 });
				break;
			}

		}
	}

	if (list.size()>0)
	{
		return bboxs;
	}


	int jw = 20;

	cv::Rect roi(xbz, 0, jw, imgH);

	auto li1=getEdgeInfo(imga(roi),jw,xbz,num,edgeValue);

	

	if (li1.size() == 0)
	{
		cv::Rect roi1(xbz+100, 0, jw, imgH);

		auto li2=getEdgeInfo(imga(roi1), 20, xbz, num, edgeValue);

		if (li2.size()>0)
		{
			bboxs.push_back(li2);
		}
	}
	else {
		bboxs.push_back(li1);
	}



	return bboxs;
}
