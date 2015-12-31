# �������й������£�
## A.��װcrossbridge����
1. ����[crossbridge](http://sourceforge.net/projects/crossbridge-community/files/latest/download)
2. ����[AIRSDK_Compiler](http://www.adobe.com/devnet/air/air-sdk-download.html)
3. ��ѹcrossbridge
4. ��ѹAIRSDK_Compiler��your/path/to/crossbridge/Ŀ¼�£�������Ϊflex_sdk
5. ����32λcygwin��[setup-x86.exe](https://cygwin.com/install.html) , ���滻your/path/to/crossbridge/Ŀ¼�µ�setup-x86.exe
6. ����cygwin��Դ(������Ĭ��Դ�����ٶȲ���������������һ��))
    * �༭your/path/to/crossbridge/Ŀ¼�µ�run.bat�ű�:
        > �ű������е�Դ��ַ**http://cygwin.mirror.constant.com** ���ɽϿ��Դ��������**http://mirrors.163.com/cygwin/**
7. ˫������run.bat,����ʾ��װ
8. �޸�**your/path/to/crossbridge/sdk/usr/include/malloc.h**ͷ�ļ���֧��Physx���룺
    * Դ�ļ���  
    ``` C++
            /* $FreeBSD: src/include/malloc.h,v 1.5.36.1.6.1 2010/12/21 17:09:25 kensmith Exp $ */  
            #if __STDC__
                #error "<malloc.h> has been replaced by <stdlib.h>"
	        #else
	            #include <stdlib.h>
	        #endif
    ```
    * ��Ϊ��
    ```C++
            /* $FreeBSD: src/include/malloc.h,v 1.5.36.1.6.1 2010/12/21 17:09:25 kensmith Exp $ */
			#include <stdlib.h>
	```
    
## B.����crossbridge�汾��Phsyx
1. �༭your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridgeĿ¼�µ�Makefile�ļ�:  
        `line 15�� FLASCC = D:/Unreal/CrossBridge_15.0.0.3/sdk`  
         ��Ϊ: ` FLASCC = your/path/to/crossbridge/sdk`
2. ��cygwin��cd��your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge,ִ��make

## C.����crossbridge�汾��WizzardEngine����
1. �༭your/path/to/WizzardEngine/WizzardEngine/compiler/crossbridgeĿ¼�µ�Makefile�ļ���  
        `line 13:FLASCC = /cygdrive/d/Unreal/crossbridge_15.0.0.3/sdk`   
        ��Ϊ��`FLASCC = your/path/to/crossbridge/sdk`
2. ��cygwin��cd��your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge, ִ��make

## D.���벢����WizzardEngineAsTest��������
1. �༭your/path/to/WizzardEngine/WizzardEngine/WizzardEngineAsTest/Ŀ¼�µ�Makefile�ļ���  
        `line 2: FLASCC=D:/Unreal/CrossBridge_15.0.0.3/sdk`  
        ��Ϊ:`FLASCC = your/path/to/crossbridge/sdk`
2. ��cygwin��cd ��your/path/to/WizzardEngine/WizzardEngine/WizzardEngineAsTest/Ŀ¼�£� ִ��make
3. ����: ��cygwin������ **./test.exe**