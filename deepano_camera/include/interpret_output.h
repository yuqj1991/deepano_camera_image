///
/// @file
/// @copyright All code copyright Movidius Ltd 2012, all rights reserved.
///            For License Warranty see: common/license.txt
///
/// @brief     Application configuration Leon header
///


#ifndef _INTERPRET_OUTPUT_H_
#define _INTERPRET_OUTPUT_H_

#include <stdio.h>
#include <stdlib.h>
//#include <strings.h>
#include <string.h>
#include "mv_types.h"
#include <assert.h>


#define NET_IMAGE_WIDTH  448
#define NET_IMAGE_HEIGHT 448

typedef struct _Box
{
        float x1;
        float x2;
        float y1;
        float y2;
} yolo_box;

typedef struct _Result
{
	char category[20];
	float x1;
        float x2;
        float y1;
        float y2;
	float probability;
}YOLO_result_org;

typedef struct _YOLO_Result
{
	char category[20];
	int class_idx;
	int x;
	int y;
	int width;
	int height;
	float probability;
}YOLO_Result;

typedef struct _image
{
	int w;
	int h;
} image;

#ifdef __cplusplus
extern "C" {
#endif
void fp16tofloat(float *dst, unsigned char *src, unsigned nelem);
void interpret_output(float *original_out, YOLO_Result *result, int * result_num, int img_width, int img_height,float threshold);
#if 0
void filter_object(float *original_out,int img_width, int img_height,float threshold,YOLO_result_org *result,int result_num);
void display_image(YOLO_result_org *result,int img_width, int img_height,YOLO_Result *result_image,int result_num);
#endif
#ifdef __cplusplus
}
#endif

#endif

