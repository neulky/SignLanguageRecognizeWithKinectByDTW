#ifndef DTW_h
#define DTW_h

#include<math.h>
#include<stdio.h>

#define DTWMAXNUM 5000
#define DTWSEQ1 5
#define DTWSEQ2 6
#define MAXFRAME 1000      //�ɼ����ݵ����֡��
#define POINT_NUM_EACH_FRAME 2 //ÿ֡��ĸ���
#define DTWVERYBIG 100000.0  //��ʾ�����
#define MATCHRANGE 10  //ƥ�䷶Χ

#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))

struct point{
	float x;
	float y;
};

float DTWDistanceFun(point * a, int I, point * b, int J, int r);  //ͨ��DTW�㷨�������·������
int DTWOptimalPath(point * M, int I, point * T, int J, point * module, float threshold, int turn);   //Ѱ�Ҳ���ʾDTW·��
int DTWUpdataModule(point * new_module, int newModuleFrameNum);  //����DTWģ��������

#endif // DTW_h
