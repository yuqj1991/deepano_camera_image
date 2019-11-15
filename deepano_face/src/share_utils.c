//
// Created by chenchen on 11/07/18.
//
#include "share_utils.h"

extern std::mutex video_mutex;
extern cv::Mat bgr;
extern void video_callback(dp_img_t *img, void *param);
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
    dp_stop_camera();
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

        cv::Mat myuv(img.height + img.height / 2, img.width, CV_8UC1, img.img);
        cvtColor(myuv, bgr, CV_YUV2BGR_I420, 0);
        cv::imshow("DP_FRAME",bgr);
        cv::waitKey(0);
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

#endif