
// LZ77Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "LZ77.h"
#include "LZ77Dlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLZ77Dlg dialog



CLZ77Dlg::CLZ77Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LZ77_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLZ77Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editFilePath);
}

BEGIN_MESSAGE_MAP(CLZ77Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLZ77Dlg::OnClickedButtonSelectInputFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CLZ77Dlg::OnClickedButtonCompress)
	ON_BN_CLICKED(IDC_BUTTON3, &CLZ77Dlg::OnClickedButtonDecompress)
END_MESSAGE_MAP()


// CLZ77Dlg message handlers

BOOL CLZ77Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLZ77Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLZ77Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLZ77Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

struct Token {
	int offset;
	int length;
	char next_char;
};

CString CLZ77Dlg::GetOutputFilePath(FILE_TYPE fileType) {
	CString filter;
	LPCTSTR defaultExtension = nullptr;

	switch (fileType) {
	case FILE_TYPE::TXT:
		filter = _T("Text Files (*.txt)|*.txt||");
		defaultExtension = _T("txt");
		break;
	case FILE_TYPE::BIN:
		filter = _T("Binary Files (*.bin)|*.bin||");
		defaultExtension = _T("bin");
		break;
	
	default:
		filter = _T("All Files (*.*)|*.*||");
		break;
	}

	// Create a file dialog object with the appropriate filter
	CFileDialog* dlg = new CFileDialog(TRUE, defaultExtension, NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, filter);

	// Show the file dialog
	CString filePath;
	if (dlg->DoModal() == IDOK) {
		filePath = dlg->GetPathName();
	}

	delete dlg;  // Remember to delete the dialog object to avoid memory leaks

	return filePath;
}

CString CLZ77Dlg::GetInputFilePath() {
	// Get input file path from editbox
	CString inputFileNameCS;
	m_editFilePath.GetWindowText(inputFileNameCS);

	return inputFileNameCS;
}

std::vector<Token> compressLZ77(const std::string& input, int windowSize) {
	std::vector<Token> compressedData;
	int inputSize = input.size();
	int pos = 0;

	while (pos < inputSize) {
		Token token;
		token.offset = 0;
		token.length = 0;
		token.next_char = input[pos];

		int startPos = max(0, pos - windowSize);
		int endPos = pos - 1;

		for (int i = endPos; i >= startPos; --i) {
			int matchLength = 0;
			while (input[i + matchLength] == input[pos + matchLength] && pos + matchLength < inputSize) {
				++matchLength;
			}

			if (matchLength > token.length) {
				token.length = matchLength;
				token.offset = pos - i;
				token.next_char = input[pos + matchLength];
			}
		}

		compressedData.push_back(token);
		pos += (token.length + 1);
	}

	return compressedData;
}

std::string decompressLZ77(const std::vector<Token>& compressedData) {
	std::string decompressedText;
	for (const auto& token : compressedData) {
		if (token.length == 0) {
			// If length is 0, just append the next character to the decompressed text
			decompressedText.push_back(token.next_char);
		}
		else {
			// If length is not 0, copy the data from the history window
			int startPos = decompressedText.size() - token.offset;
			for (int i = 0; i < token.length; ++i) {
				decompressedText.push_back(decompressedText[startPos + i]);
			}
			// Append the next character to the decompressed text
			decompressedText.push_back(token.next_char);
		}
	}
	return decompressedText;
}


void CLZ77Dlg::OnClickedButtonSelectInputFile()
{
	CString filePath;
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		filePath = dlg.GetPathName();
		m_editFilePath.SetWindowText(filePath);
	}
}

void CLZ77Dlg::OnClickedButtonCompress()
{
	// Convert CString to std::string
	std::string inputFileName = CT2A(GetInputFilePath());

	// Open the input file
	std::ifstream inputFile(inputFileName, std::ios::binary);
	if (!inputFile) {
		AfxMessageBox(_T("Error opening file!"), MB_ICONERROR);
		return;
	}

	// Read the entire file into a buffer
	std::vector<char> buffer((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

	// Close the input file
	inputFile.close();

	// Convert the buffer to a string
	std::string input(buffer.begin(), buffer.end());

	// Set the window size for compression (you may adjust this value)
	int windowSize = 256; // Example window size

	// Compress the input string
	std::vector<Token> compressedData = compressLZ77(input, windowSize);

	// Save the compressed data to a file
	std::string outputFileName = CT2A(GetOutputFilePath(FILE_TYPE::BIN));

	// Open the output file
	std::ofstream outputFile(outputFileName, std::ios::binary);
	if (!outputFile) {
		AfxMessageBox(_T("Error opening file for writing!"), MB_ICONERROR);
		return;
	}

	// Write the compressed data to the output file
	for (const auto& token : compressedData) {
		outputFile.write(reinterpret_cast<const char*>(&token.offset), sizeof(token.offset));
		outputFile.write(reinterpret_cast<const char*>(&token.length), sizeof(token.length));
		outputFile.write(reinterpret_cast<const char*>(&token.next_char), sizeof(token.next_char));
	}

	// Close the output file
	outputFile.close();

	AfxMessageBox(_T("File compressed and saved successfully!"), MB_ICONINFORMATION);
}


void CLZ77Dlg::OnClickedButtonDecompress()
{
	std::vector<Token> compressedData;
	std::string inputFileName = CT2A(GetInputFilePath());

	// Open the compressed file for reading
	std::ifstream inputFile(inputFileName, std::ios::binary);
	if (!inputFile.is_open()) {
		// Handle error opening the file
		AfxMessageBox(_T("Error opening compressed file for reading!"), MB_ICONERROR);
		return;
	}

	// Read the compressed data token by token
	Token token;
	while (inputFile.read(reinterpret_cast<char*>(&token.offset), sizeof(token.offset)) &&
		inputFile.read(reinterpret_cast<char*>(&token.length), sizeof(token.length)) &&
		inputFile.read(&token.next_char, sizeof(token.next_char))) {
		compressedData.push_back(token);
	}

	// Close the file
	inputFile.close();

	std::string decompressedData = decompressLZ77(compressedData);

	std::string outputFileName = CT2A(GetOutputFilePath(FILE_TYPE::TXT));

	// Open the decompressed file for writing
	std::ofstream outputFile(outputFileName, std::ios::binary);
	if (!outputFile.is_open()) {
		AfxMessageBox(_T("Error opening decompressed file for writing!"), MB_ICONERROR);
		return;
	}

	// Write the decompressed data to the file
	outputFile << decompressedData;

	// Close the file
	outputFile.close();
	AfxMessageBox(_T("File decompressed and saved successfully!"), MB_ICONINFORMATION);
}
