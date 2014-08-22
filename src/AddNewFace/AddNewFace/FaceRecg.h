//FaceRecg.h: class definition of CFaceRecognize
// Process face image to get LGBPHS feature data.
// LGBPHS: Local Gabor Binary Pattern Histogram Sequence

#include "stdafx.h"

#ifndef FACERECOGNIZE_H
#define FACERECOGNIZE_H

#include "cvgabor.h"
#include "ImageTemplate.h"

//////////////////////////////////CONSTANTS////////////////////////////////////////
//width and height of standardized face image
#define  FACEWIDTH 100
#define  FACEHEIGHT 130
//image block size when calculating histogram 
#define WINDOWH 32
#define WINDOWW 32
//scale and orientation of Gabor filter
#define ScaleNum 5
#define OrientationNum 8

//////////////////////////////////CLASS DEFINITION//////////////////////////////////
//result of comparing persons to be found with detected faces
typedef struct CompareResult
{
	string classLabel;			//class label, i.e. person's name
	long similar;		//similarity between detected faces and persons to be found
}CompareResult;

class CFaceRecognize
{
public:
	CFaceRecognize();
	~CFaceRecognize();

public:
	//class labels
	vector<string> m_classLabels;
	//feature data
	vector<vector<int>> m_featureData;
	//comparison results
	vector<CompareResult> m_compareResults;

	//Name��GetLGBPHS
	//Description��calculate LBP histogram after Gabor filtering
	//Parameters��
	//IplImage* GrayImage: input gray image
	//vector<int> &hist��output LGBP histogram
	//Return: none
	void GetLGBPHS(IplImage* GrayImage, std::vector<int> &hist);

	//Name��StandardImage
	//Description��standardize input face image
	//Parameters��
	//IplImage* img��face image to be standarded
	//Return:
	//IplImage*: standarded face image
	IplImage* StandardImage(IplImage *img);

	//Name��LoadFeatureData
	//Description��load feature data of faces to be found  from feature data file
	//Parameters��
	//String featureDataPath: path of feature data file
	//Return:
	//bool: true - load success, false - load failure
	bool LoadFeatureData(string featureDataPath);

	/*
	//Name��FindNearSamples
	//Description��find the most similar face image with input image
	//Parameters��
	//IplImage* TestGray: input gray face image
	//string& label: return class label (person's name) of input face
	//Return:
	//bool: true - recognition success, false - failure
	bool FindNearSamples(IplImage* TestGray, string& label);
	*/

private:
	//Name��DoLBP
	//Description��do LBP transformation for Gabor filtered image
	//Parameters��
	//IplImage* GaborImage��input Gabor filtered image
	//IplImage* LBPImage��output LBP transformed image
	//Return: none
	void DoLBP(IplImage* GaborImage, IplImage* LBPImage);

	//Name��CalHistogram
	//Description��calculate histogram for LBP transformed image
	//Parameters��
	//IplImage* LBPImage��input LBP transformed image
	//vector<int> &hist��output LGBPHS histogram
	//Return: none
	void CalHistogram(IplImage* LBPImage, std::vector<int> &hist);

	//comparison function in STL
	static bool CmpHtoL(CompareResult a, CompareResult b);

};

#endif
