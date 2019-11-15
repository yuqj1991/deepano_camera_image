#ifndef __DP_API_TYPE_H__
#define __DP_API_TYPE_H__

#include <stdint.h>

/** \ingroup dp_command_t
* define dp api command for internal use. User of API should not use them.
*/
typedef enum {
	//General API
	DP_CMD_INIT = 48,
	DP_CMD_UNINIT, //49
	DP_CMD_PING, //50
	DP_CMD_GET_VERSION, //51
	
	DP_CMD_UPDATE_MODEL=56, //56+ncsdk
	DP_CMD_UPDATE_FIRMWARE, //57
	
	DP_CMD_UPDATE_MODEL_2=60,//60+ncsdk
	DP_CMD_GET_BLOB_PARMS, //61+ncsdk
	
	DP_CMD_SEND_FIRST_IMAGE_BUFFER=63,//63ncsdk
	DP_CMD_SEND_SEC_IMAGE_BUFFER,//64ncsdk
	DP_CMD_HARDWARE_TEST, 
	DP_CMD_STOP_PARSE_MODEL=66,
	DP_CMD_UPDATE_MODEL_ONET=67,
	DP_CMD_GET_BLOB_PNET_SHAPE,
	DP_CMD_DETECTOR_PARAMS_THREOLD,
	DP_CMD_ALL_SCALES,
	DP_CMD_SRC_SIZE,	
}dp_command_t;

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


typedef enum {
	DP_IND_SEND_BLOB_PARSE_TIME,
	DP_IND_SEND_FIRST_NCSDK_RESULT,//ncsdk
	DP_IND_SEND_SEC_NCSDK_RESULT,//ncsdk	
	DP_IND_SEND_TULING_RESULT,//tuling
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
typedef struct _Detector_fun_param
{
        float min_size;
        float P_thres;
	float R_thres;
	float O_thres;
	float scale_factor;
}dp_detector;

typedef struct _BOX_shape_array
{
    int box_shape_2arrary;   //protxt output_Width
    int box_shape_3arrary;  //protxt output_height
} dp_box_shapearray;


/** \ingroup dp_img_info_t
* image_info structure
*/
typedef struct dp_blob_parm_t{
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

typedef struct dp_FacePoint
{
        int x;
        int y;
}dp_FacePoint;
typedef struct dp_FaceRect
{
        int x;
        int y;
        int width;
        int height;
}dp_FaceRect;
typedef struct dp_FaceDetResult
{
	float score;
	dp_FaceRect rect;
	dp_FacePoint landmarks[5];
}dp_FaceDetResult;
#endif
