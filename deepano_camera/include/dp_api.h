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
EXTERN DLL int dpCamNeuralInference();

/** \ingroup dp_stop_camera
* Stop the camera of device.
*
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_stop_camera();


/** \ingroup dp_update_firmware
* update firmware.
*
* \param[in] filename file name of firmware.
* \returns 0 if sucess, <0 if error occured.
*/
EXTERN DLL int dp_update_firmware(const char* filename);



EXTERN DLL int dpUploadGeneralModel(const char* filename);
EXTERN DLL int dpSetGraphParam(int num_model,dp_blob_parm_t *param);
typedef void(*dp_inference_cb_t)(void *result,void *param);
EXTERN DLL int dp_register_inference_db(dp_inference_cb_t cb, void *param);

/********************************************************/
EXTERN DLL int dpChangeToFaceRegister();
EXTERN DLL int dpFaceRegisterFromBuffer(unsigned char* imagebuffer,int buffersize,int imageWidth);
EXTERN DLL int dpFaceRegisterFromCam();
EXTERN DLL int dpUploadFaceDetectorModel(const char* filename);
EXTERN DLL int dpUploadFaceAngleModel(const char* filename);
EXTERN DLL int dpUploadFaceMarkModel(const char* filename);
EXTERN DLL int dpUploadFaceRecognitionModel(const char* filename);
typedef void(*dp_detector_face_cb_t)(FaceResult *result,void *param);
EXTERN DLL int dp_register_detector_face_device_cb(dp_detector_face_cb_t cb, void *param);
EXTERN DLL int dpSaveFaceFeatureTofile();
/********************************************************/
typedef void(*dp_video_frame_cb_t)(dp_img_t *img, void *param);
EXTERN DLL int dp_register_video_frame_cb(dp_video_frame_cb_t cb, void *param);

EXTERN DLL int dpExitMovidius();



#endif
