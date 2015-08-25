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
	//�����б������
	SetExtendedStyle(
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES |
		LVS_EX_AUTOSIZECOLUMNS);
	//��ʼ������б�
	va_list vlist;
	va_start(vlist, nNum);
	//ѭ����ȡ����
	for (int i = 0; i < nNum; i++)
	{
		TCHAR* pName = va_arg(vlist, TCHAR*);
		int    nWidth = va_arg(vlist, int);
		//����һ��
		InsertColumn(i, pName, NULL, nWidth);
	}
	//����������ȡ
	va_end(vlist);
}

void CMyListCtrl::Additem(int nItemNum, int nCount, ...)
{
	// ��ʼ��
	va_list vlist;
	va_start(vlist, nCount);
	// ���һ�У������õ�һ����Ԫ������
	TCHAR* pText =
		va_arg(vlist, TCHAR*);
	int nIndex = InsertItem(nItemNum, pText);
	for (int i = 1; i < nCount; i++)
	{
		//���ñ���������Ԫ������
		pText =
			va_arg(vlist, TCHAR*);
		SetItemText(nIndex,
			i, pText);
	}
	//������ȡ����
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
