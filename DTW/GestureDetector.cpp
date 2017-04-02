#include<iostream>
#include<fstream>
#include<string>
#include"DTW.h"
#include"GestureDetector.h"
using namespace std;

string recognize(point source_data[][MAXFRAME], int* sourceFrameNum)
{
	point left_hand_module_point[MAXFRAME] = {};
	point right_hand_module_point[MAXFRAME] = {};

	int acquireModuleData(point* module_point, int hand_flag);
	int leftHandFrameNum = acquireModuleData(left_hand_module_point,LEFT_HAND_FLAG); 
	int rightHandFrameNum = acquireModuleData(right_hand_module_point, RIGHT_HAND_FLAG);

	float leftDistance, rightDistance;
	float totalDistance;
	
	/*for (int i = 0; i < sourceFrameNum; i++)    //�������Դ����
	{
		cout << "(" << source_data[i].x << "," << source_data[i].y << ")" << endl;
	}*/
	if (sourceFrameNum[LEFT_HAND_FLAG] == 0)
		cout << "����δ�μӸ�����" << endl;
	else
	{
		leftDistance = DTWDistanceFun(left_hand_module_point, leftHandFrameNum, source_data[LEFT_HAND_FLAG], sourceFrameNum[LEFT_HAND_FLAG], MATCHRANGE);  //MATCHRANGE Ϊƥ�����
		DTWOptimalPath(left_hand_module_point, leftHandFrameNum, source_data[LEFT_HAND_FLAG], sourceFrameNum[LEFT_HAND_FLAG], 10000.0, 1, LEFT_HAND_FLAG);
	}
	
	if (sourceFrameNum[RIGHT_HAND_FLAG] == 0)
		cout << "����δ�μӸ�����" << endl;
	else
	{
		rightDistance = DTWDistanceFun(right_hand_module_point, rightHandFrameNum, source_data[RIGHT_HAND_FLAG], sourceFrameNum[RIGHT_HAND_FLAG], MATCHRANGE);
		DTWOptimalPath(right_hand_module_point, rightHandFrameNum, source_data[RIGHT_HAND_FLAG], sourceFrameNum[RIGHT_HAND_FLAG], 10000.0, 1, LEFT_HAND_FLAG);
	}
	
	totalDistance = leftDistance + rightDistance;
	cout << "���·������Ϊ��" << totalDistance << endl;

	return "����";
	
}

int acquireModuleData(point* module_point, int hand_flag)
{
	string sourceDirctory;
	ifstream infile;

	char s[1000][20][20] = {};
	char chr;
	int i = 0, frameNum = 0;  //frameNumΪ���i������ֵ ����¼ģ��֡�ĸ���
	int j = 0;
	int k = 0;

	if (hand_flag == LEFT_HAND_FLAG)
		sourceDirctory = "G://GitHubKinect//SignLanguageRecognizeWithDTW//DTW_Left_Module//wave.txt";
	else{
		if (hand_flag == RIGHT_HAND_FLAG)
			sourceDirctory = "G://GitHubKinect//SignLanguageRecognizeWithDTW//DTW_Right_Module//wave.txt";
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
	/*for (int i = 0; i < frameNum; i++)       //�������ģ������
	{
		cout << "(" << module_point[i].x << "," << module_point[i].y << ")" << endl;
	}*/

	infile.close();

	return frameNum;  //��������֡�ĸ��� ����������ȡ���������ĸ���
}

