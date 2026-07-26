#include "pch.h"
#include "MvsStore.h"


MvsStore::MvsStore(int drivesNum) : drivesNum(drivesNum)
{

}

MvsStore::~MvsStore()
{

}



void MvsStore::run(std::function<void(cv::Mat, int)> f)
{
    
    this->boundFunction = f;

    do
    {
        // ch:初始化SDK | en:Initialize SDK
        nRet = MV_CC_Initialize();
        if (MV_OK != nRet)
        {
            printf("Initialize SDK fail! nRet [0x%x]\n", nRet);
            break;
        }

        // ch:枚举设备 | Enum device
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE|MV_GENTL_CAMERALINK_DEVICE, &stDeviceList);
        if (MV_OK != nRet)
        {
            printf("Enum Devices fail! nRet [0x%x]\n", nRet);
            break;
        }

       /* MV_CC_DEVICE_INFO* pDeviceInfook= new MV_CC_DEVICE_INFO;*/
        if (stDeviceList.nDeviceNum > 0)
        {
            for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                printf("[device %d]:\n", i);
                MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];

                if (NULL == pDeviceInfo)
                {
                    break;
                }

                bool re = true;

                if (this->camId =="0")
                {
                   re= !(this->drivesNum == i);
                }
                else 
                {
                    unsigned char* uc = reinterpret_cast<unsigned char*>(const_cast<char*>(this->camId.c_str()));
                    unsigned char* uc1 = pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName;
                    re = std::strcmp((const char*)uc, (const char*)uc1);
                }
                

                if (!re)
                { 
                    // ch:选择设备并创建句柄 | Select device and create handle
                    nRet = MV_CC_CreateHandle(&handle, pDeviceInfo);
                    if (MV_OK != nRet)
                    {
                        printf("Create Handle fail! nRet [0x%x]\n", nRet);
                        break;
                    }
                }

               /* PrintDeviceInfo(pDeviceInfo);*/
            }
        }
        else
        {
            printf("Find No Devices!\n");
            break;
        }


        // ch:打开设备 | Open device
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            printf("Open Device fail! nRet [0x%x]\n", nRet);
            break;
        }


        // ch:设置触发模式为off | eb:Set trigger mode as off
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", MV_TRIGGER_MODE_ON);
        if (MV_OK != nRet)
        {
            printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
            break;
        }

        // ch:注册抓图回调 | en:Register image callback
        nRet = MV_CC_RegisterImageCallBackEx(handle, ImageCallBackEx, this);
        if (MV_OK != nRet)
        {
            printf("Register Image CallBack fail! nRet [0x%x]\n", nRet);
            break;
        }

        // ch:开始取流 | en:Start grab image
        nRet = MV_CC_StartGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        printf("Grabbing..... .\n");

        while (this->runed)
        {
            Sleep(1);
        }

        // ch:停止取流 | en:Stop grab image
        nRet = MV_CC_StopGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        // ch:注销抓图回调 | en:Unregister image callback
        nRet = MV_CC_RegisterImageCallBackEx(handle, NULL, NULL);
        if (MV_OK != nRet)
        {
            printf("Unregister Image CallBack fail! nRet [0x%x]\n", nRet);
            break;
        }

        // ch:关闭设备 | en:Close device
        nRet = MV_CC_CloseDevice(handle);
        if (MV_OK != nRet)
        {
            printf("Close Device fail! nRet [0x%x]\n", nRet);
            break;
        }

        // ch:销毁句柄 | en:Destroy handle
        nRet = MV_CC_DestroyHandle(handle);
        if (MV_OK != nRet)
        {
            printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
            break;
        }
        handle = NULL;

    } while (0);


    if (handle != NULL)
    {
        MV_CC_DestroyHandle(handle);
        handle = NULL;
    }


    // ch:反初始化SDK | en:Finalize SDK
    MV_CC_Finalize();
}

void MvsStore::stopCam()
{
    this->runed = false;
}

