#ifndef __DP_API_TYPE_H__
#define __DP_API_TYPE_H__

#include <stdint.h>

/** \ingroup dp_command_t
* define dp api command for internal use. User of API should not use them.
*/
typedef enum {
	DP_CMD_START_CAM_ONLY = 0,
	DP_CMD_STOP_CAM,
	DP_CMD_START_CAM_VIDEO,

	//General API
	DP_CMD_INIT = 48,
	DP_CMD_UNINIT, //49
	DP_CMD_PING, //50
	DP_CMD_GET_VERSION, //51
	DP_CMD_SET_CAM_CONFIG, //52
	DP_CMD_GET_CAM_CONFIG, //53
	DP_CMD_SET_RUNMODE, //54
	DP_CMD_GET_RUNMODE, //55
	DP_CMD_UPDATE_MODEL, //56+ncsdk
	DP_CMD_UPDATE_FIRMWARE, //57
	DP_CMD_CAPTURE_FRAME, //58
	DP_CMD_RELEASE_FRAME, //59
	DP_CMD_UPDATE_MODEL_2,//60+ncsdk
	DP_CMD_GET_BLOB_PARMS, //61+ncsdk
	DP_CMD_RECEIVE_BOX_IMAGE, //62
	DP_CMD_HARDWARE_TEST=65, 
        DP_CMD_GET_BLOB_MEAN_STD=66,
}dp_command_t;

typedef enum {
	DP_IND_FRAME = 1,
	DP_IND_FIRST_RESULT,//cdk
	DP_IND_SEC_RESULT,//cdk
	DP_IND_SENDBACK_BOX,//cdk		
}dp_indication_t;
	
/** \ingroup dp_ver_t
* device version structure
*/
typedef struct dp_ver_t {
	int major;          /* The mejor verion number. */
	int minor;          /* The mejor verion number. */
	int revision;       /* The revision number. */
} dp_ver_t;

/** \ingroup dp_ver_t
* device version structure
*/
typedef struct dp_hw_status_t {
	int flash_st;        /* The flash status. */
	int i2c_st;          /* The i2c status. */
	int sensor_st;       /* The sensor status. */
} dp_hw_status_t;

/** \ingroup dp_pixel_fmt_t
* define pixel format
*/
typedef enum {
	DP_PIX_FORMAT_GRAY,
	DP_PIX_FORMAT_BGR888,
	DP_PIX_FORMAT_NV21,
	DP_PIX_FORMAT_BGRA8888,
	DP_PIX_FORMAT_I422,
	DP_PIX_FORMAT_I420,
	DP_PIX_FORMAT_YUV444,
	DP_PIX_FORMAT_H264,
}dp_pixel_fmt_t;

/** \ingroup dp_img_info_t
* image_info structure
*/
typedef struct dp_img_info_t {
	int width;           /* The width of the image. */
	int height;          /* The height of the image. */
	int stride;          /* The number of bytes from one row of pixels. */
	int img_size;        /* The number of bytes of pixels of whole image.  */
	dp_pixel_fmt_t fmt;  /* image format. */
} dp_img_info_t;

/** \ingroup dp_img_t
* image structure
*/
typedef struct dp_img_t {
	int width;           /* The width of the image. */
	int height;          /* The height of the image. */
	int stride;          /* The number of bytes from one row of pixels. */
	int img_size;        /* The number of bytes of pixels of whole image.  */
	dp_pixel_fmt_t fmt;  /* image format. */
	unsigned char* img;  /* image data in memory. */
} dp_img_t;
/**\ingroup dp_blob_parm_t
*blob model struct
*/
typedef struct dp_blob_parm_t{
    int IsTensor_model;            //deault caffe_model value=0;tensorflow=1;
	int InputSize_width;
	int InputSize_height;
	int Output_num;
}dp_blob_parm_t;
/** \ingroup dp_camera_config_t
* camera configuration structure
*/

/**\ingroup dp_image_box_t
send first blob parseing box ponit structure
*/
typedef struct dp_image_box_t{
   int x1;
   int x2;
   int y1;
   int y2;
}dp_image_box_t;

typedef struct _dp_netMean
{
    float R_mean;
    float G_mean;
    float B_mean;
	float Std;
}dp_netMean;



typedef struct dp_camera_config_t
{
	int width;           /* The width of the camera image. */
	int height;          /* The width of the camera image. */
	int fps;             /* The fps(frame per second) of the camera. */
	int id;              /* The index of the camera. */
}dp_camera_config_t;

#endif
