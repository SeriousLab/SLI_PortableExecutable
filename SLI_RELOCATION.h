#pragma once
#include "MyListCtrl.h"
#include "SLI_PE.h"

// CSLI_RELOCATION 对话框

class CSLI_RELOCATION : public CDialogEx
{
	DECLARE_DYNAMIC(CSLI_RELOCATION)

public:
	CSLI_RELOCATION(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSLI_RELOCATION();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DIALOG_RELOCATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_RelocBlock;
	CMyListCtrl m_RelocDetail;

	CSLI_PE* m_PE;
	virtual BOOL OnInitDialog();

	BOOL m_refreshBlock();
	BOOL m_refreshDetail(DWORD dwIndex);
	afx_msg void OnNMClickRelocBlock(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownRelocBlock(NMHDR *pNMHDR, LRESULT *pResult);
};
