本套SDK给的例呈编译方法：

本套模组（ma2450）集有摄像头ov5658,可用于解析caffemodel的大部分算法模型，客户开发使用自己的模型之前，需要使用ncsdk的转换工具mvNCCompiler，将caffemodel转化为mv模组可用的graph格式数据.
测试model：链接: https://pan.baidu.com/s/1xf7sqFbwDdqisszKilpb0w 密码: 8ajp


一、ubuntu_16.04实体机:环境：opencv3.4.1 usb-1.0(官网下载，libusb-1.0.22)
编译方法：
方法1)
        cd linux_build;
        make
方法2）
        cd cmake
        cmake ..
        make
使用方法：
        cd Deepano_SDK_AND_EXAMPLES
        sudo ./Deepano_cdk_TEST(可执行文件) [cmd]
        注意：测试样例中的模型文件，需要放置在当前目录下的上一级，或者在model_sample.c文件中修改模型文件路径，如：
        const char *filename = "../google.blob";
        在test.cpp中有如下样例      
        testcase_t g_testcases[] =
        {
	{"ping", NULL, test_ping},
	{ "version", NULL, test_version },
	{ "hardware_test", NULL, test_hardware_test },	
	{ "get_frame", "get_frame", test_get_frame },
#ifdef SUPPORT_OPENCV
	{ "start_video", "start_video", test_start_video },
#endif
	{ "start_camera", "start_camera", test_start_camera },
#ifdef SUPPORT_OPENCV
	{ "get_frame", "get_frame", test_get_frame },
#endif
	{"stop_camera", "stop_camera", test_stop_camera },
        {"test_SqueezeNet","test_SqueezeNet",test_SqueezeNet},
        {"test_SSD_MobileNet","test_SSD_MobileNet",test_SSD_MobileNet},
        {"test_Resnet_18","test_Resnet_18",test_Resnet_18},
        {"test_gendernet","test_gendernet",test_gendernet},
        {"test_AgeNet","test_AgeNet",test_AgeNet},
        {"test_TinyYoloNet","test_TinyYoloNet",test_TinyYoloNet},
        {"test_googleNet","test_googleNet",test_googleNet},
        {"test_mnist","test_mnist",test_mnist},
        {"test_inception","test_inception",test_inception},
        {"test_mobilenets","test_mobilenets",test_mobilenets},
        {"test_ssd_googlenet_model","test_ssd_googlenet_model",test_ssd_googlenet_model},
        {"test_tiny_yolo_v2","test_tiny_yolo_v2",test_tiny_yolo_v2},
        {"test_ssd_resnet_model","test_ssd_resnet_model",test_ssd_resnet_model}
        };

        举例说明命令：sudo ./Deepano_cdk_TEST(可执行文件) test_tiny_yolo_v2 （测试tiny—yolo—v2）或者例如打开摄像头：sudo  ./Deepano_cdk_TEST start_video

二、win10环境：安装opencv3.4.1
        打开Deepano_VS2015.sln
        重新编译
        使用方法同上
        注意：vs2015工程属性我在设置目录的时候，设置成了绝对路径，你需要做相应修改，由于vs2015生成的exe文件路径的不同，因此可能你在测试的时候，需要修改model_sample.c中文件的路径。
        windows版本，仅提供类似编译方案，客户完全没必要在我的基础上做修改。所以日后不会再维护，后续如有新的接口我会做相应说明。或者客户也可以自己新建VS2013或着2015解决方案，可以参考API_and_GUIDE.md和src     文件夹内video_result_callback.c、model_sample.c和test.cpp。
三、通过模型测试：
   ncappzoo里面大部分模型都可运行，只有inception_v2模型，目前有些问题，注意：
        1).本测试样例所有模型的解析算法均为本人借鉴ncappzoo里面的解析算法，可能会有些不准确，如果您有非常好的准确的解析算法，请与我联系:yuqj@deepano.com,非常感谢
        2).本套ma2450模组芯片，host端一旦终止，如果想要再次运行host端程序，需要重新插拔usb
        3).本套ma2450模组芯片，device端图像前处理方法均为resize->yuv->rgb->fp16->bgr;因此本套所有模型的结果可能不会非常准确，如果客户有不同的图像前处理方式，需要单独定制
        4).本套ma2450模组芯片, Blob_size_1<=55M,Blob_size_2<=25M



