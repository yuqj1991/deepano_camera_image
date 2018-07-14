// Deepano_VS2015.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <signal.h>
#include "dp_api.h"
#include "mv_types.h"
#include "Fp16Convert.h"

#if defined _MSC_VER && defined _WIN64 || defined __linux__ || _WIN32
#define SUPPORT_OPENCV
#endif
#ifdef SUPPORT_OPENCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#endif
#define UNUSED(x) (void)(x)
using namespace cv;
std::mutex video_mutex;
Mat bgr;

double Sum_blob_parse_time = 0;
double blob_parse_stage[400];
std::string blob_parse;
int blob_stage_index;

int32_t fps;

dp_image_box_t BLOB_IMAGE_SIZE = { 0,1280,0,960 };

typedef struct _box
{
	int xmin;
	int ymin;
	int xmax;
	int ymax;
	char category[20];
}Box;

int num_box_demo = 0;
Box box_demo[100];

typedef enum NET_CAFFE_TENSFLOW
{
	DP_AGE_NET = 0,
	DP_ALEX_NET,
	DP_GOOGLE_NET,
	DP_GENDER_NET,
	DP_TINI_YOLO_NET,
	DP_SSD_MOBILI_NET,
	DP_RES_NET,
	DP_SQUEEZE_NET,
	DP_MNIST_NET,
	DP_INCEPTION_V1,
	DP_MOBILINERS_NET,
	DP_TINY_YOLO_V2_NET,
} DP_MODEL_NET;

typedef void(*test_case_fun_t)(int argc, char *argv[]);
typedef struct testcase_t {
	const char *name;
	const char *usage;
	test_case_fun_t test;
} testcase_t;


void fps_callback(int32_t *buffer_fps, void *param)
{
	fps = *(buffer_fps);
	printf("fps--:%d\n", fps);
}

//解析模型时间回调函数
void blob_parse_callback(double *buffer_fps, void *param)
{
	for (int stage = 0; stage<200; stage++)
	{
		blob_parse_stage[stage] = buffer_fps[stage * 2 + 0];
		blob_stage_index = buffer_fps[stage * 2 + 1];
		Sum_blob_parse_time += blob_parse_stage[stage];
		std::ostringstream   ostr;
		ostr << "the" << stage << "stage parse spending" << blob_parse_stage[stage] << "ms,and optType:" << OP_NAMES[blob_stage_index] << "\n";
		blob_parse.append(ostr.str());
		if ((stage + 1)<200)
		{
			if (buffer_fps[(stage + 1) * 2 + 0] == 0)
				break;
		}
	}     std::ostringstream   ostr;
	ostr << "the total spending " << Sum_blob_parse_time << " ms\n";
	blob_parse.append(ostr.str());
	Sum_blob_parse_time = 0;
}

void sighandler(int signum)
{
	switch (signum)
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

void video_callback(dp_img_t *img, void *param)
{
	Mat myuv(img->height + img->height / 2, img->width, CV_8UC1, img->img);
	video_mutex.lock();
	cvtColor(myuv, bgr, CV_YUV2BGR_I420, 0);
	DP_MODEL_NET model = *((DP_MODEL_NET*)param);
	switch (model)
	{
	case DP_SSD_MOBILI_NET:
	{
		for (int i = 0; i < num_box_demo; i++)
		{
			cv::rectangle(bgr, cvPoint(box_demo[i].xmin, box_demo[i].ymin), cvPoint(box_demo[i].xmax, box_demo[i].ymax), CV_RGB(0, 255, 0), 2);
			cv::putText(bgr, box_demo[i].category, cvPoint(box_demo[i].xmin, box_demo[i].ymin), cv::FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 255, 0), 2, 8);
		}
		std::string buffer = "fps:" + std::to_string(fps);
		cv::putText(bgr, buffer.c_str(), cvPoint(40, 40), cv::FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 255, 0), 2, 8);
		break;
	}
	default:
	{
		std::string buffer = "fps:" + std::to_string(fps);
		cv::putText(bgr, buffer.c_str(), cvPoint(40, 40), cv::FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 255, 0), 2, 8);
		break;
	}
	video_mutex.unlock();
	}
}
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
void test_update_model_parems(int blob_nums, dp_blob_parm_t*parmes)
{
	int ret;
	ret = dp_set_blob_parms(blob_nums, parmes);
	if (ret == 0)
		printf("parems transfer ok\n");
	else
		printf("parems transfer failed\n");
}
void test_set_camera_config(int argc, char *argv[])
{
	int ret;
	dp_camera_config_t config = { 640, 480, 30, 0 };

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
	int param = 15;
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

void cdk_result_model(void *result, void *param)
{
	DP_MODEL_NET model = *((DP_MODEL_NET*)param);
	switch (model)
	{
	case DP_SSD_MOBILI_NET:
	{
		char *category[] = { "background","aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
			"dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor" };
		u16* probabilities = (u16*)result;
		unsigned int resultlen = 707;
		float*resultfp32;
		resultfp32 = (float*)malloc(resultlen * sizeof(*resultfp32));
		int img_width = 1280;
		int img_height = 960;
		for (u32 i = 0; i < resultlen; i++)
			resultfp32[i] = f16Tof32(probabilities[i]);
		int num_valid_boxes = int(resultfp32[0]);
		int index = 0;
		printf("num_valid_bxes:%d\n", num_valid_boxes);
		for (int box_index = 0; box_index < num_valid_boxes; box_index++)
		{
			int base_index = 7 * box_index + 7;
			if (resultfp32[base_index + 6] < 0 || resultfp32[base_index + 6] >= 1 || resultfp32[base_index + 5] < 0 || resultfp32[base_index + 5] >= 1 || resultfp32[base_index + 4] < 0 || resultfp32[base_index + 4] >= 1 || resultfp32[base_index + 3] < 0 || resultfp32[base_index + 3] >= 1 || resultfp32[base_index + 2] >= 1 || resultfp32[base_index + 2] < 0 || resultfp32[base_index + 1] < 0)
			{
				continue;
			}
			printf("%d %f %f %f %f %f\n", int(resultfp32[base_index + 1]), resultfp32[base_index + 2], resultfp32[base_index + 3], resultfp32[base_index + 4], resultfp32[base_index + 5], resultfp32[base_index + 6]);
			box_demo[index].xmin = (int(resultfp32[base_index + 3] * img_width) > 0) ? int(resultfp32[base_index + 3] * img_width) : 0;
			box_demo[index].xmax = (int(resultfp32[base_index + 5] * img_width) < img_width) ? int(resultfp32[base_index + 5] * img_width) : img_width;
			box_demo[index].ymin = (int(resultfp32[base_index + 4] * img_height) > 0) ? int(resultfp32[base_index + 4] * img_height) : 0;
			box_demo[index].ymax = (int(resultfp32[base_index + 6] * img_height) < img_height) ? int(resultfp32[base_index + 6] * img_height) : img_height;
			memcpy(box_demo[index].category, category[int(resultfp32[base_index + 1])], 20);
			index++;
		}
		num_box_demo = index;
		free(resultfp32);
		break;
	}
	}
	return;
}

	void test_SSD_MobileNet(int argc, char *argv[])
	{
		int ret;
		const char *filename = "../SSD_MobileNet.blob";

		int blob_nums = 1; dp_blob_parm_t parms = { 0,300,300,707 * 2 };
		dp_netMean mean = { 127.5,127.5,127.5,127.5 };
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
		dp_set_blob_mean_std(blob_nums, &mean);
		ret = dp_update_model(filename);
		if (ret == 0) {
			printf("Test dp_update_model(%s) sucessfully!\n", filename);
		}
		else {
			printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
		}

		DP_MODEL_NET net = DP_SSD_MOBILI_NET;
		dp_register_video_frame_cb(video_callback, &net);
		dp_register_box_device_cb(cdk_result_model, &net);
		dp_register_fps_device_cb(fps_callback, &net);
		dp_register_parse_blob_time_device_cb(blob_parse_callback, NULL);
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

testcase_t g_testcases[] =
{
		{ "ping", NULL, test_ping },
		{ "version", NULL, test_version },
		{ "start_video", "start_video", test_start_video },
		{ "start_camera", "start_camera", test_start_camera },
		{ "stop_camera", "stop_camera", test_stop_camera },
		{ "test_SSD_MobileNet","test_SSD_MobileNet",test_SSD_MobileNet },
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
	signal(SIGINT, sighandler);
	signal(SIGABRT, sighandler);
	signal(SIGFPE, sighandler);
	signal(SIGILL, sighandler);
	signal(SIGSEGV, sighandler);
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

