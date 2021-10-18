// mfc_exampleDlg.h : header file
//

#if !defined(AFX_MFC_EXAMPLEDLG_H__290DA678_CC87_4AD5_8FD8_BD8DAB545D93__INCLUDED_)
#define AFX_MFC_EXAMPLEDLG_H__290DA678_CC87_4AD5_8FD8_BD8DAB545D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMfc_exampleDlg dialog

class CMfc_exampleDlg : public CDialog
{
// Construction
public:
	CMfc_exampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMfc_exampleDlg)
	enum { IDD = IDD_MFC_EXAMPLE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc_exampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMfc_exampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC_EXAMPLEDLG_H__290DA678_CC87_4AD5_8FD8_BD8DAB545D93__INCLUDED_)
