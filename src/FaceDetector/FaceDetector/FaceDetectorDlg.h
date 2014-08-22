// FaceDetectorDlg.h : header file
//

#pragma once

//#include "common.h"

#include "FaceRecg.h"
//CvvImage header
#include "CvvImage.h"
#include "afxcmn.h"

// CFaceDetectorDlg dialog
class CFaceDetectorDlg : public CDialog
{
// Construction
public:
	CFaceDetectorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FACEDETECTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// USER-DEFINED members
public:
	//list control for message
	CListCtrl m_listCtrl;

	//cascade
	CascadeClassifier m_faceCascade;

	//image path
	String m_imagePath;
	//image names of directory
	vector<String> m_imgNames;
	//image index
	int m_index;

	//image
	Mat m_srcImg;
	//detected image
	Mat m_srcDetectImg;
	//detected image rectangles
	std::vector<Rect> m_trueFaceRects;
	std::vector<Rect> m_faceRects;
	//recognition image
	Mat m_srcRecgImg;

	//face recognition class
	CFaceRecognize* m_faceRecg;

// user-defined functions
public:
	//show image
	void showImage(Mat img);
	//detect faces
	void detectAndDraw();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpenimage();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedRadioOriginal();
	afx_msg void OnBnClickedRadioDetect();
	afx_msg void OnBnClickedRadioRecg();
protected:
	virtual void OnOK();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
