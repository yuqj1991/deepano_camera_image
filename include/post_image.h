//
// Created by chenchen on 11/07/18.
//

#ifndef DEEPANO_SDK_AND_EXAMPLES_POST_IMAGE_H
#define DEEPANO_SDK_AND_EXAMPLES_POST_IMAGE_H

#include "share_utils.h"

class POST_IMAGE_MODEL{
public:
    void print_ssd_mobilet_result(void *fathomOutput,Box *box_demo,int *num_box_demo);
    void print_tiny_yolov1_net_result(void *fathomOutput,Box *box_demo,int *num_box_demo);
    void print_tiny_yolov2_result(void *fathomOutput,Box *box_demo,int *num_box_demo);
    void print_mobilinet_net_result(void *fathomOutput);
private:

};


#endif //DEEPANO_SDK_AND_EXAMPLES_POST_IMAGE_H
