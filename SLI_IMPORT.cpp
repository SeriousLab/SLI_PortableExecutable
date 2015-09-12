// SLI_IMPORT.cpp : implementation file
//

#include "stdafx.h"
#include "SLI_PortableExecutable.h"
#include "SLI_IMPORT.h"
#include "afxdialogex.h"


// CSLI_IMPORT dialog

IMPLEMENT_DYNAMIC(CSLI_IMPORT, CDialogEx)

CSLI_IMPORT::CSLI_IMPORT(CWnd* pParent /*=NULL*/)
: CDialogEx(CSLI_IMPORT::IDD, pParent)
{

}

CSLI_IMPORT::~CSLI_IMPORT()
{
}

void CSLI_IMPORT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IMPORT_MODULE_LIST, m_ModuleList);
	DDX_Control(pDX, IMPORT_FUNCTION_LIST, m_FunctionList);
}


BEGIN_MESSAGE_MAP(CSLI_IMPORT, CDialogEx)
	ON_NOTIFY(NM_CLICK, IMPORT_MODULE_LIST, &CSLI_IMPORT::OnNMClickModuleList)
	ON_NOTIFY(LVN_KEYDOWN, IMPORT_MODULE_LIST, &CSLI_IMPORT::OnLvnKeydownModuleList)
END_MESSAGE_MAP()


// CSLI_IMPORT message handlers


BOOL CSLI_IMPORT::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ModuleList.AddColumn(3, L"Module name", 350, L"INT", 150, L"IAT", 150);
	m_FunctionList.AddColumn(4, L"Index", 100, L"Function Name", 250, L"rVA", 150, L"Offset", 150);

	update_moduleList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSLI_IMPORT::update_moduleList()
{
	m_ModuleList.DeleteAllItems();

	for (unsigned int i = 0; i < m_PE->m_vecImportTable.size(); i++)
	{
		wchar_t szINT[32] = { 0 };
		wchar_t szIAT[32] = { 0 };

		if (m_PE->m_x64)
		{
			wsprintf(szINT, L"%#016X", m_PE->m_vecImportTable[i].uAddress_INT);
			wsprintf(szIAT, L"%#016X", m_PE->m_vecImportTable[i].uAddress_IAT);
		}
		else
		{
			wsprintf(szINT, L"%#08X", m_PE->m_vecImportTable[i].uAddress_INT);
			wsprintf(szIAT, L"%#08X", m_PE->m_vecImportTable[i].uAddress_IAT);
		}

		m_ModuleList.Additem(i, 3, m_PE->m_vecImportTable[i].szModuleName, szINT, szIAT);
	}
}

void CSLI_IMPORT::update_functionList(DWORD dwIndex)
{
	m_FunctionList.DeleteAllItems();

	for (unsigned int i = 0; i < m_PE->m_vecImportTable[dwIndex].m_vecDetais.size(); i++)
	{
		wchar_t szIndex[8] = { 0 };
		wchar_t szRVA[32] = { 0 };
		wchar_t szOffset[32] = { 0 };
		wsprintf(szIndex, L"%#X", m_PE->m_vecImportTable[dwIndex].m_vecDetais[i].dwOrdinal);

		if (m_PE->m_x64)
		{
			wsprintf(szRVA, L"%#016X", m_PE->m_vecImportTable[dwIndex].m_vecDetais[i].uAddress_rVA);
			wsprintf(szOffset, L"%#016X", m_PE->m_vecImportTable[dwIndex].m_vecDetais[i].uAddress_Offset);
		}
		else
		{
			wsprintf(szRVA, L"%#08X", m_PE->m_vecImportTable[dwIndex].m_vecDetais[i].uAddress_rVA);
			wsprintf(szOffset, L"%#08X", m_PE->m_vecImportTable[dwIndex].m_vecDetais[i].uAddress_Offset);
		}

		m_FunctionList.Additem(i, 4, szIndex, m_PE->m_vecImportTable[dwIndex].m_vecDetais[i].szFunctionName, szRVA, szOffset);
	}

}

void CSLI_IMPORT::OnNMClickModuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_ModuleList.GetFirstSelectedItemPosition();

	DWORD dwIndex = m_ModuleList.GetNextSelectedItem(pos);

	if (dwIndex<0 || dwIndex>m_PE->m_vecImportTable.size() - 1)
	{
		return;
	}

	update_functionList(dwIndex);

	*pResult = 0;
}


void CSLI_IMPORT::OnLvnKeydownModuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_ModuleList.GetFirstSelectedItemPosition();

	DWORD dwIndex = m_ModuleList.GetNextSelectedItem(pos);

	switch (pLVKeyDow->wVKey)
	{
	case VK_UP:
	{
				  if (dwIndex ==0)
				  {
					  return;
				  }
				  dwIndex -= 1;
				  update_functionList(dwIndex);
				  break;
	}
	case VK_DOWN:
	{
					if (dwIndex == m_PE->m_vecImportTable.size()-1)
					{
						return;
					}
					dwIndex++;
					update_functionList(dwIndex);
					break;
	}
	default:
		break;
	}



	*pResult = 0;
}
