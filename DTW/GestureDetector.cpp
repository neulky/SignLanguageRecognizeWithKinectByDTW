#include<iostream>
#include<fstream>
#include<string>
#include"DTW.h"
#include"GestureDetector.h"
using namespace std;

point module_point[MAXFRAME] = {};
point DTW_Module[MAXFRAME] = {};

string recognize(point* source_data,int sourceFrameNum)
{

	int acquireModuleData();
	int frameNum = acquireModuleData(); 
	float distance;
	
	/*for (int i = 0; i < sourceFrameNum; i++)    //�������Դ����
	{
		cout << "(" << source_data[i].x << "," << source_data[i].y << ")" << endl;
	}*/

	distance = DTWDistanceFun(module_point, frameNum, source_data, sourceFrameNum);
	cout << distance << endl;
	DTWOptimalPath(module_point, frameNum, source_data, sourceFrameNum, DTW_Module, 10000.0, 1);

	return "����";
	
}

int acquireModuleData()
{
	string sourceDirctory = "G://DTW_Project//DTW//DTW_Module//wave.txt";
	ifstream infile;

	char s[1000][20][20] = {};
	char chr;
	int i = 0, frameNum = 0;  //frameNumΪ���i������ֵ ����¼ģ��֡�ĸ���
	int j = 0;
	int k = 0;
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

