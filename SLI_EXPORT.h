#pragma once
#include "SLI_PE.h"
#include "MyListCtrl.h"

// CSLI_EXPORT dialog

class CSLI_EXPORT : public CDialogEx
{
	DECLARE_DYNAMIC(CSLI_EXPORT)

public:
	CSLI_EXPORT(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSLI_EXPORT();

	// Dialog Data
	enum { IDD = DIALOG_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CMyListCtrl m_Export_Table;

	CSLI_PE *m_PE;
	void update_List();
};
