#ifndef __DP_API_H__
#define __DP_API_H__

#include "dp_api_type.h"


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

/** \ingroup dp_video_frame_cb_t
* video frame callback function type.
*
* Invoke this callback when receving a video frame,
* you pass a pointer to a callback function of this type.
*
* \param[in] img the video frame.
* \param[in] param user's param.
* \returns void
*/
typedef void(*dp_video_frame_cb_t)(dp_img_t *img, void *param);

/** \ingroup dp_first_blob_out_back_cb_t
* host parseing box image to device func callack type.
*Invoke this callback when receving a first_result
*you pass a pointer to a callback function of this type.
*param[in] first blob result.
*param[in] param user's param.
* \returns void
*/
typedef void(*dp_first_blob_outresult_back_cb_t)(void * result,void *param);

typedef void(*dp_get_fps_cb_t)(int32_t *fps,void *param);

typedef void(*dp_second_blob_outresult_back_cb_t)(void * result,void *param);

typedef void(*dp_parse_blob_stage_time_cb_t)(double * result,void *param);

/** \ingroup dp_first_blob_out_back_cb_t
* host parseing box image to device func callack type.
*Invoke this callback when receving a first_result and give the boxes
*you pass a pointer to a callback function of this type.
*param[in] a pointer to the image box.
*param[in] param user's param.
* \returns void
*/
typedef void(*dp_send_to_device_box_cb_t)(dp_image_box_t *box,void *param);


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

/** \ingroup dp_hardware_test
* Get the hardware status infomation of the device.
*
* \param[out] ver return version infomation of the device.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_hardware_test(dp_hw_status_t *status);

/** \ingroup dp_start_camera
* Only start the camera of device,no output frame buffer.
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_start_camera();

/** \ingroup dp_start_camera
* Start the camera of device,and display the video with opencv.
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_start_camera_video();

/** \ingroup dp_stop_camera
* Stop the camera of device.
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_stop_camera();

/** \ingroup dp_register_video_frame_cb
* Register video frame callback, 
* Invoke  callback when receving a video frame.
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_register_video_frame_cb(dp_video_frame_cb_t cb, void *param);

/** \ingroup dp_register_video_frame_cb
* Register box to device callback, 
* Invoke  callback when receving firstresult.
*
* \returns 0 if sucess, <0 if error occured.
*/

EXTERN DLL int dp_register_fps_device_cb(dp_get_fps_cb_t cb, void *param);

EXTERN DLL int dp_register_box_device_cb(dp_first_blob_outresult_back_cb_t cb, void *param);

EXTERN DLL int dp_register_second_box_device_cb(dp_second_blob_outresult_back_cb_t cb, void *param);

EXTERN DLL int dp_register_parse_blob_time_device_cb(dp_parse_blob_stage_time_cb_t cb, void *param);

/** \ingroup dp_set_camera_config
* Set camera configuration.
*
* \param[in] config the configuration of camera
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_set_camera_config(dp_camera_config_t *config);

/** \ingroup dp_get_camera_config
* Get camera configuration.
*
* \param[out] config return config the dp_get_camera_config of camera
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_get_camera_config(dp_camera_config_t * config);

/** \ingroup dp_set_blob_parms()
* send blob parameters like inputsize width and height ,outputnum.
*
* 
* \returns 0 if sucess, <0 if error occured.
*/

EXTERN DLL int dp_set_blob_parms(int num_model,dp_blob_parm_t *param);

EXTERN DLL int dp_set_blob_mean_std(int num_model,dp_netMean *param);

/** \ingroup dp_update_model
* update algorithm model.
*
* \param[in] filename file name of algorithm model.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_update_model(const char* filename);

/** \ingroup dp_update_model_2
* update algorithm model.
*
* \param[in] filename file name of algorithm model.
* \returns 0 if sucess, <0 if error occured.
*/

EXTERN DLL int dp_update_model_2(const char* filename);

EXTERN DLL int dp_send_first_box_image(int box_num,dp_image_box_t *box);
/** \ingroup dp_update_firmware
* update firmware.
*
* \param[in] filename file name of firmware.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_update_firmware(const char* filename);


/** \ingroup dp_get_frame
* capture a frame and return image data.
*
* \param[out] img ponit to an instance of dp_img_t, save image in it.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_get_frame(dp_img_t* img);

/** \ingroup dp_release_frame
* img release the memory used by image
*
* \param[in] img ponit to an instance of dp_img_t that will be release.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_release_frame(dp_img_t* img);



#endif
