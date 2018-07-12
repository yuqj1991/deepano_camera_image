//
// Created by chenchen on 12/07/18.
//
#ifndef DEEPANO_SDK_AND_EXAMPLES_MODEL_SAMPLE_H
#define DEEPANO_SDK_AND_EXAMPLES_MODEL_SAMPLE_H
#include "share_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void test_googleNet(int argc, char *argv[]);//googlenet
void test_TinyYoloNet(int argc, char *argv[]);////yolov1-tinyyolo
void test_AgeNet(int argc, char *argv[]);//agenet
void test_gendernet(int argc, char *argv[]);//gendernet
void test_Resnet_18(int argc, char *argv[]);//resnet
void test_SqueezeNet(int argc, char *argv[]);//squezzsenet
void test_SSD_MobileNet(int argc, char *argv[]);//ssd_mobilenet
void test_inception(int argc, char *argv[]);//inceptionv1,v3,v4
void test_mnist(int argc, char *argv[]);//mnist
void test_mobilenets(int argc, char *argv[]);//mobilenet
void test_ssd_resnet_model(int argc, char *argv[]);//ssd+resnet
void test_ssd_googlenet_model(int argc, char *argv[]);//ssd_google
void test_tiny_yolo_v2(int argc, char *argv[]);//tiny_yolov2

#ifdef __cplusplus
}
#endif

#endif //DEEPANO_SDK_AND_EXAMPLES_MODEL_SAMPLE_H
