
// UpgradeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Upgrade.h"
#include "UpgradeDlg.h"
#include "afxdialogex.h"
#include "ImiNect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_COMMON                      0x0400 + 5000


// CUpgradeDlg �Ի���




CUpgradeDlg::CUpgradeDlg(CWnd* pParent /*=NULL*/) :
      CDialogEx(CUpgradeDlg::IDD, pParent)
    , m_pImiDevice(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
	DDX_Control(pDX, IDC_PROGRESS1, m_upgradeProgressCtrl);
	DDX_Control(pDX, IDC_STATIC_UPGRADE_PROGRESS_VALUE, m_upgradeProgressValue);
	DDX_Control(pDX, IDC_STATIC_MSG, m_StaticMsg);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_StaticVersion);
}

BEGIN_MESSAGE_MAP(CUpgradeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_MESSAGE(WM_USER_COMMON, onProcUserMsg)
END_MESSAGE_MAP()


// CUpgradeDlg ��Ϣ�������

BOOL CUpgradeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

    m_upgradeProgressCtrl.SetRange32(0, 100);
    m_upgradeProgressCtrl.SetStep(1);

	SetTimer(1, 1000, onTimer);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�


void CUpgradeDlg::OnPaint()
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
HCURSOR CUpgradeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpgradeDlg::OnClose()
{
    if(NULL != m_pImiDevice) {
        imiCloseDevice(m_pImiDevice);
    }

    imiDestroy();

    CDialogEx::OnClose();
}

void CUpgradeDlg::upgradeStepPrompt(IMI_UPGRADE_PROMPT promptID, float progress, IMI_UPGRADE_ERRCODE errCode, const char* errMsg)
{
    float curProgress = progress * 100;

	if(IMI_DOWNLOAD_START == promptID) {
		m_listBox.ResetContent();
	}

	CString strMsg;
	strMsg.Format("%s, current progress is %4.0f%%", errMsg, curProgress);
	m_listBox.InsertString(-1, strMsg);
	m_listBox.SetCurSel(m_listBox.GetCount() - 1);

	m_upgradeProgressCtrl.SetPos(curProgress);

	CString strProgressValue;
	strProgressValue.Format("%4.0f%%", curProgress);
	m_upgradeProgressValue.SetWindowText(strProgressValue);
	
    switch(promptID) {
	case IMI_UPGRADE_FAILED:
		{
			PostMessage(WM_USER_COMMON, (WPARAM)IMI_MSG_SHOW_MESSAGEBOX, IMI_UPGRADE_FAILED);
			break;
		}
	case IMI_UPGRADE_SUCCESS:
		{
			PostMessage(WM_USER_COMMON, (WPARAM)IMI_MSG_SHOW_MESSAGEBOX, IMI_UPGRADE_SUCCESS);
			break;
		}
	case IMI_UPGRADE_SUCCESS_NEED_REBOOT:
		{
			PostMessage(WM_USER_COMMON, (WPARAM)IMI_MSG_SHOW_MESSAGEBOX, IMI_UPGRADE_SUCCESS_NEED_REBOOT);
			break;
		}
	default:
		break;
    } 
}

void _stdcall CUpgradeDlg::ImiDeviceUpgradeStepPromptUser(IMI_UPGRADE_PROMPT promptID, float progress, IMI_UPGRADE_ERRCODE errCode, const char* errMsg, void* pUserData)
{
    printf("******  %d\n", promptID);	
	if(NULL != pUserData) {
		((CUpgradeDlg*)pUserData)->upgradeStepPrompt(promptID, progress, errCode, errMsg);
	}
}

VOID CALLBACK CUpgradeDlg::onTimer(HWND hwnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime )
{
	switch(nTimerid) {
	case 1:
		{
			((CUpgradeDlg*)AfxGetMainWnd())->checkUpdate();
			break;
		}
	default:
		break;
	} 
}

void CUpgradeDlg::checkUpdate()
{
	KillTimer(1);

	int32_t iRet = imiInitialize();
	if(0 != iRet) {
		m_StaticMsg.SetWindowTextA("SDK��ʼ��ʧ�ܣ�");
		return;
	}

	// default open the first device
	iRet = imiOpenDevice(NULL, &m_pImiDevice, 0);
	if(0 != iRet) {
		m_StaticMsg.SetWindowTextA("���豸ʧ�ܣ�");
		return;
	}

	ImiVersions m_Versions;
	memset(&m_Versions, 0, sizeof(ImiVersions));
	imiGetVersion(m_pImiDevice, &m_Versions);
	char szSerialNum[64] = {0};
	uint32_t serialSize = 64;
	iRet = imiGetDeviceProperty(m_pImiDevice, IMI_PROPERTY_GENERAL_SERIAL_NUMBER, szSerialNum, &serialSize);
	CString strSNVer;
	strSNVer.Format("SN:%s  firmware:%d.%d.%d-%d.%d.%d  hardware:%d.%d.%d \nuvcColor:%s  bridgefw:%s  sdk:%d.%d.%d", 
		szSerialNum, 
		m_Versions.fw_version.major, m_Versions.fw_version.minor, m_Versions.fw_version.revision,
		m_Versions.fw_version.ap_major, m_Versions.fw_version.ap_minor, m_Versions.fw_version.ap_version,
		m_Versions.hw_version.hardware, m_Versions.hw_version.chip, m_Versions.hw_version.ap_hardware,
		m_Versions.uvc_color_version, m_Versions.bridge_fw_version,
		m_Versions.sdk_version.major, m_Versions.sdk_version.minor, m_Versions.sdk_version.revision);

	m_StaticVersion.SetWindowTextA(strSNVer);

	m_StaticMsg.SetWindowTextA("���������...");

	CString strMsg;
	switch(imiDeviceRequestUpgrade(m_pImiDevice))
	{
	case IMI_UPGRADEERR_NOT_SUPPORT:
		strMsg = "��֧��������";
		break;
	case IMI_UPGRADEERR_OK:
		m_StaticMsg.SetWindowTextA("��������ɹ���");
		if(MessageBox("��⵽�°汾���Ƿ�����?", "����", MB_YESNO) == IDYES) {
			m_StaticMsg.SetWindowTextA("��������...�����˳���������γ��豸������");
			m_upgradecallBacks.pUserData = this;
			m_upgradecallBacks.upgradeStepPromptProc = ImiDeviceUpgradeStepPromptUser;
			imiDeviceStartUpgrade(m_pImiDevice, &m_upgradecallBacks);
		}
		return;
		break;
	case IMI_UPGRADEERR_DEVICE_UPGRADE_DISABLED:
		strMsg = "�豸δ����������";
		break;
	case IMI_UPGRADEERR_NO_NEED_UPGRADE:
		strMsg = "δ��⵽�°汾��";
		break;
	default:
		strMsg = "�������ʧ�ܣ�";
		break;
	}

	m_StaticMsg.SetWindowTextA(strMsg);
}

LRESULT CUpgradeDlg::onProcUserMsg(WPARAM wParam, LPARAM lParam)
{
    ImiMsgType msgType = (ImiMsgType)wParam;
    switch(msgType) {
        case IMI_MSG_SHOW_MESSAGEBOX: {
            CString strMsg;
            switch(lParam)
            {
			case IMI_UPGRADE_FAILED:
				strMsg = "����ʧ�ܣ�";
				break;
            case IMI_UPGRADE_SUCCESS:
                strMsg = "�����ɹ�������������Ӧ�ó���";
                break;
            case IMI_UPGRADE_SUCCESS_NEED_REBOOT:
                strMsg = "�����ɹ������������������豸��";
                break;
            default:
                break;
            }

			m_StaticMsg.SetWindowTextA(strMsg);

            break;
        }
        default: {
            break;
        }

    }

    return S_OK;
}

