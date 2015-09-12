
// SLI_PortableExecutableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SLI_PortableExecutable.h"
#include "SLI_PortableExecutableDlg.h"
#include "afxdialogex.h"
#include "SLI_EXPORT.h"
#include "SLI_IMPORT.h"
#include "SLI_RELOCATION.h"
#include "SLI_DEBUG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSLI_PortableExecutableDlg dialog



CSLI_PortableExecutableDlg::CSLI_PortableExecutableDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSLI_PortableExecutableDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSLI_PortableExecutableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BASIC_INFO, m_Basic_info);
	DDX_Control(pDX, IDC_ADDITIONAL_INFO, m_Additional_info);
	DDX_Control(pDX, IDC_SECTION_INFO, m_Sectoin_info);
}

BEGIN_MESSAGE_MAP(CSLI_PortableExecutableDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_NOTIFY(NM_RCLICK, IDC_BASIC_INFO, &CSLI_PortableExecutableDlg::OnNMRClickBasicInfo)
	ON_NOTIFY(NM_RCLICK, IDC_ADDITIONAL_INFO, &CSLI_PortableExecutableDlg::OnNMRClickAdditionalInfo)
	ON_NOTIFY(NM_RCLICK, IDC_SECTION_INFO, &CSLI_PortableExecutableDlg::OnNMRClickSectionInfo)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_EXPORT, &CSLI_PortableExecutableDlg::OnViewExport)
	ON_COMMAND(ID_VIEW_IMPORT, &CSLI_PortableExecutableDlg::OnViewImport)
	ON_COMMAND(ID_VIEW_RELOC, &CSLI_PortableExecutableDlg::OnViewReloc)
	ON_COMMAND(ID_VIEW_DEBUG, &CSLI_PortableExecutableDlg::OnViewDebug)
END_MESSAGE_MAP()


// CSLI_PortableExecutableDlg message handlers

BOOL CSLI_PortableExecutableDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	DragAcceptFiles(TRUE);

	m_Basic_info.AddColumn(2, L"Name", 150, L"Value", 150);
	m_Additional_info.AddColumn(4, L"Data_Dir", 180, L"rVA", 100, L"Offset", 100, L"Size", 100);
	m_Sectoin_info.AddColumn(5, L"Name", 100, L"rVA", 80, L"vSize", 80, L"Offset", 80, L"rawSize", 80);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSLI_PortableExecutableDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSLI_PortableExecutableDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSLI_PortableExecutableDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSLI_PortableExecutableDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	wchar_t szDropFile[MAX_PATH];
	DragQueryFile(hDropInfo, 0, szDropFile, 260);
	if (m_PE.SLI_acquire_PE(szDropFile))
	{
		Update_Lists();
	}
	else
	{
		MessageBox(L"Error get PE info...");
	}
	CDialogEx::OnDropFiles(hDropInfo);
}



void CSLI_PortableExecutableDlg::Update_Lists()
{
	m_Basic_info.DeleteAllItems();
	m_Additional_info.DeleteAllItems();
	m_Sectoin_info.DeleteAllItems();

	for (unsigned int i = 0; i < m_PE.m_vecOptional.size(); i++)
	{
		m_Basic_info.Additem(i, 2, m_PE.m_vecOptional[i].szItem_Name, m_PE.m_vecOptional[i].szItem_Value);
	}

	for (unsigned int j = 0; j < m_PE.m_vecDataDir.size(); j++)
	{
		wchar_t szRVA[32] = { 0 };
		wchar_t szOffset[32] = { 0 };
		wchar_t szSize[32] = { 0 };
		wsprintf(szRVA, L"%#08X", m_PE.m_vecDataDir[j].uAddress_rVA);
		wsprintf(szOffset, L"%#08X", m_PE.m_vecDataDir[j].uAddress_Offset);
		wsprintf(szSize, L"%#08X", m_PE.m_vecDataDir[j].uSize);
		m_Additional_info.Additem(j, 4, m_PE.m_vecDataDir[j].szDataDirName, szRVA, szOffset, szSize);
	}

	for (unsigned int k = 0; k < m_PE.m_vecSection.size(); k++)
	{
		wchar_t szRVA[32] = { 0 };
		wchar_t szOffset[32] = { 0 };
		wchar_t szVSize[32] = { 0 };
		wchar_t szRSize[32] = { 0 };
		wsprintf(szRVA, L"%#08X", m_PE.m_vecSection[k].uAddress_rVA);
		wsprintf(szOffset, L"%#08X", m_PE.m_vecSection[k].uAddress_Offset);
		wsprintf(szVSize, L"%#08X", m_PE.m_vecSection[k].uSize_virtual);
		wsprintf(szRSize, L"%#08X", m_PE.m_vecSection[k].uSize_file);
		m_Sectoin_info.Additem(k, 5, m_PE.m_vecSection[k].szSectionName, szRVA, szVSize, szOffset, szRSize);
	}
}

void CSLI_PortableExecutableDlg::OnNMRClickBasicInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CMenu menu;
	CPoint pt;
	menu.LoadMenu(IDR_MENU1);
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON
		| TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);
	*pResult = 0;
}


void CSLI_PortableExecutableDlg::OnNMRClickAdditionalInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CMenu menu;
	CPoint pt;
	menu.LoadMenu(IDR_MENU1);
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON
		| TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);
	*pResult = 0;
}


void CSLI_PortableExecutableDlg::OnNMRClickSectionInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CMenu menu;
	CPoint pt;
	menu.LoadMenu(IDR_MENU1);
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON
		| TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);
	*pResult = 0;
}




void CSLI_PortableExecutableDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	GetCursorPos(&point);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON
		| TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CSLI_PortableExecutableDlg::OnViewExport()
{
	// TODO: 在此添加命令处理程序代码
	CSLI_EXPORT sli_Export;
	sli_Export.m_PE = &m_PE;
	sli_Export.DoModal();
}


void CSLI_PortableExecutableDlg::OnViewImport()
{
	// TODO: 在此添加命令处理程序代码
	CSLI_IMPORT sli_Import;
	sli_Import.m_PE = &m_PE;
	sli_Import.DoModal();
}


void CSLI_PortableExecutableDlg::OnViewReloc()
{
	// TODO: 在此添加命令处理程序代码
	CSLI_RELOCATION sli_Reloc;
	sli_Reloc.m_PE = &m_PE;
	sli_Reloc.DoModal();
}


void CSLI_PortableExecutableDlg::OnViewDebug()
{
	// TODO: 在此添加命令处理程序代码
	CSLI_DEBUG sli_Debug;
	sli_Debug.m_PE = &m_PE;
	sli_Debug.DoModal();
}
