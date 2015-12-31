#编译运行过程如下：
##A.安装crossbridge环境
	1.下载crossbridge：[http://sourceforge.net/projects/crossbridge-community/files/15.0.0/](http://sourceforge.net/projects/crossbridge-community/files/15.0.0/)
	2.下载AIRSDK_Compiler http://www.adobe.com/devnet/air/air-sdk-download.html
	3.��ѹcrossbridge
	4.��ѹAIRSDK_Compiler��your/path/to/crossbridge/Ŀ¼�£�������Ϊflex_sdk
	5.����32λcygwin��setup-x86.ex: https://cygwin.com/install.html,  ���滻your/path/to/crossbridge/Ŀ¼�µ�setup-x86.exe
	6.����cygwin��Դ:(��������Ĭ��Դ�����ٶȲ���������������һ��)
		�༭your/path/to/crossbridge/Ŀ¼�µ�run.bat�ű�
		line 87:
			setup-x86 --arch x86 --quiet-mode --no-admin --no-startmenu --no-desktop --no-shortcuts --root %LAUNCH_DIR%\cygwin --site http://cygwin.mirror.constant.com && setup-x86 --arch x86 --quiet-mode --no-admin --no-startmenu --no-desktop --no-shortcuts --root %CD%\cygwin --site http://cygwin.mirror.constant.com --packages libuuid1,libuuid-devel && run
		��Ϊ��
			setup-x86 --arch x86 --quiet-mode --no-admin --no-startmenu --no-desktop --no-shortcuts --root %LAUNCH_DIR%\cygwin --site http://mirrors.163.com/cygwin/ && setup-x86 --arch x86 --quiet-mode --no-admin --no-startmenu --no-desktop --no-shortcuts --root %CD%\cygwin --site http://mirrors.163.com/cygwin/ --packages libuuid1,libuuid-devel && run
	7.˫������run.bat,����ʾ��װ
	8.�޸�crossbrige malloc.hͷ�ļ���֧��Physx���룺
		�༭your/path/to/crossbridge/sdk/usr/include/malloc.h:
		Դ�ļ���
			#############Դ�ļ�#######################
			/* $FreeBSD: src/include/malloc.h,v 1.5.36.1.6.1 2010/12/21 17:09:25 kensmith Exp $ */
			#if __STDC__
			#error "<malloc.h> has been replaced by <stdlib.h>"
			#else
			#include <stdlib.h>
			#endif
			############################################
		��Ϊ��
			################�޸ĺ��ļ�###################
			/* $FreeBSD: src/include/malloc.h,v 1.5.36.1.6.1 2010/12/21 17:09:25 kensmith Exp $ */
			#include <stdlib.h>
			#############################################
			

B.����crossbridge�汾��Phsyx
	1.�༭your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridgeĿ¼�µ�Makefile�ļ���
		line 15��
			FLASCC = D:/Unreal/CrossBridge_15.0.0.3/sdk
		��Ϊ: 
			FLASCC = your/path/to/crossbridge/sdk
	2.����cygwin��cd��your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge,ִ��make
C.����crossbridge�汾��WizzardEngine����
	1.�༭your/path/to/WizzardEngine/WizzardEngine/compiler/crossbridgeĿ¼�µ�Makefile�ļ���
		line 13:
			FLASCC = /cygdrive/d/Unreal/crossbridge_15.0.0.3/sdk
		��Ϊ��
			FLASCC = your/path/to/crossbridge/sdk
	2.����cygwin��cd��your/path/to/WizzardEngine/PhysXSDK/Source/compiler/crossbridge, ִ��make

D.���벢����WizzardEngineAsTest��������
	1.�༭your/path/to/WizzardEngine/WizzardEngine/WizzardEngineAsTest/Ŀ¼�µ�Makefile�ļ���
		line 2:
			FLASCC=D:/Unreal/CrossBridge_15.0.0.3/sdk
		��Ϊ:
			FLASCC = your/path/to/crossbridge/sdk
	2. ����cygwin��cd ��your/path/to/WizzardEngine/WizzardEngine/WizzardEngineAsTest/Ŀ¼�£� ִ��make
	3.���� ��cygwin������./test.exe
