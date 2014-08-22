// FaceDetectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FaceDetector.h"
#include "FaceDetectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern String g_faceCascade;
extern double g_cpuFreq;
extern String g_featureFileName;
extern String g_facesToBeFound;
extern int g_faceNum;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFaceDetectorDlg dialog




CFaceDetectorDlg::CFaceDetectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFaceDetectorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFaceDetectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CFaceDetectorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_OPENIMAGE, &CFaceDetectorDlg::OnBnClickedBtnOpenimage)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CFaceDetectorDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_RADIO_ORIGINAL, &CFaceDetectorDlg::OnBnClickedRadioOriginal)
	ON_BN_CLICKED(IDC_RADIO_DETECT, &CFaceDetectorDlg::OnBnClickedRadioDetect)
	ON_BN_CLICKED(IDC_RADIO_RECG, &CFaceDetectorDlg::OnBnClickedRadioRecg)
END_MESSAGE_MAP()


// CFaceDetectorDlg message handlers

BOOL CFaceDetectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	//load the face cascade detector
	if( !m_faceCascade.load( g_faceCascade ) )
	{
		AfxMessageBox(_T("Error loading face cascade detector!"), MB_ICONSTOP|MB_OK);
		exit(-1);
	}
	//initialize face recognition class object
	m_faceRecg = new CFaceRecognize;
	String featureDataPath  = g_facesToBeFound+"\\"+g_featureFileName;
	if(!m_faceRecg->LoadFeatureData(featureDataPath))
	{
		AfxMessageBox(_T("Error loading feature data file!"), MB_ICONSTOP|MB_OK);
		exit(-1);
	}
	//get class labels
	vector<string> classLabels = m_faceRecg->m_classLabels;
	//initialize list box
	m_listCtrl.ModifyStyle( 0, LVS_REPORT );
	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listCtrl.InsertColumn(0, _T("FaceNo."), LVCFMT_CENTER);
	for (int i = 1; i<=g_faceNum; i++)
	{
		CString num;
		num.Format("%d", i);
		m_listCtrl.InsertColumn(i, num, LVCFMT_CENTER);
	}
	//get client region
	CRect rect;
	m_listCtrl.GetClientRect(rect);
	//set width of column
	double width = ceil((double)rect.Width() / (g_faceNum+1));
	for (int i = 0; i<=g_faceNum; i++)
	{
		m_listCtrl.SetColumnWidth(i, width); 
	}

	//radio buttons' default states
	((CButton *)GetDlgItem(IDC_RADIO_ORIGINAL))->SetCheck(TRUE);		//chosen
	((CButton *)GetDlgItem(IDC_RADIO_DETECT))->SetCheck(FALSE);	//not chosen
	((CButton *)GetDlgItem(IDC_RADIO_RECG))->SetCheck(FALSE);	//not chosen

	//next button
	GetDlgItem(IDC_BTN_NEXT)->EnableWindow(FALSE);	//not available

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFaceDetectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFaceDetectorDlg::OnPaint()
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
		//check radio state
		bool isOriginal = ((CButton *)GetDlgItem(IDC_RADIO_ORIGINAL))->GetCheck();
		bool isDetect = ((CButton *)GetDlgItem(IDC_RADIO_DETECT))->GetCheck();
		bool isRecg = ((CButton *)GetDlgItem(IDC_RADIO_RECG))->GetCheck();
		//decide mode: 1- original, 2 - detection, 3 - recognition
		int mode = isOriginal ? 1 : (isDetect ? 2 : 3);
		switch(mode)
		{
		case 1:
			showImage(m_srcImg);
			break;
		case 2:
			showImage(m_srcDetectImg);
			break;
		case 3:
			showImage(m_srcRecgImg);
			break;
		}
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFaceDetectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFaceDetectorDlg::OnBnClickedBtnOpenimage()
{
	// TODO
	//open image dialog
	CFileDialog   dlgOpenFile(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
		"Windows bitmap(*.bmp;*.dib)|*.bmp;*.dib|JPEG image(*.jpeg;*.jpg;*.jpe)|*.jpeg;*.jpg;*.jpe|\
		Portable network graphics(*.png)|*.png|Portable bitmap forma(*.pbm;*.pgm;*.ppm)|*.pbm;*.pgm;*.ppm|\
		Sun rasters(*.sr;*.ras)|*.sr;*.ras|TIFF image(*.tif;*.tiff)|*.tif;*.tiff|OpenEXR HDR image(*.exr)|*.exr|\
		JPEG 2000 image(*.jp2)|*.jp2|\
		All Files(*.*)|*.*||");
	//parameters about file names
	DWORD MAXFILE = 4096;
	dlgOpenFile.m_ofn.nMaxFile = MAXFILE;
	char* fileNames = new char[MAXFILE];
	dlgOpenFile.m_ofn.lpstrFile = fileNames;
	dlgOpenFile.m_ofn.lpstrFile[0] = NULL;
	//open ok
	String firstImgName;
	if(dlgOpenFile.DoModal()==IDOK)
	{
		//clear old names
		m_imgNames.clear();
		//get new names
		POSITION pos = dlgOpenFile.GetStartPosition();
		while (pos != NULL)
		{
			String imgName = dlgOpenFile.GetNextPathName(pos);
			m_imgNames.push_back(imgName);
		}
		//show the 1st image path
		firstImgName = m_imgNames[0];
		SetDlgItemText(IDC_EDIT_IMAGEPATH, (LPCTSTR)firstImgName.c_str());
		delete []fileNames;
	}
	else
	{
		delete []fileNames;
		return;
	}
	//buffer overflow
	if(CommDlgExtendedError() == FNERR_BUFFERTOOSMALL)
	{
		AfxMessageBox("BUFFER TOO SMALL!");
		return;
	}

	//release image mat
	if(!m_srcImg.empty())
		m_srcImg.release();
	if (!m_srcDetectImg.empty())
		m_srcDetectImg.release();
	if (!m_srcRecgImg.empty())
		m_srcRecgImg.release();
	//delete content of list control
	m_listCtrl.DeleteAllItems();
	//clear face rects
	m_faceRects.clear();
	m_trueFaceRects.clear();

	//////////////////////////////load and process the 1st image///////////////////////////////////////
	//load the 1st image
	m_srcImg = imread(firstImgName);
	if (m_srcImg.empty())
	{
		AfxMessageBox("Error loading source image!", MB_ICONSTOP|MB_OK);
		return;
	}
	//check radio state
	bool isOriginal = ((CButton *)GetDlgItem(IDC_RADIO_ORIGINAL))->GetCheck();
	bool isDetect = ((CButton *)GetDlgItem(IDC_RADIO_DETECT))->GetCheck();
	bool isRecg = ((CButton *)GetDlgItem(IDC_RADIO_RECG))->GetCheck();
	//decide mode: 1- original, 2 - detection, 3 - recognition
	int mode = isOriginal ? 1 : (isDetect ? 2 : 3);
	switch(mode)
	{
	case 1:
		OnBnClickedRadioOriginal();
		break;
	case 2:
		OnBnClickedRadioDetect();
		break;
	case 3:
		OnBnClickedRadioRecg();
		break;
	}

	//is next available or not
	m_index = 1;		//start from the 2nd image
	if (m_imgNames.size()==m_index)
		GetDlgItem(IDC_BTN_NEXT)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BTN_NEXT)->EnableWindow(TRUE);
}

void CFaceDetectorDlg::OnBnClickedBtnNext()
{
	// TODO
	//release image mat
	if(!m_srcImg.empty())
		m_srcImg.release();
	if (!m_srcDetectImg.empty())
		m_srcDetectImg.release();
	if (!m_srcRecgImg.empty())
		m_srcRecgImg.release();
	//delete content of list control
	m_listCtrl.DeleteAllItems();
	//clear face rects
	m_faceRects.clear();
	m_trueFaceRects.clear();

	///////////////////////////////load and process the next image////////////////////////////
	//get image path
	String strImageName = m_imgNames[m_index++];
	SetDlgItemText(IDC_EDIT_IMAGEPATH, (LPCTSTR)strImageName.c_str());

	//load image
	m_srcImg = imread(strImageName);
	if (m_srcImg.empty())
	{
		AfxMessageBox("Error loading source image!", MB_ICONSTOP|MB_OK);
		return;
	}
	//check radio state
	bool isOriginal = ((CButton *)GetDlgItem(IDC_RADIO_ORIGINAL))->GetCheck();
	bool isDetect = ((CButton *)GetDlgItem(IDC_RADIO_DETECT))->GetCheck();
	bool isRecg = ((CButton *)GetDlgItem(IDC_RADIO_RECG))->GetCheck();
	//decide mode: 1- original, 2 - detection, 3 - recognition
	int mode = isOriginal ? 1 : (isDetect ? 2 : 3);
	switch(mode)
	{
	case 1:
		OnBnClickedRadioOriginal();
		break;
	case 2:
		OnBnClickedRadioDetect();
		break;
	case 3:
		OnBnClickedRadioRecg();
		break;
	}

	//is next available or not
	if (m_imgNames.size()==m_index)
		GetDlgItem(IDC_BTN_NEXT)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BTN_NEXT)->EnableWindow(TRUE);
}

void CFaceDetectorDlg::OnBnClickedRadioOriginal()
{
	// TODO
	if (m_srcImg.empty())
	{
		return;
	}
	showImage(m_srcImg);
}

void CFaceDetectorDlg::OnBnClickedRadioDetect()
{
	// TODO
	if (m_srcImg.empty())
	{
		return;
	}
	else
	{
		if(m_srcDetectImg.empty())
		{
			m_srcDetectImg = m_srcImg.clone();

			//face detection
			__int64 begin = GetTime();
			detectAndDraw();
			__int64 end = GetTime();
			double runningTime = (end-begin)/(g_cpuFreq*1000);

			int faceNum = m_faceRects.size();
			for (int i = 0; i < faceNum; i++)
			{
				//show message in list control
				CString num;
				num.Format(_T("%.3d"), i+1);
				m_listCtrl.InsertItem(i*2, num);
				m_listCtrl.InsertItem(i*2+1, _T(""));

				//label faces on image
				char text[32];
				sprintf(text, "%.3d", i+1);
				Point location;
				location.y = m_faceRects[i].y+1;
				location.x = m_faceRects[i].x+m_faceRects[i].width/3;
				putText(m_srcDetectImg, text, location, CV_FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255,0,255),2);
			}

			//CString msg;
			//msg.Format(_T("[Detection] face: %d, elapsed time: %.4f ms"), faceNum, runningTime);
			//m_listCtrl.SetItemText(m_msgNum, 1, msg);
			//m_msgNum++;
		}
		showImage(m_srcDetectImg);
	}
}

void CFaceDetectorDlg::OnBnClickedRadioRecg()
{
	// TODO
	//clear face rects
	//m_faceRects.clear();

	//firstly detection
	OnBnClickedRadioDetect();

	if (m_srcRecgImg.empty())
	{
		m_srcRecgImg = m_srcDetectImg.clone();

		//recognition one by one
		for( size_t i = 0; i < m_trueFaceRects.size(); i++ )
		{
			//get roi
			Mat face;
			Mat roi = m_srcImg(m_trueFaceRects[i]);
			roi.copyTo(face);

			//face standardization
			IplImage faceImg = face;
			IplImage* standImage = m_faceRecg->StandardImage(&faceImg);
			//show images
			//cvShowImage("1", standImage);
			//cvWaitKey(10);

			//face recognition
			string label;
			__int64 begin = GetTime();
			bool res = m_faceRecg->FindNearSamples(standImage, label);
			__int64 end = GetTime();
			double runningTime = (end-begin)/(g_cpuFreq*1000);
			//if can be recognized
			if (res)
			{
				Rect r(m_faceRects[i]);
				Point center;
				center.x = cvRound((r.x + r.width*0.5));
				center.y = cvRound((r.y + r.height*0.5));
				int radius = cvRound((r.width + r.height)*0.25);
				circle( m_srcRecgImg, center, radius, Scalar(0,0,255), 2, 8, 0 );
			}

			//show message
			int resNum = (m_faceRecg->m_compareResults).size();
			//similarity sum
			long similarSum = 0;
			for (int j = 0; j < resNum; j++)
				similarSum+=m_faceRecg->m_compareResults[j].similar;
			//find the most similar N faces with faces to be found and show
			for (int j = 0; j < ((resNum<g_faceNum)?resNum:g_faceNum); j++)
			{
				string label = m_faceRecg->m_compareResults[j].classLabel;
				m_listCtrl.SetItemText(i*2, j+1, label.c_str());
				long similar = m_faceRecg->m_compareResults[j].similar;
				CString strSimilar;
				strSimilar.Format("%.4f", (double)similar/similarSum);
				m_listCtrl.SetItemText(i*2+1, j+1, strSimilar);
			}
		}
	}
	showImage(m_srcRecgImg);
	
}

void CFaceDetectorDlg::detectAndDraw()
{
	//judge whether this coms from RECG or not
	//bool isRecg = ((CButton *)GetDlgItem(IDC_RADIO_RECG))->GetCheck();

	//scale factor
	double scale = 1.1;
	//enlarge factor
	double enlarge = 0.2;
	//best width for detection
	//const double detectionWidth = 320.0;

	//rgb 2 gray
	Mat grayImg;
	cvtColor( m_srcDetectImg, grayImg, CV_BGR2GRAY );
	
	//scale
	//if (grayImg.cols>detectionWidth)
		//scale = grayImg.cols / detectionWidth;
	Mat smallImg( cvRound (grayImg.rows/scale), cvRound(grayImg.cols/scale), CV_8UC1 );
	resize( grayImg, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );

	//hist equalization
	equalizeHist( smallImg, smallImg );

	//detection
	std::vector<Rect> faces;
	m_faceCascade.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		|CV_HAAR_SCALE_IMAGE
		,Size(20, 20)
		,Size(240, 240)
		);
	//draw face rectangles
	for( size_t i = 0; i < faces.size(); i++ )
	{
		//restore
		Rect newRect;
		newRect.x = faces[i].x*scale;
		newRect.y = faces[i].y*scale;
		newRect.width = faces[i].width*scale;
		newRect.height = faces[i].height*scale;
		//if (isRecg)
		m_trueFaceRects.push_back(newRect);

		//enlarge
		newRect.y = newRect.y - newRect.height*0.2;
		if (newRect.y<0)
			newRect.y = 0;
		newRect.height = newRect.height + 2*newRect.height*0.2;
		if (newRect.height+newRect.y>m_srcImg.rows)
			newRect.height = m_srcImg.rows - newRect.y;

		m_faceRects.push_back(newRect);
		rectangle(m_srcDetectImg, newRect, Scalar(255, 0, 0), 2, 8, 0);
	}
}

void CFaceDetectorDlg::showImage(Mat img)
{
	if (img.empty())
	{
		return;
	}
	//Mat to CvvImage
	IplImage iplSrcImg = img;
	CvvImage cvvSrcImg;
	cvvSrcImg.CopyOf( &iplSrcImg );

	//show image
	CDC* pDC = GetDlgItem(IDC_SHOW_IMAGE)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(IDC_SHOW_IMAGE)->GetClientRect(&rect);
	cvvSrcImg.DrawToHDC(hDC, &rect);

	ReleaseDC( pDC );
}

//cancel "enter" and "esc" function
void CFaceDetectorDlg::OnOK()
{
	// TODO
	//CDialog::OnOK();
}

BOOL CFaceDetectorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO
	if(pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == VK_ESCAPE)   
	{   
		pMsg->wParam = VK_RETURN;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
