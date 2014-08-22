
#include "stdafx.h"
#include "common.h"

////////////////////////////////////global constants//////////////////////////////////////
//cascade face detector
String g_faceCascade = "haarcascade_frontalface_alt2.xml";
//String g_faceCascade = "lbpcascade_frontalface.xml";

//path of feature data file
String g_featureFileName = "feature.dat";
//path of faces to be found
String g_facesToBeFound = "faces_to_find";

//threshold for recognition
double g_thresh1 = 200000.0;
double g_thresh2 = 0.015;

//the most similar N faces with faces to be found
int g_faceNum = 5;

////////////////////////////////////global functions//////////////////////////////////////
//get CPU frequency(MHz)
double GetCPUFreq()
{
	int start,over;
	_asm 
	{
		RDTSC
			mov start,eax
	}
	Sleep(50);
	_asm 
	{
		RDTSC
			mov over,eax
	}
	return (over-start)/50000.0;
}
//CPU frequency(MHz)
double  g_cpuFreq = GetCPUFreq();

//get running time(ms)
__int64 GetTime()
{
	DWORD dwLow;
	DWORD dwHigh;
	__asm       
	{
		RDTSC
			mov dwHigh, edx
			mov dwLow, eax
	}
	__int64 time = dwHigh;
	time = time << 32;
	time += dwLow;
	return time;
}