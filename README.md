请仔细阅读本文本的内容:谢谢！   
安装及使用方法请阅读install.md   
更新固件方法请阅读firmware/update_firmware.docx  
接口说明及使用方法请阅读doc/API_and_GUIDE.md  
固件路径：当前目录，firmware文件夹，固件名deepano_base.mvcmd
-------------------------------
2019.11.01 更新：
1、更新常用接口，去除冗余接口，因此可详细参考测试样例中的几个常用接口模型使用接口方法
2、客户调试过程中，不需要再重新插拔usb
3、新更接口为通用接口，目前只支持一个模型的推理，如有需要多个模型，可根据实际情况，联调，更新固件
4、更新的新版固件为在firmware文件夹中


-------------------------------
2018-07-05 新添接口:  
1、使用接口  
          int dp_send_second_image(unsigned short* imagebuffer,int buffersize,int box_num);
        
-------------------------------

2018-06-25 新添接口  
  自定义ROI:dp_set_blob_image_size(dp_image_box_t *box);
-------------------------------        
2018-06-15  
  客户调试过程中，不需要再重新插拔usb
------------------------------        
2018-06-05  
  本套sdk目录架构设计：  
    1）cmake-debug-build文件夹：适用于cmake编译  
    2）doc：本套相关文件说明  
    3）firmware：固件文件夹  
    4）include：.h文件  
    5）lib：vs库文件  
    6）linux_build：makefile  
    7）opencv2：vs2015编译所需opencv源文件  
    8）so：linux库文件  
    9）src：test测试样例源文件  
		10）win_include:被vs2015编译所需.h文件
