//FaceRecg.cpp: class implementation of CFaceRecognize

#include "stdafx.h"
#include "FaceRecg.h"

//extern double g_thresh1;
//extern double g_thresh2;

//LBP histogram dimension per block
int hdims = 16;

//////////////////////////////////////////////////////////////////////
// constructor / destructor
//////////////////////////////////////////////////////////////////////
CFaceRecognize::CFaceRecognize()
{
}

CFaceRecognize::~CFaceRecognize()
{
}

//////////////////////////////////////////////////////////////////////
// public member functions
//////////////////////////////////////////////////////////////////////
void CFaceRecognize::GetLGBPHS(IplImage *GrayImage, std::vector<int> &hist)
{
	IplImage* tempFaceImg=cvCreateImage(cvGetSize(GrayImage),8,GrayImage->nChannels);

	for (int j=1;j<OrientationNum+1;j++)
	{
		for (int i=0;i<ScaleNum;i++)
		{
			cvCopy(GrayImage,tempFaceImg);

			//gabor filtering
			CvGabor mGabor(j*PI/OrientationNum,i);
			mGabor.conv_img(tempFaceImg,tempFaceImg,3);

			//LBP feature extraction
			IplImage* LBPMap=cvCreateImage(cvSize(tempFaceImg->width-2,tempFaceImg->height-2),8,1);
			DoLBP(tempFaceImg,LBPMap);

			//calculate histogram
			int widthindex=LBPMap->width/WINDOWW;
			int heightindex=LBPMap->height/WINDOWH;
			vector<int> SubHist;
			CalHistogram(LBPMap,SubHist);
			for (size_t imm=0;imm<SubHist.size();imm++)
			{
				hist.push_back(SubHist[imm]);
			}
			cvReleaseImage(&LBPMap);
		}
	}
	cvReleaseImage(&tempFaceImg);
}

IplImage* CFaceRecognize::StandardImage(IplImage *img)
{
	//gray
	IplImage *grayImg = cvCreateImage(cvGetSize(img), 8, 1);
	cvCvtColor(img, grayImg, CV_BGR2GRAY);

	//hist equalization
	IplImage *eqhistImg = cvCreateImage(cvGetSize(grayImg), 8, 1);
	cvEqualizeHist(grayImg, eqhistImg);

	//resize
	IplImage *standImage = cvCreateImage(cvSize(FACEWIDTH, FACEHEIGHT), 8, 1);
	cvResize(eqhistImg, standImage, CV_INTER_LINEAR);

	cvReleaseImage(&eqhistImg);
	cvReleaseImage(&grayImg);
	return standImage;
}

bool CFaceRecognize::LoadFeatureData(string featureDataPath)
{
	//read file
	ifstream srcFile(featureDataPath.c_str(), ios::in);
	if( !srcFile )
		return false;

	//read and process content
	string str = "";
	int index = 0;
	while(srcFile && !srcFile.eof())
	{
		getline(srcFile, str);
		if(str == "")
			break;

		//get person's name, i.e. class label
		char* pChar = strtok((char*)str.c_str(), ":");
		m_classLabels.push_back(string(pChar));

		//get LGBPHS data
		vector<int> LGBPHist;
		pChar = strtok(NULL, ",");
		while (NULL != pChar)
		{
			LGBPHist.push_back(atoi(pChar));
			pChar = strtok(NULL, ",");
		}

		//create feature data
		m_featureData.push_back(LGBPHist);
	}
	srcFile.close();

	return true;
}
/*
bool CFaceRecognize::FindNearSamples(IplImage* TestGray, string& label)
{
	m_compareResults.clear();

	vector<int> LGBPHist;
	GetLGBPHS(TestGray,LGBPHist);

	for (size_t imm=0;imm<m_featureData.size();imm++)
	{
		//histogram intersection
		long long similar=0;
		vector<int> TrainHist(m_featureData[imm]);
		for (size_t jmm=0;jmm<LGBPHist.size();jmm++)
		{
			int value1=LGBPHist[jmm];
			int value2=TrainHist[jmm];
			if (value1>=value2)
			{
				similar+=value2;
			}
			else
			{
				similar+=value1;
			}
		}
		//store comparison results
		CompareResult cr;
		cr.classLabel = m_classLabels[imm];
		cr.similar = similar;
		m_compareResults.push_back(cr);
	}

	//sorting to get the max similarity
	std::sort(m_compareResults.begin(), m_compareResults.end(), CmpHtoL);

	//decide which label belongs to
	int firstMaxSimilar = m_compareResults[0].similar;
	int secondMaxSimilar = m_compareResults[1].similar;
	double ratio  = (double)(firstMaxSimilar - secondMaxSimilar) / firstMaxSimilar;
	//threshold 1
	if (firstMaxSimilar <= g_thresh1)
	{
		return false;
	}
	else	//threshold 2
	{
		if (ratio > 0.0 && ratio <= g_thresh2)
		{
			return false;
		}
		else
		{
			label = m_compareResults[0].classLabel;
			return true;
		}
	}
}
*/

//////////////////////////////////////////////////////////////////////
// private member functions
//////////////////////////////////////////////////////////////////////
void CFaceRecognize::DoLBP(IplImage *GaborImage, IplImage *LBPImage)
{
	BwImage ImgA(GaborImage);
	for (int i=1;i<GaborImage->height-1;i++)
	{
		for (int j=1;j<GaborImage->width-1;j++)
		{
			int b[8]={0};
			if (ImgA[i-1][j-1]>=ImgA[i][j])
			{
				b[0]=1;
			}
			if (ImgA[i-1][j]>=ImgA[i][j])
			{
				b[1]=1;
			}
			if (ImgA[i-1][j+1]>=ImgA[i][j])
			{
				b[2]=1;
			}
			if (ImgA[i][j+1]>=ImgA[i][j])
			{
				b[3]=1;
			}
			if (ImgA[i+1][j+1]>=ImgA[i][j])
			{
				b[4]=1;
			}
			if (ImgA[i+1][j]>=ImgA[i][j])
			{
				b[5]=1;
			}
			if (ImgA[i+1][j-1]>=ImgA[i][j])
			{
				b[6]=1;
			}
			if (ImgA[i][j-1]>=ImgA[i][j])
			{
				b[7]=1;
			}
			int LBPValue=0;
			for (int i=0;i<8;i++)
			{
				if (b[i]==1)
				{
					LBPValue+=pow(2.0,7.0-i);
				}
			}
			((uchar*)(LBPImage->imageData+(i-1)*LBPImage->widthStep))[j-1]=LBPValue;
		}
	}
}

void CFaceRecognize::CalHistogram(IplImage* LBPImage,std::vector<int> &hist)
{
	int widthindex=LBPImage->width/WINDOWW;
	int heightindex=LBPImage->height/WINDOWH;
	for (int i=0;i<heightindex;i++)
	{
		for (int j=0;j<widthindex;j++)
		{
			//image block size
			CvRect rect;
			rect.x=j*WINDOWW;
			rect.y=i*WINDOWH;
			rect.width=WINDOWW;
			rect.height=WINDOWH;

			//split into blocks
			IplImage* roiImage=cvCreateImage(cvSize(WINDOWW,WINDOWH),8,1);
			cvSetImageROI(LBPImage,rect);
			cvCopy(LBPImage,roiImage);
			cvResetImageROI(LBPImage);
			
			//create histogram
			CvHistogram *Hist=0;
			//int hdims=16;
			float hranges_arr[]={0,255};
			float *hranges =hranges_arr;
			Hist=cvCreateHist(1,&hdims,CV_HIST_ARRAY,&hranges,1);
			cvCalcHist(&roiImage,Hist,0,0);
			for (int i=0;i<hdims;i++)
			{
				double val=cvGetReal1D(Hist->bins,i);
				int value=(int)val;
				hist.push_back(value);
			}
			cvReleaseImage(&roiImage);
			cvReleaseHist(&Hist);
		}
	}
}

bool CFaceRecognize::CmpHtoL(CompareResult a, CompareResult b)
{
	long ccc=a.similar-b.similar;
	if (ccc>0)
	{
		return true;
	}
	return false;
}