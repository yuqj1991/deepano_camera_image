//
// Created by chenchen on 12/07/18.
//
#ifndef DEEPANO_SDK_AND_EXAMPLES_MODEL_SAMPLE_H
#define DEEPANO_SDK_AND_EXAMPLES_MODEL_SAMPLE_H
#include "share_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void test_TinyYoloNet(int argc, char *argv[]);////yolov1-tinyyolo
void test_SSD_MobileNet(int argc, char *argv[]);//ssd_mobilenet
void test_mobilenets(int argc, char *argv[]);//mobilenet
void test_tiny_yolo_v2(int argc, char *argv[]);//tiny_yolov2

#ifdef __cplusplus
}
#endif

#endif //DEEPANO_SDK_AND_EXAMPLES_MODEL_SAMPLE_H
