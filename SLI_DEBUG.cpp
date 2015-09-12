// SLI_DEBUG.cpp : 实现文件
//

#include "stdafx.h"
#include "SLI_PortableExecutable.h"
#include "SLI_DEBUG.h"
#include "afxdialogex.h"


// CSLI_DEBUG 对话框

IMPLEMENT_DYNAMIC(CSLI_DEBUG, CDialogEx)

CSLI_DEBUG::CSLI_DEBUG(CWnd* pParent /*=NULL*/)
	: CDialogEx(DIALOG_DEBUG, pParent)
{

}

CSLI_DEBUG::~CSLI_DEBUG()
{
}

void CSLI_DEBUG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_DEBUG, m_DebugList);
}


BEGIN_MESSAGE_MAP(CSLI_DEBUG, CDialogEx)
END_MESSAGE_MAP()


// CSLI_DEBUG 消息处理程序


BOOL CSLI_DEBUG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_DebugList.AddColumn(4, L"Description", 200, L"Size", 100, L"rVA", 150, L"Offset", 150);

	m_refreshList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

BOOL CSLI_DEBUG::m_refreshList()
{
	m_DebugList.DeleteAllItems();

	if (m_PE == nullptr)
	{
		return FALSE;
	}
	else
	{
		for (unsigned int i = 0; i < m_PE->m_vecDebug.size(); i++)
		{
			wchar_t szSize[12] = { 0 };
			wchar_t szRVA[32] = { 0 };
			wchar_t szOffset[32] = { 0 };
			if (m_PE->m_x64)
			{
				wsprintf(szOffset, L"%#016X", m_PE->m_vecDebug[i].uAddress_Offset);
				wsprintf(szRVA, L"%#016X", m_PE->m_vecDebug[i].uAddress_rVA);
			}
			else
			{
				wsprintf(szOffset, L"%#08X", m_PE->m_vecDebug[i].uAddress_Offset);
				wsprintf(szRVA, L"%#08X", m_PE->m_vecDebug[i].uAddress_rVA);
			}

			wsprintf(szSize, L"%#08X", m_PE->m_vecDebug[i].dwSize);

			m_DebugList.Additem(i, 4, m_PE->m_vecDebug[i].szType, szSize, szRVA, szOffset);
		}
		return TRUE;
	}
}