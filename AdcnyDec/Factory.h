#pragma once
#include <memory>
#include <string>
#include <CameraApt.h>
#include <ImgCal.h>
#include <FolderWare.h>
#include <MvsWare.h>
#include <VdoWare.h>
#include <VideoReadWare.h>
#include <EdgeSin.h>
#include <Crack.h>
#include <WholeDec.h>
#include <MidlleDec.h>
#include <FlySilkDec.h>

/// <summary>
/// 相机工厂：根据类型名称创建 CameraApt 实例
/// </summary>
inline std::unique_ptr<CameraApt> createCamera(const std::string& type)
{
    if (type == "folder") return std::make_unique<FolderWare>();
    if (type == "mvs")    return std::make_unique<MvsWare>();
    if (type == "vdo")    return std::make_unique<VdoWare>();
    if (type == "video")  return std::make_unique<VideoReadWare>();
    return std::make_unique<FolderWare>(); // 默认
}

/// <summary>
/// 检测算法工厂：根据类型名称创建 ImgCal 实例
/// </summary>
inline std::unique_ptr<ImgCal> createDetector(const std::string& type)
{
    if (type == "edgesin")  return std::make_unique<EdgeSin>();
    if (type == "crack")    return std::make_unique<Crack>();
    if (type == "wholedec") return std::make_unique<WholeDec>();
    if (type == "middledec") return std::make_unique<MidlleDec>();
    if (type == "flysilk")  return std::make_unique<FlySilkDec>();
    return std::make_unique<EdgeSin>(); // 默认
}
