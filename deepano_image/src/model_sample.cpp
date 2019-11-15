#include "model_sample.h"
#include<sys/time.h>
#include <iostream>
#include <string>
using namespace std;

char* pNetGraph_filename[PNET_NUM] = {PNetGraph0_FILENAME, PNetGraph1_FILENAME, 
									PNetGraph2_FILENAME, PNetGraph3_FILENAME, PNetGraph4_FILENAME, 
									PNetGraph5_FILENAME, PNetGraph6_FILENAME, PNetGraph7_FILENAME};
const int imagepyramid_width[10] =  {229, 162, 115,  81,  57,  41, 29, 20,24,48}   ;
const int imagepyramid_height[10] = {407, 288, 203, 144, 102, 72, 51, 36,24,48}    ;
const int LengthResult[]={6*199*110,6*139*76,6*97*53,6*67*36,6*46*24,6*31*16,6*21*10,6*13*5,6,16};
const int boxshape2_array[PNET_NUM]= {199, 139, 97, 67, 46, 31, 21, 13};
const int boxshape3_array[PNET_NUM]= {110,  76, 53, 36, 24, 16, 10,  5};
const float all_scales[PNET_NUM]= {0.2118, 0.149531, 0.105569, 0.074532, 0.052619, 0.037149, 0.026227, 0.018516};



#define NetGrpahh0_FILENAME  "./model/face_detector.graph"
#define NetGrpahh1_FILENAME  "./model/facenet.graph"
#define NetGrpahh2_FILENAME  "./model/age.graph"
#define NetGrpahh3_FILENAME  "./model/gender.graph"
#define NetNum 4
char *modelGpahe_filename[NetNum] = {NetGrpahh0_FILENAME, NetGrpahh1_FILENAME, NetGrpahh2_FILENAME, NetGrpahh3_FILENAME};
string imgFile = "./data/sample.jpg";
float ssdMeanValue[] = {127.5f, 127.5f, 127.5f};//face-detector 
float ssdStdValue[] = {0.007843f, 0.007843f, 0.007843f};
float fNetMeanValue[] = {127.5f, 127.5f, 127.5f};//facenet
float fNetStdValue[] = {0.007843f, 0.007843f, 0.007843f};
float ageNetMeanValue[] = {78.42633776f, 87.76891437f, 114.89584775f};//age
float ageNetStdValue[] = {1.0f, 1.0f, 1.0f};
float genderNetMeanValue[] = {78.42633776f, 87.76891437f, 114.89584775f};//gender
float genderNetStdValue[] = {1.0f, 1.0f, 1.0f};
int srcImgInfo[] = {353, 500}; //src img widht & height
dp_image_box_t box = {0,0,0,0};

/**************************/
struct timeval start;
struct timeval infernece_end;
/*************************/

dp_image_box_t showGraphResult(float *graphResult)
{
	dp_image_box_t box_demo;
	int num_valid_boxes=int(graphResult[0]);
	cout<<"num_valid_boxes: "<<num_valid_boxes <<endl;
	for(int box_index=0;box_index<num_valid_boxes;box_index++)
	{
		int base_index=7*box_index+7;
		if(graphResult[base_index+6]<0||graphResult[base_index+6]>=1||graphResult[base_index+5]<0||graphResult[base_index+5]>=1||graphResult[base_index+4]<0||graphResult[base_index+4]>=1||graphResult[base_index+3]<0||graphResult[base_index+3]>=1||graphResult[base_index+2]>=1||graphResult[base_index+2]<0||graphResult[base_index+1]<0)
		{
			continue;
		}
		box_demo.x1=(int(graphResult[base_index+3]*srcImgInfo[0])>0)?int(graphResult[base_index+3]*srcImgInfo[0]):0;
		box_demo.x2=(int(graphResult[base_index+5]*srcImgInfo[0])<srcImgInfo[0])?int(graphResult[base_index+5]*srcImgInfo[0]):srcImgInfo[0];
		box_demo.y1=(int(graphResult[base_index+4]*srcImgInfo[1])>0)?int(graphResult[base_index+4]*srcImgInfo[1]):0;
		box_demo.y2=(int(graphResult[base_index+6]*srcImgInfo[1])<srcImgInfo[1])?int(graphResult[base_index+6]*srcImgInfo[1]):srcImgInfo[1];
		cout<< "x1: " <<box_demo.x1 <<" x2: " << box_demo.x2<<" y1: "<< box_demo.y1<<" y2: "<<box_demo.y2<<endl;
	}
	return box_demo;
}

void showFacenetResult(float *graphResult, int graphLength)
{
	for(int ii =0; ii<2; ii++)
	{
		int jj=ii*6;
		cout<< " "<<graphResult[jj+0]<<" "<<graphResult[jj+1]<<" "<<graphResult[jj+2]<<" "<<graphResult[jj+3]<<" "<<graphResult[jj+4]<<" "<<graphResult[jj+5]<<endl;
	}
}

void showAgeResult(float *graphResult)
{
	char *age_list[]={"0-2","4-6","8-12","15-20","25-32","38-43","48-53","60-100"};
	float max_predected =0.0f;
	int age_index =0;
    for(int i=0;i<7;i++)
    {
	  if(max_predected<graphResult[i])
      {
          max_predected=graphResult[i];
	      age_index=i;
       }
    }
	printf("the age range is:%s and the confidence of %.4f\n", age_list[age_index], max_predected);
}

void showGenderResult(float *graphResult)
{
	char *age_list[]={"Male","Female"};
	float max_predected =0.0f;
	int index =0;
    for(int i=0;i<2;i++)
    {
	  if(max_predected<graphResult[i])
      {
          max_predected=graphResult[i];
	      index=i;
       }
    }
	printf("the gender is:%s and the confidence of %.4f\n", age_list[index], max_predected);
}



void multi_model_sample(int argc,char *argv[]){
	int ret;
	dp_blob_parm_t parms[NetNum]={{300,300,707*2}, {160,160,512*2}, {227,227,8*2}, {227, 227, 2*2}};
	test_update_model_parems(NetNum,parms);
/*&&&&&&&&&&&&&&&&&&&&&&&&&&send graph to device &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
	for (int ii =0; ii< NetNum; ii++){
		ret = dp_update_model(modelGpahe_filename[ii]);
		if (ret == 0) {
			printf("Test dp_update_model(%s) sucessfully!\n", modelGpahe_filename[ii]);
		}
		else {
			printf("Test dp_update_model(%s) failed ! ret=%d\n", modelGpahe_filename[ii], ret);
		}
	}

	IMAGE_process srcImg (ssdMeanValue, ssdStdValue, srcImgInfo[0], srcImgInfo[1]);
	for(int ii=0; ii<10; ii++)
	{
	/*##############################first graph movidius inference################################################################*/
		printf("\033[1;31;46m  this %d times inference test start\033[0m\n", ii);
		unsigned short* imgBuf = srcImg.loadimage(imgFile.c_str(), 0, box, 300, ssdMeanValue, ssdStdValue);
		int BufSize = sizeof(unsigned short)*parms[0].InputSize_width*parms[0].InputSize_height;
		unsigned int graphLength = parms[0].Output_num/2; //graphSize
		gettimeofday(&start, NULL);
		float * graphResult = movidius_inference_result(imgBuf, BufSize, 0, graphLength);
		gettimeofday(&infernece_end, NULL);
		cout << "the first graph inference runtime :" << 1000*(infernece_end.tv_sec - start.tv_sec) + (infernece_end.tv_usec - start.tv_usec)/1000<<" ms"<<endl;
		
		dp_image_box_t box_demo =showGraphResult(graphResult);
		cout << " ===================================" << endl;

	/*###############################seceond graph movidius inference#################################################################
	*if you get roi box numbers are greater than 1, 
	*then you still need send to device one by one, 
	*the sample code assume that get only one roi box from first graph
	################################################################################################################################*/
		unsigned short *imgBuf_fNet = srcImg.loadimage(imgFile.c_str(), 1, box_demo, 160, fNetMeanValue, fNetStdValue);
		BufSize = sizeof(unsigned short)*parms[1].InputSize_width*parms[1].InputSize_height;
		graphLength =  parms[1].Output_num/2; //graphSize
		gettimeofday(&start, NULL); 
		float *graphResult_fNet = movidius_inference_result(imgBuf_fNet, BufSize, 1, graphLength);
		
		gettimeofday(&infernece_end, NULL);
		cout << "the second graph inference runtime :" << 1000*(infernece_end.tv_sec - start.tv_sec) + (infernece_end.tv_usec - start.tv_usec)/1000<<" ms"<<endl;
		showFacenetResult(graphResult_fNet, graphLength);
		cout << " ===================================" << endl;


	/*###############################third graph movidius inference#################################################################*/
		unsigned short *imgBuf_age = srcImg.loadimage(imgFile.c_str(), 1, box_demo, 227, ageNetMeanValue, ageNetStdValue);
		BufSize = sizeof(unsigned short)*parms[2].InputSize_width*parms[2].InputSize_height;
		graphLength =  parms[2].Output_num/2; //graphSize
		gettimeofday(&start, NULL);
		float *ageResult = movidius_inference_result(imgBuf_age, BufSize, 2, graphLength);
		gettimeofday(&infernece_end, NULL);
		cout << "the third graph inference runtime :" << 1000*(infernece_end.tv_sec - start.tv_sec) + (infernece_end.tv_usec - start.tv_usec)/1000<<" ms"<<endl;
		showAgeResult(ageResult);
		cout << " ===================================" << endl;


	/*###############################second graph movidius inference#################################################################*/
		unsigned short *imgBuf_gender = srcImg.loadimage(imgFile.c_str(), 1, box_demo, 227, genderNetMeanValue, genderNetStdValue);
		BufSize = sizeof(unsigned short)*parms[3].InputSize_width*parms[3].InputSize_height;
		graphLength =  parms[3].Output_num/2; //graphSize
		gettimeofday(&start, NULL);
		float *genderGraph = movidius_inference_result(imgBuf_gender, BufSize, 3, graphLength);
		gettimeofday(&infernece_end, NULL);
		cout << "the fourth inference runtime :" << 1000*(infernece_end.tv_sec - start.tv_sec) + (infernece_end.tv_usec - start.tv_usec)/1000<<" ms"<<endl;
		showGenderResult(genderGraph);
		cout << "================================="<< endl;
		printf("\033[1;31;46m  this %d times inference test end\033[0m\n", ii);
	}
	cout<<"this is the whole code flew process while process multi_model_graph" << endl;
	dp_send_stop_cmd(); //send cmd to device ,make device reset memory.
}
