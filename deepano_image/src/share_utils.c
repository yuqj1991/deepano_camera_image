#include "share_utils.h"
#define UNUSED(x) (void)(x)

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
void test_update_model_parems(int blob_nums,dp_blob_parm_t*parmes)
{
        int ret;
        ret=dp_set_blob_parms(blob_nums,parmes);
        if(ret==0)
  	        printf("parems transfer ok\n");
        else
  	        printf("parems transfer failed\n");
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
//捕获异常
void sighandler(int signum) {
	switch (signum) {
		case SIGABRT: {
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");
			}
			printf("捕获信号 %d，程序异常中断，跳出...\n", signum);
			exit(1);
			break;
		}
		case SIGFPE: {
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");
			}
			printf("捕获信号 %d，算数出错，如为0,跳出...\n", signum);
			exit(1);
			break;
		}
		case SIGILL: {
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");
			}
			printf("捕获信号 %d，非法函数映象，如非法指令，跳出...\n", signum);
			exit(1);
			break;
		}
		case SIGINT: {
			dp_send_stop_cmd();
			int ret = dp_uninit();
			if (ret != 0) {
				printf("dp_uninit failed!");
			}
			printf("捕获信号 %d，程序ctrl_c中断，跳出...\n", signum);
			exit(1);
			break;
		}
		case SIGSEGV: {
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
