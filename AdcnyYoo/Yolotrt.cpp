#include "pch.h"
#include <Yolotrt.h>
#include <iostream>
#include <fstream>
#include <Yolotrt.h>


using namespace cv::dnn;


using std::ios;
using std::ofstream;

class TRTLogger : public nvinfer1::ILogger
{
public:
    virtual void log(Severity severity, nvinfer1::AsciiChar const* msg) noexcept override
    {
        if (severity <= Severity::kINFO)
        {
            //printf("%d: %s\n", severity, msg);
        }
    }
};

YoloTrt::YoloTrt()=default;


YoloTrt::~YoloTrt()
{
    //std::cout << "yolo destroy" << std::endl;
    cudaFree(buffs[0]);
    cudaFree(buffs[1]);
}

float YoloTrt::letterbox(const cv::Mat& image, cv::Mat& out_image, const cv::Size& new_shape, int stride, const cv::Scalar& color, bool fixed_shape, bool scale_up)
{
    cv::Size shape = image.size();
    float r = std::min(
        (float)new_shape.height / (float)shape.height, (float)new_shape.width / (float)shape.width);
    if (!scale_up)
    {
        r = std::min(r, 1.0f);
    }

    int newUnpad[2]{
        (int)std::round((float)shape.width * r), (int)std::round((float)shape.height * r) };

    cv::Mat tmp;
    if (shape.width != newUnpad[0] || shape.height != newUnpad[1])
    {
        cv::resize(image, tmp, cv::Size(newUnpad[0], newUnpad[1]));
    }
    else
    {
        tmp = image.clone();
    }

    float dw = new_shape.width - newUnpad[0];
    float dh = new_shape.height - newUnpad[1];

    if (!fixed_shape)
    {
        dw = (float)((int)dw % stride);
        dh = (float)((int)dh % stride);
    }

    dw /= 2.0f;
    dh /= 2.0f;

    int top = int(std::round(dh - 0.1f));
    int bottom = int(std::round(dh + 0.1f));
    int left = int(std::round(dw - 0.1f));
    int right = int(std::round(dw + 0.1f));
    cv::copyMakeBorder(tmp, out_image, top, bottom, left, right, cv::BORDER_CONSTANT, color);

    return 1.0f / r;
}


void YoloTrt::init(std::string engine_file_path, int slot, float conff, int nameint)
{
    this->connf = conff;
    this->name = nameint;
    this->slot = slot;
    this->classsize = 0;
    TRTLogger logger;
    size_t size{ 0 };
    char* trtModelStream{ nullptr };
    std::ifstream file(engine_file_path, std::ios::binary);

    if (file.good())
    {
        file.seekg(0, file.end);
        size = file.tellg();
        file.seekg(0, file.beg);
        trtModelStream = new char[size];
        assert(trtModelStream);
        file.read(trtModelStream, size);
        file.close();
    }

    cudaSetDevice(this->slot);

    runtime = nvinfer1::createInferRuntime(logger);

    assert(runtime != nullptr);
    engine = runtime->deserializeCudaEngine(trtModelStream, size);
    assert(engine != nullptr);
    context = engine->createExecutionContext();
    assert(context != nullptr);
    delete[] trtModelStream;

    auto in_dims = engine->getBindingDimensions(0);

    iH = in_dims.d[2];
    iW = in_dims.d[3];
    std::cout << iH << std::endl;
    std::cout << iW << std::endl;

    in_size = 1;
    for (int j = 0; j < in_dims.nbDims; j++)
    {
        in_size *= in_dims.d[j];
        std::cout << in_size << std::endl;
    }

    auto out_dims = engine->getBindingDimensions(1);

    out_size = 1;
    for (int j = 0; j < out_dims.nbDims; j++)
    {
        out_size *= out_dims.d[j];
        if (j == 2)
        {
            classsize = out_dims.d[j];
        }
        std::cout << out_size << std::endl;
    }

    context = engine->createExecutionContext();

    if (!context)
    {
        std::cout << "create execution context failed\n";
        std::abort();
    }

    cudaError_t state;
    state = cudaMalloc(&buffs[0], in_size * sizeof(float));
    if (state)
    {
        std::cout << "allocate memory failed\n";
        std::abort();
    }

    state = cudaMalloc(&buffs[1], out_size * sizeof(float));

    if (state)
    {
        std::cout << "allocate memory failed\n";
        std::abort();
    }

    state = cudaStreamCreate(&stream);
    if (state)
    {
        std::cout << "create stream failed\n";
        std::abort();
    }
}

void YoloTrt::Infer(int aWidth, int aHeight, int aChannel, unsigned char* aBytes)
{
    this->boxes.clear();

    cv::Mat img(aHeight, aWidth, CV_MAKETYPE(CV_8U, aChannel), aBytes);
    /*cv::Mat blob;*/

    float* det_classes = new float[out_size];

    cv::Mat pr_img;

    float scale = letterbox(img, pr_img, {iW,iH}, 32, {114,114,114}, true);

    cv::cvtColor(pr_img, pr_img, cv::COLOR_BGR2RGB);

    float* blob = blobFromImagetofloat(pr_img);

    //cv::dnn::blobFromImage(img, blob, 1 / 255.0, cv::Size(this->iW, this->iH), cv::Scalar(114, 114, 114), true, false, CV_32F);

    /* auto start = std::chrono::system_clock::now();*/
    cudaStreamSynchronize(stream);

    cudaError_t state = cudaMemcpyAsync(buffs[0], blob, in_size * sizeof(float), cudaMemcpyHostToDevice, stream);

    if (state)
    {
        std::cout << "transmit to device failed\n";
        std::abort();
    }

    context->enqueueV2(&buffs[0], stream, nullptr);

    state = cudaMemcpyAsync(det_classes, buffs[1], out_size * sizeof(float), cudaMemcpyDeviceToHost, stream);

    if (state)
    {
        std::cout << "transmit to host failed \n";
        std::abort();
    }

    /*auto end = std::chrono::system_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    std::cout << "----------------------------------------" << std::endl;*/
    nms(boxes, det_classes, this->connf,0.5);
    delete[] det_classes;
    delete[] blob;
    cudaStreamSynchronize(stream);
}

int YoloTrt::getBigClsId(int oldClsId)
{
    return this->name*1000+this->slot*100+oldClsId;
}





