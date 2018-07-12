//
// Created by chenchen on 11/07/18.
//

#ifndef DEEPANO_SDK_AND_EXAMPLES_SHARE_UTILS_H
#define DEEPANO_SDK_AND_EXAMPLES_SHARE_UTILS_H

#include "share.h"

#ifdef __cplusplus
extern "C" {
#endif
max_age argsort_age(float * a,int length);//排序
void sighandler(int signum);//信号中断
void test_ping(int argc, char *argv[]);//ping
void test_version(int argc, char *argv[]);//version
void test_hardware_test(int argc, char *argv[]);//硬件测试
void test_start_cammera(int argc, char *argv[]);//打开摄像头
void test_update_model_parems(int blob_nums,dp_blob_parm_t*parmes);//上传模型参数
void test_get_camera_config(int argc, char *argv[]);//获取摄像头参数
void test_start_camera(int argc, char *argv[]);//开启摄像头
void test_start_video(int argc, char *argv[]);//开启视频流
void test_stop_camera(int argc, char *argv[]);//关闭摄像头
void test_get_frame(int argc, char *argv[]);//获取某一帧
void reshape(float *data, float *new_data,int length_data,int c,int w,int h);
#ifdef __cplusplus
}
#endif
#endif //DEEPANO_SDK_AND_EXAMPLES_SHARE_UTILS_H
