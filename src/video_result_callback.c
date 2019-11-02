//
// Created by chenchen on 12/07/18.
//
#include "post_image.h"
POST_IMAGE_MODEL sample_model;

std::mutex video_mutex;
cv::Mat bgr;

int num_box_demo=0;
Box box_demo[100];

//视频帧回调函数
void video_callback(dp_img_t *img, void *param)
{
    cv::Mat myuv(img->height + img->height / 2, img->width, CV_8UC1, img->img);
    video_mutex.lock();
    cvtColor(myuv, bgr, CV_YUV2BGR_I420, 0);
    DP_MODEL_NET model=*((DP_MODEL_NET*)param);
    switch (model)
    {
        case DP_TINI_YOLO_NET:
        {
            for(int i=0;i<num_box_demo;i++)
            {
                cv::rectangle(bgr,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cvPoint(box_demo[i].xmax,box_demo[i].ymax),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,box_demo[i].category,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
            }
            break;
        }
        case DP_SSD_MOBILI_NET:
        {
            for(int i=0;i<num_box_demo;i++)
            {
                cv::rectangle(bgr,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cvPoint(box_demo[i].xmax,box_demo[i].ymax),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,box_demo[i].category,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
            }
            break;
        }
        case DP_TINY_YOLO_V2_NET:
        {
            for(int i=0;i<num_box_demo;i++)
            {
                cv::rectangle(bgr,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cvPoint(box_demo[i].xmax,box_demo[i].ymax),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,box_demo[i].category,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    video_mutex.unlock();
}

void cdk_result_model(void *result,void *param)
{
    DP_MODEL_NET model=*((DP_MODEL_NET*)param);
    switch (model)
    {
        case DP_TINI_YOLO_NET:
        {
            sample_model.print_tiny_yolov1_net_result(result,box_demo,&num_box_demo);
            break;
        }
        case DP_SSD_MOBILI_NET:
        {
            sample_model.print_ssd_mobilet_result(result,box_demo,&num_box_demo);
            break;
        }
        case DP_MOBILINERS_NET:
        {
            sample_model.print_mobilinet_net_result(result);
            break;
        }
        case DP_TINY_YOLO_V2_NET:
        {
            sample_model.print_tiny_yolov2_result(result,box_demo,&num_box_demo);
            break;
        }
    }

    return;
}