#include<string>
#include"DTW.h"
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<iomanip>

using namespace std;

int  dtw_path[DTWMAXNUM][DTWMAXNUM];
float  dtw_distance[DTWMAXNUM][DTWMAXNUM];
float dtw_middle_matrix[DTWMAXNUM][DTWMAXNUM];

float DTWDistanceFun(point * a, int I, point * b, int J,int r)   //通过DTW算法计算最短路径
{                                                                //r为匹配窗口大小  a为模板数据点指针 b为待测数据点指针
	memset(dtw_distance, 0, sizeof(dtw_distance));
	memset(dtw_middle_matrix, DTWVERYBIG, sizeof(dtw_middle_matrix));

	for (int i = 0; i < I; i++)
	{
		for (int j = 0; j < J; j++)
		{
			dtw_distance[i][j] = sqrt((a[i].x - b[j].x)*(a[i].x - b[j].x) + (a[i].y - b[j].y)*(a[i].y - b[j].y));
			
		}
	}

	/*for (int i = I - 1; i >= 0; i--)
	{
		for (int j = 0; j < J; j++)
		{
			cout <<setw(5)<<setprecision(3)<< dtw_distance[i][j];   //输出点之间的距离矩阵
		}
		cout << endl;
	}*/

	int r2 = r + ABS(I - J);      //匹配距离   注意r2的值 不能越界 必须小于I J

	dtw_middle_matrix[0][0] = dtw_distance[0][0];

	for (int i = 1; i < r2; i++)
	{
		dtw_middle_matrix[i][0] = dtw_middle_matrix[i - 1][0] + dtw_distance[i][0];
	}
	for (int j = 1; j < r2; j++)
	{
		dtw_middle_matrix[0][j] = dtw_middle_matrix[0][j - 1] + dtw_distance[0][j];
	}

	float d1, d2, d3;
	int istart, imax;
	for (int j = 1; j < J; j++)
	{
		istart = j - r2;
		if (j <= r2)
			istart = 1;
		imax = j + r2;
		if (imax >= I)
			imax = I - 1;

		for (int i = istart; i <= imax; i++)
		{
			d1 = dtw_middle_matrix[i - 1][j - 1] + 2 * dtw_distance[i][j];
			d2 = dtw_middle_matrix[i][j - 1] + dtw_distance[i][j];
			d3 = dtw_middle_matrix[i - 1][j] + dtw_distance[i][j];
			dtw_middle_matrix[i][j] = MIN(MIN(d1, d2), d3);
		}
	}
	
	/*for (int i = I - 1; i >= 0; i--)
	{
		for (int j = 0; j < J; j++)
		{
			cout << setw(5) << setprecision(3) << dtw_middle_matrix[i][j];   //输出点之间的距离矩阵
		}
		cout << endl;
	}*/

	return dtw_middle_matrix[I-1][J-1];
}

int DTWOptimalPath(point * M,int I,point * T,int J ,point * module,float threshold,int turn) //寻找并显示最短路径 生成最新模板
{
	float DTWDistanceFun(point * a, int I, point * b, int J,int r);
	int DTWUpdataModule(point * new_module, int newModuleFrameNum);

	float dist;
	int i, j;
	int pathsig = 1;
	dist = DTWDistanceFun(M, I, T, J,MATCHRANGE);
	if (dist>threshold){
		printf("\nSample doesn't match!\n");
		return -1;
	}
	memset(dtw_path, 0, sizeof(dtw_path));

	
	/*寻找路径,这里我采用了逆向搜索法*/
	i = I - 1;
	j = J - 1;
	while (j >= 1 || i >= 1){
		float m;
		if (i>0 && j>0){
			m = MIN(MIN(dtw_middle_matrix[i - 1][j], (dtw_middle_matrix[i - 1][j - 1] + dtw_distance[i][j])), dtw_middle_matrix[i][j - 1]);
			if (m == (dtw_middle_matrix[i - 1][j - 1] + dtw_distance[i][j])){
				dtw_path[i - 1][j - 1] = pathsig;
				i--;
			    j--;
			}
			else if (m == dtw_middle_matrix[i - 1][j]){
				dtw_path[i - 1][j] = pathsig;
				i--;
			}
			else{
				dtw_path[i][j - 1] = pathsig;
				j--;
			}
		}
		else if (i == 0){
			dtw_path[0][j - 1] = pathsig;
			j--;
		}
		else{/*j==0*/
			dtw_path[i - 1][0] = pathsig;
			i--;
		}
	}
	dtw_path[0][0] = pathsig;
	dtw_path[I - 1][J - 1] = pathsig;

	/*建立模板*/
	for (i = 0; i<I; i++){
		point temp = {0,0};
		int ntemp = 0;
		for (j = 0; j<J; j++){
			if (dtw_path[i][j] == pathsig){
				temp.x += T[j].x;
				temp.y += T[j].y;
				ntemp++;
			}
		}
		temp.x /= (ntemp);
		temp.y /= (ntemp);
		module[i].x = (M[i].x * turn + temp.x) / (turn + 1);/*注意这里的权值*/
		module[i].y = (M[i].y * turn + temp.y) / (turn + 1);
		//cout << "(" << module[i].x << "," << module[i].y << ")" << endl;   输出更新后模板的数据
	}
	DTWUpdataModule(module, I);
	for (int i = I-1; i >= 0; i--)
	{
		for (int j = 0; j < J; j++)
		{
			cout << dtw_path[i][j] << " ";
		}
		cout << endl;
	}
		
	return I;
}

//将建立的模板写入文档
int DTWUpdataModule(point * new_module, int newModuleFrameNum)
{
	string sourceFile = "G://DTW_Project//DTW//DTW_Module//wave.txt";
	ofstream outfile;
	outfile.open(sourceFile);

	//char moduleData[10000] = {};
	string moduleDataStr;
	//int count = 0;
	for (int i = 0; i < newModuleFrameNum; i++)
	{
		for (int j = 0; j < POINT_NUM_EACH_FRAME; j++)
		{
			switch (j)
			{
			case 0: moduleDataStr += to_string(new_module[i].x); moduleDataStr += ','; break;
			case 1: moduleDataStr += to_string(new_module[i].y); break;
			default: 
				break;
			}
		}
		moduleDataStr += '@';
	}

	//cout << moduleDataStr << endl;

	outfile << moduleDataStr;

	outfile.close();

	return 1;
}
