// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FACESDK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FACESDK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FACESDK_EXPORTS
#define FACESDK_API __declspec(dllexport)
#else
#define FACESDK_API __declspec(dllimport)
#endif

#include "define.h"

typedef void (*Callback_OnFaceInfo)(
	int device,
	int sex,
	int age,
	int id,
	void* userParam
	);

FACESDK_API int Face_Init(int *deviceCnt);

FACESDK_API int Face_Create(int device, HWND hwnd, Callback_OnFaceInfo info, void* userParam);

FACESDK_API int Face_SetFaceWnd(int faceId, HWND hwnd);

FACESDK_API int Face_Delete(int device);

FACESDK_API int Face_InitTrajectory();

typedef void(*Callback_OnTrajectoryInfo)(
	const _TrajectoryInfo *info,
	void* userParam
	);

FACESDK_API int Face_StartTrajectory(HWND hwnd, Callback_OnTrajectoryInfo info, void* userParam);

FACESDK_API void Face_StopTrajectory();
