
// testDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "FaceSdk.h"

// CtestDemoDlg �Ի���
class CtestDemoDlg : public CDialogEx
{
// ����
public:
	CtestDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnStart();

	static void OnCallbackInfo(int device, int sex, int age, int id, void* userParam);
	void CallbackInfo(int device, int sex, int age, int id);

	static void OnActionCallbackInfo(const _TrajectoryInfo* info, void* userParam);
	void ActionCallbackInfo(_TrajectoryInfo info);

	CEdit m_edtSex;
	CEdit m_edtAge;
	afx_msg void OnBnClickedBtnAction();
};
