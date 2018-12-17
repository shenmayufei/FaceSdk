#pragma once

#ifdef _WIN32
#ifdef CAMERA_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif

#include <ImiSkeleton.h>
#include "../include/common_type.h"
#ifdef __cplusplus
extern "C" {
#endif
	DLL_EXPORT void* initA100(int id = 0);
	DLL_EXPORT void* initFolder(const char* folder);

	/*
	vImage ͬһ֡�õ��Ĳ�ɫ���ݺ�������ݣ� vImage[0]�ǲ�ɫ�� vImage[1]�����
	return 1:succeed 0:failed
	*/
	DLL_EXPORT int getImageA100(void* hdl, shop_image_t* vImage, int& image_count, ImiSkeletonFrame& skFrame);

	DLL_EXPORT void releaseA100(void* hdl);
	DLL_EXPORT void releaseFolder(void* hdl);
#ifdef __cplusplus
}
#endif

//cv::Mat shop_image_2_cvmat(shop_image_t image);
//shop_image_t cvmat_2_shop_image(cv::Mat m);

