// mfc_example.h : main header file for the MFC_EXAMPLE application
//

#if !defined(AFX_MFC_EXAMPLE_H__09396F61_4012_40FE_875B_C434DF11C8C5__INCLUDED_)
#define AFX_MFC_EXAMPLE_H__09396F61_4012_40FE_875B_C434DF11C8C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfc_exampleApp:
// See mfc_example.cpp for the implementation of this class
//

class CMfc_exampleApp : public CWinApp
{
public:
	CMfc_exampleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc_exampleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMfc_exampleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC_EXAMPLE_H__09396F61_4012_40FE_875B_C434DF11C8C5__INCLUDED_)
