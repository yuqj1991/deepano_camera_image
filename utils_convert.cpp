#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

int Speed[20][2]={{240,255},{0,15},{0,30},{0,10},{0,0},{0,100},{0,200},{0,15},{0,0},{0,5},{0,0},{0,30},{0,40},{0,30},{0,15},{0,0},{0,5},{0,0},{0,200},{0,0}};
char ImageSet[20]="../train.txt";  //图像数据集
char ImageFolder[20]="../JPEGImage/"; //图像文件夹路径
char LabelSet[20]="../labels/"; //相应标注文件夹路径
char ForthSet[20]="../JPEGImage/"; //相应第四通道路径
char ImageFile[1024];
char bufferImage[500];
char LabelFile[1024];
char forthChannelFile[1024];
char fmt[2][10]={".jpg",".txt"};
int class_index,w_x1,w_x2,h_y1,h_y2; //类别，及标注坐标
void make_the_fourth_channal_file()
{
	FILE *fImageSet=fopen(ImageSet,"r");
	if(fImageSet==NULL)
	{
		printf("can not open the (%s) file\n",ImageSet);
		return;
	}
	while(fgets(bufferImage,500,fImageSet)!=NULL)
	{
		bufferImage[strlen(bufferImage)-1]='\0';
		printf("bufferImage:%s\n",bufferImage);
		strcat(ImageFile,bufferImage);
		strcat(ImageFile,fmt[0]);
		printf("ImageFile:(%s)\n",ImageFile);
		IplImage *img_src=cvLoadImage(ImageFile);
		strcat(LabelFile,bufferImage);
		strcat(LabelFile,fmt[1]);
		printf("label file (%s)\n",LabelFile);
		FILE *Labelfile=fopen(LabelFile,"r");
		if(Labelfile==NULL)
		{
			printf("can not open the (%s) file\n",LabelFile);
			return;
		}
		strcat(forthChannelFile,bufferImage);
		FILE *forth_stream=fopen(forthChannelFile,"wb");

		//char temp_label_buffer[500];
		char *forth_value=(char*)malloc(img_src->height*img_src->width);
		memset(forth_value,0,img_src->height*img_src->width);
		srand((unsigned)time(NULL));
		while(fscanf(Labelfile,"%d,%d,%d,%d,%d",&class_index,&w_x1,&w_x2,&h_y1,&h_y2)!=EOF)
		{
			printf("%d,%d,%d,%d,%d\n",class_index,w_x1,w_x2,h_y1,h_y2);
			if(Speed[class_index][0]==Speed[class_index][1])
			{
				for(int i=h_y1;i<h_y2;++i)
				{
					for(int j=w_x1;j<w_x2;++j)
					{
						forth_value[i*(w_x2-w_x1)+j]=Speed[class_index][0];
					}
				}
			}
			else
			{
				for(int i=h_y1;i<h_y2;++i)
				{
					for(int j=w_x1;j<w_x2;++j)
					{
						forth_value[i*(w_x2-w_x1)+j]=rand()%Speed[class_index][1]+Speed[class_index][0];
					}
				}
			}
		}
		fwrite(forth_value,sizeof(char),img_src->height*img_src->width,forth_stream);
		memset(ImageFile+strlen(ImageFolder),'\0',strlen(bufferImage)+5);
		memset(LabelFile+strlen(LabelSet),'\0',strlen(bufferImage)+5);
		memset(forthChannelFile+strlen(ForthSet),'\0',strlen(bufferImage));
		free(forth_value);
		fclose(forth_stream);
		fclose(Labelfile);
		cvReleaseImage(&img_src);
	}
	fclose(fImageSet);
	return;
}


int main(int argc, char *argv[])
{
	strcpy(ImageFile,ImageFolder);
	strcpy(LabelFile,LabelSet);
	strcpy(forthChannelFile,ForthSet);
	make_the_fourth_channal_file();
   return 0;
}
