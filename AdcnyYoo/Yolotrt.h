#pragma once

#include <NvInfer.h>
#include <cuda_runtime.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <CompareContours.h>


#ifdef ADCNYYOO_EXPORTS
#define ADCNYYOO_API __declspec(dllexport)
#else
#define ADCNYYOO_API __declspec(dllimport)
#endif

class __declspec(dllexport) YoloTrt
{
public:
    YoloTrt();
    ~YoloTrt();

    float letterbox(
        const cv::Mat& image,
        cv::Mat& out_image,
        const cv::Size& new_shape = cv::Size(640, 640),
        int stride = 32,
        const cv::Scalar& color = cv::Scalar(114, 114, 114),
        bool fixed_shape = false,
        bool scale_up = true);

    float* blobFromImagetofloat(cv::Mat& img)
    {
        float* blob = new float[img.total() * 3];
        int channels = 3;
        int img_h = img.rows;
        int img_w = img.cols;
        for (size_t c = 0; c < channels; c++)
        {
            for (size_t h = 0; h < img_h; h++)
            {
                for (size_t w = 0; w < img_w; w++)
                {
                    blob[c * img_w * img_h + h * img_w + w] = (float)img.at<cv::Vec3b>(h, w)[c] / 255.0;
                }
            }
        }
        return blob;
    }

    cv::Rect scale_boxes(cv::Mat& img, float bbox[4]) const {

        float l, r, t, b;

        float r_w = iW / (img.cols * 1.f);
        float r_h = iH / (img.rows * 1.f);
        float xx = 0.f;
        float yy = 0.f;
        float ratio;

        if (r_h > r_w)
        {
            yy = iW - r_w * img.rows;
            ratio = r_w;
        }
        else {
            xx = iH - r_h * img.cols;
            ratio = r_h;

        }
        l = bbox[0] - bbox[2] * 0.5 - xx * 0.5;
        r = bbox[0] + bbox[2] * 0.5 - xx * 0.5;
        t = bbox[1] - bbox[3] * 0.5 - yy * 0.5;
        b = bbox[1] + bbox[3] * 0.5 - yy * 0.5;
        l = round(l / ratio);
        r = round(r / ratio);
        t = round(t / ratio);
        b = round(b / ratio);

        int rl = r - l > img.cols ? img.cols : r - l;
        int bt = b - t > img.rows ? img.rows : b - t;

        return cv::Rect(l<0?0:l, t<0?0:t, rl, bt);
    }

    float iou(float lbox[4], float rbox[4]) {
        float interBox[] = {
            (std::max)(lbox[0] - lbox[2] / 2.f , rbox[0] - rbox[2] / 2.f), //left
            (std::min)(lbox[0] + lbox[2] / 2.f , rbox[0] + rbox[2] / 2.f), //right
            (std::max)(lbox[1] - lbox[3] / 2.f , rbox[1] - rbox[3] / 2.f), //top
            (std::min)(lbox[1] + lbox[3] / 2.f , rbox[1] + rbox[3] / 2.f), //bottom
        };

        if (interBox[2] > interBox[3] || interBox[0] > interBox[1])
            return 0.0f;

        float interBoxS = (interBox[1] - interBox[0]) * (interBox[3] - interBox[2]);
        return interBoxS / (lbox[2] * lbox[3] + rbox[2] * rbox[3] - interBoxS);
    }

    int getClassId(float* output, int start, int num) {
        int maxint = 0;
        float d = 0;
        for (size_t i = 0; i < num; i++)
        {
            //std::cout << d << std::endl;
            if (output[start + i] > d)
            {
                d = output[start + i];
                maxint = i;
            }
        }
        return maxint;
    }

    void nmsforiou(std::vector<Detection>& res, std::map<float, std::vector<Detection>> ma, float nms_thresh = 0.5) {
        for (auto it = ma.begin(); it != ma.end(); it++) {
            auto& dets = it->second;
            std::sort(dets.begin(), dets.end(), CompareContours());
            for (size_t m = 0; m < dets.size(); ++m) {
                auto& item = dets[m];
                res.push_back(item);
                for (size_t n = m + 1; n < dets.size(); ++n) {
                    if (iou(item.bbox, dets[n].bbox) > nms_thresh) {
                        dets.erase(dets.begin() + n);
                        --n;
                    }
                }
            }
        }
    }

    void nms(std::vector<Detection>& res, float* output, float conf_thresh, float nms_thresh = 0.5) {
        int det_size = classsize * 4 / sizeof(float);
        std::map<float, std::vector<Detection>> ma;
        for (int i = 0; i < out_size / det_size; i++) {
            if (output[1 + det_size * i + 3] > conf_thresh)
            {
                Detection det;
                memcpy(&det, &output[0 + det_size * i], 5 * sizeof(float));
                det.class_id = getClassId(output, 1 + det_size * i + 3 + 1, classsize - 5);
                if (ma.count(det.class_id) == 0) ma.emplace(det.class_id, std::vector<Detection>());
                ma[det.class_id].push_back(det);
            }
        }

        nmsforiou(res, ma, nms_thresh);
    }

    void init(std::string engine_file_path, int slot, float conff, int nameint);

    void Infer(
        int aWidth,
        int aHeight,
        int aChannel,
        unsigned char* aBytes);
    int getBigClsId(int oldClsId);
    std::vector<Detection> boxes = {};

private:
    // 用于获取engine 输入输出的name

    const char* INPUT_BLOB_NAME = "images";
    const char* OUTPUT_BLOB_NAME = "output0";

    // runtime创建engine，engine创建context
    nvinfer1::IRuntime* runtime;
    nvinfer1::ICudaEngine* engine;
    nvinfer1::IExecutionContext* context;

    cudaStream_t stream = nullptr;

    void* buffs[2];

    int iH, iW, in_size, out_size, classsize;

    int name;

    int slot;

    float connf;
};

