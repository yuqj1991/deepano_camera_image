
#ifndef SHOWRESULT_
#define SHOWRESULT_

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dp_api.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "mv_types.h"
#include "Fp16Convert.h"
#include "share_utils.h"
using namespace std;
using namespace cv;
class IMAGE_process
{
        public:
                IMAGE_process(float *mean,float *std,int width,int height);
                unsigned short* loadimage(const char *path, int Ifuse,dp_image_box_t box, int reqsize, float *mean, float *std);
                unsigned short* loadimage_incept(const char *path, int Ifuse,dp_image_box_t box, int reqsize, float *mean, float *std);
                unsigned short* loadimage_ali(const char *path, int Ifuse,dp_image_box_t box, int reqsize, float mean, float std);
                unsigned short* loadimage_non_square(const char *path, int reqsize_w,int reqsize_h, float *mean, float *std);
                unsigned short* loadimage_video(IplImage* img_src, int Ifuse,dp_image_box_t box, int reqsize, float *mean, float *std);
                unsigned char* loadimage_tuling(const char *path);

        private:
                float img_mean[3];
                float img_std[3];
                int img_width;
                int img_height;
        
        public:
                void print_googleNet_classify_result(void *fathomOutput);
                void print_ssd_result(void *fathomOutput,char*imagefile);
                Box *print_ssd_xingniao_result(void *fathomOutput,char*imagefile,int *box_nums);
                void get_bbox_from_result(float *input_data,int data_lenth,int insize_w,int insize_h,int side,int num_class,int num_object,int *bbox_x,int *bbox_y,int *bbox_w,int *bbox_h,float *max_scale,int *max_scale_label,float *max_scale_prob);
                dp_image_box_t print_det_result(void *fathomOutput);
                void print_tunicon_result(dp_FaceDetResult *fathomOutput,void *param);
                void print_inception_result(void *fathomOutput);
                void print_ssd_mobilnet_result(void* fathomOutput,Box* box_demo,int box_nums,int frame_width,int frame_height);
        
};

#endif
