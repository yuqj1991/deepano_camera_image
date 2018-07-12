请仔细阅读本文本的内容:谢谢！

安装请阅读install.md

-------------------------------
2018-07-05 新添接口，
1、使用接口
        int dp_send_second_image(unsigned short* imagebuffer,int buffersize,int box_num);
2、调用方法逻辑：
（1）、像之前的传入两个blob和相关的参数不变，在这之后，第一个模型解析摄像头buffer，并将结果返回给主机，主机接收，并同时接收传到主机的图像buffer（bgr），（接下来是你们需要操作的地方）：你们需要对这个buffer（变量bgr进行相关的操作（人脸对齐之类的）进行一些操作，产生一个或几个已处理好的buffer，注意（该buffer的数据类型为short，你需要将之转化为float32->float16类型），然后在box_callback_model_two_demo相应的模型接口下方添加代码，如下：

----------------------------------------------------------
类似双模型接口不需要做改变
//双模型tiny_yolo_v2+tiny_yolo_v2
void test_whole_model_2_video_tiny_yolo_v2(int argc, char *argv[])
{
	int ret;
	const char *filename = "../TINY_YOLO_V2.Blob";//"/home/yu/tini_yolo.blob";
        const char *filename2 = "../TINY_YOLO_V2.Blob";
	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,416,416,18000*2},{0,416,416,18000*2}};
        dp_netMean mean[2]={{0,0,0,255},{0,0,0,255}};
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);
	test_update_model_parems(blob_nums, parms);
        dp_set_blob_mean_std(blob_nums,mean);
	ret = dp_update_model(filename);
        
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	ret = dp_update_model_2(filename2);
	if (ret == 0) {
		printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
	}
	else {
		printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
	}
	DP_MODEL_NET net_1=DP_TINY_YOLO_V2_NET;
	dp_register_box_device_cb(box_callback_model_two_demo, &net_1);	
	DP_MODEL_NET net_2=DP_TINY_YOLO_V2_NET;
	dp_register_second_box_device_cb(box_callback_model_demo,&net_2);
	dp_register_video_frame_cb(video_callback, &net_1);
        dp_register_fps_device_cb(fps_callback,&net_1);
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
        while(1){
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

------------------------------------------------------------
结果解析回调函数
//双模型，解析获取box回传给板子
void box_callback_model_two_demo(void *result,void *param)
{
  DP_MODEL_NET model=*((DP_MODEL_NET*)param);
  switch (model)
  {
    
	case DP_SSD_MOBILI_NET:
	{
	     char *category[]={"background","aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
              "dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=707;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
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
		   box_demo[index].x1=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
		   box_demo[index].x2=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
		   box_demo[index].y1=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;	   
		   box_demo[index].y2=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
		   memcpy(categoles[index],category[int(resultfp32[base_index+1])],20);
		   index++;
		 }
------------------------------------------------------
以下代码，对你来说不需要，可以全部删掉
                 num_box_demo=index; 
                 int box_demo_num=0;
		 if((num_valid_boxes<=2)&&(num_valid_boxes>0))
		 {
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(num_valid_boxes*sizeof(dp_image_box_t));

		   for (int i = 0; i < num_valid_boxes; ++i)
           { 
			  if(((box_demo[i].x2-box_demo[i].x1)!=0)&&((box_demo[i].y2-box_demo[i].y1)!=0))
			  {
			     box_second[box_demo_num].x1=box_demo[i].x1;
			     box_second[box_demo_num].x2=box_demo[i].x2;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=box_demo[i].y1;	  
			     box_second[box_demo_num].y2=box_demo[i].y2;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
           }
		   dp_send_first_box_image(box_demo_num, box_second);
		   free(box_second);
		 }
		 else if(num_valid_boxes>2)
		 { 
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(2*sizeof(dp_image_box_t));
		   for(int i=0;i<num_valid_boxes;i++)
		   {
		     if(((box_demo[i].x2-box_demo[i].x1)!=0)&&((box_demo[i].y2-box_demo[i].y1)!=0))
			  {
			     box_second[box_demo_num].x1=box_demo[i].x1;
			     box_second[box_demo_num].x2=box_demo[i].x2;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=box_demo[i].y1;	  
			     box_second[box_demo_num].y2=box_demo[i].y2;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
			  if(box_demo_num==2)
			  {
			     break;
			  }
		   	}
		   dp_send_first_box_image(2, box_second);		   
		   free(box_second);
		 }
这部分需要删掉
-----------------------------------------
-----------------------------------------
以下是你需要添加的代码：
             第一步，上面ssd会产生很多box，你需要将这些box产生的坐标，在bgr（板子回传到主机的摄像头buffer，这是全局变量）上截取出这些相关的buffer，然后你们再做一些相关的处理
             最后调用int dp_send_second_image(unsigned short* imagebuffer,int buffersize,int box_num);//buffersize总字节数，box_num，产生的这些buffer个数
-----------------------------------------	          
        free(resultfp32);
		break;
	  }

dp_send_second_image(unsigned short* imagebuffer,int buffersize,int box_num);
 //imagebuffer，比如第一个模型分割出5个图，则imagebuffer是5个图的buffer，该数据类型为unsigned short*，也就是需要进行如下转换unsigned char->float->fp16;
 buffersize总字节数，比如5个图的总字节数，
 box_num，产生的这些buffer个数，比如说为5
 ps，每个图的字节数应该为第二个模型的输入的图像的长×宽×3;
最后，传回来的模型的结果数据类型是void*,需要做如下转换：void->unsigned short（类似于我在结果显示回调函数中的 u16* probabilities = (u16*)result）->float（类似于我在结果显示回调函数中的
            resultfp32[i]= f16Tof32(probabilities[i]);  
	 

2018.06.26:更新：test.cpp在图像显示回调函数中，添加锁
新添加dp_send_second_image(unsigned short* imagebuffer,int buffersize,int box_num);
 //imagebuffer，比如第一个模型分割出5个图，则imagebuffer是5个图的buffer，该数据类型为unsigned short*，也就是需要进行如下转换unsigned char->float->fp16;
 buffersize总字节数，比如5个图的总字节数，
 box_num，产生的这些buffer个数，比如说为5
 ps，每个图的字节数应该为第二个模型的输入的图像的长×宽×3;
最后，传回来的模型的结果数据类型是void*,需要做如下转换：void->unsigned short（类似于我在结果显示回调函数中的 u16* probabilities = (u16*)result）->float（类似于我在结果显示回调函数中的
            resultfp32[i]= f16Tof32(probabilities[i]);）,这之后也就是你想要的结果特征	  
	  

-------------------------------
2018.05.29:更新：为使板子通用固件的稳定性，将传入板子的模型1固定为55m，模型2固定为35m
请使用新的升级固件：cdk_FLIC_1xOV5658_TF.mvcmd（此为通用固件，模型文件大小可修改，如客户有特殊需求会单独邮件回复！）

-------------------------------
2018.05.25:更新：板子新增：
1)可以对模型解析的carmera图象帧进行自定义ROI,可以保证模型输入资料宽高比和送入检测的宽高比一致;
2）每次调试玩之后，不再需要重新插拔usb了
3）新增：    32位win库，为dp_api_d.dll和dp_api_d.lib;
             64位win库：dp_api.dll和dp_api.lib
自定义ROI接口：dp_set_blob_image_size（dp_image_box_t *box）;其中，dp_image_box_t{
        int x1;//box宽起始位
        int x2;//box宽终止位
        int y1;//box高起始位
        int y2;//box高终止位
}
此接口和传入模型参数以及均值和std的代码逻辑如下：
        dp_set_blob_image_size(&BLOB_IMAGE_SIZE);//自定义ROI
	test_update_model_parems(blob_nums, parms);//传入模型参数
        dp_set_blob_mean_std(blob_nums,mean);//传入mean和std
请使用新的升级固件：cdk_FLIC_1xOV5658_TF.mvcmd（此为通用固件，如客户有特殊需求会单独邮件回复！）
-------------------------------
2018.05.05:更新：板子新增：
获取模型解析所需的时间相应接口为：
dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);//解析模型所需要的时间注册回调函数，blob_parse_callback（）回调函数
此为回调函数，大约每隔40ms向主机发送数据
-------------------------------

[1]本SDK相关API简介：
   1、typedef void(*dp_video_frame_cb_t)(dp_img_t *img, void *param);//接收从设备返回的视频流回调接口
   2、typedef void(*dp_first_blob_outresult_back_cb_t)(float * result,void *param);int dp_register_video_frame_cb(dp_video_frame_cb_t cb, void *param);//接收解析第一blob之后的原始结果的回调函数接口
   3、typedef void(*dp_send_to_device_box_cb_t)(dp_image_box_t *box,void *param);//如果解析两个blob，则调用此回调函数，向设备端传输minibox坐标值
   4、typedef void(*dp_second_blob_outresult_back_cb_t)(void * result,void *param)；int dp_register_send_to_box_device_cb(dp_send_to_device_box_cb_t cb, void *param);//接收解析第二blob原始结果的回调函数接口
   5、EXTERN DLL int dp_set_blob_parms(int num_model,dp_blob_parm_t *param);//向设备端发送blob相关参数，注意，num_blob，表示向设备需要传输的个数
   6、EXTERN DLL int dp_update_model(const char* filename);//传输第一个blob
   7、EXTERN DLL int dp_update_model_2(const char* filename);//传输第二个blob
   8、EXTERN DLL int dp_start_camera_video();//打开摄像头获取视频流
   9、EXTERN DLL int dp_start_camera();//打开摄像头

[2]使用测试用例步骤：
   1-- cdk: 解析blob的一般步骤：
   开机（识别usb设备，需登上10s左右）->向设备端传输一个或两个blob的相关初始化参数->传输相应个数的blob->传输blob的mean和std(如果没有，则必须设mean为0,std为1,否则系统报错)->调用dp_start_camera()接口，设备端开始解析图像。
  （1）使用板子只解析一个blob，则可以借鉴test.cpp中的接口中的test_whole_model_1_video_SSD_MobileNet接口方法，以下详细说明
void test_whole_model_1_video_SSD_MobileNet(int argc, char *argv[])
{
	int ret;
	const char *filename = "../SSD_MobileNet.blob";//模型文件路径

	int blob_nums = 1; dp_blob_parm_t parms = {0,300,300,707*2};//传入的模型个数及相应的模型参数设置，如此例中，{0（是否为tf框架模型）,300（输入模型图片宽）,300（输入模型图片高）,707*2（解析模型的输出结果长度）}
    dp_netMean mean={127.5,127.5,127.5,127.5};//传入的模型图片三通道均值和标准差，如果没有，则设为{0,0,0,1}
	if (argc > 0)
	{
		filename = argv[0];
		blob_nums = atoi(argv[1]);
		for (int i = 0; i<blob_nums; i++)
		{
			parms.InputSize_height = atoi(argv[i * 3 + 2 + 0]);
			parms.InputSize_width = atoi(argv[i * 3 + 2 + 1]);
			parms.Output_num = atoi(argv[i * 3 + 2 + 2]);
		}
	}

	test_update_model_parems(blob_nums, &parms);//传入板子的模型参数
        dp_set_blob_mean_std(blob_nums,&mean);//向板子传输图像的均值和std
	ret = dp_update_model(filename);
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}

	DP_MODEL_NET net=DP_SSD_MOBILI_NET;//模型枚举常量
	dp_register_video_frame_cb(video_callback, &net);//注册视频流函数
	dp_register_box_device_cb(box_callback_model_demo,&net);//模型解析之后，结果显示注册函数box_callback_model_demo（）回调函数
        dp_register_fps_device_cb(fps_callback,&net);//视频帧回调注册函数，fps_callback（）回调函数
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);//解析模型所需要的时间注册回调函数，blob_parse_callback（）回调函数
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
	for (;;) {
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);//结果显示
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}
操作方法，其中对于回传到主机的模型结果显示的接口是在void box_callback_model_demo(void *result,void *param)，其中，param则为相应模型的枚举常量
对于ssd_mobilet模型来说，相应代码如下：
    case DP_SSD_MOBILI_NET:
	  {
	     char *category[]={"background","aeroplane","bicycle","bird","boat","bottle","bus","car","cat","chair","cow","diningtable",
              "dog","horse","motorbike","person","pottedplant","sheep","sofa","train","tvmonitor"};
		 u16* probabilities = (u16*)result;
         unsigned int resultlen=707;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
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
		   box_demo[index].x1=(int(resultfp32[base_index+3]*img_width)>0)?int(resultfp32[base_index+3]*img_width):0;
		   box_demo[index].x2=(int(resultfp32[base_index+5]*img_width)<img_width)?int(resultfp32[base_index+5]*img_width):img_width;
		   box_demo[index].y1=(int(resultfp32[base_index+4]*img_height)>0)?int(resultfp32[base_index+4]*img_height):0;	   
		   box_demo[index].y2=(int(resultfp32[base_index+6]*img_height)<img_height)?int(resultfp32[base_index+6]*img_height):img_height;
		   memcpy(categoles[index],category[int(resultfp32[base_index+1])],20);
		   index++;
		 }
                num_box_demo=index;
                free(resultfp32);
		 break;
	  }
  （2）使用板子解析两个blob的时候，则可以借鉴test.cpp中的test_whole_model_2_video接口
void test_whole_model_2_video_model(int argc, char *argv[])
{
	int ret;
	const char *filename = "../tini_yolo.blob";//第一个模型文件路径
	const char *filename2 = "../google.blob";//第二个模型文件路径

	int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,448,448,1470*2 },{0,224,224,1000*2}};//传入板子的两个模型的相应参数，其含义如上，例子
    dp_netMean mean[2]={{0,0,0,255},{104.0068,116.6886,122.6789,1}};//两个模型的图片处理，均值和std

	test_update_model_parems(blob_nums, parms);//向板子传入模型的参数
    dp_set_blob_mean_std(blob_nums,mean);//向板子传入均值
	ret = dp_update_model(filename);//传入第一个模型文件
	if (ret == 0) {
		printf("Test dp_update_model(%s) sucessfully!\n", filename);
	}
	else {
		printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
	}
	ret = dp_update_model_2(filename2);//传入第二个模型文件
	if (ret == 0) {
		printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
	}
	else {
		printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
	}
	FILE *fp=fopen("synset_words.txt","r");//以下是针对googlenet模型解析结果类型标签获取的代码
    if(fp==NULL)
    {
     printf("can not open the file\n");
    }
    label_cagerioes=(char **)malloc(1000*sizeof(char*));
    char buffer[500];
    int index_label=0;
    while(fgets(buffer,500,fp)!=NULL)
    {
      label_cagerioes[index_label]=(char*)malloc(500*sizeof(char));
      memcpy(label_cagerioes[index_label],buffer,500);
      for(int m=0;m<500;m++)
      {
        label_cagerioes[index_label][m]=label_cagerioes[index_label][m+10];
        if(m==489)
        {
            break;
        }
      }
	  label_cagerioes[index_label][strlen(label_cagerioes[index_label])-1]='\0';
      index_label++;
    }
	fclose(fp);
	
	DP_MODEL_NET net_1=DP_TINI_YOLO_NET;//第一个模型的枚举变量
	dp_register_box_device_cb(box_callback_model_two_demo, &net_1);	//解析第一个模型的结果，获取box，并将其回传回板子的注册回调函数，box_callback_model_two_demo回调函数
	DP_MODEL_NET net_2=DP_GOOGLE_NET;//第二个模型的枚举变量
	dp_register_second_box_device_cb(box_callback_model_demo,&net_2);//解析对box进行二次模型解析，并结果显示，box_callback_model_demo回调函数
	dp_register_video_frame_cb(video_callback, &net_1);//视频注册回调函数
        dp_register_fps_device_cb(fps_callback,&net_1);//视频帧回调函数
        dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);//解析模型所需时间的回调函数
	ret = dp_start_camera_video();
	if (ret == 0) {
		printf("Test test_start_video successfully!\n");
	}
	else {
		printf("Test test_start_video failed! ret=%d\n", ret);
	}

	const char *win_name = "video";
	namedWindow(win_name);
	int key = -1;
        while(1){
		video_mutex.lock();
		if (!bgr.empty())
			imshow(win_name, bgr);
		video_mutex.unlock();
		key = waitKey(30);
	}
	destroyWindow(win_name);
}

中的操作方法，其中void box_callback_model_two_demo(void *result,void *param)是解析第一个模型并产生box回传给板子的回调函数，针对tiny_yolov1,其相应接口代码为：
case DP_TINI_YOLO_NET:
    {
         u16* probabilities = (u16*)result;
         unsigned int resultlen=1470;
         YOLO_Result result[20];
         int result_num=0;
         float*resultfp32;
         resultfp32=(float*)malloc(resultlen * sizeof(*resultfp32));
         int img_width=1280;
         int img_height=960;
         for (u32 i = 0; i < resultlen; i++)
            resultfp32[i]= f16Tof32(probabilities[i]);
         interpret_output(resultfp32,result, &result_num, img_width, img_height,0.2);
         num_box_demo=result_num;
		  for (int i = 0; i < result_num; ++i)
           { 
              printf("class:%s, x:%d, y:%d, width:%d, height:%d, probability:%.2f.\n",result[i].category,result[i].x,result[i].y,result[i].width,result[i].height,result[i].probability);
              box_demo[i].x1=result[i].x;
              box_demo[i].x2=result[i].x+result[i].width;
	          if(box_demo[i].x2>img_width)
	 	         box_demo[i].x2=img_width;
              box_demo[i].y1=result[i].y;
              box_demo[i].y2=result[i].y+result[i].height;
	          if(box_demo[i].y2>img_height)
	 	         box_demo[i].y2=img_height;
	          memcpy(categoles[i],result[i].category,20);
           }
		 int box_demo_num=0;
		 if((result_num<=2)&&(result_num>0))
		 {
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(result_num*sizeof(dp_image_box_t));

		   for (int i = 0; i < result_num; ++i)
           { 
			  if((result[i].width!=0)&&(result[i].height!=0))
			  {
			     box_second[box_demo_num].x1=result[i].x;
			     box_second[box_demo_num].x2=result[i].x+result[i].width;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=result[i].y;	  
			     box_second[box_demo_num].y2=result[i].y+result[i].height;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
           }
		   dp_send_first_box_image(box_demo_num, box_second);  //回传到板子的box坐标
		   free(box_second);
		 }
		 else if(result_num>2)
		 { 
		   dp_image_box_t *box_second=(dp_image_box_t*)malloc(2*sizeof(dp_image_box_t));
		   for(int i=0;i<result_num;i++)
		   {
		      if((result[i].width!=0)&&(result[i].height!=0))
			  {
			     box_second[box_demo_num].x1=result[i].x;
			     box_second[box_demo_num].x2=result[i].x+result[i].width;
				 if(box_second[box_demo_num].x2>img_width)
				 	 box_second[box_demo_num].x2=img_width;
			     box_second[box_demo_num].y1=result[i].y;	  
			     box_second[box_demo_num].y2=result[i].y+result[i].height;
				 if(box_second[box_demo_num].y2>img_height)
				 	box_second[box_demo_num].y2=img_height;
				 box_demo_num++;
			  } 
			  if(box_demo_num==2)
			  {
			     break;
			  }
		   	}
		   dp_send_first_box_image(2, box_second);	//回传到板子的box坐标	   ，目前考虑到两个模型的性能时间，最好产生的box控制在两个box以内。
		   free(box_second);
		 }         
        free(resultfp32);
		break;

   第二个模型产生的输出结果在box_callback_model_demo（void* result,void*param）里面进行相应的代码操作，类似于同上。
  （3）关于系统初始化过程中的一些参数说明：
  （4）dp_blob_parm_t
  {
        int IsTensor_model;  //神经网络是tensor网络框架还是caffe网络框架
	int InputSize_width; //神经网络入口的图片尺寸
	int InputSize_height;//神经网络入口的图片尺寸
	int Output_num;//神经网络解析之后原始的数据长度，等于神经棒上mvnvGetResult()接口获取的原始数据长度; 
  }
 （5）dp_image_box_t{
       int x1;//box宽起始位
       int x2;//box宽终止位
       int y1;//box高起始位
       int y2;//box高终止位
}
 （6）typedef struct _dp_netMean
  {
    float R_mean;//图像前处理，r通道均值
    float G_mean;//图像前处理，g通道均值
    float B_mean;//图像前处理，b通道均值
    float Std;//图像前处理，std标准差
}dp_netMean;如果模型没有对图像进行前处理，则设为（0,0,0,1）
   2--cdk:解析blob之后的原始数据结果为void*数据类型，该数据的后续处理过程，可通过注册回调函数dp_register_box_device_cb(dp_first_blob_outresult_back_cb_t cb, void *param)进行相应处理
   与设备通信的相关命令在dp_type.h文件中
   ps:this version of cdk only run 8 shaves with blob,the other 4 shaves are used to image preprocessing. 
