#include "pre_image.h"
#include <vector>
#include <fstream>
extern void floattofp16(unsigned char *dst, float *src, unsigned nelem);

static float *cmpdata;
int indexcmp(const void *a1,const void *a2)
{
   int *a=(int*)a1;
   int *b=(int*)a2;
   float diff=cmpdata[*b]-cmpdata[*a];
   if(diff<0)
	{
		return -1;
	}
	else if(diff>0)
	{
		return 1;
	}
   else 
   	return 0;
}

unsigned char *IplImage2RGBImgData(IplImage* in_img)
{
	unsigned char *re_img_data,*in_pt,*temp;
	int i;

	if(in_img==NULL)
	{
		return NULL;
	}
	IplImage* cpy_img=cvCloneImage(in_img);
	int img_width = in_img->width;
	int img_height = in_img->height;

	re_img_data = (unsigned char *)malloc(img_width * img_height * 3);
	temp=re_img_data;
	cvConvertImage(in_img,cpy_img,CV_CVTIMG_SWAP_RB);
	in_pt= (unsigned char*)(cpy_img->imageData);
	
	for(i=0;i<img_height;i++)
	{
		
		memcpy(temp,in_pt,img_width*3);
		in_pt= in_pt+cpy_img->widthStep;
		temp=temp+img_width*3;
	}

	cvReleaseImage(&cpy_img);
	return re_img_data;
}

unsigned char *IplImage2RGBImgData_ali(IplImage* in_img)
{
	unsigned char *re_img_data,*in_pt,*temp;
	int i;

	if(in_img==NULL)
	{
		return NULL;
	}
	IplImage* cpy_img=cvCloneImage(in_img);
	int img_width = in_img->width;
	int img_height = in_img->height;

	re_img_data = (unsigned char *)malloc(img_width * img_height * 1);
	temp=re_img_data;
	cvConvertImage(in_img,cpy_img,CV_CVTIMG_SWAP_RB);
	in_pt= (unsigned char*)(cpy_img->imageData);
	for(i=0;i<img_height;i++)
	{
		
		memcpy(temp,in_pt,img_width*1);
		in_pt= in_pt+cpy_img->widthStep;
		temp=temp+img_width*1;
	}

	cvReleaseImage(&cpy_img);
	return re_img_data;
}
IMAGE_process::IMAGE_process(float *mean,float *std,int width,int height)
{
	img_height=height;
	img_width=width;
	memcpy(img_mean,mean,3);
	memcpy(img_std,std,3);
}

//单通道
unsigned short *IMAGE_process::loadimage_ali(const char *path, int Ifuse,dp_image_box_t box, int reqsize, float mean, float std)
{
	int width, height, cp, i;
	float *imgfp32;
	unsigned short *imgfp16;
        int m=0;
	IplImage *img_src=cvLoadImage(path);
	printf("width:%d,height:%d\n",img_src->width,img_src->height);
	
	if(Ifuse==0)
	{
	        IplImage *img_gray=cvCreateImage(cvSize(img_src->width,img_src->height),img_src->depth,1);
		IplImage *img_resize=cvCreateImage(cvSize(reqsize,reqsize),img_src->depth,1);
		cvCvtColor(img_src,img_gray,CV_BGR2GRAY);
	        //cvResize(img_gray,img_resize); 
	        unsigned char *imgresized=(unsigned char *)malloc(reqsize * reqsize * 1);
	        for(int i=0;i<img_resize->height;i++)
	        {
	                for(int j=0;j<img_resize->width;j++)
	                {
	                        imgresized[i*img_resize->width+j]=((unsigned char*)(img_gray->imageData + i*img_gray->widthStep))[j];
	                }
	        }
	        if(!imgresized)
	        {
		        cvReleaseImage(&img_resize);
		        return 0;
	        }
	        cvReleaseImage(&img_resize);
	        imgfp32 = (float *)malloc(sizeof(*imgfp32) * 320 * 320 * 1);
	        if(!imgfp32)
	        {
		        free(imgresized);
		        return 0;
	        }
	
	        for(i = 0; i < reqsize * reqsize * 1; i++)
	        {
		        imgfp32[i] = imgresized[i];
	        }	
	        free(imgresized);
	        imgfp16 = (unsigned short *)malloc(sizeof(*imgfp16) * reqsize * reqsize * 1);
	        if(!imgfp16)
	        {
		        free(imgfp32);
		        return 0;
	        }
	        for(i = 0; i < reqsize*reqsize; i++)
	        {
		        float blue,red,green;
		        red  =imgfp32[i];
		        imgfp32[i] =(float) (red-mean)*6/std;
	        }
	        for(int i=0;i<10;i++)
	        printf("the %d---value---:%f\n",i,imgfp32[i]);	
	        /*
	        ifstream fin("../test_bmp.txt");
	        string s;
	        int i=0;
	        while (fin>>s)
	        {
	                imgfp32[i]=::atof(s.c_str());
	                //printf("%f\n",imgfp32[i]);
	                i++;
	        }
	        fin.close();*/
	        floattofp16((unsigned char *)imgfp16, imgfp32, 1*reqsize*reqsize);
	        free(imgfp32);
	        cvReleaseImage(&img_src);
	        return imgfp16;
        }
}

unsigned short* IMAGE_process::loadimage_non_square(const char *path, int reqsize_w,int reqsize_h, float *mean, float *std)
{
        int width, height, cp, i;
	float *imgfp32;
	unsigned short *imgfp16;
        int m=0;
	IplImage *img_src=cvLoadImage(path);
	printf("width:%d,height:%d\n",img_src->width,img_src->height); 
	IplImage *img_resize=cvCreateImage(cvSize(reqsize_w,reqsize_h),img_src->depth,img_src->nChannels);
	cvResize(img_src,img_resize); 

	unsigned char *imgresized=IplImage2RGBImgData(img_resize);
	if(!imgresized)
	{
		cvReleaseImage(&img_resize);
		return 0;
	}
	cvReleaseImage(&img_resize);
	imgfp32 = (float *)malloc(sizeof(*imgfp32) * reqsize_w * reqsize_h * 3);
	if(!imgfp32)
	{
		free(imgresized);
		return 0;
	}
	for(i = 0; i < reqsize_w * reqsize_h * 3; i++)
	{
		imgfp32[i] = imgresized[i];
	}	
	free(imgresized);
	imgfp16 = (unsigned short *)malloc(sizeof(*imgfp16) * reqsize_w * reqsize_h * 3);
	if(!imgfp16)
	{
		free(imgfp32);
		return 0;
	}
	for(i = 0; i < reqsize_w*reqsize_h; i++)
	{
		float blue,red,green;
		blue =imgfp32[3*i+2];
		green=imgfp32[3*i+1];
		red  =imgfp32[3*i+0];
		imgfp32[3*i+0] = (blue-mean[0])*std[1];
		imgfp32[3*i+1] = (green-mean[1])*std[2];
		imgfp32[3*i+2] = (red-mean[2])*std[3];
	}
	floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqsize_w*reqsize_h);
	free(imgfp32);
	cvReleaseImage(&img_src);
	return imgfp16;
}

unsigned char* IMAGE_process::loadimage_tuling(const char *path)
{
	cv::Mat img=cv::imread (path);
	printf("img.width :%d ,img.height :%d\n",img.cols,img.rows);
	cv::Mat sample_normalized;
	sample_normalized=img.t();
	printf("sample_normalized.width :%d ,sample_normalized.height :%d\n",sample_normalized.cols,sample_normalized.rows);
	unsigned char *imgresized_normesd=(unsigned char *)malloc(sizeof(unsigned char)*1920*1080*3);
	std::vector<cv::Mat> sample_norm_channels;
	cv::split ( sample_normalized, sample_norm_channels );
	cv::Mat tmp = sample_norm_channels[0];
	sample_norm_channels[0] = sample_norm_channels[2];
	sample_norm_channels[2] = tmp;
        unsigned char *Rvlaue=sample_norm_channels[0].data;
        unsigned char *Gvlaue=sample_norm_channels[1].data;
        unsigned char *Bvlaue=sample_norm_channels[2].data;

        for(int ii=0;ii<1920*1080;ii++)
        {
                imgresized_normesd[ii]=Rvlaue[ii];
                imgresized_normesd[ii+1*1920*1080]=Gvlaue[ii];
                imgresized_normesd[ii+2*1920*1080]=Bvlaue[ii];
        }
	return imgresized_normesd;	
}


unsigned short* IMAGE_process::loadimage_video(IplImage* img_src, int Ifuse,dp_image_box_t box, int reqsize, float *mean, float *std)
{
        int width, height, cp, i;
	float *imgfp32;
	unsigned short *imgfp16;
        IplImage *img_resize=cvCreateImage(cvSize(reqsize,reqsize),img_src->depth,img_src->nChannels);
	cvResize(img_src,img_resize); 
	unsigned char *imgresized=IplImage2RGBImgData(img_resize);
	if(!imgresized)
	{
	        cvReleaseImage(&img_resize);
	        return 0;
        }
	cvReleaseImage(&img_resize);
	imgfp32 = (float *)malloc(sizeof(*imgfp32) * reqsize * reqsize * 3);
	if(!imgfp32)
	{
                free(imgresized);
		return 0;
	}
	for(i = 0; i < reqsize * reqsize * 3; i++)
	{
                imgfp32[i] = imgresized[i];
	}	
	free(imgresized);
	imgfp16 = (unsigned short *)malloc(sizeof(*imgfp16) * reqsize * reqsize * 3);        
	if(!imgfp16)
	{
		free(imgfp32);
		return 0;
	}
	for(i = 0; i < reqsize*reqsize; i++)
	{
		float blue,red,green;
		blue =imgfp32[3*i+2];
		green=imgfp32[3*i+1];
		red  =imgfp32[3*i+0];
                imgfp32[3*i+0] = (blue-mean[0])*std[0];
	        imgfp32[3*i+1] = (green-mean[0])*std[0];
	        imgfp32[3*i+2] = (red-mean[0])*std[0];
	}
        floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqsize*reqsize);
        free(imgfp32);
        return imgfp16;        
}

unsigned short* IMAGE_process::loadimage_incept(const char *path, int Ifuse,dp_image_box_t box, int reqsize, float *mean, float *std)
{
	int width, height, cp, i;
	float *imgfp32;
	unsigned short *imgfp16;
    	int m=0;
	IplImage *img_src=cvLoadImage(path);
        if(Ifuse==0)
	{
	if(img_src->height>img_src->width)
	   cvSetImageROI(img_src,cvRect(0,int(0.5*(img_src->height-img_src->width)),img_src->width,img_src->width));
	else if(img_src->height<img_src->width)
	   cvSetImageROI(img_src,cvRect(int(0.5*(img_src->width-img_src->height)),0,img_src->height,img_src->height));
        //cvShowImage("img",img_src);
        //cvWaitKey(0);
	IplImage *img_resize=cvCreateImage(cvSize(reqsize,reqsize),img_src->depth,img_src->nChannels);
	cvResize(img_src,img_resize); 

	unsigned char *imgresized=IplImage2RGBImgData(img_resize);
	if(!imgresized)
	{
		cvReleaseImage(&img_resize);
		return 0;
	}
	cvReleaseImage(&img_resize);
	imgfp32 = (float *)malloc(sizeof(*imgfp32) * reqsize * reqsize * 3);
	if(!imgfp32)
	{
		free(imgresized);
		return 0;
	}
	for(i = 0; i < reqsize * reqsize * 3; i++)
	{
		imgfp32[i] = imgresized[i];
	}	
	free(imgresized);
	imgfp16 = (unsigned short *)malloc(sizeof(*imgfp16) * reqsize * reqsize * 3);
	if(!imgfp16)
	{
		free(imgfp32);
		return 0;
	}
	    for(i = 0; i < reqsize*reqsize; i++)
	{
		float blue,red,green;
		blue =imgfp32[3*i+2];
		green=imgfp32[3*i+1];
		red  =imgfp32[3*i+0];
		imgfp32[3*i+0] = (red-mean[0])*std[0];
		imgfp32[3*i+1] = (green-mean[1])*std[1];
		imgfp32[3*i+2] = (blue-mean[2])*std[2];
	}
	floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqsize*reqsize);
	free(imgfp32);
	cvReleaseImage(&img_src);
	return imgfp16;
   }
}

unsigned short *IMAGE_process::loadimage(const char *path, int Ifuse,dp_image_box_t box, int reqsize, float *mean, float *std)
{
	int width, height, cp, i;
	float *imgfp32;
	unsigned short *imgfp16;
        int m=0;
	IplImage *img_src=cvLoadImage(path);
	printf("width:%d,height:%d\n",img_src->width,img_src->height);
	if(Ifuse==1)
	{
	    
		int w = (box.x2-box.x1);
		int h=  (box.y2-box.y1);
		CvSize size=CvSize(w,h);
		printf("w:%d,h:%d\n",w,h);
		cvSetImageROI(img_src,cvRect(box.x1,box.y1,w,h));
		
		//cvShowImage("imageROI",img_src);
		//cvWaitKey(0);
		//IplImage *imgresized=cvCreateImage(size,img_src->depth,img_src->nChannels);
	       // assert(imgresized);
		//cvCopy(img_src,imgresized);
		//cvResetImageROI(img_src);
		
		CvSize size_resize=CvSize(reqsize,reqsize);
		IplImage *sec_resized_t=cvCreateImage(size_resize,img_src->depth,img_src->nChannels);
		cvResize(img_src,sec_resized_t);

		unsigned char *sec_resized=(unsigned char *)malloc(reqsize * reqsize * 3);
		
		for(int j=0;j<sec_resized_t->height;j++)
		{
		        uchar *data=(uchar*)(sec_resized_t->imageData+j*sec_resized_t->widthStep);
		        for(int k=0;k<sec_resized_t->width;k++)
		        {
		                sec_resized[j*sec_resized_t->width*3+k*3+0]=data[k*3+0];
			        sec_resized[j*sec_resized_t->width*3+k*3+1]=data[k*3+1];
			        sec_resized[j*sec_resized_t->width*3+k*3+2]=data[k*3+2];
		        }
		}
	        imgfp32 =(float*) malloc(sizeof(*imgfp32) * reqsize * reqsize * 3);
	        assert(imgfp32);
	        for(i = 0; i < reqsize * reqsize * 3; i++)
		        imgfp32[i] = sec_resized[i];
	        imgfp16 = (unsigned short *)malloc(sizeof(*imgfp16) * reqsize * reqsize * 3);
	        assert(imgfp16);
	        for(i = 0; i < reqsize*reqsize; i++)
	        {
		        imgfp32[3*i+0] = (imgfp32[3*i+2]-mean[0])*std[0];
		        imgfp32[3*i+1] = (imgfp32[3*i+1]-mean[1])*std[1];
		        imgfp32[3*i+2] = (imgfp32[3*i+0]-mean[2])*std[2];
	        }
		floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqsize*reqsize);
	        free(imgfp32);
	        free(sec_resized);	
	        cvReleaseImage(&sec_resized_t);
	        cvReleaseImage(&img_src);
	        return imgfp16;
	}
	else if(Ifuse==0)
	{
		IplImage *img_resize=cvCreateImage(cvSize(reqsize,reqsize),img_src->depth,img_src->nChannels);
		cvResize(img_src,img_resize); 
		unsigned char *imgresized=IplImage2RGBImgData(img_resize);
		if(!imgresized)
		{
			cvReleaseImage(&img_resize);
			return 0;
		}
		cvReleaseImage(&img_resize);
		imgfp32 = (float *)malloc(sizeof(*imgfp32) * reqsize * reqsize * 3);
		if(!imgfp32)
		{
			free(imgresized);
			return 0;
		}
		for(i = 0; i < reqsize * reqsize * 3; i++)
		{
			imgfp32[i] = imgresized[i];
		}	
		free(imgresized);
		imgfp16 = (unsigned short *)malloc(sizeof(*imgfp16) * reqsize * reqsize * 3);
		if(!imgfp16)
		{
			free(imgfp32);
			return 0;
		}
		for(i = 0; i < reqsize*reqsize; i++)
		{
			float blue,red,green;
			blue =imgfp32[3*i+2];
			green=imgfp32[3*i+1];
			red  =imgfp32[3*i+0];
			imgfp32[3*i+0] = (blue-mean[0])*std[0];
			imgfp32[3*i+1] = (green-mean[1])*std[1];
			imgfp32[3*i+2] = (red-mean[2])*std[2];
		}
		floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqsize*reqsize);
		free(imgfp32);
		cvReleaseImage(&img_src);
		return imgfp16;
   	}
}

void IMAGE_process::get_bbox_from_result(float *input_data,int data_lenth,int insize_w,int insize_h,int side,int num_class,int num_object,int *bbox_x,int *bbox_y,int *bbox_w,int *bbox_h,float *max_scale,int *max_scale_label,float *max_scale_prob)
{
	int locations = side*side;
	float m_max_scale = 0;                  //每个格子的最大定位概率
	float m_max_box_x = 0;                 //每个格子的最大定位概率框坐标
	float m_max_box_y = 0;
	float m_max_box_w = 0;
	float m_max_box_h = 0;
	int   m_max_label=0;
	float m_max_prob=0;
	float m_max_scale_multiply_prob=0;
	for (int i = 0; i < locations; ++i)                    // ==========遍历7*7个小格子========
	{
		int pred_label = 0;                //每个格子的分类标签
		float max_prob_temp = input_data[i];    //每个格子的最大概率

	
		for (int j = 1; j < num_class; ++j)                            // 1.找140类中最大的概率和类别
		{
			int class_index = j * locations + i;   
			if (input_data[class_index] > max_prob_temp) 
			{
				pred_label = j;
				max_prob_temp = input_data[class_index];
			}
		}

		int obj_index = num_class * locations + i;
		int coord_index = (num_class + num_object) * locations + i;
    
		float pred_box_x = 0;              //每个格子的最大定位概率框坐标
		float pred_box_y = 0;
		float pred_box_w = 0;
		float pred_box_h = 0;	
		for (int k = 0; k < num_object; ++k)                          // 2.两个框
		{
			float scale = input_data[obj_index + k * locations];

			int box_index = coord_index + k * 4 * locations;

			pred_box_x= (i % side + input_data[box_index + 0 * locations])*insize_w / side ;
			pred_box_y= (i / side + input_data[box_index + 1 * locations])*insize_h / side ;
			float w = input_data[box_index + 2 * locations];
			float h = input_data[box_index + 3 * locations];
			pred_box_w=w*w*insize_w;
			pred_box_h=h*h*insize_h;



			if(scale*max_prob_temp>m_max_scale_multiply_prob)
			{
				m_max_scale				= scale;
				m_max_box_x 				= pred_box_x;                       //每个格子的最大定位概率框坐标
				m_max_box_y 				= pred_box_y;
				m_max_box_w 				= pred_box_w;
				m_max_box_h 				= pred_box_h;
				m_max_scale_multiply_prob		= scale*max_prob_temp;
				m_max_prob				= max_prob_temp;
				m_max_label				= pred_label;
			}
		}//for (int k = 0; k < num_object; ++k)

	}// for (int i = 0; i < locations; ++i)
	
	*max_scale=m_max_scale;
	*bbox_x=m_max_box_x-m_max_box_w/2;
	*bbox_y=m_max_box_y-m_max_box_h/2;
	*bbox_w=m_max_box_w;
	*bbox_h=m_max_box_h;
	*max_scale_label=m_max_label;
	*max_scale_prob=m_max_prob;
}
dp_image_box_t IMAGE_process::print_det_result(void* fathomOutput)
{
        u16* probabilities = (u16*)fathomOutput;
        float max_cale = 0;                  //每个格子的最大定位概率
	int max_box_x = 0;                 //每个格子的最大定位概率框坐标
	int max_box_y = 0;
	int max_box_w = 0;
	int max_box_h = 0;
	int resultlen=588;
	int reqsize=224;
	int   max_scale_label=0;
	float max_scale_prob=0.0;
	
	float *resultfp32;
	resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));  
        for (u32 i = 0; i < resultlen; i++)
                resultfp32[i]= f16Tof32(probabilities[i]);
        get_bbox_from_result(resultfp32,resultlen,reqsize,reqsize,7,2,2,&max_box_x,&max_box_y,&max_box_w,&max_box_h,&max_cale,&max_scale_label,&max_scale_prob);
        printf("max_cale:%f,max_box_x:%d,max_box_y:%d,max_box_w:%d,max_box_h:%d------------\n",max_cale,max_box_x,max_box_y,max_box_w,max_box_h);
        dp_image_box_t box;
        box.x1=max_box_x;
        box.x2=max_box_x+max_box_w;
        box.y1=max_box_y;
        box.y2=max_box_y+max_box_h;
        free(resultfp32);
        return box;
}
Box *IMAGE_process::print_ssd_xingniao_result(void *fathomOutput,char*imagefile,int *box_nums)
{
        u16* probabilities = (u16*)fathomOutput;	
        printf("\nClassification probabilities:\n");  
        int resultlen=707*2;
        float resultfp32[1414];
        int index=0;
        IplImage *img_src=cvLoadImage(imagefile);
        Box box[20];
        for (u32 i = 0; i < resultlen; i++){
                resultfp32[i]= f16Tof32(probabilities[i]);
        }
        for(int box_index=0;box_index<201*2;box_index++)
        {
	        int base_index=7*box_index+0;
                float conf=resultfp32[base_index+2];
                if(conf<0.6)
                        continue;
                if(resultfp32[base_index+6]<0||resultfp32[base_index+6]>1||resultfp32[base_index+5]<0||resultfp32[base_index+5]>1||resultfp32[base_index+4]<0||resultfp32[base_index+4]>1||resultfp32[base_index+3]<0||resultfp32[base_index+3]>1||resultfp32[base_index+1]<0||resultfp32[base_index+1]>1)
                {
		   	   continue;}
	        int top_xmin=int(resultfp32[base_index+3]*1600);
                int top_ymin=int(resultfp32[base_index+4]*1200);
                int top_xmax=int(resultfp32[base_index+5]*1600);
                int top_ymax=int(resultfp32[base_index+6]*1200);
                cvRectangle(img_src,cvPoint(top_xmin,top_ymin),cvPoint(top_xmax,top_ymax),CV_RGB(0, 255, 0), 2);
                box[index].xmin=top_xmin;
                box[index].ymin=top_ymin;
                box[index].xmax=top_xmax;
                box[index].ymax=top_ymax;
                index++;
                printf("top_xmin:%d,top_ymin:%d,top_xmax:%d,top_ymax:%d\n",top_xmin,top_ymin,top_xmax,top_ymax);
        }        
        cvSaveImage("../resoult_test.jpg",img_src);
        cvReleaseImage(&img_src);
        *box_nums=index;
        Box *det_box=(Box*)malloc(sizeof(Box)*index);
        memcpy(det_box,box,sizeof(Box)*index);
        return det_box;
}

void IMAGE_process::print_googleNet_classify_result(void *fathomOutput)
{
        u16* probabilities = (u16*)fathomOutput;	
        printf("\nClassification probabilities:\n");  
        int resultlen=200;
        float resultfp32[200];
        for (u32 i = 0; i < resultlen; i++)
                resultfp32[i]= f16Tof32(probabilities[i]);
	int *indexes;
	indexes = (int*)malloc(sizeof(*indexes) * resultlen);		
	for(u32 i = 0; i < (int)resultlen; i++)
	{	
	    indexes[i] = i;
        }
	cmpdata = resultfp32;
	qsort(indexes, resultlen, sizeof(*indexes), indexcmp);
	for(u32 i = 0; i < 5 && i < resultlen; i++)
	{
	     printf("%d (%.2f) \n",indexes[i], resultfp32[indexes[i]] * 100.0);
	}   
	free(indexes);
}

void IMAGE_process::print_tunicon_result(dp_FaceDetResult *fathomOutput,void *param)
{
        int *result=(int *)param;
        printf("detector %d person face\n",*result);
        
        IplImage *img_src=cvLoadImage("../hejianfeng.jpg");
        CvPoint P1,P2,PCircle;  
        
        for(int ii=0;ii<(*result);ii++)
        {
                P1.x=fathomOutput[ii].rect.x;  
                P1.y=fathomOutput[ii].rect.y;  
                P2.x=fathomOutput[ii].rect.x+fathomOutput[ii].rect.width;  
                P2.y=fathomOutput[ii].rect.y+fathomOutput[ii].rect.height;  
                cvRectangle(img_src,P1 ,P2, CV_RGB(0, 255, 0), 2);
                for(int jj=0;jj<5;jj++)
                {
                        PCircle.x=fathomOutput[ii].landmarks[jj].x;
                        PCircle.y=fathomOutput[ii].landmarks[jj].y;
                        cvCircle(img_src,PCircle, 1, cv::Scalar ( 0,255,255 ));
                }
        }
        
        cvSaveImage("../resoult_test.jpg",img_src);
        cvReleaseImage(&img_src);
}

void IMAGE_process::print_inception_result(void *fathomOutput)
{
        u16* probabilities = (u16*)fathomOutput;
        unsigned int resultlen=1001;
        float*resultfp32;
        resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
        for (u32 i = 0; i < resultlen; i++)
                resultfp32[i]= f16Tof32(probabilities[i]);
        float temp_predeiction=0.0;
	int index_temp=0;
        for(int i=0;i<5;i++)
	{
                temp_predeiction=resultfp32[i];
		index_temp=i;
		for(int j=i+1;j<resultlen;j++)
		{
		        if(temp_predeiction<=resultfp32[j])
		        {
		                temp_predeiction=resultfp32[j];
				index_temp=j;
		        }
		}
		resultfp32[index_temp]=resultfp32[i];
		resultfp32[i]=temp_predeiction;
   		printf("prediction classes: %d and the probabilityes:%0.3f\n",index_temp,temp_predeiction);
	}
        free(resultfp32);
}

void IMAGE_process::print_ssd_mobilnet_result(void* fathomOutput,Box* box_demo,int box_nums,int frame_width,int frame_height)
{
        char *category[]={"background","aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable","dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
        u16* probabilities = (u16*)fathomOutput;
        unsigned int resultlen=707;
        float resultfp32[707];
        int img_width=frame_width;
        int img_height=frame_height;
        for (u32 i = 0; i < resultlen; i++)
                 resultfp32[i]= f16Tof32(probabilities[i]);
        int num_valid_boxes=int(resultfp32[0]);
        int index=0;
	printf("num_valid_bxes:%d\n",num_valid_boxes);
	for(int box_index=0;box_index<num_valid_boxes;box_index++)
	{
	        int base_index=7*box_index+7;
	        if(resultfp32[base_index+6]<0||resultfp32[base_index+6]>=1||resultfp32[base_index+5]<0||resultfp32[base_index+5]>=1||resultfp32[base_index+4]<0||resultfp32[base_index+4]>=1||resultfp32[base_index+3]<0||resultfp32[base_index+3]>=1||resultfp32[base_index+2]>=1||resultfp32[base_index+2]<0||resultfp32[base_index+1]<0)
	        {
	   	        continue;
	        }
		printf("%d %f %f %f %f %f\n",int(resultfp32[base_index+1]),resultfp32[base_index+2],resultfp32[base_index+3],resultfp32[base_index+4],resultfp32[base_index+5],resultfp32[base_index+6]);
		box_demo[index].xmin=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
		box_demo[index].xmax=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
		box_demo[index].ymin=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;	   
		box_demo[index].ymax=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
		memcpy(box_demo[index].category,category[int(resultfp32[base_index+1])],20);
		index++;
        }
        box_nums=index;	 
}
