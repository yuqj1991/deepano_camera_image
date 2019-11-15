
#include "mtcnn.h"
#include<iostream>
using namespace std;
#define PNET_NUM 8
extern void floattofp16(unsigned char *dst, float *src, unsigned nelem);
typedef unsigned short half;
const int pnet_imagepyramid_width[PNET_NUM] =   {229, 162, 115,  81,  57,  41, 29, 20};
const int pnet_imagepyramid_height[PNET_NUM] = {407, 288, 203, 144, 102, 72, 51, 36};
const int boxshape2_array[PNET_NUM]= {199, 139, 97, 67, 46, 31, 21, 13};
const int boxshape3_array[PNET_NUM]= {110,  76, 53, 36, 24, 16, 10,  5};
void MTCNNDetector::wrapInputLayer ( std::vector<cv::Mat>* input_channels ,int width,int height)
{
	        float* input_data = (float*)malloc(3*width*height*sizeof(float));
	
		for ( int i = 0; i < 3; i++ )
		{
		        memset(input_data,0x0,sizeof(input_data));
			cv::Mat channel ( height, width, CV_32FC1, input_data );
			input_channels->push_back ( channel );
		}
}

//description: resize the picture of img_channels to the size of input_channels, and store in input_channels
//src: img_channels
//dst: input_channels
void MTCNNDetector::pyrDown ( const std::vector<cv::Mat>& img_channels, std::vector<cv::Mat>* input_channels )
{
	assert ( img_channels.size() == input_channels->size() );
	int hs = ( *input_channels ) [0].rows;
	int ws = ( *input_channels ) [0].cols;
	cout<<"    in pyrDown Function: "<<"input width: "<<img_channels[0].rows<<" input height: "<<img_channels[0].cols<<endl;
	cout<<"    in pyrDown Function: "<<"hs: "<<hs<<" ws: "<<ws<<endl;
	for ( unsigned int i = 0; i < img_channels.size(); i++ )
	{
		cv::resize ( img_channels[i], ( *input_channels ) [i], cv::Size ( ws, hs ) );
	}
}
unsigned short* MTCNNDetector::loadimage(const char *imagepath){
        half* imageBufFp16=(half*)malloc(3*407*229*sizeof(half));// = LoadImage(IMAGE_FILE_NAME, networkDim, networkMean);
        
        // calculate the length of the buffer that contains the half precision floats.
        // 3 channels * width * height * sizeof a 16bit float 
        unsigned int lenBufFp16 = 3*407*229*sizeof(half);
        cv::Mat img = cv::imread ( imagepath );
        const COLOR_ORDER color_order =BGR;
	
	cv::Mat sample;
	int box_shape1 = 6;  //fixed
	//struct timeval tv;
	const int min_size = 40;
	const float P_thres = 0.6f;
	const float R_thres = 0.7f;
	const float O_thres = 0.9f;
	const float scale_factor = 0.706f;
	const int num_channels_=3;
	cout<<"image channel is "<<img.channels() <<" num_channels_ is "<<num_channels_<<endl;
	cout<<"min_size "<<min_size <<" P_thres "<< P_thres <<" R_thres "<< R_thres<<" O_thres "<<O_thres<<" scale_factor "<<scale_factor <<endl;

	if ( img.channels() == 3 && num_channels_ == 1 )
	{
		cv::cvtColor ( img, sample, cv::COLOR_BGR2GRAY );
	}
	else if ( img.channels() == 4 && num_channels_ == 1 )
	{
		cv::cvtColor ( img, sample, cv::COLOR_BGRA2GRAY );
	}
	else if ( img.channels() == 4 && num_channels_ == 3 )
	{
		if ( color_order == RGBA )
		{
			cv::cvtColor ( img, sample, cv::COLOR_RGBA2RGB );
		}
		else
		{
			cv::cvtColor ( img, sample, cv::COLOR_BGRA2BGR );
		}
	}
	else if ( img.channels() == 1 && num_channels_ == 3 )
	{
		cv::cvtColor ( img, sample, cv::COLOR_GRAY2RGB );
	}
	else
	{
		sample = img;
	}
	cv::Mat sample_normalized;
	// convert to float and normalize
	sample.convertTo ( sample_normalized, CV_32FC3, 0.0078125, -127.5 * 0.0078125 );	//image normalize then store as 32 bits float
	sample_normalized = sample_normalized.t();

	cout<<"rows: "<<sample_normalized.rows<<", cols: "<<sample_normalized.cols<<endl;
	const int img_H = sample_normalized.rows;
	const int img_W = sample_normalized.cols;
	int minl  = cv::min ( img_H, img_W );
	// split
	std::vector<cv::Mat> sample_norm_channels;
	cv::split ( sample_normalized, sample_norm_channels );
	if ( color_order == BGR || color_order == BGRA )
	{
		cv::Mat tmp = sample_norm_channels[0];
		sample_norm_channels[0] = sample_norm_channels[2];
		sample_norm_channels[2] = tmp;
	}

	float m = 12.0f / min_size;
	minl *= m;
	std::vector<float> all_scales;
	float cur_scale = scale_factor; //1.0f;
	minl *= cur_scale;

	while ( float ( minl ) >= 16.0f )
	{
		all_scales.push_back ( m * cur_scale );
		printf ( "m * cur_scale is %f, minl is %d \n", m * cur_scale, minl );
		cur_scale *= scale_factor;
		minl *= scale_factor;
	}

	// stage 1: P_Net forward can get rectangles and regression coords
	        float imageBufFp32[407*229*3];
	        float resultData32[6*199*110];

	        std::vector<BoundingBox> totalBoxes;
	        cout<<"Going to calc P-Net, all_scales.size is "<<all_scales.size() <<endl;
	        unsigned int i=0;
		std::vector<cv::Mat> pyr_channels;
		cur_scale = all_scales[i];
		int hs = cvCeil ( img_H * cur_scale );
		int ws = cvCeil ( img_W * cur_scale );
		std::vector<BoundingBox> filterOutBoxes;
		std::vector<BoundingBox> nmsOutBoxes;
		wrapInputLayer ( &pyr_channels,ws,hs ); //?可能存在问题

		pyrDown ( sample_norm_channels, &pyr_channels );

		float* imageBufFp32_1 = ( float* ) pyr_channels[0].data;
		float* imageBufFp32_2 = ( float* ) pyr_channels[1].data;
		float* imageBufFp32_3 = ( float* ) pyr_channels[2].data;
		memset ( imageBufFp32, 0x0, sizeof ( imageBufFp32 ) );
		for ( int j=0; j < pnet_imagepyramid_width[i]*pnet_imagepyramid_height[i]; j++ )
		{
			imageBufFp32[3*j] = imageBufFp32_1[j];
			imageBufFp32[3*j + 1] = imageBufFp32_2[j];
			imageBufFp32[3*j + 2] = imageBufFp32_3[j];
		}
		floattofp16((unsigned char *)imageBufFp16, imageBufFp32, 3*20*36);
		return imageBufFp16;
}
