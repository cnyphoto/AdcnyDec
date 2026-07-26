#include "pch.h"
#include "WholeDec.h"

using namespace std;
using namespace cv;


WholeDec::WholeDec()
{
}

WholeDec::~WholeDec()
{
	std::cout << "DomainDec" << std::endl;
}


int WholeDec::imgValid(cv::Mat mat)
{
	//int c = 0;
	Rect r(imgW* 0.48, 0, 96, 20);
	Mat imga1 = mat(r);
	cv::Scalar sumScalar = cv::mean(imga1);
	//std::cout << "-=-=-=-=: " << sumScalar.val[0] << std::endl;
	return static_cast<int>(sumScalar.val[0] * 100);
}


cv::Mat WholeDec::setDst(cv::Mat img)
{
	cv::Mat dst(img.size(), CV_8UC1, cv::Scalar(255));

	int cutSize = 256;
	double dePar = 23;
	int wf_Size = (int)ceil(img.cols / cutSize);
	int hf_Size = (int)ceil(img.rows / cutSize);

	cv::parallel_for_(cv::Range(0, hf_Size), [&](const cv::Range& range)
		{
			for (int j = range.start; j < range.end; j++)
			{
				for (int i = 0; i < wf_Size + 1; i++)
				{
					int leftddd = i * cutSize;

					if (i == wf_Size && wf_Size * cutSize < img.cols)
					{
						leftddd = img.cols - cutSize;
					}

					cv::Rect roi(leftddd, j * cutSize, cutSize, cutSize);
					cv::Mat image = img(roi);
					cv::blur(image, image, cv::Size(1, 1));
					cv::Scalar my_mean;
					my_mean = cv::mean(image);
					int xf = (int)(my_mean[0] * (dePar / 100));
					cv::Mat dst_threshold;
					cv::inRange(image, cv::Scalar(my_mean[0] - xf), cv::Scalar(my_mean[0] + xf + 20), dst_threshold);
					cv::bitwise_not(dst_threshold, dst_threshold);
					dst_threshold.copyTo(dst(roi));
				}
			}
		});

	return dst;
}


std::vector<std::vector<int>> WholeDec::getBboxs(int num)
{
	std::vector<cv::Mat> matList;
	std::vector<std::vector<int>> bboxs;
	std::vector<cv::Mat> copeList;

	bboxs.push_back({ -1 });

	{
		cv::Mat mat = this->img;

		int colorval = imgValid(mat);
		if (colorval < 5000)
		{
			//std::cout << "-=-=-=-=: " << imgValid(mat) << std::endl;
			return bboxs;
		}
		bboxs[0][0] = colorval;

		if (this->img.step == 3)
		{
			cv::Mat imgs[3];
			split(mat, imgs);

			for (size_t i = 0; i < 3; i++)
			{
				matList.push_back(imgs[i]);
			}
		}
		else 
		{

			matList.push_back(mat);
		}

		/*cv::Mat lab;
		cv::cvtColor(mat, lab, cv::COLOR_RGB2Lab);
		split(lab, imgs);
		for (size_t i = 0; i < 3; i++)
		{
			matList.push_back(imgs[i]);
		}*/
	}
	

	std::vector<int> box1;
	std::vector<int> box2;

	int a=0;
	int b=0;
	int ft = 20;
	int ge = 200;

	cv::Mat mat1, mat2, mat3;

	size_t ss = matList.size();
	for (size_t i = 0; i < ss; i++)
	{
		mat1 = matList.at(i);

		if (i == 0)
		{
			EdgCalTcm edgeCal(0,0,0);

			a = edgeCal.edgeTodoe(mat1, 25, 0);
			b = edgeCal.edgeTodoe(mat1, 25, 1);
			this->edge = a;
			a = a + ge;
			b = (4096 - (800 - b)) - ge;

		}	
		
		std::cout << "-------------edge£º" << this->edge << "---------------" << std::endl;
		
		cv::Rect r(a, 0, b - a, 2048);

		mat2 = mat1(r);

		mat3 = setDst(mat2);
	}

	bboxs.push_back({ this->edge });

	cv::Mat dst = mat3;
	cv::Mat labels, stats, centroids, stats1;
	int n = cv::connectedComponentsWithStats(dst, labels, stats, centroids, 8, CV_32S);
	//std::cout << "-------------n£º" << n << "---------------" << std::endl;
	
	for (int i = 0; i < n; i++)
	{
		if (i == 0) {
			continue;
		}

		int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
		int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
		int l = stats.at<int>(i, cv::CC_STAT_LEFT);
		int t = stats.at<int>(i, cv::CC_STAT_TOP);
		int area = stats.at<int>(i, cv::CC_STAT_AREA);
		int wxh = w * h;

		if (area > 28 && l > 150)
		{
			//bboxs.push_back({l,t,w,h});
			bboxs.push_back({ 
				area,
				l + a,
				t ,
				w ,
				h ,
				num,
				this->edge,
				-1 });
		}

		if (bboxs.size() > 10)
		{
			break;
		}
	}


	std::cout << "-------------defect£º" << bboxs.size()-2 << "---------------" << std::endl;
	return bboxs;
}


