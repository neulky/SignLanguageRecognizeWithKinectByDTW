#include<iostream>
#include<string>
#include"GestureDetector.h"
#include"DTW.h"
#include"AcquireKinectData.h"
using namespace std;
int main()
{
	string recogizeResult;
	//point B[DTWSEQ2] = { { 0, 0 }, { 0, 1 }, { 1, 2 }, { 2, 2.9 }, { 2, 3.1 }, { 3, 4.1 } };
	point kinectDataPointSeq[MAXFRAME] = {};
	int kinectFrameCount;

	kinectFrameCount = AcquireKinectData(kinectDataPointSeq);    //通过Kinect采集数据存入数组中
	DTWUpdataModule(kinectDataPointSeq, kinectFrameCount);     //第一次将采到数据存入模板文件当中

	recogizeResult = recognize(kinectDataPointSeq, kinectFrameCount);
	cout << recogizeResult << endl;

	return 0;
}