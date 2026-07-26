#include "pch.h"
#include "imgOp.h"
#include <corecrt_io.h>


imgOp::imgOp(std::string folder)
{
	this->folder = folder;
}

imgOp::~imgOp()
{

}


void imgOp::saveImg(int num, cv::Mat mat, long long coilnum, int scoild,int detected)
{
	std::string fd = this->folder;

	if (detected == 3 || detected == 1)
	{
		auto aaa= fd.replace(3, 3, "img1");
		fd = aaa;
	}

	std::string folderName =fd+ "\\" + std::to_string(scoild) + "\\" + std::to_string(coilnum); // 要创建的文件夹名称

	if ((_access(folderName.c_str(), 0)) == -1) {
		std::string command = "mkdir ";
		command += folderName;
		int result = system(command.c_str()); // 执行系统命令
	}

	char str[100];

	snprintf(str, sizeof(str), "%s%c%ld%s", folderName.c_str(), '\\', num, ".jpg");
	std::string save_path(str);
	std::cout << save_path << std::endl;
	std::vector<int> compression_params;
	compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
	compression_params.push_back(80);

	cv::Mat image_copy = mat.clone();

	std::thread([save_path, image_copy, compression_params]() {
		try {

			bool success = cv::imwrite(save_path, image_copy, compression_params);
		}
		catch (...) {
			std::cerr << "Exception in thread for: " << save_path << std::endl;
		}
		}).detach();
}

cv::Mat imgOp::enhancedPhotoForMultiply(cv::Mat mat)
{
	size_t size = 1;

	cv::Mat new_image = mat;
	auto start = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < size; i++)
	{
		new_image = contrastRatio(new_image);
	}

	mat.convertTo(mat, CV_32F, 1.0 / 255.0);
	new_image.convertTo(new_image, CV_32F, 1.0 / 255.0);

	cv::Mat result;
	cv::multiply(mat, new_image, result,1);

	// 将结果转换回8位无符号整型
	result.convertTo(result, CV_8U, 255.0);
	return result;
}

cv::Mat imgOp::contrastRatio(cv::Mat mat)
{
	cv::Mat new_image;
	double alpha = 1.8; // 对比度控制 (1.0 表示不变)
	int beta = 0;       // 亮度控制 (0 表示不变)

	mat.convertTo(new_image, -1, alpha, beta);

	return new_image;
}

std::map<std::string, int> imgOp::readCalINI(const std::string& filename)
{
	std::map<std::string, int> content;
	std::ifstream file(filename);
	std::string line;
	std::string key, value;

	while (std::getline(file, line)) {
		// 跳过空行和注释行
		if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[') continue;

		// 分割键和值
		std::stringstream ss(line);
		std::getline(ss, key, '=');
		std::getline(ss, value);
		content[key] = std::atoi(value.c_str());
	}

	return content;
}
