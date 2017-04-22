#include<iostream>
#include<string>
#include"GestureDetector.h"
#include"DTW.h"
#include"AcquireKinectData.h"
#include"Speech.h"
#include"Client.h"
using namespace std;
int main()
{
	point kinectDataPointSeq[HAND_COUNT][MAXFRAME] = {};
	int kinectFrameCount[HAND_COUNT];

	AcquireKinectData(kinectDataPointSeq, kinectFrameCount);    //通过Kinect采集数据存入数组中
	
	Client* data_client = new Client();
	data_client->Connect();
	data_client->sendMessage(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG]);
	data_client->Close();

	//string temp = "_0.txt";

    //DTWUpdataModule(kinectDataPointSeq[LEFT_HAND_FLAG], kinectFrameCount[LEFT_HAND_FLAG], LEFT_HAND_FLAG, temp);     //第一次将采到数据存入模板文件当中
	//DTWUpdataModule(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG], RIGHT_HAND_FLAG, temp);     //第一次将采到数据存入模板文件当中

	//string result;
	//result = recognize(kinectDataPointSeq, kinectFrameCount);   //确认左手
	//
	//string speech = result.substr(0, 1);
	//ResultSpeech(speech);

	return 0;
}