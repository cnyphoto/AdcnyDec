#pragma once
#include <atomic>
#include <functional>
#include <string>

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

class Wsoc
{
public:
	Wsoc(long long coilnum, int n, int boundary);
	~Wsoc();
	int runSoc(std::string modetype, std::string webSockectIpPort);
	void setCoil(long long cnum);
	void setImgNum(int n);
	void setBoundary(int boundary);
	void requestStop();
	bool isStopped() const { return stopRequested.load(); }
	void setOnExitCallback(std::function<void(int n, long long coilnum)> callback);
private:
	long long coilnum;
	int n;
	int boundary;
	std::function<void(int n, long long coilnum)> onExitCallback;
	std::atomic<bool> stopRequested{ false };
};



