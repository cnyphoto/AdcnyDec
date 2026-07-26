#include "Wsoc.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include "easywsclient.h"

Wsoc::Wsoc(long long coilnum, int n, int boundary)
{
    this->coilnum = coilnum;
    this->n = n;
    this->boundary = boundary;
}

Wsoc::~Wsoc()
{
    requestStop();
}

void Wsoc::requestStop()
{
    stopRequested.store(true);
}

void Wsoc::setOnExitCallback(std::function<void(int n, long long coilnum)> callback)
{
    onExitCallback = std::move(callback);
}

int Wsoc::runSoc(std::string modetype, std::string webSockectIpPort)
{
    using easywsclient::WebSocket;
#ifdef _WIN32
    INT rc;
    WSADATA wsaData;

    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        printf("WSAStartup Failed.\n");
        return 1;
    }
#endif
    std::string ads = webSockectIpPort + modetype;
    WebSocket::pointer ws = WebSocket::from_url(ads);
    if (!ws) {
        printf("WebSocket::from_url failed.\n");
        WSACleanup();
        return 1;
    }

    while (ws && ws->getReadyState() != WebSocket::CLOSED && !stopRequested.load())
    {
        std::string msg = "";
        ws->send("0#" + modetype + "-" + std::to_string(this->n) + "-" + std::to_string(this->coilnum) + "-" + std::to_string(this->boundary));
        std::cout << "0#" + modetype + "-" + std::to_string(this->n) << std::endl;
        ws->poll();
        ws->dispatch([&msg](const std::string& message)
            {
                printf(">>> %s\n", message.c_str());
                if (message == "5-exit")
                {
                    msg = "5-exit";
                }
            });

        if (msg == "5-exit")
        {
            if (onExitCallback)
                onExitCallback(this->n, this->coilnum);
            ws->close();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(499));
    }

    // 等待 WebSocket 完全关闭
    while (ws && ws->getReadyState() != WebSocket::CLOSED) {
        ws->poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    delete ws;
    ws = nullptr;

#ifdef _WIN32
    printf(">>> %s\n", "关闭socket");
    WSACleanup();
#endif

    return 0;
}

void Wsoc::setCoil(long long cnum)
{
    this->coilnum = cnum;
}

void Wsoc::setImgNum(int n)
{
    this->n = n;
}

void Wsoc::setBoundary(int boundary)//+++++++++++++++++++++++++
{
    this->boundary = boundary;
}
