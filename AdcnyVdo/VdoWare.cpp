#include "pch.h"
#include "VdoWare.h"

std::vector<cv::Mat> frames;
std::mutex mtx;


int GetVideo(const SorcePar& spar, std::function<void(cv::Mat, int)> f,int countOut)
{
    std::string rtsp1 = "rtsp://admin:a1b2c3d4@192.168.1.64:554/h264/ch1/min/av_stream";

    cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);

    stream1.set(cv::CAP_PROP_BUFFERSIZE, 10);
    stream1.set(cv::CAP_PROP_FPS, 10);

    if (!stream1.isOpened())
    {
        std::cout << "video stream not opened" << std::endl;
        return 0;
    }

    cv::Mat frame1;

    int count = countOut;

    while (true)
    {
        if (!stream1.read(frame1))
        {
            std::cout << "video stream not read" << std::endl;
            stream1.release();
            GetVideo(spar, f,count);
        }

        count++;
        f(frame1, count);

        if (cv::waitKey(spar.filepolling) >= 0) break;
    }

    return 0;
}


void VdoWare::specificRequest(const SorcePar& spar, std::function<void(cv::Mat, int)> f)
{
    int count = 0;
    cv::VideoCapture cap("rtsp://admin:a1b2c3d4@192.168.1.64:554/h264/ch1/min/av_stream");

    std::thread t([&cap, spar, f, &count]() {
        cv::Mat frame;
        while (cap.read(frame)) {
            mtx.lock();
            frames.push_back(frame.clone());
            mtx.unlock();
        }
    });
    t.detach();

    while (true) {
        mtx.lock();
        if (!frames.empty()) {
            cv::Mat frame = frames.back();
            frames.pop_back();
            count++;
            f(frame, count);
            mtx.unlock();
        }
        else {
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}



void VdoWare::stop()
{
    this->exied = true;
}