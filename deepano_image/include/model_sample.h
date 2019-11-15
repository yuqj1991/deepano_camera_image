#ifndef MODEL_EXAMPLAMES_
#define MODEL_EXAMPLAMES_
#include "share_utils.h"
#include <pthread.h>
#include <thread>
#include <chrono>
#include "pre_image.h"
#ifdef __cplusplus
extern "C"{
#endif
typedef enum model_sample{
    SSD_MODEL_TEST,
    GOOGLE_MODEL_TEST
}SAMPLE_TEST;

void multi_model_sample(int argc,char *argv[]);

#ifdef __cplusplus
}
#endif
#endif
