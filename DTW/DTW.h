#ifndef DTW_h
#define DTW_h

#include<math.h>
#include<stdio.h>

#define DTWMAXNUM 5000
#define DTWSEQ1 5
#define DTWSEQ2 6
#define MAXFRAME 1000      //采集数据的最大帧数
#define POINT_NUM_EACH_FRAME 2 //每帧点的个数
#define DTWVERYBIG 100000.0  //表示无穷大
#define MATCHRANGE 10  //匹配范围

#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))

struct point{
	float x;
	float y;
};

float DTWDistanceFun(point * a, int I, point * b, int J, int r);  //通过DTW算法计算最短路径长度
int DTWOptimalPath(point * M, int I, point * T, int J, point * module, float threshold, int turn);   //寻找并显示DTW路径
int DTWUpdataModule(point * new_module, int newModuleFrameNum);  //更新DTW模板中数据

#endif // DTW_h
