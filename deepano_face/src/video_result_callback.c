//
// Created by chenchen on 12/07/18.
//
#include "post_image.h"
#include "dp_api_type.h"

POST_IMAGE_MODEL sample_model;
double Sum_blob_parse_time=0;
double blob_parse_stage[400];
std::string blob_parse;
int blob_stage_index;

int32_t fps;

std::mutex video_mutex;
cv::Mat bgr;

int num_box_demo=0;
Box box_demo[20];

//帧率回调函数
void fps_callback(int32_t *buffer_fps,void *param)
{
    fps=*(buffer_fps);
}

//解析模型时间回调函数
void blob_parse_callback(double *buffer_fps,void *param)
{
    for(int stage=0;stage<200;stage++)
    {
        blob_parse_stage[stage]=buffer_fps[stage*2+0];
        blob_stage_index=buffer_fps[stage*2+1];
        Sum_blob_parse_time+=blob_parse_stage[stage];
        std::ostringstream   ostr;
        ostr<<"the"<<stage<<"stage parse spending"<<blob_parse_stage[stage]<<"ms,and optType:"<<OP_NAMES[blob_stage_index]<<"\n";
        blob_parse.append(ostr.str());
        if((stage+1)<200)
        {
            if(buffer_fps[(stage+1)*2+0]==0)
                break;
        }
    }     std::ostringstream   ostr;
    ostr <<"the total spending "<<Sum_blob_parse_time<<" ms\n";
    blob_parse.append(ostr.str());
    Sum_blob_parse_time=0;
}

//视频帧回调函数
void video_callback(dp_img_t *img, void *param)
{
    cv::Mat myuv(img->height + img->height / 2, img->width, CV_8UC1, img->img);
    video_mutex.lock();
    cvtColor(myuv, bgr, CV_YUV2BGR_I420, 0);
    DP_MODEL_NET model=*((DP_MODEL_NET*)param);
    switch (model)
    {
        case DP_SSD_MOBILI_NET:
        {
            for(int i=0;i<num_box_demo;i++)
            {
                cv::rectangle(bgr,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cvPoint(box_demo[i].xmax,box_demo[i].ymax),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,box_demo[i].category,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
            }
            break;
        }
		case DP_MTCNN_FACENET:
		{
			for(int i=0;i<num_box_demo;i++)
            {
                cv::rectangle(bgr,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cvPoint(box_demo[i].xmax,box_demo[i].ymax),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,box_demo[i].category,cvPoint(box_demo[i].xmin,box_demo[i].ymin),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
            }
		}
        default:
        {
            break;
        }
    }
    video_mutex.unlock();
}

void mtcnn_face_model(FaceResult *mresult,void *param){
	num_box_demo = mresult->result_box;
	printf("\033[1;31;46m num_box_demo: %d \033[0m\n", mresult->result_box);
	for(int ii = 0; ii< mresult->result_box; ii++)
	{
		box_demo[ii].xmin = mresult->faceResult[ii].rect.x;
		box_demo[ii].ymin = mresult->faceResult[ii].rect.y;
		box_demo[ii].xmax = mresult->faceResult[ii].rect.x + mresult->faceResult[ii].rect.width;
		box_demo[ii].ymax = mresult->faceResult[ii].rect.y + mresult->faceResult[ii].rect.height;
		int sim = sample_model.print_facenet_result(mresult->faceResult[ii].feature);
		memcpy(box_demo[ii].category, sample_model.m_facedescription[sim].c_str(), 20);
	}
}
