//
// Created by chenchen on 12/07/18.
//

#ifndef DEEPANO_SDK_AND_EXAMPLES_SHARE_H
#define DEEPANO_SDK_AND_EXAMPLES_SHARE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mutex>
#include <chrono>
#include <signal.h>
#include "dp_api.h"
#include "Fp16Convert.h"
#include "mv_types.h"
#include "interpret_output.h"
#include "Common.h"
#include "Region.h"
#include <fstream>
#include <iostream>

#if defined _MSC_VER && defined _WIN64 || defined __linux__ || _WIN32
#define SUPPORT_OPENCV
#endif
#ifdef SUPPORT_OPENCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#endif
#define UNUSED(x) (void)(x)
typedef struct _max_age
{
    float max_predected;
    int   index;
}max_age;
typedef struct _box
{
    int xmin;
    int ymin;
    int xmax;
    int ymax;
    char category[20];
}Box;
typedef enum NET_CAFFE_TENSFLOW
{
    DP_AGE_NET=0,
    DP_ALEX_NET,
    DP_GOOGLE_NET,
    DP_GENDER_NET,
    DP_TINI_YOLO_NET,
    DP_SSD_MOBILI_NET,
    DP_RES_NET,
    DP_SQUEEZE_NET,
    DP_MNIST_NET,
    DP_INCEPTION_V1,
    DP_MOBILINERS_NET,
    DP_TINY_YOLO_V2_NET,
} DP_MODEL_NET;

#endif //DEEPANO_SDK_AND_EXAMPLES_SHARE_H
