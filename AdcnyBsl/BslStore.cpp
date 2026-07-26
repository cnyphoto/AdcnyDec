#include "pch.h"
#include "BslStore.h"



BslStore::BslStore()
{

}

BslStore::~BslStore()
{

}


void BslStore::run(std::function<void(cv::Mat, int)> f)
{
	this->stoped=false;
	// Before using any pylon methods, the pylon runtime must be initialized.

	PylonInitialize();

    try
    {
        // Create an instant camera object with the first found camera device.
        CBaslerUniversalInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());

        // Print the model name of the camera.
        std::cout << "Using device: " << camera.GetDeviceInfo().GetModelName() << std::endl << std::endl;

        // Register an image event handler that accesses the chunk data.
        //camera.RegisterImageEventHandler(new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);

        // Open the camera.
        camera.Open();

        // A GenICam node map is required for accessing chunk data. That's why a small node map is required for each grab result.
        // Creating a lot of node maps can be time consuming.
        // The node maps are usually created dynamically when StartGrabbing() is called.
        // To avoid a delay caused by node map creation in StartGrabbing() you have the option to create
        // a static pool of node maps once before grabbing.
        //camera.StaticChunkNodeMapPoolSize = camera.MaxNumBuffer.GetValue();

        // Enable chunks in general.
        if (!camera.ChunkModeActive.TrySetValue(true))
        {
            throw RUNTIME_EXCEPTION("The camera doesn't support chunk features");
        }

        // Enable time stamp chunks.
        camera.ChunkSelector.SetValue(ChunkSelector_Timestamp);
        camera.ChunkEnable.SetValue(true);

        // Enable frame counter chunks?
        if (camera.ChunkSelector.TrySetValue(ChunkSelector_Framecounter))
        {
            // USB camera devices provide generic counters.
            // An explicit FrameCounter value is not provided by USB camera devices.
            // Enable frame counter chunks.
            camera.ChunkEnable.SetValue(true);
        }

        // Enable CRC checksum chunks.
        camera.ChunkSelector.SetValue(ChunkSelector_PayloadCRC16);
        camera.ChunkEnable.SetValue(true);

        //camera.AcquisitionMode.SetIntValue(AcquisitionMode_Continuous);

        camera.TriggerSelector.SetValue(TriggerSelector_FrameStart);
        camera.TriggerMode.SetValue(TriggerMode_On);

        camera.TriggerSource.SetValue(TriggerSource_Line3);
        camera.TriggerActivation.SetIntValue(TriggerActivation_RisingEdge);

        camera.AcquisitionMode.SetIntValue(AcquisitionMode_Continuous);


        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.
        camera.StartGrabbing();

        // This smart pointer will receive the grab result data.
        CBaslerUniversalGrabResultPtr ptrGrabResult;

        // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
        // when c_countOfImagesToGrab images have been retrieved.
        while (camera.IsGrabbing()&&!this->stoped)
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            // RetrieveResult calls the image event handler's OnImageGrabbed method.
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            std::cout << "GrabSucceeded: " << ptrGrabResult->GrabSucceeded() << std::endl;

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
//#ifdef PYLON_WIN_BUILD
//                // Display the image
//                Pylon::DisplayImage( 1, ptrGrabResult );
//#endif

                // The result data is automatically filled with received chunk data.
                // (Note:  This is not the case when using the low-level API)
                //cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
                //cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;

                const uint8_t* pImageBuffer = (uint8_t*) ptrGrabResult->GetBuffer();

                f(cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, ptrGrabResult->GetBuffer()), ptrGrabResult->GetImageNumber());

                //cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl;

                // Check to see if a buffer containing chunk data has been received.
                if (PayloadType_ChunkData != ptrGrabResult->GetPayloadType())
                {
                    throw RUNTIME_EXCEPTION( "Unexpected payload type received." );
                }

                // Since we have activated the CRC Checksum feature, we can check
                // the integrity of the buffer first.
                // Note: Enabling the CRC Checksum feature is not a prerequisite for using
                // chunks. Chunks can also be handled when the CRC Checksum feature is deactivated.
                if (ptrGrabResult->HasCRC() && ptrGrabResult->CheckCRC() == false)
                {
                    throw RUNTIME_EXCEPTION( "Image was damaged!" );
                }

                // Access the chunk data attached to the result.
                // Before accessing the chunk data, you should check to see
                // if the chunk is readable. When it is readable, the buffer
                // contains the requested chunk data.
                if (ptrGrabResult->ChunkTimestamp.IsReadable())
                {
                    //cout << "TimeStamp (Result): " << ptrGrabResult->ChunkTimestamp.GetValue() << endl;
                }

                // USB camera devices provide generic counters. An explicit FrameCounter value is not provided by USB camera devices.
                if (ptrGrabResult->ChunkFramecounter.IsReadable())
                {
                    //cout << "FrameCounter (Result): " << ptrGrabResult->ChunkFramecounter.GetValue() << endl;
                }

                //cout << endl;
            }
            else
            {
                std::cout << "Error: " << std::hex << ptrGrabResult->GetErrorCode() << std::dec << " " << ptrGrabResult->GetErrorDescription() << std::endl;
            }
        }

         //Disable chunk mode.
        camera.ChunkModeActive.SetValue(false);
    }
    catch (const GenericException& e)
    {
       
        
    }
}

void BslStore::stopCam()
{
    this->stoped = TRUE;
    // Before using any pylon methods, the pylon runtime must be initialized.
}