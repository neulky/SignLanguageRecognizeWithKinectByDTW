#ifndef GESTUREDETECTOR_H
#define GESTUREDETECTOR_H

#include"DTW.h"
#include<string>
using namespace std;

string recognize(point source_data[][MAXFRAME], int* sourceFrameNum);

#endif // !GESTUREDETECTOR_H

