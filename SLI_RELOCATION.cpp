// SLI_RELOCATION.cpp : 实现文件
//

#include "stdafx.h"
#include "SLI_PortableExecutable.h"
#include "SLI_RELOCATION.h"
#include "afxdialogex.h"


// CSLI_RELOCATION 对话框

IMPLEMENT_DYNAMIC(CSLI_RELOCATION, CDialogEx)

CSLI_RELOCATION::CSLI_RELOCATION(CWnd* pParent /*=NULL*/)
	: CDialogEx(DIALOG_RELOCATION, pParent)
{

}

CSLI_RELOCATION::~CSLI_RELOCATION()
{
}

void CSLI_RELOCATION::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_RELOC_BLOCK, m_RelocBlock);
	DDX_Control(pDX, LIST_RELOC_DETAIL, m_RelocDetail);
}


BEGIN_MESSAGE_MAP(CSLI_RELOCATION, CDialogEx)
	ON_NOTIFY(NM_CLICK, LIST_RELOC_BLOCK, &CSLI_RELOCATION::OnNMClickRelocBlock)
	ON_NOTIFY(LVN_KEYDOWN, LIST_RELOC_BLOCK, &CSLI_RELOCATION::OnLvnKeydownRelocBlock)
END_MESSAGE_MAP()


// CSLI_RELOCATION 消息处理程序


BOOL CSLI_RELOCATION::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_RelocBlock.AddColumn(4, L"Section", 330, L"rVA", 200, L"Offset", 200, L"Count", 100);
	m_RelocDetail.AddColumn(6, L"Index", 50, L"rVA", 200, L"Offset", 200, L"Type", 50, L"Description", 120, L"FarAddress", 200);

	if (!m_refreshBlock())
	{
		MessageBox(L"Error, m_PE points to null.");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CSLI_RELOCATION::m_refreshBlock()
{
	m_RelocBlock.DeleteAllItems();
	if (m_PE == nullptr)
	{
		return FALSE;
	}
	else
	{
		for (unsigned int i = 0; i < m_PE->m_vecRelocationTable.size(); i++)
		{
			wchar_t szRVA[32] = { 0 };
			wchar_t szOffset[32] = { 0 };
			wchar_t szCount[8] = { 0 };
			if (m_PE->m_x64)
			{
				wsprintf(szRVA, L"%#016X", m_PE->m_vecRelocationTable[i].uAddress_rVA);
				wsprintf(szOffset, L"%#016X", m_PE->m_vecRelocationTable[i].uAddress_Offset);
			}
			else
			{
				wsprintf(szRVA, L"%#08X", m_PE->m_vecRelocationTable[i].uAddress_rVA);
				wsprintf(szOffset, L"%#08X", m_PE->m_vecRelocationTable[i].uAddress_Offset);
			}
			wsprintf(szCount, L"%#04x", m_PE->m_vecRelocationTable[i].dwCount);
			m_RelocBlock.Additem(i, 4, m_PE->m_vecRelocationTable[i].szSection_Name, szRVA, szOffset, szCount);
		}
		return TRUE;
	}
}


BOOL CSLI_RELOCATION::m_refreshDetail(DWORD dwIndex)
{
	m_RelocDetail.DeleteAllItems();
	if (m_PE == nullptr)
	{
		return FALSE;
	}
	else
	{
		for (unsigned int i = 0; i < m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation.size(); i++)
		{
			wchar_t szIndex[8] = { 0 };
			wchar_t szTypeCode[8] = { 0 };
			wchar_t szRVA[32] = { 0 };
			wchar_t szOffset[32] = { 0 };
			wchar_t szFar[32] = { 0 };

			if (m_PE->m_x64)
			{
				wsprintf(szRVA, L"%#016X", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].uAddress_rVA);
				wsprintf(szOffset, L"%#016X", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].uAddress_Offset);
				wsprintf(szFar, L"%#016X", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].u.uFar_Address);
			}
			else
			{
				wsprintf(szRVA, L"%#08X", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].uAddress_rVA);
				wsprintf(szOffset, L"%#08X", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].uAddress_Offset);
				wsprintf(szFar, L"%#08X", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].u.dwFar_Address);
			}

			wsprintf(szIndex, L"%#03x", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].dwIndex);
			wsprintf(szTypeCode, L"%#02x", m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].dwType);

			m_RelocDetail.Additem(i, 6, szIndex, szRVA, szOffset, szTypeCode, m_PE->m_vecRelocationTable[dwIndex].m_vecRelocation[i].szType, szFar);
		}
		return TRUE;
	}
}




void CSLI_RELOCATION::OnNMClickRelocBlock(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	POSITION pos = m_RelocBlock.GetFirstSelectedItemPosition();

	DWORD dwIndex = m_RelocBlock.GetNextSelectedItem(pos);

	if (dwIndex<0 || dwIndex>m_PE->m_vecRelocationTable.size() - 1)
	{
		return;
	}

	m_refreshDetail(dwIndex);
	*pResult = 0;
}




void CSLI_RELOCATION::OnLvnKeydownRelocBlock(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	POSITION pos = m_RelocBlock.GetFirstSelectedItemPosition();

	DWORD dwIndex = m_RelocBlock.GetNextSelectedItem(pos);

	switch (pLVKeyDow->wVKey)
	{
	case VK_UP:
	{
		if (dwIndex == 0)
		{
			return;
		}
		dwIndex -= 1;
		m_refreshDetail(dwIndex);
		break;
	}
	case VK_DOWN:
	{
		if (dwIndex == m_PE->m_vecRelocationTable.size() - 1)
		{
			return;
		}
		dwIndex++;
		m_refreshDetail(dwIndex);
		break;
	}
	default:
		break;
	}


	*pResult = 0;
}
