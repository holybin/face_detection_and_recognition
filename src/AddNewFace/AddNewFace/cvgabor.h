/*
Class Name: CvGabor
Description: extract Gabor feature of image
*/

#include "stdafx.h"
#ifndef CVGABOR_H
#define CVGABOR_H

//opencv headers
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;

#include <vector>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

//#include <iostream>
//using namespace std;
//#include <cv.h>
//#include <highgui.h>
//using namespace cv;

#define PI 3.14159265
#define CV_GABOR_REAL 1
#define CV_GABOR_IMAG 2
#define CV_GABOR_MAG  3
#define CV_GABOR_PHASE 4

class CvGabor{
public:
    CvGabor();
    ~CvGabor();

  
     CvGabor(int iMu, int iNu, double dSigma);
     CvGabor(int iMu, int iNu, double dSigma, double dF);
     CvGabor(double dPhi, int iNu);
     CvGabor(double dPhi, int iNu, double dSigma);
     CvGabor(double dPhi, int iNu, double dSigma, double dF);
    bool IsInit();
    long mask_width();
    IplImage* get_image(int Type);
    bool IsKernelCreate();
    long get_mask_width();
    void Init(int iMu, int iNu, double dSigma, double dF);
    void Init(double dPhi, int iNu, double dSigma, double dF);
    void output_file(const char *filename, int Type);
    CvMat* get_matrix(int Type);
    void show(int Type);
    void conv_img(IplImage *src, IplImage *dst, int Type);
     CvGabor(int iMu, int iNu);
    void normalize( const CvArr* src, CvArr* dst, double a, double b, int norm_type, const CvArr* mask );
    void conv_img_a(IplImage *src, IplImage *dst, int Type);

protected:
    double Sigma;
    double F;
    double Kmax;
    double K;
    double Phi;
    bool bInitialised;
    bool bKernel;
    long Width;
    CvMat *Imag;
    CvMat *Real;
  
private:
    void creat_kernel();
    

};

#endif
