#include "pch.h"
#include "VdoWare.h"

std::vector<cv::Mat> frames;
std::mutex mtx;


int GetVideo(const SorcePar& spar, std::function<void(cv::Mat, int)> f,int countOut)
{
    std::string rtsp1 = "rtsp://admin:a1b2c3d4@192.168.1.64:554/h264/ch1/min/av_stream";


    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);

	stream1.set(cv::CAP_PROP_BUFFERSIZE, 10); // 设置缓冲区大小为1帧
    stream1.set(cv::CAP_PROP_FPS, 10);

    if (!stream1.isOpened())
    {
        std::cout << "有视频流未打开" << std::endl;
        return 0;
    }

    cv::Mat frame1;

    int count = countOut;
    // // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    // cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        if (!stream1.read(frame1))
        {
            std::cout << "有视频流未读取" << std::endl;
            stream1.release();
            GetVideo(spar, f,count);
			/*return -1;*/
            /*continue;*/
        }

        /*cv::Mat frame1Copy;
        frame1.copyTo(frame1Copy);*/
        count++;
        // 显示帧（可选）
        f(frame1, count);

        if (cv::waitKey(spar.filepolling) >= 0) break; // 按任意键退出循环
    }

	return 0;
}




void processFrame(cv::VideoCapture& cap) {
    cv::Mat frame;
    while (cap.read(frame)) {
        mtx.lock();
        frames.push_back(frame.clone());  // 复制帧以避免线程共享同一Mat对象的问题
        mtx.unlock();
    }
}



void VdoWare::specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f)
{

    int count = 0;
    cv::VideoCapture cap("rtsp://admin:a1b2c3d4@192.168.1.64:554/h264/ch1/min/av_stream");
    //cap.set(cv::CAP_PROP_FPS, 10);
    std::thread t(processFrame, std::ref(cap));
    t.detach();  // 或者使用join等待线程结束，根据实际情况选择

    while (true) {
        mtx.lock();
        if (!frames.empty()) {
            cv::Mat frame = frames.back();
            frames.pop_back();
            // 处理frame
            count++;
            f(frame, count);

            mtx.unlock();
        }
        else {
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 防止忙等待
    }
}



void VdoWare::stop()
{
    this->exied = true;
}