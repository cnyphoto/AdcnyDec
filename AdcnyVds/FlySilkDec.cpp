#include "pch.h"
#include "FlySilkDec.h"



FlySilkDec::FlySilkDec()
{
	this->edgeCal = nullptr;
}

FlySilkDec::~FlySilkDec()
{
	std::cout << "FlySilkDec" << std::endl;
}


int FlySilkDec::imgValid(cv::Mat mat)
{
	return 0;
}

cv::Mat FlySilkDec::setDst(cv::Mat img)
{
	cv::Mat dst(img.size(), CV_8UC1, cv::Scalar(255));
	return dst;
}


std::vector<std::vector<int>> FlySilkDec::getBboxs(int num)
{
	
	std::vector<std::vector<int>> bboxs;
	bboxs.push_back({ -1 });
	
	{

		if (this->config.size()==0)
		{
			if (this->rl==1)
			{
				this->config = this->imgop->readCalINI("calPar1.ini");//1
			}
			else {
				this->config = this->imgop->readCalINI("calPar.ini");//2
			}
		}
		

		cv::Rect roi(config["roix"], config["roiy"], config["roiw"], config["roih"]);


		// 创建用于存储区域的Mat对象
		cv::Mat mat; // 直接使用operator()复制区域

		cv::cvtColor(this->img(roi), mat, cv::COLOR_BGR2GRAY);

		if (this->edgeCal == nullptr)
		{
			this->edgeCal = new EdgeCalSilk(mat.rows, 4, 3);
		}

		int starty1 = mat.rows / 2;
		

		cv::Mat mat1; // 直接使用operator()复制区域


		if (this->rl==1)
		{
			int edgeNum1 = this->edgeCal->edgeTodo(mat, true, starty1)- config["detectw"];
			mat1 = mat({ edgeNum1,starty1,config["detectw"],config["detecth"]});
		}
		else {
			int edgeNum1 = this->edgeCal->edgeTodo(mat, true, starty1);
			mat1 = mat({ edgeNum1,starty1,config["detectw"],config["detecth"]});
		}
		


		cv::Mat dst;
		cv::threshold(mat1, dst, config["thresh"], config["maxval"], cv::THRESH_BINARY);

		//cv::imshow("Display window", dst);
	 //   cv::waitKey(0); // 等待用户按键，0表示无限期等待
	 //   imgValid(dst);

		int nonZeroCount = cv::countNonZero(dst);
		
		if (nonZeroCount> config["nonzerocount"])
		{
			bboxs.push_back({
				0,
				0,
				0 ,
				0 ,
				0 ,
				num,
				0,
				1 });
		}
	
		std::cout << "nonZeroCount: "<<"-------------"<<num<<"----------" << nonZeroCount << std::endl;
	}



	return bboxs;
}