#include<iostream>
#include<string>
#include"GestureDetector.h"
#include"DTW.h"
#include"AcquireKinectData.h"
using namespace std;
int main()
{
	
	//point B[DTWSEQ2] = { { 0, 0 }, { 0, 1 }, { 1, 2 }, { 2, 2.9 }, { 2, 3.1 }, { 3, 4.1 } };
	point kinectDataPointSeq[HAND_COUNT][MAXFRAME] = {};
	int kinectFrameCount[HAND_COUNT];

	AcquireKinectData(kinectDataPointSeq, kinectFrameCount);    //ͨ��Kinect�ɼ����ݴ���������
	
	string temp = "0.txt";
    //DTWUpdataModule(kinectDataPointSeq[LEFT_HAND_FLAG], kinectFrameCount[LEFT_HAND_FLAG], LEFT_HAND_FLAG, temp);     //��һ�ν��ɵ����ݴ���ģ���ļ�����
	//DTWUpdataModule(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG], RIGHT_HAND_FLAG, temp);     //��һ�ν��ɵ����ݴ���ģ���ļ�����

	recognize(kinectDataPointSeq, kinectFrameCount);   //ȷ������


	return 0;
}