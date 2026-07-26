#pragma once

#ifdef ADCNYBSE_EXPORTS
#define ADCNYBSE_API __declspec(dllexport)
#else
#define ADCNYBSE_API __declspec(dllimport)
#endif


struct __declspec(dllexport) alignas(float) Detection {
    float bbox[4];
    float conf;  // bbox_conf * cls_conf
    int class_id;
};

class __declspec(dllexport) CompareContours
{
public:
    bool operator()(const Detection& a, const Detection& b) const
    {
        return a.conf > b.conf;
    }
};