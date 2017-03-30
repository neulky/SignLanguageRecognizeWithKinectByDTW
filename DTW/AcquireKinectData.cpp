#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include "DTW.h"

using namespace std;
using namespace cv;

void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage);
CvPoint extrudeRightHandPosition(HANDLE &skeletonEvent);
CvPoint rightHandPosition;


int AcquireKinectData(point * kinectDataPoint)
{

	Mat depthImage;
	depthImage.create(240, 320, CV_8UC3);

	HANDLE depthEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE skeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HANDLE depthStreamHandle = NULL;

	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (hr != S_OK)
	{
		cout << "NuiInitialize failed" << endl;
		return hr;
	}

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, NULL, 2, depthEvent, &depthStreamHandle);
	if (hr != S_OK)
	{
		cout << "Open the depth Stream failed" << endl;
		NuiShutdown();
		return hr;
	}

	hr = NuiSkeletonTrackingEnable(skeletonEvent, 0);//打开骨骼跟踪事件     
	if (hr != S_OK)
	{
		cout << "NuiSkeletonTrackingEnable failed" << endl;
		NuiShutdown();
		return hr;
	}

	namedWindow("RightHandTrack", CV_WINDOW_AUTOSIZE);

	int frameCount = 0;
	while (1)
	{
		if (WaitForSingleObject(skeletonEvent, INFINITE) == 0)
			rightHandPosition = extrudeRightHandPosition(skeletonEvent);
		cout << "(" << rightHandPosition.x << "," << rightHandPosition.y << ")" << endl;

		if (WaitForSingleObject(depthEvent, 0) == 0)
			getDepthImage(depthEvent, depthStreamHandle, depthImage);

		imshow("RightHandTrack", depthImage);

		if (cvWaitKey(1) == 27)      //次函数为等待函数，待考查
			break;

		if (rightHandPosition.x == 0 && rightHandPosition.y == 0)   //从找到手部坐标开始记录数据
			continue;

		kinectDataPoint[frameCount].x = rightHandPosition.x;
		kinectDataPoint[frameCount].y = rightHandPosition.y;
		frameCount++;

		if (frameCount >= 10)
			break;
	}

	NuiShutdown();
	return frameCount;

}

void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage)  //在深度图中显示手的位置，通过多个帧最终连成一个运动轨迹
{
	const NUI_IMAGE_FRAME *depthFrame = NULL;

	NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
	INuiFrameTexture *pTexture = depthFrame->pFrameTexture;

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	RGBQUAD q;

	if (LockedRect.Pitch != 0)
	{

		for (int i = ((0 > rightHandPosition.y - 3) ? 0 : (rightHandPosition.y - 3)); i < ((depthImage.rows < (rightHandPosition.y + 3)) ? depthImage.rows : (rightHandPosition.y + 3)); i++)
		{
			uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			for (int j = ((0 > rightHandPosition.x - 3) ? 0 : (rightHandPosition.x - 3)); j < ((depthImage.cols < (rightHandPosition.x + 3)) ? depthImage.cols : (rightHandPosition.x + 3)); j++)
			{
				int player = pBufferRun[j] & 7;
				int data = (pBufferRun[j] & 0xfff8) >> 3;

				uchar imageData = 255 - (uchar)(256 * data / 0x0fff);
				q.rgbBlue = q.rgbGreen = q.rgbRed = 0;


				switch (player)

				{
				case 0:
					q.rgbRed = 100;
					q.rgbBlue = 100;
					q.rgbGreen = 100;
					break;
				case 1:
					q.rgbRed = imageData;
					break;
				case 2:
					q.rgbGreen = imageData;
					break;
				case 3:
					q.rgbRed = imageData / 4;
					q.rgbGreen = q.rgbRed * 4;  //这里利用乘的方法，而不用原来的方法可以避免不整除的情况   
					q.rgbBlue = q.rgbRed * 4;  //可以在后面的getTheContour()中配合使用，避免遗漏一些情况   
					break;
				case 4:
					q.rgbBlue = imageData / 4;
					q.rgbRed = q.rgbBlue * 4;
					q.rgbGreen = q.rgbBlue * 4;
					break;
				case 5:
					q.rgbGreen = imageData / 4;
					q.rgbRed = q.rgbGreen * 4;
					q.rgbBlue = q.rgbGreen * 4;
					break;
				case 6:
					q.rgbRed = imageData / 2;
					q.rgbGreen = imageData / 2;
					q.rgbBlue = q.rgbGreen * 2;
					break;
				case 7:
					q.rgbRed = 100;
					q.rgbGreen = 100;
					q.rgbBlue = 100;
				}
				ptr[3 * j] = q.rgbBlue;
				ptr[3 * j + 1] = q.rgbGreen;
				ptr[3 * j + 2] = q.rgbRed;
			}
		}
		/*for (int i = 0; i < depthImage.rows; i++)
		{
		uchar *ptr = depthImage.ptr<uchar>(i);

		if (i < (leftHandPosition.y - 20) || i > (leftHandPosition.y + 20))
		{
		for (int j = 0; j < depthImage.cols; j++)
		{
		ptr[3 * j] = 255;
		ptr[3 * j + 1] = 255;
		ptr[3 * j + 2] = 255;
		}
		}
		else
		{
		for (int j = 0; j < depthImage.cols; j++)
		{
		if (j < (leftHandPosition.x - 20) || j > (leftHandPosition.x + 20))
		{
		ptr[3 * j] = 255;
		ptr[3 * j + 1] = 255;
		ptr[3 * j + 2] = 255;
		}
		}
		}
		}*/
	}
	else
	{
		cout << "捕捉深度图像出现错误" << endl;
	}

	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);
}

CvPoint extrudeRightHandPosition(HANDLE &skeletonEvent)  //返回右手在二维深度图像中的位置
{
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	bool bFoundSkeleton = false;

	if (NuiSkeletonGetNextFrame(0, &skeletonFrame) == S_OK)
	{
		for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		{
			if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
			{
				bFoundSkeleton = true;
				break;
			}
		}
	}
	else
	{
		cout << "没有找到合适的骨骼帧" << endl;
		//return;
	}

	if (!bFoundSkeleton)
	{
		//return;
	}

	NuiTransformSmooth(&skeletonFrame, NULL);//平滑骨骼帧,消除抖动     

	CvPoint rightHandPoint[NUI_SKELETON_COUNT] = { cvPoint(0, 0) };
	int flag = 0;
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			float fx, fy;

			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], &fx, &fy);
			rightHandPoint[i].x = (int)fx;
			rightHandPoint[i].y = (int)fy;
			//cout << i << " = (" << fx << "," << fy << ")" << endl;
			flag = i;
		}
	}

	return rightHandPoint[flag];
}