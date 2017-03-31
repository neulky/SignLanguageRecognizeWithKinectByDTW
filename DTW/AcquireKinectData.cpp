#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include "DTW.h"

using namespace std;
using namespace cv;

void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage, Mat &fullDepthImage);

CvPoint extrudeRightHandPosition(HANDLE &skeletonEvent);
CvPoint RightHandPosition;


int AcquireKinectData(point * kinectDataPoint)
{

	Mat depthImage;            //��ʾ�ֲ��켣ͼ��
	depthImage.create(240, 320, CV_8UC3);
	Mat fullDepthImage;        //��ʾ�������ͼ��
	fullDepthImage.create(240, 320, CV_8UC3);

	namedWindow("RightHandTrack", CV_WINDOW_AUTOSIZE);
	namedWindow("FullDepthImage", CV_WINDOW_AUTOSIZE);

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

	hr = NuiSkeletonTrackingEnable(skeletonEvent, 0);//�򿪹��������¼�     
	if (hr != S_OK)
	{
		cout << "NuiSkeletonTrackingEnable failed" << endl;
		NuiShutdown();
		return hr;
	}

	int frameCount = 0;
	while (1)
	{
		if (WaitForSingleObject(skeletonEvent, INFINITE) == 0)
			RightHandPosition = extrudeRightHandPosition(skeletonEvent);
		//cout << "(" << RightHandPosition.x << "," << RightHandPosition.y << ")" << endl;

		if (WaitForSingleObject(depthEvent, 0) == 0)
		{
			getDepthImage(depthEvent, depthStreamHandle, depthImage, fullDepthImage);
		}

		imshow("RightHandTrack", depthImage);
		imshow("FullDepthImage", fullDepthImage);

		if (cvWaitKey(1) == 27)      //�˺���Ϊ�ȴ�������������
			break;

		if (RightHandPosition.x == 0 && RightHandPosition.y == 0)   //���ҵ��ֲ����꿪ʼ��¼����
			continue;

		kinectDataPoint[frameCount].x = RightHandPosition.x;
		kinectDataPoint[frameCount].y = RightHandPosition.y;
		frameCount++;

		if (frameCount >= 30)
			break;
	
	}

	NuiShutdown();
	return frameCount;

}

void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage, Mat &fullDepthImage)  //�����ͼ����ʾ�ֵ�λ�ã�ͨ�����֡��������һ���˶��켣
{
	const NUI_IMAGE_FRAME *depthFrame = NULL;

	NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
	INuiFrameTexture *pTexture = depthFrame->pFrameTexture;

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	RGBQUAD q;

	if (LockedRect.Pitch != 0)
	{
		//���÷�Χ
		for (int i = ((0 > RightHandPosition.y - 3) ? 0 : (RightHandPosition.y - 3)); i < ((depthImage.rows < (RightHandPosition.y + 3)) ? depthImage.rows : (RightHandPosition.y + 3)); i++)
		{
			uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			for (int j = ((0 > RightHandPosition.x - 3) ? 0 : (RightHandPosition.x - 3)); j < ((depthImage.cols < (RightHandPosition.x + 3)) ? depthImage.cols : (RightHandPosition.x + 3)); j++)
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
					q.rgbGreen = q.rgbRed * 4;  //�������ó˵ķ�����������ԭ���ķ������Ա��ⲻ���������   
					q.rgbBlue = q.rgbRed * 4;  //�����ں����getTheContour()�����ʹ�ã�������©һЩ���   
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

		for (int i = 0; i<fullDepthImage.rows; i++)
		{
			uchar *ptr = fullDepthImage.ptr<uchar>(i);

			for (int j = 0; j<fullDepthImage.cols; j++)
			{
				ptr[3 * j] = 100;
				ptr[3 * j + 1] = 100;
				ptr[3 * j + 2] = 100;
			}
		}

		for (int i = 0; i < fullDepthImage.rows; i++)       //��ʾ�������ͼ��
		{
			uchar *ptr1 = fullDepthImage.ptr<uchar>(i);
			uchar *pBuffer1 = (uchar*)(LockedRect.pBits) + i*LockedRect.Pitch;
			USHORT *pBufferRun1 = (USHORT *)pBuffer1;

			for (int j = 0; j < fullDepthImage.cols; j++)
			{
				int player = pBufferRun1[j] & 7;
				int data = (pBufferRun1[j] & 0xfff8) >> 3;

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
					q.rgbGreen = q.rgbRed * 4;  //�������ó˵ķ�����������ԭ���ķ������Ա��ⲻ���������   
					q.rgbBlue = q.rgbRed * 4;  //�����ں����getTheContour()�����ʹ�ã�������©һЩ���   
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
				ptr1[3 * j] = q.rgbBlue;
				ptr1[3 * j + 1] = q.rgbGreen;
				ptr1[3 * j + 2] = q.rgbRed;
			}
		}


	}
	else
	{
		cout << "��׽���ͼ����ִ���" << endl;
	}

	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);
}

CvPoint extrudeRightHandPosition(HANDLE &skeletonEvent)  //���������ڶ�ά���ͼ���е�λ��
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
		cout << "û���ҵ����ʵĹ���֡" << endl;
		//return;
	}

	if (!bFoundSkeleton)
	{
		//return;
	}

	NuiTransformSmooth(&skeletonFrame, NULL);//ƽ������֡,��������     

	CvPoint rightHandPoint[NUI_SKELETON_COUNT] = { cvPoint(0, 0) };
	int flag = 0;
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			float Right_Hand_fx, Right_Hand_fy;
			float Hip_Center_fx, Hip_Center_fy;

			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], &Right_Hand_fx, &Right_Hand_fy);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_RIGHT], &Hip_Center_fx, &Hip_Center_fy);
			
			if ((int)Right_Hand_fy <= (int)Hip_Center_fy)    //���������֡����ȡ  		                                         
			{                                                 //���ֲ�λ�õ�һ�γ����Źؽ�ʱ��ʼ��ȡ֡�����궯�������ֲ������Źؽ�ʱ����֡��ȡ
				rightHandPoint[i].x = (int)Right_Hand_fx;
				rightHandPoint[i].y = (int)Right_Hand_fy;
			}
			else                                               //���赱�ֲ�λ�õ����Źؽ�ʱ�ֵ�����Ϊ��0��0��
			{
				rightHandPoint[i].x = 0;
				rightHandPoint[i].y = 0;
				cout << "+++" << endl;
			}
			cout << "(" << rightHandPoint[i].x << "," << rightHandPoint[i].y << ")   ||";
			cout << "(" << (int)Hip_Center_fx << "," << (int)Hip_Center_fy << ")" << endl;

			flag = i;                        //�˳���ֻ���ظ��ٵ����һ���˵��ֵ�λ�� 
		}
	}

	return rightHandPoint[flag];
}