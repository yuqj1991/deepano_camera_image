#include "model_sample.h"

using namespace std;

typedef void(*test_case_fun_t)(int argc, char *argv[]);
typedef struct testcase_t {
	const char *name;
	const char *usage;
	test_case_fun_t test;
} testcase_t;


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
	{"squeezeNet","test_SqueezeNet",test_SqueezeNet},
	{"ssd_mobileNet","test_SSD_MobileNet",test_SSD_MobileNet},
	{"resnet_18","test_Resnet_18",test_Resnet_18},
	{"gendernet","test_gendernet",test_gendernet},
	{"ageNet","test_AgeNet",test_AgeNet},
	{"tinyyolo","tinyyolo",test_TinyYoloNet},
	{"googleNet","googleNet",test_googleNet},
	{"test_mnist","test_mnist",test_mnist},
	{"test_inception","test_inception",test_inception},
	{"test_mobilenets","test_mobilenets",test_mobilenets},
	{"test_ssd_googlenet_model","test_ssd_googlenet_model",test_ssd_googlenet_model},
	{"tiny_yolov2","tiny_yolov2",test_tiny_yolo_v2},
	{"test_ssd_resnet_model","test_ssd_resnet_model",test_ssd_resnet_model}
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




