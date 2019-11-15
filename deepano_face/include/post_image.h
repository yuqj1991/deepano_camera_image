//
// Created by yuqianjin on 11/07/18.
//

#ifndef DEEPANO_SDK_AND_EXAMPLES_POST_IMAGE_H
#define DEEPANO_SDK_AND_EXAMPLES_POST_IMAGE_H

#include "share_utils.h"
#include <vector>
#include <iostream>
#include <map>
using namespace std;

float cosValueThresold = 0.65f;
float euclideanValueThresold = 0.25f;
struct encodeFeature{
        std::vector<float> featureFace;
};
struct featureCmp{
    bool operator()(const encodeFeature &leftValue, const encodeFeature &rightValue) const{
        float top =0.0f, bottomLeft=0.0f, bottomRight=0.0f, EuclideanValue = 0.0f;
        assert(leftValue.featureFace.size()==rightValue.featureFace.size());
        assert(leftValue.featureFace.size() == 512);
        for(int ii = 0; ii < 512; ii++){
            top += leftValue.featureFace[ii]*rightValue.featureFace[ii];
            bottomLeft += leftValue.featureFace[ii]*leftValue.featureFace[ii];
            bottomRight += rightValue.featureFace[ii]*rightValue.featureFace[ii];
            EuclideanValue += std::pow((leftValue.featureFace[ii]-rightValue.featureFace[ii]),2);
        }
        float cosValue = (float) (top/(sqrt(bottomLeft)*sqrt(bottomRight)));
        if(cosValue >= cosValueThresold || EuclideanValue <= euclideanValueThresold){
            return false;
        }else{
            if(bottomLeft != bottomRight){
                return bottomLeft < bottomRight;
            }else{
                return (bottomLeft + 0.00025) > bottomRight;
            }
        }
    }
};
typedef std::map<encodeFeature, std::string, featureCmp> mapFeature;


class POST_IMAGE_MODEL{
public:
    void print_ssd_mobilet_result(void *fathomOutput,Box *box_demo,int *num_box_demo);
    void print_age_net_result(void *fathomOutput);
    void print_gender_net_result(void *fathomOutput);
	int print_facenet_result(float *fathomOutput);
public:	
	mapFeature m_faceDataBase;
	std::vector<std::string> m_facedescription;
};
#endif //DEEPANO_SDK_AND_EXAMPLES_POST_IMAGE_H
