# 编译运行过程如下：
## A.安装crossbridge环境
1. 下载[crossbridge](http://sourceforge.net/projects/crossbridge-community/files/latest/download)
2. 下载[AIRSDK_Compiler](http://www.adobe.com/devnet/air/air-sdk-download.html)
3. 解压crossbridge
4. 解压AIRSDK_Compiler到your/path/to/crossbridge/目录下，重命名为flex_sdk
5. 下载32位cygwin的[setup-x86.exe](https://cygwin.com/install.html) , 并替换your/path/to/crossbridge/目录下的setup-x86.exe
6. 更换cygwin的源(如果这个默认源下载速度不慢，可以跳过这一步))
    * 编辑your/path/to/crossbridge/目录下的run.bat脚本:
        > 脚本中所有的源地址**http://cygwin.mirror.constant.com** 换成较快的源，国内如**http://mirrors.163.com/cygwin/**
7. 双击运行run.bat,按提示安装
8. 修改**your/path/to/crossbridge/sdk/usr/include/malloc.h**头文件以支持Physx编译：
    * 源文件：  
    ``` C++
            /* $FreeBSD: src/include/malloc.h,v 1.5.36.1.6.1 2010/12/21 17:09:25 kensmith Exp $ */  
            #if __STDC__
                #error "<malloc.h> has been replaced by <stdlib.h>"
	        #else
	            #include <stdlib.h>
	        #endif
    ```
    * 改为：
    ```C++
            /* $FreeBSD: src/include/malloc.h,v 1.5.36.1.6.1 2010/12/21 17:09:25 kensmith Exp $ */
			#include <stdlib.h>
	```
    
## B.编译crossbridge版本的Phsyx
1. 编辑your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge目录下的Makefile文件:  
        `line 15： FLASCC = D:/Unreal/CrossBridge_15.0.0.3/sdk`  
         改为: ` FLASCC = your/path/to/crossbridge/sdk`
2. 打开cygwin，cd到your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge,执行make

## C.编译crossbridge版本的WizzardEngine工程
1. 编辑your/path/to/WizzardEngine/WizzardEngine/compiler/crossbridge目录下的Makefile文件：  
        `line 13:FLASCC = /cygdrive/d/Unreal/crossbridge_15.0.0.3/sdk`   
        改为：`FLASCC = your/path/to/crossbridge/sdk`
2. 打开cygwin，cd到your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge, 执行make

## D.编译并运行WizzardEngineAsTest测试例程
1. 编辑your/path/to/WizzardEngine/WizzardEngine/WizzardEngineAsTest/目录下的Makefile文件：  
        `line 2: FLASCC=D:/Unreal/CrossBridge_15.0.0.3/sdk`  
        改为:`FLASCC = your/path/to/crossbridge/sdk`
2. 打开cygwin，cd 到your/path/to/WizzardEngine/WizzardEngine/WizzardEngineAsTest/目录下， 执行make
3. 运行: 在cygwin下输入 **./test.exe**