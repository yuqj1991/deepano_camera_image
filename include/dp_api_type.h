#ifndef __DP_API_TYPE_H__
#define __DP_API_TYPE_H__

#include <stdint.h>
#include "mv_types.h"
/** \ingroup dp_command_t
* define dp api command for internal use. User of API should not use them.
*/
typedef enum {
	DP_CMD_START_CAM_Neural = 0,
	DP_CMD_STOP_CAM,
	DP_CMD_START_CAM_VIDEO_ONLY,
	//General API
	DP_CMD_INIT = 48,
	DP_CMD_UNINIT, //49
	DP_CMD_PING, //50
	DP_CMD_UPDATE_FIRMWARE, //51
	//General graph options
	DP_CMD_SEND_GRAPH_FILE, //52
	DP_CMD_SEND_GRAPH_PARM, //53
	DP_CMD_HARDWARE_TEST,  //54
	DP_CMD_EXIT_PTHREAD=63,
	//face recognition
	DP_CMD_CHANGE_FUNCTION_STATE = 69,
	DP_CMD_FACE_REFISTER_BUFFER = 70,
	DP_CMD_FACE_REFISTER_CAM = 71,
	DP_CMD_UPLOAD_FACEDETECTOR=72,
	DP_CMD_UPLOAD_FACEANGLE=73,
	DP_CMD_UPLOAD_FACIALMARK=74,
	DP_CMD_UPLOAD_FACERECOGNITION=75,
	DP_CMD_FACE_REGISTER_SAVE_FEATURE = 76,
}dp_command_t;

typedef enum {
	DP_IND_FRAME = 1,
	DP_IND_FACE_RESULT = 2,
	DP_IND_GENERAL_RESULT = 1024,
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
	int InputSize_width;
	int InputSize_height;
	int Output_num;
    float R_mean;
    float G_mean;
    float B_mean;
	float Std;
}dp_blob_parm_t;

/**\ingroup dp_image_box_t
send first blob parseing box ponit structure
*/
typedef struct dp_image_box_t{
   int xmin;
   int xmax;
   int ymin;
   int ymax;
   float yaw;
   float pitch;
   float roll;
   int gender;
   int boolFaceRecogn;
   float feature[512];
}dp_roiBoxInfo;


typedef struct _FaceResult
{
	dp_roiBoxInfo faceResult[20];
	int nResult;
}FaceResult;

#endif
