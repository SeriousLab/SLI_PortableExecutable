
// SLI_PortableExecutable.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSLI_PortableExecutableApp:
// See SLI_PortableExecutable.cpp for the implementation of this class
//

class CSLI_PortableExecutableApp : public CWinApp
{
public:
	CSLI_PortableExecutableApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSLI_PortableExecutableApp theApp;