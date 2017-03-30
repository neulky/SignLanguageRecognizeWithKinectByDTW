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

	kinectFrameCount = AcquireKinectData(kinectDataPointSeq);    //ͨ��Kinect�ɼ����ݴ���������
	DTWUpdataModule(kinectDataPointSeq, kinectFrameCount);     //��һ�ν��ɵ����ݴ���ģ���ļ�����

	recogizeResult = recognize(kinectDataPointSeq, kinectFrameCount);
	cout << recogizeResult << endl;

	return 0;
}