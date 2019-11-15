#ifndef __DP_API_H__
#define __DP_API_H__

#include "dp_api_type.h"
#include "mv_types.h"

#ifdef _MSC_VER
#ifdef _WINDLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#else
#define DLL __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

//NCSDK
typedef void(*dp_ncsdk_first_blob_outresult_back_cb_t)(void *result,void *param);
//NCSDK
typedef void(*dp_ncsdk_second_blob_outresult_back_cb_t)(void *result,void *param);

typedef void(*dp_parse_blob_stage_time_cb_t)(double * result,void *param);
typedef void(*dp_tuling_result_cb_t)(dp_FaceDetResult* result,void *param);
/********************************************
general purpose functions
*******************************************/

/** \ingroup dp_init
* Initialize deepano API library
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_init();

/** \ingroup dp_init
* Uninitialize deepano API library
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_uninit();

/** \ingroup dp_ping
* Ping the device
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_ping();

/** \ingroup dp_version
* Get the version infomation of the device.
*
* \param[out] ver return version infomation of the device.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_version(dp_ver_t* ver);


EXTERN DLL int dp_register_ncsdk_first_result_device_cb(dp_ncsdk_first_blob_outresult_back_cb_t cb, void *param);


EXTERN DLL int dp_register_parse_blob_time_device_cb(dp_parse_blob_stage_time_cb_t cb, void *param);

EXTERN DLL int dp_register_tuling_result(dp_tuling_result_cb_t cb,void *param);

/** \ingroup dp_set_blob_parms()
* send blob parameters like inputsize width and height ,outputnum.
*
* 
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_set_blob_box_shape(int num,dp_box_shapearray *param);

EXTERN DLL int dp_set_blob_detector(int num,dp_detector *param);

EXTERN DLL int dp_set_blob_parms(int num_model,dp_blob_parm_t *param);

/** \ingroup dp_update_model
* update algorithm model.
*
* \param[in] filename file name of algorithm model.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_update_model(const char* filename);


EXTERN DLL int dp_send_stop_cmd();

/** \ingroup dp_send_first_image
* send image buffer.
*
* 
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_send_first_image(unsigned short* imagebuffer,int buffersize, int graph_stream_id);

EXTERN DLL int dp_send_first_tuling_image(unsigned char* imagebuffer,int buffersize);


/** \ingroup dp_update_firmware
* update firmware.
*
* \param[in] filename file name of firmware.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_update_firmware(const char* filename);

float* movidius_inference_result(unsigned short *imageBuf, int BufSize, unsigned int graph_stream_id, unsigned int graphresultLength);

#endif
