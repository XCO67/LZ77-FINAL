
// LZ77.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLZ77App:
// See LZ77.cpp for the implementation of this class
//

class CLZ77App : public CWinApp
{
public:
	CLZ77App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CLZ77App theApp;
