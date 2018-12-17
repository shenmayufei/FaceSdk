
// testDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "testDemo.h"
#include "testDemoDlg.h"
#include "afxdialogex.h"
#include "FaceSdk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestDemoDlg �Ի���



CtestDemoDlg::CtestDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SEX, m_edtSex);
	DDX_Control(pDX, IDC_EDIT_AGE, m_edtAge);
}

BEGIN_MESSAGE_MAP(CtestDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CtestDemoDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_ACTION, &CtestDemoDlg::OnBnClickedBtnAction)
END_MESSAGE_MAP()


// CtestDemoDlg ��Ϣ�������

BOOL CtestDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtestDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtestDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestDemoDlg::OnCallbackInfo(int device, int sex, int age, int id, void* userParam)
{
	CtestDemoDlg* pdlg = (CtestDemoDlg*)userParam;
	if (pdlg)
	{
		pdlg->CallbackInfo(device, sex, age, id);
	}
}

void CtestDemoDlg::CallbackInfo(int device, int sex, int age, int id)
{
	CString strSex;
	CString strAge;
	if (sex == Female)
	{
		strSex = "Ů";
	}
	else
	{
		strSex = "��";
	}
	strAge.Format("%d", age);
	m_edtSex.SetWindowText(strSex);
	m_edtAge.SetWindowText(strAge);
}

void CtestDemoDlg::OnBnClickedBtnStart()
{
	int nDevCount = 0;
	Face_Init(&nDevCount);
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_PIC_WND);
	Face_Create(0, pWnd->GetSafeHwnd(), OnCallbackInfo, this);
// 	Face_Create(0, NULL, OnCallbackInfo, this);
	Face_SetFaceWnd(0, ((CWnd*)GetDlgItem(IDC_PIC_FACE1))->GetSafeHwnd());
	Face_SetFaceWnd(1, ((CWnd*)GetDlgItem(IDC_PIC_FACE2))->GetSafeHwnd());
	Face_SetFaceWnd(2, ((CWnd*)GetDlgItem(IDC_PIC_FACE3))->GetSafeHwnd());
	Face_SetFaceWnd(3, ((CWnd*)GetDlgItem(IDC_PIC_FACE4))->GetSafeHwnd());
}

void CtestDemoDlg::OnActionCallbackInfo(const _TrajectoryInfo* info, void* userParam)
{
	CtestDemoDlg* pdlg = (CtestDemoDlg*)userParam;
	if (pdlg)
	{
		pdlg->ActionCallbackInfo(*info);
	}
}

void CtestDemoDlg::ActionCallbackInfo(_TrajectoryInfo info)
{
	CString strTemp;
	strTemp.Format("total:[%d],fall:[%d],punch[%d],raise[%d],id:[%d],[%d,%d]\n", info.total, info.fall, info.punch, info.raise, info.id, info.x, info.y);
	TRACE(strTemp);
}

void CtestDemoDlg::OnBnClickedBtnAction()
{
	Face_InitTrajectory();
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_PIC_WND2);
	Face_StartTrajectory(pWnd->GetSafeHwnd(), OnActionCallbackInfo, this);
}
