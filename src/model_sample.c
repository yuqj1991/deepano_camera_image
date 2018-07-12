//
// Created by chenchen on 12/07/18.
//

#include "model_sample.h"

char **label_cagerioes;
extern std::mutex video_mutex;
extern cv::Mat bgr;
dp_image_box_t box_second[2];
dp_image_box_t BLOB_IMAGE_SIZE={0,1280,0,960};
unsigned short Resnet_Image_Buffer[224*224*3*50];


extern void fps_callback(int32_t *buffer_fps,void *param);
extern void blob_parse_callback(double *buffer_fps,void *param);
extern void video_callback(dp_img_t *img, void *param);
extern void cdk_result_model(void *result,void *param);
extern void cdk_two_result_model(void *result,void *param);


void test_googleNet(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_TinyYoloNet(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_AgeNet(int argc, char *argv[]) {
    int ret;
    const char *filename = "../agenet.blob";

    int blob_nums = 1;
    dp_blob_parm_t parms = {0, 227, 227, 8 * 2};
    dp_netMean mean = {78.42633776, 87.76891437, 114.89584775, 1};
    if (argc > 0) {
        filename = argv[0];
        blob_nums = atoi(argv[1]);
        for (int i = 0; i < blob_nums; i++) {
            parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
            parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
            parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
        }
    }
    dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
    test_update_model_parems(blob_nums, &parms);
    dp_set_blob_mean_std(blob_nums, &mean);
    ret = dp_update_model(filename);
    if (ret == 0) {
        printf("Test dp_update_model(%s) sucessfully!\n", filename);
    } else {
        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
    }


    DP_MODEL_NET net = DP_AGE_NET;
    dp_register_video_frame_cb(video_callback, &net);
    dp_register_box_device_cb(cdk_result_model, &net);
    dp_register_fps_device_cb(fps_callback, &net);
    dp_register_parse_blob_time_device_cb(blob_parse_callback, NULL);
    ret = dp_start_camera_video();
    if (ret == 0) {
        printf("Test test_start_video successfully!\n");
    } else {
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
void test_gendernet(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_Resnet_18(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_SqueezeNet(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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
    dp_register_box_device_cb(cdk_result_model,&net);
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
void test_inception(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_mnist(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_mobilenets(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_ssd_resnet_model(int argc, char *argv[])
{
    int ret;
    const char *filename = "../SSD_MobileNet.blob";
    const char *filename2 = "../Resnet.blob";

    int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,300,300,707*2 },{0,224,224,1000*2}};
    dp_netMean mean[2]={{127.5,127.5,127.5,127.5},{104.00698793,116.66876762,122.67891434,1}};
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
    dp_register_box_device_cb(/*box_callback_model_two_demo*/cdk_result_model, &net_1);
    DP_MODEL_NET net_2=DP_RES_NET;
    dp_register_second_box_device_cb(cdk_result_model,&net_2);
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
    const char *filename = "../TINY_YOLO_V2.Blob";
    int blob_nums = 1; dp_blob_parm_t parms = {0,640,640,4320*2};
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
    dp_register_box_device_cb(cdk_result_model,&net);
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

void test_ssd_googlenet_model(int argc, char *argv[])
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
    dp_register_box_device_cb(cdk_two_result_model, &net_1);
    DP_MODEL_NET net_2=DP_GOOGLE_NET;
    dp_register_second_box_device_cb(cdk_result_model,&net_2);
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
