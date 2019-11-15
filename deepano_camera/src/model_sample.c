//
// Created by chenchen on 12/07/18.
//

#include "model_sample.h"

char **label_cagerioes;
extern std::mutex video_mutex;
extern cv::Mat bgr;

extern void video_callback(dp_img_t *img, void *param);
extern void cdk_result_model(void *result,void *param);


void test_TinyYoloNet(int argc, char *argv[])
{
    int ret;
    const char *filename = "../model/tiny_yolo.graph";

    int blob_nums = 1; dp_blob_parm_t parms = {448,448,1470*2, 0,0,0,255};
    dpSetGraphParam(blob_nums, &parms);
    ret = dpUploadGeneralModel(filename);
    if (ret == 0) {
        printf("Test dp_update_model(%s) sucessfully!\n", filename);
    }
    else {
        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
    }
    DP_MODEL_NET net=DP_TINI_YOLO_NET;
    dp_register_video_frame_cb(video_callback, &net);
    dp_register_inference_db(cdk_result_model,&net);
    ret = dpCamNeuralInference();
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

void test_SSD_MobileNet(int argc, char *argv[])
{
    int ret;
    const char *filename = "../model/SSD_MobileNet.graph";

    int blob_nums = 1; dp_blob_parm_t parms = {300,300,707*2, 127.5,127.5,127.5,127.5};
    dpSetGraphParam(blob_nums, &parms);
    ret = dpUploadGeneralModel(filename);
    if (ret == 0) {
        printf("Test dp_update_model(%s) sucessfully!\n", filename);
    }
    else {
        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
    }

    DP_MODEL_NET net=DP_SSD_MOBILI_NET;
    dp_register_video_frame_cb(video_callback, &net);
    dp_register_inference_db(cdk_result_model,&net);
    ret = dpCamNeuralInference();
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
        if (!bgr.empty()){
            cv::imshow(win_name, bgr);
        }

        video_mutex.unlock();
        key = cv::waitKey(30);
    }
    cv::destroyWindow(win_name);
}

void test_mobilenets(int argc, char *argv[])
{
    int ret;
    const char *filename = "../model/mobilenets.blob";
    int blob_nums = 1; dp_blob_parm_t parms = {224,224,1001*2, 0,0,0,255};
    dpSetGraphParam(blob_nums, &parms);
    ret = dpUploadGeneralModel(filename);
    if (ret == 0) {
        printf("Test dp_update_model(%s) sucessfully!\n", filename);
    }
    else {
        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
    }
    /*FILE *fp=fopen("doc/labels.txt","r");
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
    fclose(fp);*/
    DP_MODEL_NET net=DP_MOBILINERS_NET;
    dp_register_video_frame_cb(video_callback, &net);
    dp_register_inference_db(cdk_result_model,&net);//(推理后处理，根据实际模型，自行处理)
    ret = dpCamNeuralInference();
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

void test_tiny_yolo_v2(int argc, char *argv[])
{
    int ret;
    const char *filename = "../model/tiny_yolo_v2.graph";  //yuqj
    int blob_nums = 1; dp_blob_parm_t parms = {416,416,20*2, 0,0,0,255};  //yuqj
    dpSetGraphParam(blob_nums, &parms);
    ret = dpUploadGeneralModel(filename);
    if (ret == 0) {
        printf("Test dp_update_model(%s) sucessfully!\n", filename);
    }
    else {
        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
    }
    DP_MODEL_NET net=DP_TINY_YOLO_V2_NET;
    dp_register_video_frame_cb(video_callback, &net);
    dp_register_inference_db(cdk_result_model,&net);
    ret = dpCamNeuralInference();
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
        if (!bgr.empty()){
			cv::imshow(win_name, bgr);
        }
        video_mutex.unlock();
        key = cv::waitKey(30);
    }
    cv::destroyWindow(win_name);
}
