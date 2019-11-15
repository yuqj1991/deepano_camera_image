#ifndef MTCNNDETECTOR_H
#define MTCNNDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <sys/time.h>
class MTCNNDetector
{
  public:
        enum COLOR_ORDER
	{
		GRAY,
		RGBA,
		RGB,
		BGRA,
		BGR
	};
	enum NMS_TYPE
	{
		MIN,
		UNION,
	};
	struct BoundingBox
	{
		// top-left & bottom-right points
		float x1;
		float y1;
		float x2;
		float y2;

		// regression
		float dx1;
		float dy1;
		float dx2;
		float dy2;

		// cls
		float score;

		// landmarks
		float points_x[5];
		float points_y[5];
	};
	struct CmpBoundingBox
	{
		bool operator() ( const BoundingBox& b1, const BoundingBox& b2 )
		{
			return b1.score > b2.score;
		}
	};
	struct FaceDetResult
	{
		float score;
		cv::Rect rect;
		std::vector<cv::Point2f> landmarks;
	};
	float img_mean;
	float img_range_scale;
	
	unsigned short *loadimage(const char *imagepath);
	private:
	void wrapInputLayer ( std::vector<cv::Mat>* input_channels ,int width,int height);

	void pyrDown ( const std::vector<cv::Mat>& img_channels, std::vector<cv::Mat>* input_channels );
};	
#endif
