
// LZ77Dlg.h : header file
//

#pragma once


// CLZ77Dlg dialog
class CLZ77Dlg : public CDialogEx
{
// Construction
public:
	CLZ77Dlg(CWnd* pParent = nullptr);	// standard constructor
	enum class FILE_TYPE {
		TXT,
		BIN
	};

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LZ77_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	CString GetOutputFilePath(FILE_TYPE fileType);
	CString GetInputFilePath();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editFilePath;
	afx_msg void OnClickedButtonSelectInputFile();
	afx_msg void OnClickedButtonCompress();
	afx_msg void OnClickedButtonDecompress();
};
