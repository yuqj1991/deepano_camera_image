
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

#if 0
void boxes_to_pixel_units(float *all_boxes,int img_width, int img_height,int grid_size)
{
   int boxes_per_cell = 2;
   int box_offset[boxes_per_cell*grid_size*grid_size];  //convert 7*7*2  //14 once a time
   int new_box_offset[boxes_per_cell*grid_size*grid_size];
   for(int m=0;m<grid_size;m++)
   {
     for(int k=0;k<boxes_per_cell*grid_size;k++)
     {
       box_offset[m*boxes_per_cell*grid_size+k]=k/2;
       new_box_offset[m*boxes_per_cell*grid_size+k]=m;
     }
   }
   for(int m=0;m<boxes_per_cell*grid_size*grid_size;m++)
   {	     
	  all_boxes[0*4+m]+=box_offset[m];
	  all_boxes[1*4+m]+=new_box_offset[m];
   }
   for(int m=0;m<98;m++)
   {	     
	  all_boxes[0*4+m]=(float)(all_boxes[0*4+m]/grid_size);
	  all_boxes[1*4+m]=(float)(all_boxes[1*4+m]/grid_size);
   }   
    for(int m=0;m<98;m++)
   {	     
	  all_boxes[2*4+m]*=all_boxes[2*4+m];
	  all_boxes[3*4+m]*=all_boxes[3*4+m];
   }   
   for(int m=0;m<98;m++)
   {	     
	  all_boxes[0*4+m]*=img_width;
	  all_boxes[1*4+m]*=img_height;
	  all_boxes[2*4+m]*=img_width;
	  all_boxes[3*4+m]*=img_height;
   }
   printf("boxes_to_pixel_units end\n");   
}

//返回最大数组索引下标
int cmpdata_maxindex(float* a,int m)
{
  float tmp=a[0];
  int maxindex=0;
  for(int i=0;i<m;i++)
  {
     if(tmp<a[i])
     {
        tmp=a[i];
        maxindex=i;
     }
  }
  return maxindex;
}
//返回最大值
float cmpdata_max(float a,float b)
{
   if(a>b)
     return a;
   else
     return b; 
}
//返回最小值
float cmpdata_min(float a,float b)
{
   if(a>b)
     return b;
   else
     return a;
}
//获取数组从大到小的值的索引下标数组
int *dmpsort_down(float *a,int m)
{
   int *dmp=(int*)malloc(m*sizeof(int));
   float tmp=a[0];
   int maxindex=0;
   for(int j=0;j<m;j++)
   {
      tmp=a[j];
      for(int i=j+1;i<m;i++)
      {
        if(tmp<a[i])
        {
          tmp=a[i];
          maxindex=i; 
        }
      }
      a[maxindex]=a[j];
      a[j]=tmp;
      dmp[j]=maxindex;      
   }
   return dmp;  
}
float get_intersection_over_union(yolo_box box_1,yolo_box box_2)
{
   float intersection_dim_1=cmpdata_min((box_1.x1+0.5*box_1.x2),(box_2.x1+0.5*box_2.x2))-cmpdata_max((box_1.x1-0.5*box_1.x2),(box_2.x1-0.5*box_2.x2));
   float intersection_dim_2=cmpdata_min((box_1.y1+0.5*box_1.y2),(box_2.y1+0.5*box_2.y2))-cmpdata_max((box_1.y1-0.5*box_1.y2),(box_2.y1-0.5*box_2.y2));
   float intersection_area,iou,union_area;
   if(intersection_dim_1<0||intersection_dim_2<0)
   {
      intersection_area=0;
   }
   else
      intersection_area=intersection_dim_1*intersection_dim_2;
   union_area=box_1.x2*box_1.y2+box_2.x2*box_2.y2-intersection_area;
   iou=(float)(intersection_area / union_area);
printf("iou:%f\n",iou);
   return iou;
}

int* get_duplicate_box_mask(yolo_box *Box,float max_iou,int length_box,int index)
{
    int box_mask[length_box];
    for(int i=0;i<length_box;i++)
    {
       box_mask[i]=1;
    }
    for(int i=0;i<length_box;i++)
    {
       if(box_mask[i]==0) continue;
       for(int j=i+1;j<length_box;j++)
       {
          if(get_intersection_over_union(Box[i],Box[j])>max_iou)
               box_mask[j]=0.0;      
       }
    }
    for(int i=0;i<length_box;i++)
    {
       if(box_mask[i]==1) 
         { index++;
printf("1----\n");}
    }
    return box_mask;
}

void filter_object(float *original_out,int img_width, int img_height,float threshold,YOLO_result_org *result,int result_num)
{
    char categories[20][20]={"aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
              "dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
	int w_img = img_width;
	int h_img = img_height;
	int num_classifications=20;
	int grid_size=7;
	int boxes_per_grid_cell=2;
	float all_probabilities[grid_size*grid_size*boxes_per_grid_cell*num_classifications];
	float classification_probabilities[grid_size*grid_size*num_classifications];
        int num_of_class_probs = grid_size*grid_size*num_classifications;
	float probability_threshold = threshold;
	for(int i=0;i<grid_size;i++)
	{
	  for(int j=0;j<grid_size;j++)
	  {
	     for(int m=0;m<num_classifications;m++)
	     {
	       classification_probabilities[i*grid_size*num_classifications+j*num_classifications+m]=original_out[i*grid_size*num_classifications+j*num_classifications+m];
	     }
	  }
	}
	
	float box_prob_scale_factor[grid_size*grid_size*boxes_per_grid_cell];
        for(int i=0;i<grid_size;i++)
	{
	  for(int j=0;j<grid_size;j++)
	  {
	     for(int m=0;m<boxes_per_grid_cell;m++)
	     {
	       box_prob_scale_factor[i*grid_size*boxes_per_grid_cell+j*boxes_per_grid_cell+m]=original_out[num_of_class_probs+i*grid_size*boxes_per_grid_cell+j*boxes_per_grid_cell+m];
	     }
	  }
	}

	float all_boxes[grid_size*grid_size*boxes_per_grid_cell*4];
        for(int i=0;i<grid_size;i++)
	{
	  for(int j=0;j<grid_size;j++)
	  {
	    for(int n=0;n<boxes_per_grid_cell;n++)
	   {
	     for(int m=0;m<4;m++)
	     {
	       all_boxes[i*grid_size*boxes_per_grid_cell*4+j*boxes_per_grid_cell*4+n*4+m]=original_out[1078+i*grid_size*boxes_per_grid_cell*4+j*boxes_per_grid_cell*4+n*4+m];
	     }
	   }
	 }
        }

	boxes_to_pixel_units(all_boxes,img_width,img_height,grid_size);
//may have some problems

        for(int box_index=0;box_index<boxes_per_grid_cell;box_index++)
	{
            for(int class_index=0; class_index<num_classifications;class_index++)
	    {  
                for(int m=0;m<grid_size*grid_size;m++)
                {
                    all_probabilities[class_index*num_classifications+box_index*boxes_per_grid_cell+m]=classification_probabilities[class_index*20+m]*box_prob_scale_factor[box_index*2+m];
                }   
	   }
	}

        int probability_threshold_mask[grid_size*grid_size*boxes_per_grid_cell*num_classifications];
        int nonzero_num=0;
	for(int m=0;m<grid_size*grid_size*boxes_per_grid_cell*num_classifications;m++)
	{
	  if(all_probabilities[m]>=probability_threshold)
	  {
	      probability_threshold_mask[m]=1;
	      nonzero_num++;
	  }
	  else
	  {
	  	probability_threshold_mask[m]=0;
	  }
	}
printf("nonzero_num_1:%d\n",nonzero_num);
        int box_threshold_mask[nonzero_num*4];
        int mask_box_i[nonzero_num],mask_box_j[nonzero_num],mask_box_n[nonzero_num],mask_box_m[nonzero_num];
        int nonnum=0;
        for(int i=0;i<7;i++)
        {
           for(int j=0;j<7;j++)
           {
	     for(int n=0;n<2;n++)
	      {
	         for(int m=0;m<20;m++)
	         {
	           if(probability_threshold_mask[i*7*2*20+j*2*20+n*20+m]==1)
	           {
	             mask_box_i[nonnum]=i;
                     mask_box_j[nonnum]=j;
                     mask_box_n[nonnum]=n;
                     mask_box_m[nonnum]=m;
		     nonnum++;
	           }
	        }
	    }
          }
        }
printf("nonnum_2:%d\n",nonnum);
        for(int i=0;i<nonzero_num;i++)
        {
          box_threshold_mask[i*4+0]=mask_box_i[i];
          box_threshold_mask[i*4+1]=mask_box_j[i];
          box_threshold_mask[i*4+2]=mask_box_n[i];
          box_threshold_mask[i*4+3]=mask_box_m[i];
        }
        yolo_box boxes_above_threshold[nonzero_num];
        for(int i=0;i<nonzero_num;i++)
        {
          boxes_above_threshold[i].x1=all_boxes[box_threshold_mask[i*4+0]*7*2*4+box_threshold_mask[i*4+1]*2*4+box_threshold_mask[i*4+2]*4+0];
          boxes_above_threshold[i].y1=all_boxes[box_threshold_mask[i*4+0]*7*2*4+box_threshold_mask[i*4+1]*2*4+box_threshold_mask[i*4+2]*4+1];
          boxes_above_threshold[i].x2=all_boxes[box_threshold_mask[i*4+0]*7*2*4+box_threshold_mask[i*4+1]*2*4+box_threshold_mask[i*4+2]*4+2];
          boxes_above_threshold[i].y2=all_boxes[box_threshold_mask[i*4+0]*7*2*4+box_threshold_mask[i*4+1]*2*4+box_threshold_mask[i*4+2]*4+3];
        }
      
        int for_boxes_above[7*7*2];
        float tmpdata[20];
        for(int i=0;i<7;i++)
        {
          for(int j=0;j<7;j++)
          {
             for(int m=0;m<2;m++)
             {
               for(int n=0;n<20;n++)
               {
                 tmpdata[n]=all_probabilities[i*7*2*20+j*2*20+m*20+n];
               }
               for_boxes_above[i*7*2+j*2+m]=cmpdata_maxindex(tmpdata,20);
            }
          }
        }
        int classifications_for_boxes_above[nonzero_num];
        for(int i=0;i<nonzero_num;i++)
        {
           classifications_for_boxes_above[i]=for_boxes_above[box_threshold_mask[i*4+0]*7*2+box_threshold_mask[i*4+1]*2+box_threshold_mask[i*4+2]];
        }
        float probabilities_above_threshold[nonzero_num];
        int index=0;
        for(int i=0;i<7;i++)
        {
          for(int j=0;j<7;j++)
          {
	     for(int n=0;n<2;n++)
	     {
	       for(int m=0;m<20;m++)
	       {
	         if(probability_threshold_mask[i*7*2*20+j*2*20+n*20+m]==1)
	         {
	           probabilities_above_threshold[index]=all_probabilities[i*7*2*20+j*2*20+n*20+m];
                   index++;  
	         }             
	       }
	     }
          }
        }
printf("-----index:%d---\n",index);
        int *argsort=dmpsort_down(probabilities_above_threshold,nonzero_num);
        yolo_box new_boxes_above_threshold[nonzero_num];
        for(int i=0;i<nonzero_num;i++)
        {
           new_boxes_above_threshold[i].x1=boxes_above_threshold[argsort[i]].x1;
           new_boxes_above_threshold[i].x2=boxes_above_threshold[argsort[i]].x2;
           new_boxes_above_threshold[i].y1=boxes_above_threshold[argsort[i]].y1;
           new_boxes_above_threshold[i].y2=boxes_above_threshold[argsort[i]].y2;
        }
        int new_classifications_for_boxes_above[nonzero_num];
        for(int i=0;i<nonzero_num;i++)
        {
           new_classifications_for_boxes_above[i]=classifications_for_boxes_above[argsort[i]];
        }
        float new_probabilities_above_threshold[nonzero_num];
        for(int i=0;i<nonzero_num;i++)
        {
           new_probabilities_above_threshold[i]=probabilities_above_threshold[argsort[i]];
        }
        int index_num=0;
        int* duplicate_box_mask = get_duplicate_box_mask(new_boxes_above_threshold,0.55,nonzero_num,index_num);
printf("-----index_num:%d---\n",index_num);
        if(index_num!=0)
        {
           yolo_box new_boxes_above_threshold_valid[index_num];
           int new_classifications_for_boxes_above_valid[index_num];
           float new_probabilities_above_threshold_valid[index_num];
           int m_index=0;
           for(int n=0;n<nonzero_num;n++)
           {
             if(duplicate_box_mask[n]==1)
             {
              new_classifications_for_boxes_above_valid[m_index] =new_classifications_for_boxes_above[n];
              new_probabilities_above_threshold_valid[m_index] = new_probabilities_above_threshold[n];
              new_boxes_above_threshold_valid[m_index].x1=new_boxes_above_threshold[n].x1;
              new_boxes_above_threshold_valid[m_index].y1=new_boxes_above_threshold[n].y1;
              new_boxes_above_threshold_valid[m_index].x2=new_boxes_above_threshold[n].x2;
              new_boxes_above_threshold_valid[m_index].y2=new_boxes_above_threshold[n].y2;
              m_index++;     
            }
          }
          for(int i=0;i<m_index;i++)
          {
             memcpy(result[i].category,categories[new_classifications_for_boxes_above_valid[i]],20);
             result[i].x1     =new_boxes_above_threshold_valid[i].x1;
             result[i].y1     =new_boxes_above_threshold_valid[i].y1;
             result[i].x2     =new_boxes_above_threshold_valid[i].x2;
             result[i].y2     =new_boxes_above_threshold_valid[i].y2;
             result[i].probability=new_probabilities_above_threshold_valid[i];
          }
          result_num=m_index;
        }
printf("result_num:%d\n",result_num);
}
void display_image(YOLO_result_org *result,int img_width, int img_height,YOLO_Result *result_image,int result_num)
{
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
}

#endif
