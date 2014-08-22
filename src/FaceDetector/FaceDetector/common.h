#pragma once

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

//get CPU frequency(MHz)
double GetCPUFreq();
//get running time(ms)
__int64 GetTime();

