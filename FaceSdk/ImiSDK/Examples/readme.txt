/********************************************************************************
  Copyright (c) 2015 Huajie IMI Technology Co., Ltd.
  All rights reserved.

  @File Name     : readme.txt
  @Author        : Zhang hongliang
  @Date          : 2015-06-19
  
  @Description   : ���ĵ����ܸ�example�����ݡ��÷��Լ����ע������
  
  @Version       : 0.1.0
  @History       :
  1.2015-06-19 Zhang Hongliang Created file
********************************************************************************/
  
ImiSDK
    |
    |_3rdparty  ��������
    |    |_d3d
    |    |_freeglut-2.8.1
    |    |_lib 
    |
    |_Documentation
    |
    |_Driver   ImiDevice������װ�����Զ���װ��ע��Win8,Win10ϵͳ���������������װǿ��ǩ�����ܰ�װ�ɹ���        
    |
    |_Examples SDKʹ�����ӣ�ʹ��VS2010��д.
    |
    |_Include  ImiSDKͷ�ļ�
    |
    |_Redist   Imi��̬��ImiNect.dll��ImiNect.lib��ImiSensor.dll
    |
    |_Tools    ��ʾ����
     
           

ColorDepthViewer
	ColorDepthViewer��ʾ[������ȡ��ɫͼ�����ͼ֡����]������.
	����������2��channelʵ��,���ͷֱ���IMI_COLOR_FRAME[��ɫͼ],IMI_DEPTH_FRAME[���ͼ];
	ͨ��setDataCallback(needImage, NULL), ����ѭ������needImage(),ִ��imiWaitForStream�Լ�imiReadNextFrame��
	������ȡ֡����,����ʾ��������.
	
ColorViewer	
	colorViewer��ʾ[������ȡ��ɫͼ֡����]������.
	����������1��channelʵ��������ΪIMI_COLOR_FRAME[��ɫͼ].
	ͨ��setDataCallback(needImage, NULL), ����ѭ������needImage(),ִ��imiWaitForStream�Լ�imiReadNextFrame��
	������ȡ֡����,����ʾ��������.
	
DepthViewer
	DepthViewer��ʾ[������ȡ���ͼ֡����]������
	����������1��channelʵ��,������IMI_DEPTH_FRAME[���ͼ];
	ͨ��setDataCallback(needImage, NULL), ����ѭ������needImage(),ִ��imiWaitForStream�Լ�imiReadNextFrame��
	������ȡ֡����,����ʾ��������.
	
PointCloudViewer
	PointCloudViewer��ʾ[������ȡ���ͼ֡���ݲ����е���ת����ʾ����ͼ]������
	����������1��channelʵ��,������IMI_DEPTH_FRAME[���ͼ];
	ͨ��setDataCallback(needImage, NULL), ����ѭ������needImage(),ִ��imiWaitForStream��imiReadNextFrame�Լ�imiConvertDepthToPointCloud��
	������ȡ֡����,��ת��Ϊ�������ݣ�����ʾ��������.

UserTracker
	UserTracker��ʾ[������ȡǰ���͹Ǽ�����]������.
	����������1��channelʵ��,����ΪIMI_USER_INDEX_SKELETON_FRAME[�Ǽ�ͼ];
	ͨ��imiRegisterChannelCallbackע��ص�,����ѭ������needImage()��ִ��imiWaitForStream�Լ�imiReadNextFrame��
	������ȡ֡����,����ʾ��������.

UserSegmentation
	UserSegmentation��ʾ����ͼƬ�в��������ͼƬ.
	������ȡIMI_COLOR_FRAME[��ɫͼ]��IMI_USER_INDEX_SKELETON_FRAME[�Ǽ�ͼ];
	���ݹǼ�ͼ��ǰ��ͼ�����λ�ã����²�ɫͼ�Ķ�Ӧλ�ã�����ͼƬ����ʾ������

Skeleton
	Skeleton��ʾ�˵ĹǼ�.
	������ȡIMI_USER_INDEX_SKELETON_FRAME[�Ǽ�ͼ]�����Ǽ�ͼ��ÿ����λ����������
	
	
����ʹ�ùǼ���ʱ��Ҫ�� Redist\Win32 Ŀ¼�� NiTE2Ŀ¼��OpenNI2Ŀ¼��NiTE.ini��OpenNI.ini ����������Ŀ¼

	
Render.h,.cpp��Render2.h,.cpp����openGLʵ�ֵ�չʾ����,��ע��:	

1��Windows�£���Ҫfreeglut��;
	��ͷ�ļ���freeglut.lib�ļ�λ�ڡ�ImiSDK��װĿ¼3rdparty\freeglut-2.8.1����
	freeglut.dllλ�ڡ�ImiSDK��װĿ¼3rdparty3rdparty/lib��
	
	
2��Ubuntu:
sudo apt-get install freeglut2.8.1 freeglut2.8.1-dev 
��ȡ����װfreeglut

3) Linux:
��Ҫ����FreeglutԴ��
cd 3rdparty/freeglut-2.8.1
./autogen.sh
./configure
make