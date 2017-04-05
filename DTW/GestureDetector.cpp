#include<iostream>
#include<fstream>
#include<string>
#include"DTW.h"
#include"GestureDetector.h"
#include<io.h>
using namespace std;

string recognize(point source_data[][MAXFRAME], int* sourceFrameNum)
{
	int acquireModuleData(point* module_point, int hand_flag, string moduleFileName);
	string module_file_name[MAX_MODULE_NUM];          //模板文件的文件名，及手语名称
	int moduleFileCount = 0;
	long Handle;
	struct _finddata_t FileInfo;
	/* 该段为依次读取DTW_Left_Module目录下的所有txt文件*/
	if ((Handle = _findfirst("G:\\GitHubKinect\\SignLanguageRecognizeWithDTW\\DTW_Left_Module\\*.txt", &FileInfo)) == -1L)
		printf("没有找到匹配的项目!\n");
	else{
		module_file_name[moduleFileCount] = FileInfo.name;
		moduleFileCount++;
		while (_findnext(Handle, &FileInfo) == 0)
		{
			module_file_name[moduleFileCount] = FileInfo.name;
			moduleFileCount++;
		}
	}
	_findclose(Handle);

	float totalDistance[MAX_MODULE_NUM];
	float minDistance = DTWVERYBIG;    //记录最小距离
	string successModuleName;   //匹配成功的模板名称

	for (int i = 0; i < moduleFileCount; i++)
	{
		point left_hand_module_point[MAXFRAME] = {};
		point right_hand_module_point[MAXFRAME] = {};

		int leftHandFrameNum = acquireModuleData(left_hand_module_point, LEFT_HAND_FLAG, module_file_name[i]);
		int rightHandFrameNum = acquireModuleData(right_hand_module_point, RIGHT_HAND_FLAG, module_file_name[i]);

		float leftDistance = 0, rightDistance = 0;

		if (sourceFrameNum[LEFT_HAND_FLAG] == 0)
			cout << "左手未参加该手语" << endl;
		else
		{
			leftDistance = DTWDistanceFun(left_hand_module_point, leftHandFrameNum, source_data[LEFT_HAND_FLAG], sourceFrameNum[LEFT_HAND_FLAG], MATCHRANGE);  //MATCHRANGE 为匹配距离
		}

		if (sourceFrameNum[RIGHT_HAND_FLAG] == 0)
			cout << "右手未参加该手语" << endl;
		else
		{
			rightDistance = DTWDistanceFun(right_hand_module_point, rightHandFrameNum, source_data[RIGHT_HAND_FLAG], sourceFrameNum[RIGHT_HAND_FLAG], MATCHRANGE);
		}

		totalDistance[i] = leftDistance + rightDistance;
		cout << module_file_name[i] << ": " << totalDistance[i] << endl;
		if (minDistance >= totalDistance[i])
		{
			minDistance = totalDistance[i];
			successModuleName = module_file_name[i];
		}
	}
	/*该模块为训练模板*/
	if (minDistance < MATCHTHRESHOLD) 
	{
		point left_hand_module_point[MAXFRAME] = {};
		point right_hand_module_point[MAXFRAME] = {};

		int leftHandFrameNum = acquireModuleData(left_hand_module_point, LEFT_HAND_FLAG, successModuleName);
		int rightHandFrameNum = acquireModuleData(right_hand_module_point, RIGHT_HAND_FLAG, successModuleName);

		if (sourceFrameNum[LEFT_HAND_FLAG] == 0)
			cout << "左手未参加该手语" << endl;
		else
		{
			DTWOptimalPath(left_hand_module_point, leftHandFrameNum, source_data[LEFT_HAND_FLAG], sourceFrameNum[LEFT_HAND_FLAG], MATCHTHRESHOLD, 1000, LEFT_HAND_FLAG, successModuleName);
		}

		if (sourceFrameNum[RIGHT_HAND_FLAG] == 0)
			cout << "右手未参加该手语" << endl;
		else
		{
			DTWOptimalPath(right_hand_module_point, rightHandFrameNum, source_data[RIGHT_HAND_FLAG], sourceFrameNum[RIGHT_HAND_FLAG], MATCHTHRESHOLD, 1000, RIGHT_HAND_FLAG, successModuleName);
		}
	}

	cout << "最小匹配距离为：" << minDistance << endl;
	cout << "识别的手势为：" << successModuleName << endl;
	return successModuleName;
}

int acquireModuleData(point* module_point, int hand_flag, string moduleFileName)
{
	string sourceDirctory;
	ifstream infile;

	char s[1000][20][20] = {};
	char chr;
	int i = 0, frameNum = 0;  //frameNum为标记i的最终值 及记录模板帧的个数
	int j = 0;
	int k = 0;

	if (hand_flag == LEFT_HAND_FLAG)
		sourceDirctory = "G:\\GitHubKinect\\SignLanguageRecognizeWithDTW\\DTW_Left_Module\\" + moduleFileName;
	else{
		if (hand_flag == RIGHT_HAND_FLAG)
			sourceDirctory = "G:\\GitHubKinect\\SignLanguageRecognizeWithDTW\\DTW_Right_Module\\" + moduleFileName;
		else
			return 0;
	}
		
	infile.open(sourceDirctory);
	while (!(infile.peek() == EOF))
	{
		infile.get(chr);
		//cout << chr << " ";
		if (chr == '@'){
			i++;
			j = 0;
			k = 0;
			frameNum++;
		}
		else
		{
			if (chr == ','){
				j++;
				k = 0;
			}
			else
			{
				s[i][j][k++] = chr;
			}
		}
	}
	for (int i = 0; i < frameNum; i++)
	{
		for (int j = 0; j < POINT_NUM_EACH_FRAME; j++)
		{
			switch (j)
			{
			case 0: module_point[i].x = (float)atof(s[i][j]); break;
			case 1: module_point[i].y = (float)atof(s[i][j]); break;
			default:
				break;
			}
		}
	}
	/*for (int i = 0; i < frameNum; i++)       //测试输出模板数据
	{
		cout << "(" << module_point[i].x << "," << module_point[i].y << ")" << endl;
	}*/

	infile.close();

	return frameNum;  //返回数据帧的个数 ，及返回提取到的坐标点的个数
}

