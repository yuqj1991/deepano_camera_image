由于md文本格式问题，请clone下来，阅读本文件的内容,谢谢
安装编译和更新固件请阅读install.md

2019.2.16:更新：更新稳定版固件，此版本针对不带摄像头模组，可以执行多个模型
注意：目前板子端由于内存容量的限制，最多所有graph大小总和120M，单个graph大小50M，如果你们的模型有特殊大小，请yuqj@deepano.com
模型大小input层，系统设定不超过，448×448×3,output层数据维度设定为3×1500, 如有特殊大小，请@ yuqj@deepano.com
固件名称：deepano_mdk.mvcmd

本套sdk arch：
data/ 存放图像image
doc/ 相关文档
firmware/ 板子固件更新方法，请打开install.md
include/ 头文件
lib/ win64版本库文件
so/ ubuntu版本库文件
src/ 源文件

使用测试用例步骤：（源代码在src/model_sample.cpp）
相关结构体：
typedef struct dp_blob_parm_t{
	int InputSize_width; //inputlayer 宽
	int InputSize_height; //inputlayer 高
	int Output_num; //outputlayer  数据长度
}dp_blob_parm_t;
(1)、初始化device：
	int ret = dp_init();
	if (ret != 0) {
		fprintf(stderr, "dp_init failed!");
        return ret;
	}
(2)、向device端，传递graph相关信息：  int dp_set_blob_parms(int num_model,dp_blob_parm_t *param)
	dp_blob_parm_t parms[NetNum]={{300,300,707*2}, {160,160,512*2}, {227,227,8*2}, {227, 227, 2*2}};
	test_update_model_parems(NetNum,parms);
(3)、向device端，传递graph文件：int dp_update_model(const char* filename);
	for (int ii =0; ii< NetNum; ii++){
		ret = dp_update_model(modelGpahe_filename[ii]);
		if (ret == 0) {
			printf("Test dp_update_model(%s) sucessfully!\n", modelGpahe_filename[ii]);
		}
		else {
			printf("Test dp_update_model(%s) failed ! ret=%d\n", modelGpahe_filename[ii], ret);
		}
	}
(4)、向device端传递imagebuffer的同时，指定由哪个模型（模型索引号），进行推理，并返回结果： float* movidius_inference_result(unsigned short *imageBuf, int BufSize, unsigned int graph_stream_id, unsigned int graphresultLength)
	unsigned short* imgBuf = srcImg.loadimage(imgFile.c_str(), 0, box, 300, ssdMeanValue, ssdStdValue);
	int BufSize = sizeof(unsigned short)*parms[0].InputSize_width*parms[0].InputSize_height;
	unsigned int graphLength = parms[0].Output_num/2; //graphSize
	gettimeofday(&start, NULL);
	float * graphResult = movidius_inference_result(imgBuf, BufSize, 0, graphLength);

