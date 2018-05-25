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
	DP_CMD_STOP_PARSE_MODEL=67,
	DP_CMD_IMAGE_BLOB_SIZE=68,
}dp_command_t;

typedef enum {
	DP_IND_FRAME = 1,
	DP_IND_FIRST_RESULT,//cdk
	DP_IND_SEC_RESULT,//cdk
        DP_Ddurcallriton_stage,	//cdk	
}dp_indication_t;

const char * const OP_NAMES[] =
    {
        "kConv",                 ///< Convolution
        "kMaxPool",              ///< Max-Pooling
        "kAvgPool",              ///< Average-Pooling
        "kSoftMax",              ///< SoftMax
        "kFC",                   ///< Fully connected layer
        "kNone0",                ///< No post operation
        "kRelu",                 ///< rectified linear unit (Relu) rectifier
        "kReluX",                ///< rectified linear unit (Relu) rectifier - clamp(0,X)
        "kDepthConv",            ///< Depthwise Convolution
        "kBias",                 ///< Bias
        "kPRelu",                ///< PReLU
        "kLRN",                  ///< LRN
        "kSum",                  ///< Sum of input and weight
        "kProd",                 ///< Prod of input and weight
        "kMax",                  ///< Max between input and weight
        "kScale",                ///< Multiply each plane with a multiplier and add a bias
        "kRelayout",
        "kSquare",               ///< Square the input
        "kInnerLRN",             ///< Output = (1 + alpha * input) ^ -beta
        "kCopy",                 ///< Copy considering input and output strides
        "kSigmoid",              ///< Sigmoid
        "kTanh",                 ///< Tanh
        "kDeconvolution",        ///< Deconvolution a.k.a. Transposed convolution
        "kElu",                  ///< Exponential linear unit (ELU) rectifier
        "kReshape",              ///< Reshape
        "kToPlaneMajor",         ///< Convert from plane major to plane minor
        "kPower",                ///< Power layer
        "kCrop",                 ///< Crop layer
        "kTile",                 ///< Tile layer
        "kRegionYolo",           ///< Region layer
        "kReorgYolo",            ///< Reorg layer
        "kConvert_u8f16",        ///< Convert_u8f16 layer
        "kConvert_f32f16",       ///< Convert_f32f16 layer
        "kMyriadXHwConvolution", ///< Reserved for Hardware Ops
        "kMyriadXHwPooling",     ///< Reserved for Hardware Ops
        "kMyriadXHwFCL",         ///< Reserved for Hardware Ops
        "kMyriadXHwPostOps",     ///< Reserved for Hardware Ops
        "kConvertHwSw",          ///< Convert HW/SW layer
        "kPermute",              ///< Permute
        "kNormalize",            ///< Normalize
        "kPriorBox",             ///< PriorBox
        "kDetectionOutput",      ///< kDetectionOutput
        "kLeakyRelu",            ///< LeakyRelu
        "kSumReduce",			 ///< Sum of channels
        "kMaxWithConstant"		 ///< Maximum of input and constant
	"kRsqrt"				 ///< Inverse square root
	"kScaleWithScalar"		 ///< Multiply tensor by scalar
    };

	
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
