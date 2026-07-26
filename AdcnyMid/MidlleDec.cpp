#include "pch.h"
#include "MidlleDec.h"


MidlleDec::MidlleDec()
{
}

MidlleDec::~MidlleDec()
{
	std::cout << "MidlleDec" << std::endl;
}

int MidlleDec::imgValid(cv::Mat mat)
{
	//int c = 0;
	Rect r(imgW * 0.48, 0, 96, 20);
	Mat imga1 = mat(r);
	cv::Scalar sumScalar = cv::mean(imga1);
	//std::cout << "-=-=-=-=: " << sumScalar.val[0] << std::endl;
	return static_cast<int>(sumScalar.val[0] * 100);
}


cv::Mat MidlleDec::setDst(cv::Mat img)
{
	cv::Mat dst(img.size(), CV_8UC1, cv::Scalar(255));

	int cutHSize = 256;
	int cutWSize = 256;
	double dePar = 23;
	int wf_Size = (int)ceil(img.cols / cutHSize);
	int hf_Size = (int)ceil(img.rows / cutWSize);

	cv::parallel_for_(cv::Range(0, hf_Size), [&](const cv::Range& range)
		{
			for (int j = range.start; j < range.end; j++)
			{
				for (int i = 0; i < wf_Size + 1; i++)
				{
					int leftddd = i * cutWSize;

					if (i == wf_Size && wf_Size * cutWSize < img.cols)
					{
						leftddd = img.cols - cutWSize;
					}

					cv::Rect roi(leftddd, j * cutWSize, cutWSize, cutWSize);
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


std::vector<std::vector<int>> MidlleDec::getBboxs(int num)
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

		
	}


	std::vector<int> box1;
	std::vector<int> box2;

	int a = 0;
	int b = 0;

	cv::Mat mat1, mat2, mat3;

	size_t ss = matList.size();

	for (size_t i = 0; i < ss; i++)
	{
		cv::Rect r(0, 0, this->img.cols, this->img.rows);

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


	std::cout << "-------------defect£º" << bboxs.size() - 2 << "---------------" << std::endl;
	return bboxs;
}
