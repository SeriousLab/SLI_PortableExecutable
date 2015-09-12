#pragma once
#include "MyListCtrl.h"
#include "SLI_PE.h"

// CSLI_DEBUG �Ի���

class CSLI_DEBUG : public CDialogEx
{
	DECLARE_DYNAMIC(CSLI_DEBUG)

public:
	CSLI_DEBUG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSLI_DEBUG();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = DIALOG_DEBUG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_DebugList;
	virtual BOOL OnInitDialog();
	BOOL m_refreshList();

	CSLI_PE* m_PE;
};
