#include "pch.h"
#include "EdgeSin.h"
#include <corecrt_io.h>


#include <numeric>

EdgeSin::EdgeSin()
{
	this->edgeCa = nullptr;
}

EdgeSin::~EdgeSin()
{
	delete this->edgeCa;    // ← 在这里补上
	this->edgeCa = nullptr; // ←
}

std::vector<std::vector<int>> EdgeSin::getBboxs(int num)
{
	std::vector<std::vector<int>> bboxs;

	bboxs.push_back({ 6000 });

	int edgeValue=0;
	cv::Mat imga = this->img;


	if (this->edgeCa == nullptr)
	{
		this->edgeCa = new EdgeCa(this->img.rows, 4, 3);
	}

	int starty1 = this->img.rows / 3;
	int edgeNum1 = this->edgeCa->edgeTodo(imga, true, starty1);
	int edgeNum2 = this->edgeCa->edgeTodo(imga, true, starty1 * 2);

	edgeValue = edgeNum1 > edgeNum2 ? edgeNum1 : edgeNum2;


	int bb = this->du == 1 ? (this->rl == 0 ? 10 : 10) : (this->rl == 0 ? 10 : 10);

	int xbz = edgeValue + 10;

	bboxs.push_back({ edgeValue });


	bboxs.push_back({ 0,0,0,0,0,num,edgeValue,-1 });

	return bboxs;
}
