//
// Created by chenchen on 11/07/18.
//

#ifndef DEEPANO_SDK_AND_EXAMPLES_SHARE_UTILS_H
#define DEEPANO_SDK_AND_EXAMPLES_SHARE_UTILS_H

#include "share.h"

#ifdef __cplusplus
extern "C" {
#endif
void sighandler(int signum);//信号中断
void test_ping(int argc, char *argv[]);//ping
void reshape(float *data, float *new_data,int length_data,int c,int w,int h);
#ifdef __cplusplus
}
#endif
#endif //DEEPANO_SDK_AND_EXAMPLES_SHARE_UTILS_H
