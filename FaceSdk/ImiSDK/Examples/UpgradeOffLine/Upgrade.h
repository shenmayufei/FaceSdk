
// Upgrade.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUpgradeApp:
// �йش����ʵ�֣������ Upgrade.cpp
//

class CUpgradeApp : public CWinApp
{
public:
	CUpgradeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUpgradeApp theApp;