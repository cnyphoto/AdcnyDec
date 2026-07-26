#include "pch.h"
#include "FolderWare.h"
#include <vector>
#include <iostream>
#include <io.h>
#include <conio.h>
#include <thread>


void FolderWare::specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f)
{
	_finddata64i32_t fileInfo;
	
	intptr_t hFile = _findfirst((spar.imgRoot + "\\*." + spar.fileType).c_str(), &fileInfo);

	if (hFile == -1) {
		std::cout << "not find image!\n";
		return;
	}

	int count = 0;

	do
	{
		std::string imgPath = spar.imgRoot + "\\" + std::string(fileInfo.name);

		std::cout << imgPath << std::endl;

		count++;

		cv::Mat img = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);

		f(img, count);

		std::this_thread::sleep_for(std::chrono::milliseconds(spar.filepolling));

	} while (_findnext(hFile, &fileInfo) == 0||this->exied);
}


void FolderWare::stop()
{
	this->exied = true;
}



