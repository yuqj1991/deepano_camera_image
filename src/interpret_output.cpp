
#include "interpret_output.h" 


void getbox(float *result, float *pro_obj, int *idx_class, int *bboxs, int *bboxs_num, float thresh, int img_width, int img_height)
{
	float pro_class[49];
	int idx;
	int max_idx;
	float max;

	int Fathom=0;

	if(Fathom==0)//to normalization the Fathom output
	{
	int Fathom_prob_max_idx_i=0;
	int Fathom_prob_max_idx_j=0;
	int Fathom_prob_max_idx_k=0;
	int Fathom_scale_max_idx_i=0;
	int Fathom_scale_max_idx_j=0;
	int Fathom_scale_max_idx_m=0;		
	float Fathom_prob_max=0.00001f;
	float Fathom_scale_max=0.00001f;
	
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; ++j)
		{
			for (int k = 0; k < 20; k++)
			{
				if (result[(i * 7 + j) * 20  + k] > Fathom_prob_max)
				{
					Fathom_prob_max = result[(i * 7 + j) * 20 + k];
					Fathom_prob_max_idx_i=i;
					Fathom_prob_max_idx_j=j;
					Fathom_prob_max_idx_k=k;	
				}

			}
			for (int m = 0; m < 2; m++)
			{
				if (result[ 980 + (i * 7 + j) * 2  + m] > Fathom_scale_max)
				{
					Fathom_scale_max = result[ 980 + (i * 7 + j) * 2 + m];
					Fathom_scale_max_idx_i=i;
					Fathom_scale_max_idx_j=j;
					Fathom_scale_max_idx_m=m;	
				}

			}
		}
	}
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; ++j)
		{
			for (int k = 0; k < 20; k++)
			{
				result[(i * 7 + j) * 20 + k]/=Fathom_prob_max;

			}
			for (int m = 0; m < 2; m++)
			{
				result[ 980 + (i * 7 + j) * 2 + m]/=Fathom_scale_max;
			}
		}
	}	
	}

	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; ++j)
		{
			max = 0;
			max_idx = 0;
			idx = (i * 7 + j) * 20;
			for (int k = 0; k < 20; k++)
			{
				if (result[idx + k] > max)
				{
					max = result[idx + k];
					max_idx = k + 1;
				}

			}
			idx_class[i * 7 + j] = max_idx;
			pro_class[i * 7 + j] = max;
			pro_obj[(i * 7 + j) * 2] = max*result[7 * 7 * 20 + (i * 7 + j) * 2];
			pro_obj[(i * 7 + j) * 2 + 1] = max*result[7 * 7 * 20 + (i * 7 + j) * 2 + 1];
		}
	}
	int x_min, x_max, y_min, y_max;
	float x, y, w, h;
/*
int source_image_width  = img_width;
        int source_image_height = img_height;
        float x_ratio =  float(source_image_width/NET_IMAGE_WIDTH);
        float y_ratio =  float(source_image_height/NET_IMAGE_HEIGHT);
    for(int i=0;i<result_num;i++)
    {
       memcpy(result_image[i].category,result[i].category,20);
       int center_x=int(result[i].x1*x_ratio);
       int center_y=int(result[i].y1*y_ratio);
       int half_width=int(result[i].x2*x_ratio)/2;
       int half_height=int(result[i].y2*y_ratio)/2;
       result_image[i].x=((center_x-half_width)>0?(center_x-half_width):0);
       result_image[i].y=((center_y-half_height)>0?(center_x-half_height):0);
       int x2=((center_x+half_width)<source_image_width?(center_x+half_width):source_image_width);
       int y2=((center_y+half_height)<source_image_height?(center_y+half_height):source_image_height);
       result_image[i].width=x2-result_image[i].x;
       result_image[i].height=y2-result_image[i].y;
       result_image[i].probability=result[i].probability;
    }

*/
	*bboxs_num = 0;
	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				if (pro_obj[(i * 7 + j) * 2 + k] > thresh)
				{

					idx = 49 * 20 + 49 * 2 + ((i * 7 + j) * 2 + k) * 4;
					x = img_width*(result[idx++] + j) / 7;
					y = img_height*(result[idx++] + i) / 7;
					w = img_width*result[idx] * result[idx++];
					h = img_height*result[idx] * result[idx];

					x_min = x - w / 2;
					y_min = y - h / 2;
					x_max = x + w / 2;
					y_max = y + h / 2;
					bboxs[*bboxs_num* 6 + 0] = idx_class[i * 7 + j];
					bboxs[*bboxs_num* 6 + 1] = x_min;
					bboxs[*bboxs_num* 6 + 2] = y_min;
					bboxs[*bboxs_num* 6 + 3] = x_max;
					bboxs[*bboxs_num* 6 + 4] = y_max;
					bboxs[*bboxs_num* 6 + 5] = (int)(pro_obj[(i * 7 + j) * 2 + k] * 100);
					*bboxs_num = *bboxs_num + 1;

				}
			}
		}
	}
}
int lap(int x1_min, int x1_max, int x2_min, int x2_max){
	if (x1_min < x2_min){
		if (x1_max < x2_min){
			return 0;
		}
		else{
			if (x1_max > x2_min){
				if (x1_max < x2_max){
					return x1_max - x2_min;
				}
				else{
					return x2_max - x2_min;
				}
			}
			else{
				return 0;
			}
		}
	}
	else{
		if (x1_min < x2_max){
			if (x1_max < x2_max)
				return x1_max - x1_min;
			else
				return x2_max - x1_min;
		}
		else{
			return 0;
		}
	}
}


void interpret_output(float *original_out, YOLO_Result *result, int * result_num, int img_width, int img_height,float threshold)
{
    char categories[20][20]={"aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
              "dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
	int w_img = img_width;
	int h_img = img_height;
	float overlap;
	float overlap_thresh = 0.05;
	int num_class = 20;

	float pro_obj[49][2];
	int idx_class[49];
	int bbox_num = 0;
	int bboxs[98][6];

	getbox(original_out, &pro_obj[0][0], idx_class, &bboxs[0][0], &bbox_num, threshold, w_img, h_img);
        int *mark = (int*)malloc(bbox_num*sizeof(int));
	for (int i = 0; i < bbox_num; i++)
	{
		mark[i] = 1;
	}
	for (int i = 0; i < bbox_num; ++i)
	{
		for (int j = i + 1; j < bbox_num; ++j)
		{
			int overlap_x = lap(bboxs[i][0], bboxs[i][2], bboxs[j][0], bboxs[j][2]);
			int overlap_y = lap(bboxs[i][1], bboxs[i][3], bboxs[j][1], bboxs[j][3]);
			overlap = (overlap_x*overlap_y)*1.0 / ((bboxs[i][0] - bboxs[i][2])*(bboxs[i][1] - bboxs[i][3]) + (bboxs[j][0] - bboxs[j][2])*(bboxs[j][1] - bboxs[j][3]) - (overlap_x*overlap_y));
			if (overlap > overlap_thresh)
			{
				if (bboxs[i][4] > bboxs[j][4])
				{

					mark[j] = 0;
				}
				else
				{
					mark[i] = 0;
				}
			}
		}
	}
	int filtered_bbox_num=0;  
	for (int i = 0; i < bbox_num; ++i)
	{
		if (mark[i])
		{
			strcpy(result[filtered_bbox_num].category, categories[bboxs[i][0] - 1]);
			result[filtered_bbox_num].class_idx =bboxs[i][0] - 1;
			result[filtered_bbox_num].x =(bboxs[i][3]+bboxs[i][1])/4;
			result[filtered_bbox_num].y =(bboxs[i][4]+bboxs[i][2])/4;
			result[filtered_bbox_num].width =(bboxs[i][3]-bboxs[i][1])/2;
			result[filtered_bbox_num].height =(bboxs[i][4]-bboxs[i][2])/2;
			result[filtered_bbox_num].probability=bboxs[i][5]*1.0/100.0;
			filtered_bbox_num++;
			*result_num=filtered_bbox_num;
		}
	}
	free(mark);
}

//new parse bloboutput alghtorightm
