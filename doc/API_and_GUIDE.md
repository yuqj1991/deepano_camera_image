[1]本SDK相关API简介：
   1、 int dp_init();//初始化打开设备
   2、int dp_uninit();//释放设备
   3、int dp_ping();//测试与主机与设备是否联通
   4、dp_start_camera();//打开摄像头
   5、int dp_start_camera_video();//开启视频流
   
   6、typedef void(*dp_video_frame_cb_t)(dp_img_t *img, void *param);//接收从设备返回的视频流回调接口，本测试样例中回调函数为video_result_callback.c中的video_callback(),其中注册函数接口为
   int dp_register_video_frame_cb(dp_video_frame_cb_t cb, void *param);
   
   7、typedef void(*dp_first_blob_outresult_back_cb_t)(float * result,void *param);//接收解析第一blob之后的原始结果的回调函数接口，本测试样例中回调函数为video_result_callback.c中为cdk_result_model(),其中注册函数接口为：int dp_register_box_device_cb(dp_first_blob_outresult_back_cb_t cb, void *param);
   
   8、typedef void(*dp_second_blob_outresult_back_cb_t)(void * result,void *param)；//如果解析两个blob，则调用此回调函数，向设备端传输box坐标值，本测试样例中回调函数为video_two_result_callback.c中为cdk_two_result_model(),其中注册函数接口为：int dp_register_second_box_device_cb(dp_second_blob_outresult_back_cb_t cb, void *param);
   9、int dp_set_blob_parms(int num_model,dp_blob_parm_t *param);//向设备端发送blob相关参数，注意，num_blob，表示向设备需要传输的个数
   10、int dp_update_model(const char* filename);//传输第一个blob
   11、int dp_update_model_2(const char* filename);//传输第二个blob
   
   12、typedef void(*dp_parse_blob_stage_time_cb_t)(double * result,void *param);//解析模型所需要的时间注册回调函数，本测试样例中回调函数为video_result_callback.c中void blob_parse_callback(double *buffer_fps,void *param)函数，注册函数为：dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);
   
   13、dp_set_blob_image_size(dp_image_box_t *box);//自定义ROI，可以对模型解析的carmera图象帧进行自定义ROI,可以保证模型输入资料宽高比和送入检测的宽高比一致;
   
   14、int dp_send_first_box_image(int box_num,dp_image_box_t *box);对于双模型，第一模型将结果返回主机，解析产生box，利用本接口将box坐标值回传板子，用于第二模型解析
   
   15、int dp_send_second_image(unsigned short* imagebuffer,int buffersize,int box_num);//对于双模型，第一模型将结果返回主机，解析产生box，利用本接口将box坐标值对应的imagebuffer进行相应的处理，并回传给板子

[2]相关数据结构体：
        dp_blob_parm_t
        {
                int IsTensor_model;  //神经网络是tensor网络框架还是caffe网络框架
                int InputSize_width; //神经网络入口的图片尺寸
                int InputSize_height;//神经网络入口的图片尺寸
                int Output_num;//神经网络解析之后原始的数据长度，等于神经棒上mvnvGetResult()接口获取的原始数据长度; 
        }
        dp_image_box_t{
                int x1;//box宽起始位
                int x2;//box宽终止位
                int y1;//box高起始位
                int y2;//box高终止位
        }
        typedef struct _dp_netMean
        {
                float R_mean;//图像前处理，r通道均值
                float G_mean;//图像前处理，g通道均值
                float B_mean;//图像前处理，b通道均值
                float Std;//图像前处理，std标准差
        }dp_netMean;如果模型没有对图像进行前处理，则设为（0,0,0,1）
        typedef struct _box
        {
                int xmin;
                int ymin;
                int xmax;
                int ymax;
                char category[20];
        }Box;
        typedef enum NET_CAFFE_TENSFLOW
        {
                DP_AGE_NET=0,
                DP_ALEX_NET,
                DP_GOOGLE_NET,
                DP_GENDER_NET,
                DP_TINI_YOLO_NET,
                DP_SSD_MOBILI_NET,
                DP_RES_NET,
                DP_SQUEEZE_NET,
                DP_MNIST_NET,
                DP_INCEPTION_V1,
                DP_MOBILINERS_NET,
                DP_TINY_YOLO_V2_NET,
        } DP_MODEL_NET;//相关测试样例枚举常量


模组解析模型的一般步骤：
        开机（识别usb设备，需登上10s左右）->向设备端传输一个或两个blob的相关初始化参数->传输相应个数的blob->传输blob的mean和std(如果没有，则必须设mean为0,std为1,否则系统报错)->调用dp_start_camera()接口，设备端开始解析图像。
   
[3]测试样例调用接口——单模型调用接口逻辑，以ssd_mobilenet为例：

        void test_SSD_MobileNet(int argc, char *argv[])
        {
                int ret;
                const char *filename = "../SSD_MobileNet.blob";         //模型文件路径

                int blob_nums = 1; dp_blob_parm_t parms = {0,300,300,707*2};    //传入的模型个数及相应的模型参数设置，如此例中，{0（是否为tf框架模型）,300（输入模型图片宽）,300（输入模型图片高）,707*2（解析模型的输出结果长度）}
                dp_netMean mean={127.5,127.5,127.5,127.5};      //传入的模型图片三通道均值和标准差，如果没有，则设为{0,0,0,1}
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
                dp_set_blob_image_size(&BLOB_IMAGE_SIZE);       //设定ROI
                test_update_model_parems(blob_nums, &parms);    //传入板子的模型参数
                dp_set_blob_mean_std(blob_nums,&mean);  //向板子传输图像的均值和std
                ret = dp_update_model(filename);        //传入模型文件
                if (ret == 0) {
                        printf("Test dp_update_model(%s) sucessfully!\n", filename);
                }
                else {
                        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
                }

                DP_MODEL_NET net=DP_SSD_MOBILI_NET;     //模型枚举常量
                dp_register_video_frame_cb(video_callback, &net);       //注册视频流函数
                dp_register_box_device_cb(cdk_result_model,&net);       //模型解析之后，结果显示注册函数video_result_callback.c中的cdk_result_model()回调函数
                dp_register_fps_device_cb(fps_callback,&net);   //视频帧回调注册函数，video_result_callback.c中的fps_callback（）回调函数
                dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);        //解析模型所需要的时间注册回调函数，video_result_callback.c中的blob_parse_callback（）回调函数
                ret = dp_start_camera_video();  //开启摄像头，进行板子工作
                if (ret == 0) {
                        printf("Test test_start_video successfully!\n");
                }
                else {
                        printf("Test test_start_video failed! ret=%d\n", ret);
                }
                const char *win_name = "video";
                cv::namedWindow(win_name);
                int key = -1;   //显示结果
                for (;;) {
                        video_mutex.lock();
                        if (!bgr.empty())
                        cv::imshow(win_name, bgr);
                        video_mutex.unlock();
                        key = cv::waitKey(30);
                }
                cv::destroyWindow(win_name);
        }
    
    
[3]测试样例调用接口——双模型调用接口逻辑（以ssd_mobilenet和resnet为例）：
        void test_ssd_resnet_model(int argc, char *argv[])
        {
                int ret;
                const char *filename = "../SSD_MobileNet.blob";
                const char *filename2 = "../Resnet.blob";       //双模型文件

                int blob_nums = 2; dp_blob_parm_t parms[2] = {{0,300,300,707*2 },{0,224,224,1000*2}};  //双模型文件参数
                dp_netMean mean[2]={{127.5,127.5,127.5,127.5},{104.00698793,116.66876762,122.67891434,1}};//双模型均值std
                dp_set_blob_image_size(&BLOB_IMAGE_SIZE);       //上传自定义roi
                test_update_model_parems(blob_nums, parms);     //上传模型参数
                dp_set_blob_mean_std(blob_nums,mean);           //上传模型均值std
                ret = dp_update_model(filename);                //上传第一个模型
                if (ret == 0) {
                        printf("Test dp_update_model(%s) sucessfully!\n", filename);
                }
                else {
                        printf("Test dp_update_model(%s) failed ! ret=%d\n", filename, ret);
                }
                ret = dp_update_model_2(filename2);             //上传第二个模型
                if (ret == 0) {
                        printf("Test dp_update_model_2(%s) sucessfully!\n", filename2);
                }
                 else {
                        printf("Test dp_update_model_2(%s) failed ! ret=%d\n", filename2, ret);
                }
                FILE *fp=fopen("doc/synset_words.txt","r");     //模型解析结果分类文件
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
                DP_MODEL_NET net_1=DP_SSD_MOBILI_NET;  //模型枚举常量
                dp_register_box_device_cb(/*box_callback_model_two_demo*/cdk_two_result_model, &net_1); //第一模型注册函数
                DP_MODEL_NET net_2=DP_RES_NET;
                dp_register_second_box_device_cb(cdk_result_model,&net_2);      //第二模型注册函数
                dp_register_video_frame_cb(video_callback, &net_1);             //视频流注册函数
                dp_register_fps_device_cb(fps_callback,&net_1);                 //fps注册函数
                dp_register_parse_blob_time_device_cb(blob_parse_callback,NULL);        //模型解析时间注册函数
                ret = dp_start_camera_video();  //打开视频流板子开始工作
                if (ret == 0) {
                        printf("Test test_start_video successfully!\n");
                }
                else {
                        printf("Test test_start_video failed! ret=%d\n", ret);
                }
                const char *win_name = "video";
                cv::namedWindow(win_name);
                int key = -1;
                for (;;) {
                        video_mutex.lock();
                        if (!bgr.empty())
                        cv::imshow(win_name, bgr);
                        video_mutex.unlock();
                        key = cv::waitKey(30);
                }
                cv::destroyWindow(win_name);
}

        

