#pragma once
#include <memory>
#include "Wsoc.h"
#include <Target.h>
#include <AccLibary.h>
#include <Crack.h>
#include <DbToCode.h>
#include <CameraApt.h>
#include <imgOp.h>

class Operation
{
public:
    Operation(DbToCode &dt,const SorcePar& spar);
    ~Operation();
    void star(bool upCoilum,int arc, char* argv[]);
    void disOutImg(int result, Mat data) const;
    void runDetection(int n) const;
    void runCom(int n) const;
    void runinfer() const;

private:
    std::unique_ptr<AccLibary> pool;
    std::unique_ptr<Wsoc> wsoc;
    std::vector<std::unique_ptr<ImgCal>> ccklist;
    std::unique_ptr<CameraApt> cameraApt;
    DbToCode* dtcode;
    std::unique_ptr<Target> target;
    long long coild;
    SorcePar spar;
    float edgeInt;
	int startNum;
};

