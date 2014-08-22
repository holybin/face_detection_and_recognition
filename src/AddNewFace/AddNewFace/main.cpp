#include "stdafx.h"
#include "FaceRecg.h"

#include "Windows.h"

////////////////////////////detect face//////////////////////////////////
static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
//detect face and draw
bool detect_and_draw( IplImage* srcImg, CvRect& roi);
//face cascade detector
const char* cascade_name = "haarcascade_frontalface_alt2.xml";

////////////////////////////feature data file////////////////////////////
const char* feature_file_name = "feature.dat";

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("\n--Argument error!\n--Please specify input/output directory. ");
		printf("(e.g. AddNewFace.exe faces_to_find)\n");
		return -1;
	}
	//description
	printf("* This program is used to ADD NEW FACE images to be found.\n");
	printf("* Transfer face images to FEATURE DATA, and store in file 'feature.dat'.\n");
	printf("* Face image should be better named as PERSON's name.\n");
	printf("Press any key to continue...");
	getchar();

	//load face cascade
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
	if( !cascade )
	{
		fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
		return -1;
	}
	storage = cvCreateMemStorage(0);

	//find all images
	string strFilePath(argv[1]);
	strFilePath.append("\\*");
	WIN32_FIND_DATAA FileData;
	HANDLE hFind;
	hFind = FindFirstFileA(strFilePath.c_str(), &FileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf ("\n--Invalid File Handle. GetLastError reports %d/n", GetLastError ());
		return -1;
	}
	//get image names
	vector<string> fileNames;
	do
	{
		//eliminate directories
		//if(!strcmp(FileData.cFileName,"..") || !strcmp(FileData.cFileName,"."))
		if(FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;
		fileNames.push_back(string(FileData.cFileName));
	}while (FindNextFileA(hFind, &FileData));
	//FindClose(&hFind);

	//file to store feature data
	ofstream featureDataFile;
	char dataFileName[256];
	sprintf(dataFileName, "%s\\%s", argv[1], feature_file_name);
	featureDataFile.open(dataFileName, ofstream::out);
	//class object of processing
	CFaceRecognize* pFaceRec = new CFaceRecognize;
	//process images
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		string fileName = fileNames[i];
		int index = fileName.rfind(".");
		string name = fileName.substr(0, index);	//image name, i.e. person's name
		string extend = fileName.substr(index);		//extend name

		//skip feature data file
		if (extend == ".dat")
			continue;
		
		printf("\nprocessing image: %s.", fileName.c_str());

		//1--load image
		char filePath[256];
		sprintf(filePath, "%s\\%s", argv[1], fileName.c_str());
		IplImage* srcImg = cvLoadImage(filePath, 1);		//rgb
		IplImage* dstImg = 0;
		CvRect roi;
		if( srcImg )
		{
			//get key parts of face
			bool res = detect_and_draw( srcImg, roi);
			if (!res)	continue;
			
			//use roi to crop image
			dstImg = cvCreateImage(cvSize(roi.width,roi.height),srcImg->depth,srcImg->nChannels);
			cvSetImageROI(srcImg, roi);
			cvCopy(srcImg, dstImg);
			cvResetImageROI(srcImg);
			cvReleaseImage( &srcImg );
		}
		else
		{
			printf("--Error loading source image!\n");
			continue;
		}

		//2--standard image
		IplImage* standImage = pFaceRec->StandardImage(dstImg);
		cvShowImage("standard face",standImage);
		cvWaitKey(30);

		//3--generate LGBPHS data
		vector<int> LGBPHist;
		pFaceRec->GetLGBPHS(standImage, LGBPHist);

		//4--write into feature data file
		size_t imm = 0;
		featureDataFile<<name<<":";
		for (; imm<LGBPHist.size()-1; imm++)
			featureDataFile<<LGBPHist[imm]<<",";
		featureDataFile<<LGBPHist[imm]<<"\n";
	}

	cvReleaseMemStorage(&storage);

	printf("\n\nAll finished...\n");
	//system("pause");
	return 0;
}

bool detect_and_draw( IplImage* srcImg, CvRect& roi)
{
	double scale = 1.1;
	IplImage* gray = cvCreateImage( cvSize(srcImg->width,srcImg->height), 8, 1 );
	IplImage* small_img = cvCreateImage( cvSize( cvRound (srcImg->width/scale),
		cvRound (srcImg->height/scale)),
		8, 1 );
	int i;

	cvCvtColor( srcImg, gray, CV_BGR2GRAY );
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );
	cvClearMemStorage( storage );

	if( cascade )
	{
		CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
			1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
			cvSize(20, 20) );

		int index = 0;
		//must contain face
		if (faces->total==0)
		{
			printf("\n--Error with the face image: no face detected!\n");
			return false;
		}
		//more than one face
		else if (faces->total>1)
		{
			printf("\n--Warning with the face image: more than one face detected!\n");

			//get rect with max area
			double area = 0.0;
			for (int i = 0; i < faces->total; i++)
			{
				CvRect* tr = (CvRect*)cvGetSeqElem( faces, i );
				if(tr->height*tr->width > area)
				{
					area = tr->height*tr->width;
					index = i;
				}
			}
		}
		//get roi
		CvRect* r = (CvRect*)cvGetSeqElem( faces, index );
		roi.x = r->x*scale;
		roi.y = r->y*scale;
		roi.width = r->width*scale;
		roi.height = r->height*scale;
	}
	cvReleaseImage( &gray );
	cvReleaseImage( &small_img );
	return true;
} 