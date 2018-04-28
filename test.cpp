#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <math.h>
#include <mutex>
#include <chrono>
#include "dp_api.h"
#include "Fp16Convert.h"
#include "mv_types.h"
#include "interpret_output.h"
#if defined _MSC_VER && defined _WIN64 || defined __linux__
#define SUPPORT_OPENCV
#endif

#ifdef SUPPORT_OPENCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
#endif

using namespace std;
char **label_cagerioes;
dp_image_box_t box_demo[100];
char categoles[100][20];
int num_box_demo=0;
std::mutex video_mutex;
Mat bgr;
static int num_box=1;

#define UNUSED(x) (void)(x)


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
   DP_INCEPTION_V4,
   DP_MOBILINERS_NET,
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

max_age argsort_age(float * a,int length)
{
  max_age max;
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
#ifdef SUPPORT_OPENCV
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
void test_update_model_parems(int blob_nums,dp_blob_parm_t*parmes)
{
  int ret;
  ret=dp_set_blob_parms(blob_nums,parmes);
  if(ret==0)
  	printf("parems transfer ok\n");
  else
  	printf("parems transfer failed\n");
}

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
	     break;
	  }
	  case DP_SSD_MOBILI_NET:
	  {
	     for(int i=0;i<num_box_demo;i++)
             {   
                cv::rectangle(bgr,cvPoint(box_demo[i].x1,box_demo[i].y1),cvPoint(box_demo[i].x2,box_demo[i].y2),CV_RGB(0, 255, 0), 2);
                cv::putText(bgr,categoles[i],cvPoint(box_demo[i].x1,box_demo[i].y1),cv::FONT_HERSHEY_PLAIN,2,CV_RGB(0, 255, 0),2,8);
             }
             break;
	  }
          default:
	     break;	  
      }
	video_mutex.unlock();
}

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
#endif
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
void test_update_firmware(int argc, char *argv[])
{
	int ret;
	const char *filename = "dp_api.dll";
	if (argc > 0)
		filename = argv[0];
	ret = dp_update_firmware(filename);
	if (ret == 0) {
		printf("Test dp_update_firmware(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_firmware(%s) failed ! ret=%d\n", filename, ret);
	}
}

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
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(result_num*sizeof(dp_image_box_t));

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
		   dp_send_first_box_image(box_demo_num, box_second);
		   free(box_second);
		 }
		 else if(result_num>2)
		 { 
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(2*sizeof(dp_image_box_t));
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
		   dp_send_first_box_image(2, box_second);		   
		   free(box_second);
		 }         
        free(resultfp32);
		break;
    }
	default:
		break;
  }
}




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
		max_age pre_age;
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
		 max_age pre_gender;
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
		 printf("num_valid_bxes:%d\n",num_valid_boxes);
		 for(int box_index=0;box_index<num_valid_boxes;box_index++)
		 {
		   int base_index=7*box_index+7;
		   if(resultfp32[base_index+6]<0||resultfp32[base_index+6]>=1||resultfp32[base_index+5]<0||resultfp32[base_index+5]>=1||resultfp32[base_index+4]<0||resultfp32[base_index+4]>=1||resultfp32[base_index+3]<0||resultfp32[base_index+3]>=1||resultfp32[base_index+2]>=1||resultfp32[base_index+2]<0||resultfp32[base_index+1]<0)
		   {
		   	   continue;
		   }
		   printf("%d %f %f %f %f %f\n",int(resultfp32[base_index+1]),resultfp32[base_index+2],resultfp32[base_index+3],resultfp32[base_index+4],resultfp32[base_index+5],resultfp32[base_index+6]);
		   box_demo[num_box_demo].x1=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
		   box_demo[num_box_demo].x2=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
		   box_demo[num_box_demo].y1=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;	   
		   box_demo[num_box_demo].y2=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
		   memcpy(categoles[num_box_demo],category[int(resultfp32[base_index+1])],20);
		   num_box_demo++;
		 }
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
		case DP_INCEPTION_V4:
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
  }

  return;
}
void test_whole_model_1_video_alexnet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../alexnet.blob";//"/home/yu/tini_yolo.blob";

	int blob_nums = 1; dp_blob_parm_t parms = {0,227,227,1000*2};
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
	
	DP_MODEL_NET net=DP_ALEX_NET;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
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
	
	dp_register_video_frame_cb(video_callback, &net);
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


void test_whole_model_1_video_inception_v2(int argc, char *argv[])
{
	int ret;
	const char *filename = "../inception_v2";

	int blob_nums = 1; dp_blob_parm_t parms = {1,224,224,1001*2};
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

void test_whole_model_1_video_inception_v4(int argc, char *argv[])
{
	int ret;
	const char *filename = "../inception_v4";

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
	DP_MODEL_NET net=DP_INCEPTION_V4;
	dp_register_video_frame_cb(video_callback, &net);
	dp_register_box_device_cb(box_callback_model_demo,&net);
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

void test_whole_model_2_video_model(int argc, char *argv[])
{
	int ret;
	const char *filename = "../tini_yolo.blob";
	const char *filename2 = "../google.blob";

	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,448,448,1470*2 },{0,224,224,1000*2}};
    dp_netMean mean[2]={{0,0,0,255},{104.0068,116.6886,122.6789,1}};

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
	
	DP_MODEL_NET net_1=DP_TINI_YOLO_NET;
	dp_register_box_device_cb(box_callback_model_two_demo, &net_1);	
	DP_MODEL_NET net_2=DP_GOOGLE_NET;
	dp_register_second_box_device_cb(box_callback_model_demo,&net_2);
	dp_register_video_frame_cb(video_callback, &net_1);
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

testcase_t g_testcases[] =
{
	{"ping", NULL, test_ping},
	{ "version", NULL, test_version },
	{ "hardware_test", NULL, test_hardware_test },	
	{ "update_firmware", "update_firmware $filename", test_update_firmware },
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
    {"test_whole_model_1_video_alexnet","test_whole_model_1_video_alexnet",test_whole_model_1_video_alexnet},
    {"test_whole_model_1_video_mnist","test_whole_model_1_video_mnist",test_whole_model_1_video_mnist},
    {"test_whole_model_1_video_inception_v4","test_whole_model_1_video_inception_v4",test_whole_model_1_video_inception_v4},
    {"test_whole_model_1_video_inception_v3","test_whole_model_1_video_inception_v3",test_whole_model_1_video_inception_v3},
    {"test_whole_model_1_video_inception_v2","test_whole_model_1_video_inception_v2",test_whole_model_1_video_inception_v2},
    {"test_whole_model_1_video_inception_v1","test_whole_model_1_video_inception_v1",test_whole_model_1_video_inception_v1},
    {"test_whole_model_1_video_mobilenets","test_whole_model_1_video_mobilenets",test_whole_model_1_video_mobilenets},
    {"test_whole_model_2_video_model","test_whole_model_2_video_model",test_whole_model_2_video_model},
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




