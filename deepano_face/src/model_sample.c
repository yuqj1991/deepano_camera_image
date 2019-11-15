//
// Created by chenchen on 12/07/18.
//

#include "model_sample.h"
#include <unistd.h>
#include <dirent.h>

extern std::mutex video_mutex;
extern cv::Mat bgr;
dp_image_box_t BLOB_IMAGE_SIZE={0,1280,0,960};

extern void fps_callback(int32_t *buffer_fps,void *param);
extern void blob_parse_callback(double *buffer_fps,void *param);
extern void video_callback(dp_img_t *img, void *param);
extern void mtcnn_face_model(FaceResult *mresult,void *param);

#define  PNetGraph0_FILENAME  "./model/pNet_graph00"
#define  PNetGraph1_FILENAME  "./model/pNet_graph11"
#define  PNetGraph2_FILENAME  "./model/pNet_graph22"
#define  PNetGraph3_FILENAME  "./model/pNet_graph33"
#define  PNetGraph4_FILENAME  "./model/pNet_graph44"
#define  PNetGraph5_FILENAME  "./model/pNet_graph55"
#define  PNetGraph6_FILENAME  "./model/pNet_graph66"
#define  PNetGraph7_FILENAME  "./model/pNet_graph77"
#define	 PNET_NUM 8
char rNetGraph_filename[]= "./model/rNet_graph";
char oNetGraph_filename[]= "./model/oNet_graph";
char fNetGraph_filename[]= "./model/fNet_graph";
char* pNetGraph_filename[PNET_NUM] = {PNetGraph0_FILENAME, PNetGraph1_FILENAME, PNetGraph2_FILENAME, PNetGraph3_FILENAME, PNetGraph4_FILENAME, PNetGraph5_FILENAME, PNetGraph6_FILENAME, PNetGraph7_FILENAME};

void deepano_ssd_facenet(int argc, char *argv[])
{
	int ret;
	const char *filename = "./model/face_detector.graph";
   	const char *filename2 = "./model/fNet_graph";
   	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,300,300,707*2 },{0,160,160,512*2}};
   	dp_netMean mean[2]={{0,0,0,255},{127.5,127.5,127.5,128}};
   	dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
   	test_update_model_parems(blob_nums, parms);
   	dp_set_blob_mean_std(blob_nums,mean);
   	ret = dp_update_model(filename);
   	if (ret == 0) {
       	printf("Test dp_update_model_1(%s) sucessfully!\n", filename);
   	}
   	else {
       	printf("Test dp_update_model_1(%s) failed ! ret=%d\n", filename, ret);
   	}
   	ret = dp_update_model_2(filename2);
   	if (ret == 0) {
       	printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
   	}
   	else {
       	printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
   	}
   	DP_MODEL_NET net_1=DP_SSD_MOBILI_NET;
   	dp_register_detector_face_device_cb(mtcnn_face_model,&net_1);
   	dp_register_video_frame_cb(video_callback, &net_1);
   	ret = dp_start_camera_video();
   	if (ret == 0) {
       	printf("Test test_start_video successfully!\n");
  	}
   	else {
       	printf("Test test_start_video failed! ret=%d\n", ret);
   	}
    const char *win_name = "video";
    cv::namedWindow(win_name);
    int key = -1;
    for (;;) {
        video_mutex.lock();
        if (!bgr.empty())
        	cv::imshow(win_name, bgr);
        video_mutex.unlock();
        key = cv::waitKey(30);
    }
    cv::destroyWindow(win_name);
}

void deepano_mtcnn_facenet(int argc, char *argv[])
{
	int ret;
	for(int ii =0; ii<PNET_NUM; ii++){
		ret = dp_update_pmodel(pNetGraph_filename[ii]);
		if (ret == 0) {
       		printf("Test dp_update_model_p(%s) sucessfully!\n", pNetGraph_filename[ii]);
   		}
   		else {
       		printf("Test dp_update_model_p(%s) failed ! ret=%d\n", pNetGraph_filename[ii], ret);
   		}
	}
	ret = dp_update_rmodel(rNetGraph_filename);
	if (ret == 0) {
   		printf("Test dp_update_model_r(%s) sucessfully!\n", rNetGraph_filename);
	}
	else {
   		printf("Test dp_update_model_r(%s) failed ! ret=%d\n", rNetGraph_filename, ret);
	}
	ret = dp_update_omodel(oNetGraph_filename);
	if (ret == 0) {
   		printf("Test dp_update_model_o(%s) sucessfully!\n", oNetGraph_filename);
	}
	else {
   		printf("Test dp_update_model_o(%s) failed ! ret=%d\n", oNetGraph_filename, ret);
	}
	DP_MODEL_NET net_1=DP_MTCNN_FACENET;
	//dp_register_detector_face_device_cb(mtcnn_face_model,&net_1);
   	dp_register_video_frame_cb(video_callback, &net_1);
   	ret = dp_start_camera_video();
   	if (ret == 0) {
       	printf("Test test_start_video successfully!\n");
  	}
   	else {
       	printf("Test test_start_video failed! ret=%d\n", ret);
   	}
    const char *win_name = "video";
    cv::namedWindow(win_name);
    int key = -1;
    for (;;) {
        video_mutex.lock();
        if (!bgr.empty())
        	cv::imshow(win_name, bgr);
        video_mutex.unlock();
        key = cv::waitKey(30);
    }
    cv::destroyWindow(win_name);
}
