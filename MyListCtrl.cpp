// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SLI_PortableExecutable.h"
#include "MyListCtrl.h"


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{

}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CMyListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CMyListCtrl message handlers


void CMyListCtrl::AddColumn(int nNum, ...)
{
	//设置列表框属性
	SetExtendedStyle(
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES |
		LVS_EX_AUTOSIZECOLUMNS);
	//初始化变参列表
	va_list vlist;
	va_start(vlist, nNum);
	//循环获取参数
	for (int i = 0; i < nNum; i++)
	{
		TCHAR* pName = va_arg(vlist, TCHAR*);
		int    nWidth = va_arg(vlist, int);
		//插入一列
		InsertColumn(i, pName, NULL, nWidth);
	}
	//结束参数获取
	va_end(vlist);
}

void CMyListCtrl::Additem(int nItemNum, int nCount, ...)
{
	// 初始化
	va_list vlist;
	va_start(vlist, nCount);
	// 添加一行，并设置第一个单元格内容
	TCHAR* pText =
		va_arg(vlist, TCHAR*);
	int nIndex = InsertItem(nItemNum, pText);
	for (int i = 1; i < nCount; i++)
	{
		//设置本行其他单元格内容
		pText =
			va_arg(vlist, TCHAR*);
		SetItemText(nIndex,
			i, pText);
	}
	//结束获取参数
	va_end(vlist);
}

void CMyListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	NMLVCUSTOMDRAW* pNMCD = (NMLVCUSTOMDRAW*)(pNMHDR);

	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pNMCD->nmcd.dwDrawStage)
	{
		COLORREF  clrNewTextColor, clrNewBkColor;

		int nItem = static_cast<int>(pNMCD->nmcd.dwItemSpec);
		if (nItem % 2)
		{
			clrNewTextColor = RGB(0, 0, 0);
			clrNewBkColor = RGB(0xff, 0xff, 0xff);
			// 			if (pNMCD->iSubItem == 0)
			// 			{
			// 				clrNewTextColor = RGB(255, 55, 0);
			// 				clrNewBkColor = RGB(0xff, 0xff, 0xff);
			// 			}
		}
		else
		{
			clrNewTextColor = RGB(0, 0, 0);
			clrNewBkColor = RGB(0xe8, 0xe8, 0xe8);
		}

		pNMCD->clrText = clrNewTextColor;
		pNMCD->clrTextBk = clrNewBkColor;
		*pResult = CDRF_DODEFAULT;
	}
}
