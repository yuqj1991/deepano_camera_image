本套SDK给的例呈编译方法：

本套模组（ma2450）用于解析不带摄像头的类似图片buffer,可用于解析caffemodel和tf框架的大部分算法模型，使用之前，需要使用ncsdk的mvNCComplimer转换工具，将caffemodel转化为mv模组可用的格式数据.


一、ubuntu_16.04实体机:环境：opencv3.4.1 usb-1.0(官网下载，libusb-1.0.22)  
编译方法：    
方法1)    
<p style="text-indent:2em"> cd linux_build </p>
<p style="text-indent:2em"> make  </p>   
方法2  
<p style="text-indent:2em"> mkdir cmake</p>
<p style="text-indent:2em"> cd cmake</p>
<p style="text-indent:2em"> cmake ..</p>
<p style="text-indent:2em"> make</p>

使用方法  
<p style="text-indent:2em">sudo ./deepano_image(可执行文件) [cmd]</p>  
在test.cpp中有如下样例      
testcase_t g_testcases[] ={  
<p style="text-indent:2em">{"ping", NULL, test_ping},</p>  
<p style="text-indent:2em">{ "version", NULL, test_version }</p>,
};
则举例说明命令：sudo ./deepano_image(可执行文件) test_ping （测试test_ping）或者例如测试inception model：sudo ./deepano_image mulitGraph (多模型测试sample)


三、 PS:关于固件烧录软件，请下载百度云网盘：
  链接：https://pan.baidu.com/s/17omr5qlQ7WeCveAf5hE4ng  密码：2l58. 烧录方法：将模组与win电脑相连，点击该软件，出现画面，然后点击开始按钮，然后在固件更新栏里填写固件文件名，注意：将该固件移至该软件文件夹内


