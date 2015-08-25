// SLI_EXPORT.cpp : implementation file
//

#include "stdafx.h"
#include "SLI_PortableExecutable.h"
#include "SLI_EXPORT.h"
#include "afxdialogex.h"


// CSLI_EXPORT dialog

IMPLEMENT_DYNAMIC(CSLI_EXPORT, CDialogEx)

CSLI_EXPORT::CSLI_EXPORT(CWnd* pParent /*=NULL*/)
: CDialogEx(CSLI_EXPORT::IDD, pParent)
{

}

CSLI_EXPORT::~CSLI_EXPORT()
{
}

void CSLI_EXPORT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, EXPORT_TABLE, m_Export_Table);
}


BEGIN_MESSAGE_MAP(CSLI_EXPORT, CDialogEx)
END_MESSAGE_MAP()


// CSLI_EXPORT message handlers


BOOL CSLI_EXPORT::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_Export_Table.AddColumn(3, L"Index", 100, L"Name", 150, L"Address", 100);

	update_List();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSLI_EXPORT::update_List()
{
	m_Export_Table.DeleteAllItems();

	CEdit* edit_Field = (CEdit*)this->GetDlgItem(EDIT_NAME);
	edit_Field->SetWindowTextW(m_PE->m_Export.szFileName);

	edit_Field = (CEdit*)this->GetDlgItem(EDIT_BASE_INDEX);
	wchar_t szBase_Index[12] = { 0 };
	wsprintf(szBase_Index, L"%#08X", m_PE->m_Export.uIndex_Base);
	edit_Field->SetWindowTextW(szBase_Index);

	edit_Field = (CEdit*)this->GetDlgItem(EDIT_FUNCTION_COUNT);
	wchar_t szFunction_Count[12] = { 0 };
	wsprintf(szFunction_Count, L"%#08X", m_PE->m_Export.uFunction_Count);
	edit_Field->SetWindowTextW(szFunction_Count);

	edit_Field = (CEdit*)this->GetDlgItem(EDIT_NAME_COUNT);
	wchar_t szName_Count[12] = { 0 };
	wsprintf(szName_Count, L"%#08X", m_PE->m_Export.uName_Count);
	edit_Field->SetWindowTextW(szName_Count);

	edit_Field = (CEdit*)this->GetDlgItem(EDIT_ADDRESS_TABLE);
	wchar_t szAddress_Table[12] = { 0 };
	wsprintf(szAddress_Table, L"%#08X", m_PE->m_Export.uFunction_Address_Table);
	edit_Field->SetWindowTextW(szAddress_Table);

	edit_Field = (CEdit*)this->GetDlgItem(EDIT_NAME_TABLE);
	wchar_t szName_Table[12] = { 0 };
	wsprintf(szName_Table, L"%#08X", m_PE->m_Export.uName_Table_Address);
	edit_Field->SetWindowTextW(szName_Table);

	edit_Field = (CEdit*)this->GetDlgItem(EDIT_NAME_INDEX);
	wchar_t szName_Index[12] = { 0 };
	wsprintf(szName_Index, L"%#08X", m_PE->m_Export.uIndex_Table_Address);
	edit_Field->SetWindowTextW(szName_Index);

	edit_Field = nullptr;

	for (unsigned int i = 0; i < m_PE->m_Export.m_vecExportFunc.size(); i++)
	{
		wchar_t szIndex[12] = { 0 };
		wchar_t szAddress[12] = { 0 };
		wsprintf(szIndex, L"%#08X", m_PE->m_Export.m_vecExportFunc[i].dwIndex);
		wsprintf(szAddress, L"%#08X", m_PE->m_Export.m_vecExportFunc[i].uFunction_Address);
		m_Export_Table.Additem(i, 3, szIndex, m_PE->m_Export.m_vecExportFunc[i].szFunctionName, szAddress);
	}
}