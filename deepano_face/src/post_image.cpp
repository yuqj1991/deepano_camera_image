//
// Created by chenchen on 11/07/18.
//
#include "post_image.h"
#include <math.h>

void POST_IMAGE_MODEL::print_ssd_mobilet_result(void *fathomOutput,Box *box_demo,int *num_box_demo)
{
    u16* probabilities = (u16*)fathomOutput;
    unsigned int resultlen=707;
    float*resultfp32;
    resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
    int img_width=1280;
    int img_height=960;
    for (u32 i = 0; i < resultlen; i++)
        resultfp32[i]= f16Tof32(probabilities[i]);
    int num_valid_boxes=int(resultfp32[0]);
    int index=0;
    for(int box_index=0;box_index<num_valid_boxes;box_index++)
    {
        int base_index=7*box_index+7;
        if(resultfp32[base_index+6]<0||resultfp32[base_index+6]>=1||
                resultfp32[base_index+5]<0||resultfp32[base_index+5]>=1||
                        resultfp32[base_index+4]<0||resultfp32[base_index+4]>=1||
                            resultfp32[base_index+3]<0||resultfp32[base_index+3]>=1||
                                resultfp32[base_index+2]>=1||resultfp32[base_index+2]<0||
                                    resultfp32[base_index+1]<0){
            continue;
        }
        box_demo[index].xmin=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
        box_demo[index].xmax=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
        box_demo[index].ymin=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;
        box_demo[index].ymax=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
	if(box_demo[index].xmin-30>0)
		box_demo[index].xmin=box_demo[index].xmin-30;
	if(box_demo[index].xmax+30<img_width)
		box_demo[index].xmax=box_demo[index].xmax+30;
	if(box_demo[index].ymin-20>0)
		box_demo[index].ymin=box_demo[index].ymin-20;
	if(box_demo[index].ymax+30<img_height)
		box_demo[index].ymax=box_demo[index].ymax+30;
        index++;
    }
    *num_box_demo=index;
    free(resultfp32);
}

void POST_IMAGE_MODEL::print_age_net_result(void *fathomOutput){
    char *age_list[]={"0-2","4-6","8-12","15-20","25-32","38-43","48-53","60-100"};
    u16* probabilities=(u16*)fathomOutput;
    unsigned int resultlen=8;
    float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
    for(u32 i=0;i<resultlen;i++)
        resultfp32[i]=f16Tof32(probabilities[i]);
    max_age pre_age = {0,0};
    for(int i=0;i<resultlen;i++)
    {
        if(pre_age.max_predected<=resultfp32[i])
        {
            pre_age.max_predected=resultfp32[i];
            pre_age.index=i;
        }
    }
    printf("the age range is:%s and the confidence of%.2f\n",age_list[pre_age.index],pre_age.max_predected);
    free(resultfp32);
}

void POST_IMAGE_MODEL::print_gender_net_result(void *fathomOutput)
{
    char *gender_list[]={"Male","Female"};
    u16* probabilities=(u16*)fathomOutput;
    unsigned int resultlen=2;
    float *resultfp32=(float *)malloc(resultlen*sizeof(*resultfp32));
    for(u32 i=0;i<resultlen;i++)
        resultfp32[i]=f16Tof32(probabilities[i]);
    max_age pre_gender = {0,0};
    for(int i=0;i<resultlen;i++)
    {
        if(pre_gender.max_predected<=resultfp32[i])
        {
            pre_gender.max_predected=resultfp32[i];
            pre_gender.index=i;
        }
    }
    printf("the age range is:%s and the confidence of %.2f\n",gender_list[pre_gender.index],pre_gender.max_predected);
    free(resultfp32);
}

int POST_IMAGE_MODEL::print_facenet_result(float *fathomOutput){
	int index;
	unsigned int resultlen = 512;
	encodeFeature newfeature;
	for(u32 i=0;i<resultlen;i++){
		newfeature.featureFace.push_back(fathomOutput[i]);
	}
	if(m_faceDataBase.size()==0){
		m_faceDataBase.insert(std::make_pair(newfeature, "person_0"));
	}
	else{
		if (m_faceDataBase.find(newfeature)==m_faceDataBase.end()){
            printf("所检测到该人不在该数据库中，是否添加进来");
			//m_faceDataBase.push_back(newfeature);
			//m_facedescription.push_back("person_" +to_string(m_faceDataBase.size()));
		}else{
            printf("已经检测到该人\n");
        }
	}
	return index;
}

