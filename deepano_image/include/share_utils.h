#ifndef SHARE_UTILS_
#define SHARE_UTILS_

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "dp_api.h"
#include "mv_types.h"
#if defined _MSC_VER && defined _WIN64 || defined __linux__ || _WIN32
#define SUPPORT_OPENCV
#endif
#ifdef SUPPORT_OPENCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
#endif

#define PNET_NUM 8
#define  PNetGraph0_FILENAME  "../tuling/PNet.graph0"
#define  PNetGraph1_FILENAME  "../tuling/PNet.graph1"
#define  PNetGraph2_FILENAME  "../tuling/PNet.graph2"
#define  PNetGraph3_FILENAME  "../tuling/PNet.graph3"
#define  PNetGraph4_FILENAME  "../tuling/PNet.graph4"
#define  PNetGraph5_FILENAME  "../tuling/PNet.graph5"
#define  PNetGraph6_FILENAME  "../tuling/PNet.graph6"
#define  PNetGraph7_FILENAME  "../tuling/PNet.graph7"
#define  PNetGraph8_FILENAME  "../tuling/pNet_graph8"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _box
{
  int xmin;
  int ymin;
  int xmax;
  int ymax;
  char category[20];
}Box;



void reshape(float *data, float *new_data,int length_data,int c,int w,int h);
void test_update_model_parems(int blob_nums,dp_blob_parm_t*parmes);
void test_version(int argc, char *argv[]);
void test_ping(int argc, char *argv[]);
//捕获异常
void sighandler(int signum);
#ifdef __cplusplus
}
#endif

#endif
