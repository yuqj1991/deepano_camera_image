//
// Created by chenchen on 11/07/18.
//
#include "share_utils.h"

//信号捕捉事件
void sighandler(int signum)
{
    switch(signum)
    {
        case SIGABRT:
        {
            dpExitMovidius();
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
            dpExitMovidius();
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
            dpExitMovidius();
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
            dpExitMovidius();
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
            dpExitMovidius();
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
