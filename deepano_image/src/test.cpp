#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mutex>

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
	{"version", NULL, test_version },
	{"mulitGraph", "", multi_model_sample},
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
		printf("dp_uninit failed!\n");
		return ret;
	}
	return 0;
}
