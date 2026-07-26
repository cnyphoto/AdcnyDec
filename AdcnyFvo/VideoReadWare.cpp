#include "pch.h"
#include "VideoReadWare.h"
#include <corecrt_io.h>


void VideoReadWare::specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f)
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
        std::string VideoPath = spar.imgRoot + "\\" + std::string(fileInfo.name);

        std::cout << VideoPath << std::endl;

        cv::VideoCapture cap(VideoPath); // 替换为你的视频文件路径

        // 检查视频是否成功打开
        if (!cap.isOpened()) {
            std::cerr << "Error opening video file." << std::endl;
            return ;
        }

        cv::Mat frame;
        while (true) {
            // 逐帧读取视频
            cap >> frame; // 或者使用 cap.read(frame);

            // 检查是否正确读取帧
            if (frame.empty()) {
                std::cout << "No captured frame -- break!" << std::endl;
                break;
            }

            count++;
            // 显示帧（可选）
            f(frame, count);

            // 等待1毫秒（等待按键事件或帧显示时间）
            if (cv::waitKey(40) >= 0) break; // 按任意键退出循环
        }

        // 释放VideoCapture对象
        cap.release();

        std::this_thread::sleep_for(std::chrono::milliseconds(spar.filepolling));

    } while (_findnext(hFile, &fileInfo) == 0 || this->exied);

}

void VideoReadWare::stop()
{
	this->exied = true;
}