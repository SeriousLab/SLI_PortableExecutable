
// SLI_PortableExecutableDlg.h : header file
//

#pragma once
#include "MyListCtrl.h"
#include "SLI_PE.h"

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
public:
	CMyListCtrl m_Basic_info;
	CMyListCtrl m_Additional_info;
	CMyListCtrl m_Sectoin_info;

	CSLI_PE m_PE;

	void Update_Lists();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMRClickBasicInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickAdditionalInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickSectionInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewExport();
	afx_msg void OnViewImport();
	afx_msg void OnViewReloc();
	afx_msg void OnViewDebug();
};
