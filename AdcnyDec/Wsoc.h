#pragma once
#include <iostream>
#include "easywsclient.h"
#include <queue>
#include <thread>
#include <atomic>

//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <DbToCode.h>
#include <CameraApt.h>




class Wsoc
{
public:
	Wsoc(DbToCode& dt,long long coilnum,int n, CameraApt& ca,int boundary);
	~Wsoc();
	int runSoc(std::string modetype,std::string webSockectIpPort);
	void setCoil(long long cnum);
	void setImgNum(int n);
	void setBoundary(int boundary);
	void requestStop();
	bool isStopped() const { return stopRequested.load(); }
private:
	long long coilnum;
	int n;
	DbToCode* dtcode;
	CameraApt* ca;
	int boundary;
	std::atomic<bool> stopRequested{ false };
};



