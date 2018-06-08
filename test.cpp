#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>  
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

#if defined _MSC_VER && defined _WIN64 || defined __linux__ || defined _WIN32
#define SUPPORT_OPENCV
#endif

#ifdef SUPPORT_OPENCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
#endif

#ifndef Blob_Yolo_PRIORITY
#define Blob_Yolo_PRIORITY		0
#endif


using namespace std;
char **label_cagerioes;
dp_image_box_t box_demo[100];
char categoles[100][20];
int num_box_demo=0;
std::mutex video_mutex;
Mat bgr;
static int num_box=1;

int32_t fps;

double blob_parse_stage[400];
string blob_parse;
int blob_stage_index;
double Sum_blob_parse_time=0;
dp_image_box_t box_second[2];
dp_image_box_t BLOB_IMAGE_SIZE={0,1280,0,960};
int dp_image_box_mask=0;
#define UNUSED(x) (void)(x)

//模型实例枚举
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
   DP_INCEPTION_V2,
   DP_INCEPTION_V3,
   DP_MOBILINERS_NET,
   DP_ALI_FACENET,
   DP_TINY_YOLO_V2_NET,
   DP_CAFFE_NET,
} DP_MODEL_NET;

typedef void(*test_case_fun_t)(int argc, char *argv[]);
typedef struct testcase_t {
	const char *name;
	const char *usage;
	test_case_fun_t test;
} testcase_t;
typedef struct _max_age
{
   float max_predected;
   int   index;
}max_age;

int write_string_to_file_append(const std::string & file_string, const std::string str )  
{  
    std::ofstream   OsWrite(file_string,std::ofstream::app);  
    OsWrite<<str;  
    OsWrite<<std::endl;  
    OsWrite.close();  
   return 0;  
} 
//排序
max_age argsort_age(float * a,int length)
{
  max_age max={0,0};
  for(int i=0;i<length;i++)
  {
    if(max.max_predected<=a[i])
    {
      max.max_predected=a[i];
	  max.index=i;
    }
  }
  return max;
}

//信号捕捉事件
void sighandler(int signum)  
{
	switch(signum)
	{
		case SIGABRT:
		{
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");			
			}
			printf("捕获信号 %d，程序异常中断，跳出...\n", signum);  
			exit(1);
			break;  
		}
		case SIGFPE:
		{
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");			
			}
			printf("捕获信号 %d，算数出错，如为0,跳出...\n", signum);  
			exit(1);
			break;  
		}
		case SIGILL:
		{
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");			
			}
			printf("捕获信号 %d，非法函数映象，如非法指令，跳出...\n", signum);  
			exit(1);
			break;  
		}
		case SIGINT:
		{
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");			
			}
			printf("捕获信号 %d，程序ctrl_c中断，跳出...\n", signum);  
			exit(1);
			break;  
		}
		case SIGSEGV:
		{
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");			
			}
			printf("捕获信号 %d，内存地址访问出错，跳出...\n", signum);  
			exit(1);
			break;  
		}
	}  
}  

//ping
void test_ping(int argc, char *argv[])
{
	int ret;

	UNUSED(argv); UNUSED(argc);
	ret = dp_ping();
	if (ret == 0) {
		printf("Test dp_ping successfully!\n");
	}
	else {
		printf("Test dp_ping failed! ret=%d\n", ret);
	}
}

//版本号
void test_version(int argc, char *argv[])
{
	int ret;
	dp_ver_t ver;

	UNUSED(argv); UNUSED(argc);
	ret = dp_version(&ver);
	if (ret == 0) {
		printf("Test dp_version successfully!,and major = %d,minor = %d,revision = %d\n", ver.major, ver.minor, ver.revision);
	}
	else {
		printf("Test dp_version failed! ret=%d\n", ret);
	}
}

void test_hardware_test(int argc, char *argv[])
{
	int ret;
	dp_hw_status_t status;

	UNUSED(argv); UNUSED(argc);
	ret = dp_hardware_test(&status);
	if (ret == 0) {
		printf("Test test_hardware_test successfully!,and falsh = %d,i2c = %d,sensor = %d\n", status.flash_st, status.i2c_st, status.sensor_st);
	}
	else {
		printf("Test test_hardware_test failed! ret=%d\n", ret);
	}
}
#ifdef SUPPORT_OPENCV//开启摄像头
void test_start_cammera(int argc, char *argv[])
{
	int ret;
	UNUSED(argv); UNUSED(argc);
	ret = dp_start_camera();
	if (ret == 0) {
		printf("Test dp_start_camera successfully!\n");
	}
	else {
		printf("Test dp_start_camera failed! ret=%d\n", ret);
	}
}
//上传板子的模型参数
void test_update_model_parems(int blob_nums,dp_blob_parm_t*parmes)
{
  int ret;
  ret=dp_set_blob_parms(blob_nums,parmes);
  if(ret==0)
  	printf("parems transfer ok\n");
  else
  	printf("parems transfer failed\n");
}

//帧率回调函数
void fps_callback(int32_t *buffer_fps,void *param)
{
  //int *buffer=(int *)buffer_fps;
  fps=*(buffer_fps);
  printf("fps--:%d\n",fps);
  time_t timep;  
  char s[30];        
  time(&timep);   
  strcpy(s,ctime(&timep)); 
  printf("%s\n", s);  
}
//解析模型时间回调函数
void blob_parse_callback(double *buffer_fps,void *param)
{
  for(int stage=0;stage<200;stage++)
  {
     blob_parse_stage[stage]=buffer_fps[stage*2+0];
     blob_stage_index=buffer_fps[stage*2+1];
     Sum_blob_parse_time+=blob_parse_stage[stage];
     //printf("\nthe %d stage parse spending %f ms,and optType:%s\n",stage,blob_parse_stage[stage],OP_NAMES[blob_stage_index]);
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
        write_string_to_file_append(std::string("blob_parse_stage_file.txt"),blob_parse);
        Sum_blob_parse_time=0; 
}
//视频帧回调函数
void video_callback(dp_img_t *img, void *param)
{
	Mat myuv(img->height + img->height / 2, img->width, CV_8UC1, img->img);
	video_mutex.lock();
	cvtColor(myuv, bgr, CV_YUV2BGR_I420, 0);
	DP_MODEL_NET model=*((DP_MODEL_NET*)param);
	switch (model)
       {
	  case DP_TINI_YOLO_NET:
	  {
             for(int i=0;i<num_box_demo;i++)
             {   
               cv::rectangle(bgr,cvPoint(box_demo[i].x1,box_demo[i].y1),cvPoint(box_demo[i].x2,box_demo[i].y2),CV_RGB(0, 255, 0), 2);
               cv::putText(bgr,categoles[i],cvPoint(box_demo[i].x1,box_demo[i].y1),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
               
             }
             string buffer="fps:"+std::to_string(fps);
             cv::putText(bgr,buffer.c_str(),cvPoint(40,40),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
	     break;
	  }
	  case DP_SSD_MOBILI_NET:
	  {
	     for(int i=0;i<num_box_demo;i++)
             {   
                cv::rectangle(bgr,cvPoint(box_demo[i].x1,box_demo[i].y1),cvPoint(box_demo[i].x2,box_demo[i].y2),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,categoles[i],cvPoint(box_demo[i].x1,box_demo[i].y1),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
             }
                string buffer="fps:"+std::to_string(fps);
                cv::putText(bgr,buffer.c_str(),cvPoint(40,40),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
             break;
	  }
          case DP_TINY_YOLO_V2_NET:
	  {
	     for(int i=0;i<num_box_demo;i++)
             {   
                cv::rectangle(bgr,cvPoint(box_demo[i].x1,box_demo[i].y1),cvPoint(box_demo[i].x2,box_demo[i].y2),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,categoles[i],cvPoint(box_demo[i].x1,box_demo[i].y1),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
             }
                string buffer="fps:"+std::to_string(fps);
                cv::putText(bgr,buffer.c_str(),cvPoint(40,40),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
             break;
	  }
          default:
          {
               string buffer="fps:"+std::to_string(fps);
               cv::putText(bgr,buffer.c_str(),cvPoint(40,40),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
	     break;
          }	  
      }
	video_mutex.unlock();
}
//打开视频帧
void test_start_video(int argc, char *argv[])
{
	int ret;
	UNUSED(argv); UNUSED(argc);
        int param=15;
	dp_register_video_frame_cb(video_callback, &param);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (int i = 0; i < 30000 && key == -1; ++i) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
	dp_stop_camera();
}
#endif//打开摄像头
void test_start_camera(int argc, char *argv[])
{
	int ret;
	UNUSED(argv); UNUSED(argc);

	ret = dp_start_camera();
	if (ret == 0) {
		printf("Test dp_start_camera successfully!\n");
	}
	else {
		printf("Test dp_start_camera failed! ret=%d\n", ret);
	}
}
//关闭摄像头
void test_stop_camera(int argc, char *argv[])
{
	int ret;
	UNUSED(argv); UNUSED(argc);

	ret = dp_stop_camera();
	if (ret == 0) {
		printf("Test dp_stop_camera successfully!\n");
	}
	else {
		printf("Test dp_stop_camera failed! ret=%d\n", ret);
	}
}
//设置摄像头参数，现在还有些小bug
void test_set_camera_config(int argc, char *argv[])
{
	int ret;
	dp_camera_config_t config = { 640, 480, 30, 0};

	UNUSED(argv); UNUSED(argc);
	if (argc > 0) config.width = atoi(argv[0]);
	if (argc > 1) config.height = atoi(argv[1]);
	if (argc > 2) config.fps = atoi(argv[2]);
	if (argc > 3) config.id = atoi(argv[3]);

	ret = dp_set_camera_config(&config);
	if (ret == 0) {
		printf("Test dp_set_camera_config(width=%d, height=%d, fps=%d, idx=%d) successfully!\n",
			config.width, config.height, config.fps, config.id);
	}
	else {
		printf("Test dp_set_camera_config(width=%d, height=%d, fps=%d, idx=%d) failed! ret=%d\n",
			config.width, config.height, config.fps, config.id, ret);
	}
}
//获取摄像头参数
void test_get_camera_config(int argc, char *argv[])
{
	int ret;
	dp_camera_config_t config;

	UNUSED(argv); UNUSED(argc);
	ret = dp_get_camera_config(&config);
	if (ret == 0) {
		printf("Test dp_get_camera_config successfully! config=(width=%d, height=%d, fps=%d, idx=%d)\n",
			config.width, config.height, config.fps, config.id);
	}
	else {
		printf("Test dp_get_camera_config failed! ret=%d\n", ret);
	}
}
//获取图象帧
void test_get_frame(int argc, char *argv[])
{
	int ret;
	dp_img_t img;
        ret = dp_start_camera();
	if (ret == 0) {
		printf("Test dp_start_camera successfully!\n");
	}
	else {
		printf("Test dp_start_camera failed! ret=%d\n", ret);
	}
	ret = dp_get_frame(&img);
	if (ret == 0) {
		printf("Test dp_capture_frame sucessfully!\n"
			"\twidth = %d, height=%d, stride=%d, img_size=%d \n",
			img.width, img.height, img.stride, img.img_size);

#ifdef SUPPORT_OPENCV
		Mat myuv(img.height + img.height / 2, img.width, CV_8UC1, img.img);
		cvtColor(myuv, bgr, CV_YUV2BGR_I420, 0);
		imshow("DP_FRAME",bgr);
		waitKey(0);
		//destroyWindow("DP_FRAME");
#endif
		dp_release_frame(&img);
	}
	else {
		printf("Test dp_capture_frame failed ! ret=%d\n", ret);
	}
}

void reshape(float *data, float *new_data,int length_data,int c,int w,int h)
{
   for(int i=0;i<c;i++)
   {
      for(int m=0;m<w*h;m++)
      {
         new_data[i*w*h+m]=data[m*c+i];
      }
   }
}

//双模型，解析获取box回传给板子
void box_callback_model_two_demo(void *result,void *param)
{
  DP_MODEL_NET model=*((DP_MODEL_NET*)param);
  switch (model)
  {
    case DP_TINI_YOLO_NET:
    {
         u16* probabilities = (u16*)result;
         unsigned int resultlen=1470;
         YOLO_Result result[20];
         int result_num=0;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
         interpret_output(resultfp32,result, &result_num, img_width, img_height,0.2);
         num_box_demo=result_num;
		  for (int i = 0; i < result_num; ++i)
           { 
              printf("class:%s, x:%d, y:%d, width:%d, height:%d, probability:%.2f.\n",result[i].category,result[i].x,result[i].y,result[i].width,result[i].height,result[i].probability);
              box_demo[i].x1=result[i].x;
              box_demo[i].x2=result[i].x+result[i].width;
	          if(box_demo[i].x2>img_width)
	 	         box_demo[i].x2=img_width;
              box_demo[i].y1=result[i].y;
              box_demo[i].y2=result[i].y+result[i].height;
	          if(box_demo[i].y2>img_height)
	 	         box_demo[i].y2=img_height;
	          memcpy(categoles[i],result[i].category,20);
           }
		 int box_demo_num=0;
		 if((result_num<=2)&&(result_num>0))
		 {
		   //dp_image_box_t *box_second=(dp_image_box_t*)malloc(result_num*sizeof(dp_image_box_t));

		   for (int i = 0; i < result_num; ++i)
           { 
			  if((result[i].width!=0)&&(result[i].height!=0))
			  {
			     box_second[box_demo_num].x1=result[i].x;
			     box_second[box_demo_num].x2=result[i].x+result[i].width;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=result[i].y;	  
			     box_second[box_demo_num].y2=result[i].y+result[i].height;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
           }
		   //dp_send_first_box_image(box_demo_num, box_second);
		   //free(box_second);
                   dp_image_box_mask=1;
		 }
		 else if(result_num>2)
		 { 
		   //dp_image_box_t *box_second=(dp_image_box_t*)malloc(2*sizeof(dp_image_box_t));
		   for(int i=0;i<result_num;i++)
		   {
		      if((result[i].width!=0)&&(result[i].height!=0))
			  {
			     box_second[box_demo_num].x1=result[i].x;
			     box_second[box_demo_num].x2=result[i].x+result[i].width;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=result[i].y;	  
			     box_second[box_demo_num].y2=result[i].y+result[i].height;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
			  if(box_demo_num==2)
			  {
			     break;
			  }
		   	}
		   //dp_send_first_box_image(2, box_second);		   
		   //free(box_second);
                   dp_image_box_mask=1;
		 }         
        free(resultfp32);
		break;
    }
	case DP_SSD_MOBILI_NET:
	{
	     char *category[]={"background","aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
              "dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=707;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
         int num_valid_boxes=int(resultfp32[0]);
         int index=0;
		 printf("num_valid_bxes:%d\n",num_valid_boxes);
		 for(int box_index=0;box_index<num_valid_boxes;box_index++)
		 {
		   int base_index=7*box_index+7;
		   if(resultfp32[base_index+6]<0||resultfp32[base_index+6]>=1||resultfp32[base_index+5]<0||resultfp32[base_index+5]>=1||resultfp32[base_index+4]<0||resultfp32[base_index+4]>=1||resultfp32[base_index+3]<0||resultfp32[base_index+3]>=1||resultfp32[base_index+2]>=1||resultfp32[base_index+2]<0||resultfp32[base_index+1]<0)
		   {
		   	   continue;
		   }
		   printf("%d %f %f %f %f %f\n",int(resultfp32[base_index+1]),resultfp32[base_index+2],resultfp32[base_index+3],resultfp32[base_index+4],resultfp32[base_index+5],resultfp32[base_index+6]);
		   box_demo[index].x1=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
		   box_demo[index].x2=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
		   box_demo[index].y1=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;	   
		   box_demo[index].y2=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
		   memcpy(categoles[index],category[int(resultfp32[base_index+1])],20);
		   index++;
		 }
                 num_box_demo=index; 
                 int box_demo_num=0;
		 if((num_valid_boxes<=2)&&(num_valid_boxes>0))
		 {
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(num_valid_boxes*sizeof(dp_image_box_t));

		   for (int i = 0; i < num_valid_boxes; ++i)
           { 
			  if(((box_demo[i].x2-box_demo[i].x1)!=0)&&((box_demo[i].y2-box_demo[i].y1)!=0))
			  {
			     box_second[box_demo_num].x1=box_demo[i].x1;
			     box_second[box_demo_num].x2=box_demo[i].x2;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=box_demo[i].y1;	  
			     box_second[box_demo_num].y2=box_demo[i].y2;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
           }
		   dp_send_first_box_image(box_demo_num, box_second);
		   free(box_second);
		 }
		 else if(num_valid_boxes>2)
		 { 
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(2*sizeof(dp_image_box_t));
		   for(int i=0;i<num_valid_boxes;i++)
		   {
		     if(((box_demo[i].x2-box_demo[i].x1)!=0)&&((box_demo[i].y2-box_demo[i].y1)!=0))
			  {
			     box_second[box_demo_num].x1=box_demo[i].x1;
			     box_second[box_demo_num].x2=box_demo[i].x2;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=box_demo[i].y1;	  
			     box_second[box_demo_num].y2=box_demo[i].y2;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
			  if(box_demo_num==2)
			  {
			     break;
			  }
		   	}
		   dp_send_first_box_image(2, box_second);		   
		   free(box_second);
		 }         
        free(resultfp32);
		break;
	  }
          case DP_TINY_YOLO_V2_NET:
	  {
	     u16* probabilities = (u16*)result;
             unsigned int resultlen=18000;
             std::vector<DetectedObject> results;
             int result_num=0;
             float* resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
             float* new_data=(float*)malloc(resultlen * sizeof(*new_data));
             int img_width=1280;
             int img_height=960;
             for (u32 i = 0; i < resultlen; i++)
               resultfp32[i]= f16Tof32(probabilities[i]);
             reshape(resultfp32, new_data,resultlen,125,12,12);
             int dim[2] ={416,416};
             int blockwd = 12;
             int wh =blockwd*blockwd;
             int targetBlockwd = 13;
             int classes = 20;
             float threshold = 0.25;
             float nms = 0.4;
             Region region_obj;
	     region_obj.GetDetections(new_data,125,blockwd,blockwd,classes,img_width,img_height,threshold,nms,targetBlockwd,results);
             num_box_demo= results.size();
             printf("results.size():%d\n",results.size());
             for (int i = 0; i <  results.size(); ++i)
             { 
               printf("class:%s, x:%d, y:%d, width:%d, height:%d, probability:%.2f.\n",results[i].name.c_str(),results[i].left,results[i].top,(results[i].right-results[i].left),(results[i].bottom-results[i].top),results[i].confidence);
               box_demo[i].x1=results[i].left;
               box_demo[i].x2=results[i].right;
	       if(box_demo[i].x2>img_width)
	 	     box_demo[i].x2=img_width;
               box_demo[i].y1=results[i].top;
               box_demo[i].y2=results[i].bottom;
	       if(box_demo[i].y2>img_height)
	 	      box_demo[i].y2=img_height;
	       results[i].name.copy(categoles[i],20, 0);
            }
            int box_demo_num=0;
		 if((num_box_demo<=2)&&(num_box_demo>0))
		 {
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(num_box_demo*sizeof(dp_image_box_t));

		   for (int i = 0; i < num_box_demo; ++i)
                   { 
			  if(((box_demo[i].x2-box_demo[i].x1)!=0)&&((box_demo[i].y2-box_demo[i].y1)!=0))
			  {
			     box_second[box_demo_num].x1=box_demo[i].x1;
			     box_second[box_demo_num].x2=box_demo[i].x2;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=box_demo[i].y1;	  
			     box_second[box_demo_num].y2=box_demo[i].y2;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
                   }
		   dp_send_first_box_image(box_demo_num, box_second);
		   free(box_second);
                   dp_image_box_mask=1;
		 }
		 else if(num_box_demo>2)
		 { 
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(2*sizeof(dp_image_box_t));
		   for(int i=0;i<num_box_demo;i++)
		   {
		     if(((box_demo[i].x2-box_demo[i].x1)!=0)&&((box_demo[i].y2-box_demo[i].y1)!=0))
		     {
			     box_second[box_demo_num].x1=box_demo[i].x1;
			     box_second[box_demo_num].x2=box_demo[i].x2;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=box_demo[i].y1;	  
			     box_second[box_demo_num].y2=box_demo[i].y2;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
			  if(box_demo_num==2)
			  {
			     break;
			  }
		    }
		   dp_send_first_box_image(2, box_second);		   
		   free(box_second);
                   dp_image_box_mask=1;
                 }
            free(resultfp32);
            free(new_data);	 
	    break;
          }
	default:
		break;
   }
}
//单模型结果解析
void box_callback_model_demo(void *result,void *param)
{
  DP_MODEL_NET model=*((DP_MODEL_NET*)param);
  switch (model)
  {
	  case DP_AGE_NET:
	  {
	    char *age_list[]={"0-2","4-6","8-12","15-20","25-32","38-43","48-53","60-100"};
	    u16* probabilities=(u16*)result;
		unsigned int resultlen=8;
		float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		max_age pre_age = {0,0};
        for(int i=0;i<resultlen;i++)
        {
		  if(pre_age.max_predected<=resultfp32[i])
          {
              pre_age.max_predected=resultfp32[i];
	          pre_age.index=i;
           }
        }
		printf("the age range is:%s and the confidence of%.2f\n",age_list[pre_age.index],pre_age.max_predected);
		free(resultfp32);
	    break;
	  }
	  case DP_GENDER_NET:
	  {
	     char *gender_list[]={"Male","Female"};
		 u16* probabilities=(u16*)result;
		 unsigned int resultlen=2;
		 float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		 for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		 max_age pre_gender = {0,0};
         for(int i=0;i<resultlen;i++)
         {
		    if(pre_gender.max_predected<=resultfp32[i])
            {
              pre_gender.max_predected=resultfp32[i];
	          pre_gender.index=i;
            }
         }
		 printf("the age range is:%s and the confidence of %.2f\n",gender_list[pre_gender.index],pre_gender.max_predected);
		 free(resultfp32);
		 break;
	  }
	  case DP_ALEX_NET:
	  {
	    u16* probabilities=(u16*)result;
		unsigned int resultlen=1000;
		float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		float temp_predeiction=0.0;
		int index_temp=0;
		for(int i=0;i<5;i++)
		{
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %sand the probabilityes:%0.3f\n",label_cagerioes[index_temp],temp_predeiction);
		}
		free(resultfp32);
		break;
	  }
	  case DP_GOOGLE_NET:
	  {
	     printf("google net---\n");
	     u16* probabilities=(u16*)result;
		 unsigned int resultlen=1000;
		 float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		 for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		float temp_predeiction=0.0;
		int index_temp=0;
		for(int i=0;i<5;i++)
		{
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp],temp_predeiction);
		}
		free(resultfp32);
	    break;
	  }
	  case DP_RES_NET:
	  {  
	     u16* probabilities=(u16*)result;
		 unsigned int resultlen=1000;
		 float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		 for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
		 for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp],temp_predeiction);
		 }
		 free(resultfp32);
	     break;
	  }
	  case DP_SQUEEZE_NET:
	  {  
	     u16* probabilities=(u16*)result;
		 unsigned int resultlen=1000;
		 float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		 for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
		 for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp],temp_predeiction);
		 }
		 free(resultfp32);
	     break;
	  }
	  case DP_TINI_YOLO_NET:
	  {
	     u16* probabilities = (u16*)result;
         unsigned int resultlen=1470;
         YOLO_Result result[20];
         int result_num=0;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
         interpret_output(resultfp32,result, &result_num, img_width, img_height,0.2);
         num_box_demo=result_num;
         for (int i = 0; i < result_num; ++i)
         { 
           printf("class:%s, x:%d, y:%d, width:%d, height:%d, probability:%.2f.\n",result[i].category,result[i].x,result[i].y,result[i].width,result[i].height,result[i].probability);
           box_demo[i].x1=result[i].x;
           box_demo[i].x2=result[i].x+result[i].width;
	       if(box_demo[i].x2>img_width)
	 	     box_demo[i].x2=img_width;
           box_demo[i].y1=result[i].y;
           box_demo[i].y2=result[i].y+result[i].height;
	       if(box_demo[i].y2>img_height)
	 	      box_demo[i].y2=img_height;
	       memcpy(categoles[i],result[i].category,20);
         }
        free(resultfp32);	 
		break;
	  }
	  case DP_SSD_MOBILI_NET:
	  {
	     char *category[]={"background","aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
              "dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=707;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
         int num_valid_boxes=int(resultfp32[0]);
         int index=0;
		 printf("num_valid_bxes:%d\n",num_valid_boxes);
		 for(int box_index=0;box_index<num_valid_boxes;box_index++)
		 {
		   int base_index=7*box_index+7;
		   if(resultfp32[base_index+6]<0||resultfp32[base_index+6]>=1||resultfp32[base_index+5]<0||resultfp32[base_index+5]>=1||resultfp32[base_index+4]<0||resultfp32[base_index+4]>=1||resultfp32[base_index+3]<0||resultfp32[base_index+3]>=1||resultfp32[base_index+2]>=1||resultfp32[base_index+2]<0||resultfp32[base_index+1]<0)
		   {
		   	   continue;
		   }
		   printf("%d %f %f %f %f %f\n",int(resultfp32[base_index+1]),resultfp32[base_index+2],resultfp32[base_index+3],resultfp32[base_index+4],resultfp32[base_index+5],resultfp32[base_index+6]);
		   box_demo[index].x1=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
		   box_demo[index].x2=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
		   box_demo[index].y1=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;	   
		   box_demo[index].y2=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
		   memcpy(categoles[index],category[int(resultfp32[base_index+1])],20);
		   index++;
		 }
                num_box_demo=index;
                free(resultfp32);
		 break;
	  }
	  case DP_INCEPTION_V1:
	  {
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=1001;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
         for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp-2],temp_predeiction);
		 }
         free(resultfp32);
		 break;
	  }
	   case DP_INCEPTION_V2:
	  {
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=1001;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
         for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp-2],temp_predeiction);
		 }
         free(resultfp32);
		 break;
	  }
	   case DP_INCEPTION_V3:
	  {
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=1001;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
         for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp-2],temp_predeiction);
		 }
         free(resultfp32);
		 break;
	  }
		case DP_MNIST_NET:
	  {
		 u16* probabilities = (u16*)result;
		 char *labels[]={"0","1","2","3","4","5","6","7","8","9"};
         unsigned int resultlen=10;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
         for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",labels[index_temp],temp_predeiction);
		 }
         free(resultfp32);
		 break;
	  }
		case DP_MOBILINERS_NET:
	  {
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=1001;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
		 float temp_predeiction=0.0;
		 int index_temp=0;
         for(int i=0;i<5;i++)
		 {
		   temp_predeiction=resultfp32[i];
		   index_temp=i;
		   for(int j=i+1;j<resultlen;j++)
		   {
		     if(temp_predeiction<=resultfp32[j])
		     {
		        temp_predeiction=resultfp32[j];
				index_temp=j;
		     }
		   }
		   resultfp32[index_temp]=resultfp32[i];
		   resultfp32[i]=temp_predeiction;
   		   printf("prediction classes: %s and the probabilityes:%0.3f\n",label_cagerioes[index_temp],temp_predeiction);
		 }
         free(resultfp32);
		 break;
	  }
              case DP_ALI_FACENET:
          {
                u16* probabilities = (u16*)result;
         unsigned int resultlen=80*80;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
         /*
         *add your code with resultfp32
          */
          free(resultfp32);
		 break;
          }
          case DP_TINY_YOLO_V2_NET:
	  {
	        u16* probabilities = (u16*)result;
                unsigned int resultlen=4320;
                std::vector<DetectedObject> results;
                int result_num=0;
                float* resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
                float* new_data=(float*)malloc(resultlen * sizeof(*new_data));
                int img_width=1280;
                int img_height=960;
                for (u32 i = 0; i < resultlen; i++)
                        resultfp32[i]= f16Tof32(probabilities[i]);
                reshape(resultfp32, new_data,resultlen,40,9,12);
                int dim[2] ={320,416};
                int blockwd =12;
                int blockhd = 9;
                int wh =blockwd*blockwd;
                int targetBlockwd = 13;
                int targetBlockhd = 10;
                int classes = 3;
                float threshold = 0.20;
                float nms = 0.4;
                Region region_obj;
	        region_obj.GetDetections_non_square(new_data,40,blockhd,blockwd,classes,img_width,img_height,threshold,nms,targetBlockwd,targetBlockhd,results);
                num_box_demo= results.size();
                printf("second_results.size():%d\n",results.size());
                for (int i = 0; i <  results.size(); ++i)
                { 
                        printf("second_class:%s, x:%d, y:%d, width:%d, height:%d, probability:%.2f.\n",results[i].name.c_str(),results[i].left,results[i].top,(results[i].right-results[i].left),(results[i].bottom-results[i].top),results[i].confidence);
                        box_demo[i].x1=results[i].left;
                        box_demo[i].x2=results[i].right;
	                if(box_demo[i].x2>img_width)
	 	                box_demo[i].x2=img_width;
                        box_demo[i].y1=results[i].top;
                        box_demo[i].y2=results[i].bottom;
	                if(box_demo[i].y2>img_height)
	 	                box_demo[i].y2=img_height;
	                results[i].name.copy(categoles[i],20, 0);
                }
                free(resultfp32);
                free(new_data);	 
	        break;
}
 case DP_CAFFE_NET:
	  {
		 u16* probabilities=(u16*)result;
		 unsigned int resultlen=128;
		 float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
		 for(u32 i=0;i<resultlen;i++)
			resultfp32[i]=f16Tof32(probabilities[i]);
		 /**
		 add your own code to do with resultfp32
         **/
		 
		 free(resultfp32);
		 break;
}
  }

  return;
}
//ssd+caffenet
void test_whole_model_2_video_model_jingdong(int argc, char *argv[])
{
	int ret;
	const char *filename = "../mobilenetssd.blob";
	const char *filename2 = "../caffenet.blob";

	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,128,128,707*2 },{0,114,114,128*2}};
        dp_netMean mean[2]={{127.5,127.5,127.5,127.5},{104,117,123,255}};
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, parms);
        dp_set_blob_mean_std(blob_nums,mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	ret = dp_update_model_2(filename2);
	if (ret == 0) {
		printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
	}
	else {
		printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
	}

	DP_MODEL_NET net_1=DP_SSD_MOBILI_NET;
	dp_register_box_device_cb(box_callback_model_two_demo, &net_1);	
	DP_MODEL_NET net_2=DP_CAFFE_NET;
	dp_register_second_box_device_cb(box_callback_model_demo,&net_2);
	dp_register_video_frame_cb(video_callback, &net_1);
        dp_register_fps_device_cb(fps_callback,&net_1);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
        while(1){
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

//googlenet
void test_whole_model_1_video_googleNet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../google.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,224,224,1000*2};
        dp_netMean mean={104.0068,116.6886,122.6789,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	FILE *fp=fopen("synset_words.txt","r");
        if(fp==NULL)
        {
                printf("can not open the file\n");
        }
        label_cagerioes=(char **)malloc(1000*sizeof(char*));
        char buffer[500];
        int index_label=0;
        while(fgets(buffer,500,fp)!=NULL)
        {
                label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
                memcpy(label_cagerioes[index_label],buffer,500);
                for(int m=0;m<500;m++)
                {
                        label_cagerioes[index_label][m]=label_cagerioes[index_label][m+10];
                        if(m==489)
                        {
                                break;
                        }
                }
	        label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
                index_label++;
        }
	fclose(fp);
        DP_MODEL_NET net=DP_GOOGLE_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

//yolov1-tinyyolo
void test_whole_model_1_video_TinyYoloNet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../tini_yolo.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,448,448,1470*2};
        dp_netMean mean={0,0,0,255};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	DP_MODEL_NET net=DP_TINI_YOLO_NET;
	dp_register_box_device_cb(box_callback_model_demo,&net);
	dp_register_video_frame_cb(video_callback, &net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

void test_whole_model_1_video_AgeNet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../agenet.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,227,227,8*2};
        dp_netMean mean={78.42633776,87.76891437,114.89584775,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	
	
	DP_MODEL_NET net=DP_AGE_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//gendernet
void test_whole_model_1_video_gendernet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../gendernet.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,227,227,2*2};
        dp_netMean mean={78.42633776,87.76891437,114.89584775,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	
	
        DP_MODEL_NET net=DP_GENDER_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
	dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//resnet
void test_whole_model_1_video_Resnet_18(int argc, char *argv[])
{
	int ret;
	const char *filename = "../Resnet.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,224,224,1000*2};
        dp_netMean mean={104.00698793,116.66876762,122.67891434,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
        FILE *fp=fopen("synset_words.txt","r");
        if(fp==NULL)
        {
                printf("can not open the file\n");
        }
        label_cagerioes=(char **)malloc(1000*sizeof(char*));
        char buffer[500];
        int index_label=0;
        while(fgets(buffer,500,fp)!=NULL)
        {
                label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
                memcpy(label_cagerioes[index_label],buffer,500);
                for(int m=0;m<500;m++)
                {
                        label_cagerioes[index_label][m]=label_cagerioes[index_label][m+10];
                        if(m==489)
                        {
                                break;
                        }
                }
	        label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
                index_label++;
        }
	fclose(fp);	
        DP_MODEL_NET net=DP_RES_NET;
	dp_register_box_device_cb(box_callback_model_demo,&net);
	dp_register_video_frame_cb(video_callback, &net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//squezzse
void test_whole_model_1_video_SqueezeNet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../SqueezeNet.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,227,227,1000*2};
        dp_netMean mean={104.00698793,116.66876762,122.67891434,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}

	FILE *fp=fopen("synset_words.txt","r");
        if(fp==NULL)
        {
                printf("can not open the file\n");
        }
        label_cagerioes=(char **)malloc(1000*sizeof(char*));
        char buffer[500];
        int index_label=0;
        while(fgets(buffer,500,fp)!=NULL)
        {
                label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
                memcpy(label_cagerioes[index_label],buffer,500);
                for(int m=0;m<500;m++)
                {
                        label_cagerioes[index_label][m]=label_cagerioes[index_label][m+10];
                        if(m==489)
                        {
                                break;
                        }
                }
	        label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
                index_label++;
        }
	fclose(fp);
	DP_MODEL_NET net=DP_SQUEEZE_NET;
	dp_register_box_device_cb(box_callback_model_demo,&net);
	dp_register_fps_device_cb(fps_callback,&net);
	dp_register_video_frame_cb(video_callback, &net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
        //while(1){
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//ssd—mobiletnet
void test_whole_model_1_video_SSD_MobileNet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../SSD_MobileNet.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,300,300,707*2};
        dp_netMean mean={127.5,127.5,127.5,127.5};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}

	DP_MODEL_NET net=DP_SSD_MOBILI_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//inception-v1
void test_whole_model_1_video_inception_v1(int argc, char *argv[])
{
	int ret;
	const char *filename = "../inception_v1";

	int blob_nums = 1; dp_blob_parm_t parms = {1,223,223,1001*2};
        dp_netMean mean={128,128,128,128};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	FILE *fp=fopen("categories.txt","r");
        if(fp==NULL)
        {
                printf("can not open the file\n");
        }
        label_cagerioes=(char **)malloc(1002*sizeof(char*));
        char buffer[500];
        int index_label=0;
        while(fgets(buffer,500,fp)!=NULL)
        {
                label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
                memcpy(label_cagerioes[index_label],buffer,500);
	        label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
                index_label++;
        }
	fclose(fp);
	DP_MODEL_NET net=DP_INCEPTION_V1;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

//incetpion-v2
void test_whole_model_1_video_inception_v2(int argc, char *argv[])
{
	int ret;
	const char *filename = "../inception_v2";

	int blob_nums = 1; dp_blob_parm_t parms = {0,224,224,1001*2};
    dp_netMean mean={128,128,128,128};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	
	FILE *fp=fopen("categories.txt","r");
	if(fp==NULL)
	{
	   printf("can not open the file\n");
	}
	label_cagerioes=(char **)malloc(1002*sizeof(char*));
	char buffer[500];
	int index_label=0;
	while(fgets(buffer,500,fp)!=NULL)
	{
	   label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
	   memcpy(label_cagerioes[index_label],buffer,500);
	   label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
	   index_label++;
	}
	fclose(fp);
	DP_MODEL_NET net=DP_INCEPTION_V2;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

//incetption-v3
void test_whole_model_1_video_inception_v3(int argc, char *argv[])
{
	int ret;
	const char *filename = "../inception_v3";

	int blob_nums = 1; dp_blob_parm_t parms = {1,299,299,1001*2};
        dp_netMean mean={128,128,128,128};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	
	FILE *fp=fopen("categories.txt","r");
	if(fp==NULL)
	{
	   printf("can not open the file\n");
	}
	label_cagerioes=(char **)malloc(1002*sizeof(char*));
	char buffer[500];
	int index_label=0;
	while(fgets(buffer,500,fp)!=NULL)
	{
		label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
	     memcpy(label_cagerioes[index_label],buffer,500);
		 label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
		 index_label++;
	}
	fclose(fp);
	DP_MODEL_NET net=DP_INCEPTION_V3;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
	dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//mnist
void test_whole_model_1_video_mnist(int argc, char *argv[])
{
	int ret;
	const char *filename = "../mnist_inference.graph";

	int blob_nums = 1; dp_blob_parm_t parms = {1,28,28,10*2};
    dp_netMean mean={0,0,0,255};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	
	DP_MODEL_NET net=DP_MNIST_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

//mobiletnet
void test_whole_model_1_video_mobilenets(int argc, char *argv[])
{
	int ret;
	const char *filename = "../mobilenets.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {1,224,224,1001*2};
        dp_netMean mean={0,0,0,255};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	FILE *fp=fopen("labels.txt","r");
        if(fp==NULL)
        {
                printf("can not open the file\n");
        }
        label_cagerioes=(char **)malloc(1000*sizeof(char*));
        char buffer[500];
        int index_label=0;
        while(fgets(buffer,500,fp)!=NULL)
        {
                label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
                memcpy(label_cagerioes[index_label],buffer,500);
                for(int m=0;m<500;m++)
                {
                        label_cagerioes[index_label][m]=label_cagerioes[index_label][m+3];
                        if(m==496)
                        {
                                break;
                        }
                }
	        label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
                index_label++;
        }
	fclose(fp);	
	DP_MODEL_NET net=DP_MOBILINERS_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//ssd+google
void test_whole_model_2_video_model(int argc, char *argv[])
{
	int ret;
	const char *filename = "../SSD_MobileNet.blob";
	const char *filename2 = "../google.blob";

	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,300,300,707*2 },{0,224,224,1000*2}};
        dp_netMean mean[2]={{127.5,127.5,127.5,127.5},{104.0068,116.6886,122.6789,1}};
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, parms);
        dp_set_blob_mean_std(blob_nums,mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	ret = dp_update_model_2(filename2);
	if (ret == 0) {
		printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
	}
	else {
		printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
	}
	FILE *fp=fopen("synset_words.txt","r");
        if(fp==NULL)
        {
                printf("can not open the file\n");
        }
        label_cagerioes=(char **)malloc(1000*sizeof(char*));
        char buffer[500];
        int index_label=0;
        while(fgets(buffer,500,fp)!=NULL)
        {
                label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
                memcpy(label_cagerioes[index_label],buffer,500);
                for(int m=0;m<500;m++)
                {
                        label_cagerioes[index_label][m]=label_cagerioes[index_label][m+10];
                        if(m==489)
                        {
                                break;
                        }
                }
	        label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
                index_label++;
        }
	fclose(fp);	
	DP_MODEL_NET net_1=DP_SSD_MOBILI_NET;
	dp_register_box_device_cb(box_callback_model_two_demo, &net_1);	
	DP_MODEL_NET net_2=DP_GOOGLE_NET;
	dp_register_second_box_device_cb(box_callback_model_demo,&net_2);
	dp_register_video_frame_cb(video_callback, &net_1);
        dp_register_fps_device_cb(fps_callback,&net_1);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
        while(1){
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//tiny_yolo_v2
void test_whole_model_1_video_tiny_yolo_v2(int argc, char *argv[])
{
	int ret;
	//const char *filename = "../TINY_YOLO_V2.Blob";//"/home/yu/tini_yolo.blob";
        const char *filename = "../graph_plate_416x320";
	int blob_nums = 1; dp_blob_parm_t parms = {0,416,320,4320*2};
        dp_netMean mean={0,0,0,255};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}	
	DP_MODEL_NET net=DP_TINY_YOLO_V2_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
//双模型tiny_yolo_v2+tiny_yolo_v2
void test_whole_model_2_video_tiny_yolo_v2(int argc, char *argv[])
{
	int ret;
	const char *filename = "../TINY_YOLO_V2.Blob";//"/home/yu/tini_yolo.blob";
        const char *filename2 = "../TINY_YOLO_V2.Blob";
	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,416,416,18000*2},{0,416,416,18000*2}};
        dp_netMean mean[2]={{0,0,0,255},{0,0,0,255}};
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, parms);
        dp_set_blob_mean_std(blob_nums,mean);
	ret = dp_update_model(filename);
        
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	ret = dp_update_model_2(filename2);
	if (ret == 0) {
		printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
	}
	else {
		printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
	}
	DP_MODEL_NET net_1=DP_TINY_YOLO_V2_NET;
	dp_register_box_device_cb(box_callback_model_two_demo, &net_1);	
	DP_MODEL_NET net_2=DP_TINY_YOLO_V2_NET;
	dp_register_second_box_device_cb(box_callback_model_demo,&net_2);
	dp_register_video_frame_cb(video_callback, &net_1);
        dp_register_fps_device_cb(fps_callback,&net_1);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
        while(1){
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}


void test_whole_model_1_video_jieshang(int argc, char *argv[])
{
	int ret;
	const char *filename = "../jieshang.blob";//"/home/yu/tini_yolo.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,224,224,1000*2};
        dp_netMean mean={104,116.7,122.7,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
#if 0	
	DP_MODEL_NET net=DP_TINY_YOLO_V2_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net); 
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
#endif
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}


void test_whole_model_1_video_face(int argc, char *argv[])
{
	int ret;
	const char *filename = "../mvoutput.graph";

	int blob_nums = 1; dp_blob_parm_t parms = {1,320,320,6400*2};
        dp_netMean mean={127,127,127,1};
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, &parms);
        dp_set_blob_mean_std(blob_nums,&mean);
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	
	DP_MODEL_NET net=DP_ALI_FACENET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
        dp_register_fps_device_cb(fps_callback,&net);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		 video_mutex.lock();
                 //clock_t start;
                 //start=clock();
		if (!bgr.empty())
			imshow(win_name, bgr);
 
                //printf("\ninterval = %d ms\n", start);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

testcase_t g_testcases[] =
{
	{"ping", NULL, test_ping},
	{ "version", NULL, test_version },
	{ "hardware_test", NULL, test_hardware_test },	
	{ "get_frame", "get_frame", test_get_frame },
#ifdef SUPPORT_OPENCV
	{ "start_video", "start_video", test_start_video },
#endif
	{ "start_camera", "start_camera", test_start_camera },
#ifdef SUPPORT_OPENCV
	{ "get_frame", "get_frame", test_get_frame },
#endif
	{"stop_camera", "stop_camera", test_stop_camera },
        {"test_whole_model_1_video_SqueezeNet","test_whole_model_1_video_SqueezeNet",test_whole_model_1_video_SqueezeNet},
        {"test_whole_model_1_video_SSD_MobileNet","test_whole_model_1_video_SSD_MobileNet",test_whole_model_1_video_SSD_MobileNet},
        {"test_whole_model_1_video_Resnet_18","test_whole_model_1_video_Resnet_18",test_whole_model_1_video_Resnet_18},
        {"test_whole_model_1_video_gendernet","test_whole_model_1_video_gendernet",test_whole_model_1_video_gendernet},
        {"test_whole_model_1_video_AgeNet","test_whole_model_1_video_AgeNet",test_whole_model_1_video_AgeNet},
        {"test_whole_model_1_video_TinyYoloNet","test_whole_model_1_video_TinyYoloNet",test_whole_model_1_video_TinyYoloNet},
        {"test_whole_model_1_video_googleNet","test_whole_model_1_video_googleNet",test_whole_model_1_video_googleNet},
        {"test_whole_model_1_video_mnist","test_whole_model_1_video_mnist",test_whole_model_1_video_mnist},
        {"test_whole_model_1_video_inception_v3","test_whole_model_1_video_inception_v3",test_whole_model_1_video_inception_v3},
        {"test_whole_model_1_video_inception_v2","test_whole_model_1_video_inception_v2",test_whole_model_1_video_inception_v2},
        {"test_whole_model_1_video_inception_v1","test_whole_model_1_video_inception_v1",test_whole_model_1_video_inception_v1},
        {"test_whole_model_1_video_mobilenets","test_whole_model_1_video_mobilenets",test_whole_model_1_video_mobilenets},
        {"test_whole_model_2_video_model","test_whole_model_2_video_model",test_whole_model_2_video_model},
        {"test_whole_model_1_video_tiny_yolo_v2","test_whole_model_1_video_tiny_yolo_v2",test_whole_model_1_video_tiny_yolo_v2},
        {"test_whole_model_1_video_jieshang","test_whole_model_1_video_jieshang",test_whole_model_1_video_jieshang},
        {"test_whole_model_1_video_face","test_whole_model_1_video_face",test_whole_model_1_video_face},
        {"test_whole_model_2_video_model_jingdong","test_whole_model_2_video_model_jingdong",test_whole_model_2_video_model_jingdong},
        {"test_whole_model_2_video_tiny_yolo_v2","test_whole_model_2_video_tiny_yolo_v2",test_whole_model_2_video_tiny_yolo_v2}
};
int g_case_count = sizeof(g_testcases) / sizeof(testcase_t);

void usage() {
	printf("Usage:\n");
	for (int i = 0; i < g_case_count; ++i) {
		printf("%s\n", (g_testcases[i].usage == NULL) ? g_testcases[i].name : g_testcases[i].usage);
	}
}
int main(int argc, char *argv[])
{
	int i;
    int ret = dp_init();
	if (ret != 0) {
		fprintf(stderr, "dp_init failed!");
        return ret;
	}
        signal(SIGINT,sighandler);
	signal(SIGABRT,sighandler);
	signal(SIGFPE,sighandler);
	signal(SIGILL,sighandler);
	signal(SIGSEGV,sighandler);
	if (argc == 1) {
		for (i = 0; i < g_case_count; i++) {
			g_testcases[i].test(0, NULL);
			printf("----------------------------------------------\n");
		}
	}
	else if (argc > 1) {
		for (i = 0; i < g_case_count; i++) {
			if (strcmp(argv[1], g_testcases[i].name) == 0)
				break;
		}
		if (i < g_case_count) {
			g_testcases[i].test(argc - 2, argc > 2 ? argv + 2 : NULL);
		}
		else {
			usage();
		}
	}
	else {
		usage();
	}
    ret = dp_uninit();
	if (ret != 0) {
		printf("dp_uninit failed!");
			return ret;
	}
	return 0;
}




