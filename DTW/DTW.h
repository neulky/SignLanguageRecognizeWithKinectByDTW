#ifndef DTW_h
#define DTW_h

#include<math.h>
#include<stdio.h>
#include<string>
using namespace std;

#define DTWMAXNUM 5000
#define DTWSEQ1 5
#define DTWSEQ2 6
#define MAXFRAME 1000      //�ɼ����ݵ����֡��
#define POINT_NUM_EACH_FRAME 3 //ÿ֡��ĸ���
#define DTWVERYBIG 100000.0  //��ʾ�����
#define MATCHRANGE 3  //ƥ�䷶Χ
#define LEFT_HAND_FLAG 0
#define RIGHT_HAND_FLAG 1
#define HAND_COUNT 2
#define MAX_MODULE_NUM 100  //���ģ�͸���
#define MATCHTHRESHOLD 10.0

#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))

struct point{
	float x;
	float y;
	float z;
};

float DTWDistanceFun(point * a, int I, point * b, int J, int r);  //ͨ��DTW�㷨�������·������
int DTWOptimalPath(point * M, int I, point * T, int J, float threshold, int turn, int hand_flag, string fileName);   //Ѱ�Ҳ���ʾDTW·��
int DTWUpdataModule(point * new_module, int newModuleFrameNum, int hand_flag, string fileName);  //����DTWģ��������

#endif // DTW_h
