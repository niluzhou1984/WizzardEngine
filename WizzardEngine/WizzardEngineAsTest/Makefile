#!/usr/bin/make
FLASCC=D:/Unreal/CrossBridge_15.0.0.3/sdk

IPATH_SWC :=
IPATH_SWC += ./../Public/Include

LPATH_SWC :=
LPATH_SWC += ./../../PhysXSDK/Lib/crossbridge
LPATH_SWC += ./../Lib/crossbridge


LIBRARIES_SWC :=
LIBRARIES_SWC += WizzardEngine_x86
LIBRARIES_SWC += LowLevel
LIBRARIES_SWC += LowLevelCloth
LIBRARIES_SWC += PhysX3_x86
LIBRARIES_SWC += PhysX3CharacterKinematic_x86
LIBRARIES_SWC += PhysX3Common_x86
LIBRARIES_SWC += PhysX3Cooking_x86
LIBRARIES_SWC += PhysX3Extensions
##LIBRARIES_SWC += PhysX3Vehicle
LIBRARIES_SWC += PhysXProfileSDK
##LIBRARIES_SWC += PhysXVisualDebuggerSDK
##LIBRARIES_SWC += PvdRuntime
LIBRARIES_SWC += PxTask
LIBRARIES_SWC += SceneQuery
LIBRARIES_SWC += SimulationController

DEFINES_SWC += __CYGWIN__ __CROSSBRIDGE__
CFLAGS_SWC :=
CFLAGS_SWC += $(addprefix -D, $(DEFINES_SWC))
CFLAGS_SWC += -pthread -Werror -Wno-write-strings -Wno-trigraphs -fexceptions
CFLAGS_SWC += $(addprefix -I, $(IPATH_SWC))
LFLAGS_SWC :=
LFLAGS_SWC += $(addprefix -L, $(LPATH_SWC))
LFLAGS_SWC += -Wl,--start-group $(addprefix -l, $(LIBRARIES_SWC)) -Wl,--end-group


all:
	"$(FLASCC)/usr/bin/g++" $(CFLAGS_SWC)  test.cpp  $(LFLAGS_SWC)  -o test.exe