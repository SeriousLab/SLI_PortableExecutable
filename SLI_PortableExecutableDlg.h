
// SLI_PortableExecutableDlg.h : header file
//

#pragma once


// CSLI_PortableExecutableDlg dialog
class CSLI_PortableExecutableDlg : public CDialogEx
{
// Construction
public:
	CSLI_PortableExecutableDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SLI_PORTABLEEXECUTABLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
