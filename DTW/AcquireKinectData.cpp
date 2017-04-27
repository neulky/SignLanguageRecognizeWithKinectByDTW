#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include "DTW.h"

using namespace std;
using namespace cv;

void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage, Mat &fullDepthImage, point * HandPoint);
int extrudeRightHandPosition(HANDLE &skeletonEvent, point * HandPoint);
point HandPosition1[HAND_COUNT] = {};           //������ʾ�ֲ�λ��

/********************************************************************
*�������ܣ�ͨ��Kinect�ɼ������ֵ�λ��
*handCount �ֵ�������ĿǰΪ2�����ֺ�����
*kinectDataPoint[][] ��ά���飬��¼�����ֵ�λ��
*********************************************************************/
int AcquireKinectData(point kinectDataPoint[][MAXFRAME], int* kinectFrameCount) 
{

	Mat depthImage;            //��ʾ�ֲ��켣ͼ��
	depthImage.create(240, 320, CV_8UC3);
	Mat fullDepthImage;        //��ʾ�������ͼ��
	fullDepthImage.create(240, 320, CV_8UC3);

	kinectFrameCount[LEFT_HAND_FLAG] = 0;
	kinectFrameCount[RIGHT_HAND_FLAG] = 0;

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

	int leftStartFlag = 0;   //��ʼ��־  ����ʼ��¼֡ʱ��startFlag��Ϊ1�����ٴγ����ֲ�����Ϊ��0��0����startFlag=1ʱ��������¼
	int rightStartFlag = 0;
	point  HandPosition[HAND_COUNT] = {};

	while (1)
	{
		if (WaitForSingleObject(skeletonEvent, INFINITE) == 0)
			extrudeRightHandPosition(skeletonEvent, HandPosition);      //�޸�����HandPosition,���޸�����������

		if (WaitForSingleObject(depthEvent, 0) == 0)
		{
			getDepthImage(depthEvent, depthStreamHandle, depthImage, fullDepthImage, HandPosition1);   //��ʾ����������
		}

		imshow("RightHandTrack", depthImage);
		imshow("FullDepthImage", fullDepthImage);

		if (cvWaitKey(1) == 27)      //�˺���Ϊ�ȴ�������������
			break;

		if (HandPosition[RIGHT_HAND_FLAG].x == 0 && HandPosition[RIGHT_HAND_FLAG].y == 0 &&
			HandPosition[LEFT_HAND_FLAG].x == 0 && HandPosition[LEFT_HAND_FLAG].y == 0 && (rightStartFlag == 1 || leftStartFlag ==1))    //��ֻ��ͬʱ�������棬��������һֻ����ȥ��
			break;

		if (HandPosition[RIGHT_HAND_FLAG].x == 0 && HandPosition[RIGHT_HAND_FLAG].y == 0 && rightStartFlag == 0)    //������Ϊ��׼ �����ֲ�λ�õ�һ�γ����Źؽ�ʱ��ʼ��ȡ֡�����궯�������ֲ������Źؽ�ʱ����֡��ȡ
		{

		}
		else{
			rightStartFlag = 1;
			if (HandPosition[RIGHT_HAND_FLAG].x != 0 && HandPosition[RIGHT_HAND_FLAG].y != 0)
			{
				kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].x = HandPosition[RIGHT_HAND_FLAG].x;
				kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].y = HandPosition[RIGHT_HAND_FLAG].y;
				kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].z = HandPosition[RIGHT_HAND_FLAG].z;
				cout <<"Right: "<< "(" << kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].x << "," << kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].y << ","
					<< kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].z << ")" << endl;
				kinectFrameCount[RIGHT_HAND_FLAG]++;
			}
		}
		if (HandPosition[LEFT_HAND_FLAG].x == 0 && HandPosition[LEFT_HAND_FLAG].y == 0 && leftStartFlag == 0)    //������Ϊ��׼ �����ֲ�λ�õ�һ�γ����Źؽ�ʱ��ʼ��ȡ֡�����궯�������ֲ������Źؽ�ʱ����֡��ȡ
		{

		}
		else{
			leftStartFlag = 1;
			if (HandPosition[LEFT_HAND_FLAG].x != 0 && HandPosition[LEFT_HAND_FLAG].y != 0)
			{
				kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].x = HandPosition[LEFT_HAND_FLAG].x;
				kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].y = HandPosition[LEFT_HAND_FLAG].y;
				kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].z = HandPosition[LEFT_HAND_FLAG].z;
				cout << "Left: " << "(" << kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].x << "," << kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].y << ","
					 << kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].z << ")" << endl;

				kinectFrameCount[LEFT_HAND_FLAG]++;
			}	
		}
	}

	NuiShutdown();
	return 1;

}

void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage, Mat &fullDepthImage, point * HandPoint)  //�����ͼ����ʾ�ֵ�λ�ã�ͨ�����֡��������һ���˶��켣
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
		for (int i = ((0 > int(HandPoint[RIGHT_HAND_FLAG].y - 3)) ? 0 : int(HandPoint[RIGHT_HAND_FLAG].y - 3)); i < ((depthImage.rows < int(HandPoint[RIGHT_HAND_FLAG].y + 3)) ? depthImage.rows : int(HandPoint[RIGHT_HAND_FLAG].y + 3)); i++)
		{
			uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			for (int j = ((0 > int(HandPoint[RIGHT_HAND_FLAG].x - 3)) ? 0 : int(HandPoint[RIGHT_HAND_FLAG].x - 3)); j < ((depthImage.cols < int(HandPoint[RIGHT_HAND_FLAG].x + 3)) ? depthImage.cols : int(HandPoint[RIGHT_HAND_FLAG].x + 3)); j++)
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

		for (int i = ((0 > int(HandPoint[LEFT_HAND_FLAG].y - 3)) ? 0 : int(HandPoint[LEFT_HAND_FLAG].y - 3)); i < ((depthImage.rows < int(HandPoint[LEFT_HAND_FLAG].y + 3)) ? depthImage.rows : int(HandPoint[LEFT_HAND_FLAG].y + 3)); i++)
		{
			uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			for (int j = ((0 > int(HandPoint[LEFT_HAND_FLAG].x - 3)) ? 0 : int(HandPoint[LEFT_HAND_FLAG].x - 3)); j < ((depthImage.cols < int(HandPoint[LEFT_HAND_FLAG].x + 3)) ? depthImage.cols : int(HandPoint[LEFT_HAND_FLAG].x + 3)); j++)
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

		for (int i = 0; i<fullDepthImage.rows; i++)         //ÿ�ζ���fullDepthImage�������и���
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

int extrudeRightHandPosition(HANDLE &skeletonEvent, point * HandPoint)  //ͨ��HandPointָ�� �޸Ĵ�������鼰�޸������ֵ�λ������ 
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

	//point rightHandPoint[NUI_SKELETON_COUNT] = {};        //ʹ�øò����������ͼ�е��ֲ�������Ϊԭʼ���ݣ���ֻ�У�x��y��
	//point leftHandPoint[NUI_SKELETON_COUNT] = {}; 
	
	float Right_Hand_fx, Right_Hand_fy;
	float Left_Hand_fx, Left_Hand_fy;
	float Hip_Center_fx, Hip_Center_fy;
	float Shoulder_Center_fx, Shoulder_Center_fy;
	
	int flag = 0;
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			float shoulder_x, shoulder_y, shoulder_z, hip_x, hip_y, hip_z;
			shoulder_x = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].x;
			shoulder_y = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
			shoulder_z = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].z;
			
			hip_x = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x;
			hip_y = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y;
			hip_z = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z;

			float Personality = sqrt((shoulder_x - hip_x)*(shoulder_x - hip_x) + (shoulder_y - hip_y)*(shoulder_y - hip_y) + (shoulder_z - hip_z)*(shoulder_z - hip_z));    //����������߼�����ĵ��β��ľ���
			cout << "Personality" << Personality << endl;
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], &Right_Hand_fx, &Right_Hand_fy);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT], &Left_Hand_fx, &Left_Hand_fy);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER], &Hip_Center_fx, &Hip_Center_fy);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER], &Shoulder_Center_fx, &Shoulder_Center_fy);
			
			if (Right_Hand_fy <= Hip_Center_fy)    //���������֡����ȡ   		                                         
			{                                                //���ֲ�λ�ó����Źؽ�ʱ����¼�ֲ�λ������	                                           
				//rightHandPoint[i].x = (Right_Hand_fx - Hip_Center_fx) / (Hip_Center_fy - Shoulder_Center_fy);    //��ȡ������������ĵ����λ��   
				//rightHandPoint[i].y = (Hip_Center_fy - Right_Hand_fy) / (Hip_Center_fy - Shoulder_Center_fy);
			
				HandPosition1[RIGHT_HAND_FLAG].x = Right_Hand_fx;
				HandPosition1[RIGHT_HAND_FLAG].y = Right_Hand_fy;

				HandPoint[RIGHT_HAND_FLAG].x = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x) / Personality;
				HandPoint[RIGHT_HAND_FLAG].y = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y) / Personality;
				HandPoint[RIGHT_HAND_FLAG].z = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z) / Personality;

			}
			else                                               //���ֲ�λ�õ����Źؽ�ʱ�����ֲ�λ�������Ϊ��0��0��          ���赱�ֲ�λ�õ����Źؽ�ʱ�ֵ�����Ϊ��0��0��
			{
				//rightHandPoint[i].x = 0;
				//rightHandPoint[i].y = 0;

				HandPosition1[RIGHT_HAND_FLAG].x = 0;
				HandPosition1[RIGHT_HAND_FLAG].y = 0;
				
				HandPoint[RIGHT_HAND_FLAG].x = 0;
				HandPoint[RIGHT_HAND_FLAG].y = 0;
				HandPoint[RIGHT_HAND_FLAG].z = 0;
			}
			if (Left_Hand_fy <= Hip_Center_fy)
			{
				//leftHandPoint[i].x = (Left_Hand_fx - Hip_Center_fx) / (Hip_Center_fy - Shoulder_Center_fy);     //��ȡ���λ��
				//leftHandPoint[i].y = (Hip_Center_fy - Left_Hand_fy) / (Hip_Center_fy - Shoulder_Center_fy);
			
				HandPosition1[LEFT_HAND_FLAG].x = Left_Hand_fx;
				HandPosition1[LEFT_HAND_FLAG].y = Left_Hand_fy;

				HandPoint[LEFT_HAND_FLAG].x = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x) / Personality;
				HandPoint[LEFT_HAND_FLAG].y = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y) / Personality;
				HandPoint[LEFT_HAND_FLAG].z = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z) / Personality;

			}
			else
			{
				//leftHandPoint[i].x = 0;
				//leftHandPoint[i].y = 0;

				HandPosition1[LEFT_HAND_FLAG].x = 0;
				HandPosition1[LEFT_HAND_FLAG].y = 0;

				HandPoint[LEFT_HAND_FLAG].x = 0;
				HandPoint[LEFT_HAND_FLAG].y = 0;
				HandPoint[LEFT_HAND_FLAG].z = 0;
			}
			//cout << "(" << rightHandPoint[i].x << "," << rightHandPoint[i].y << ")   ||";
			//cout << "(" << (int)Hip_Center_fx << "," << (int)Hip_Center_fy << ")" << endl;

			flag = i;                        //�˳���ֻ���ظ��ٵ����һ���˵��ֵ�λ�� 
		}
	}

	//HandPoint[LEFT_HAND_FLAG].x = leftHandPoint[flag].x;
	//HandPoint[LEFT_HAND_FLAG].y = leftHandPoint[flag].y;
	//HandPoint[RIGHT_HAND_FLAG].x = rightHandPoint[flag].x;
	//HandPoint[RIGHT_HAND_FLAG].y = rightHandPoint[flag].y;

	return 1;
}