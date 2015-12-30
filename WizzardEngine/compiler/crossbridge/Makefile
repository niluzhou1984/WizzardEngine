#!/usr/bin/make
$?UNAME=$(shell uname -s)
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?unixpath=$(shell cygpath -at unix $(1))
else
	$?nativepath=$(abspath $(1))
	$?unixpath=$(abspath $(1))
endif

ProjectName = WizzardEngine

FLASCC = /cygdrive/d/Unreal/crossbridge_15.0.0.3/sdk
PATH := $(call unixpath,$(FLASCC)/usr/bin):$(PATH)

DEPSDIR = .deps
#default defines
OBJS_DIR  = build
RMDIR     = rm -fr
ECHO      = echo
CCLD      =  g++
CXX       =  g++
CC        =  gcc
RANLIB    = ranlib
AR		 = ar
STRIP     = strip
OBJDUMP   = objdump
OBJCOPY   = objcopy

WE_cppfiles += ./../../Private/Source/WeVector.cpp
WE_cppfiles += ./../../Private/Source/WeBspReader.cpp
WE_cppfiles += ./../../Private/Source/WeDefaultSimulationEventCallback.cpp
WE_cppfiles += ./../../Private/Source/WeInternCommon.cpp
WE_cppfiles += ./../../Private/Source/WeMeshLoader.cpp
WE_cppfiles += ./../../Private/Source/WeQueryFilterCallback.cpp
WE_cppfiles += ./../../Private/Source/WeScene.cpp
WE_cppfiles += ./../../Private/Source/WeSelfFilterCallback.cpp
WE_cppfiles += ./../../Public/Source/We.cpp

WE_hpaths :=
WE_hpaths += ./../../Private/Include
WE_hpaths += ./../../Public/Include
WE_hpaths += ./../../../PhysXSDK/Include
WE_hpaths += ./../../../PhysXSDK/Source/foundation/include
WE_hpaths += ./../../../PhysXSDK/Source/Common/src
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/headers
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/include
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/contact
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/common
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/convex
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/distance
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/gjk
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/intersection
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/mesh
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/Ice
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/hf
WE_hpaths += ./../../../PhysXSDK/Source/GeomUtils/src/pcm
WE_hpaths += ./../../../PhysXSDK/Include/foundation
WE_hpaths += ./../../../PhysXSDK/Include/common
WE_hpaths += ./../../../PhysXSDK/Source/PhysXVehicle/src

WE_release_defines := 
WE_release_defines += __CYGWIN__ __CROSSBRIDGE__ __WEDEBUG__
WE_release_defines += PHYSX_PROFILE_SDK
WE_release_defines += NDEBUG 
WE_release_common_cflags := $(Samples_custom_cflags)
WE_release_common_cflags += -MMD
WE_release_common_cflags += $(addprefix -D, $(WE_release_defines))
WE_release_common_cflags += $(addprefix -I, $(WE_hpaths))
WE_release_common_cflags += -Werror  -fPIC  -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -fexceptions
WE_release_common_cflags += -pthread -emit-llvm -fno-strict-aliasing -O1
WE_release_cflags	:= $(WE_release_common_cflags)
WE_release_cppflags	:= $(WE_release_common_cflags)
WE_release_objsdir  = $(OBJS_DIR)/release
WE_release_cpp_o  = $(addprefix $(WE_release_objsdir)/, $(subst ./, , $(subst ../, , $(patsubst %.cpp, %.cpp.o, $(WE_cppfiles)))))
WE_release_cc_o = $(addprefix $(WE_release_objsdir)/, $(subst ./, , $(subst ../, , $(patsubst %.cc, %.cc.o, $(WE_ccfiles)))))
WE_release_c_o = $(addprefix $(WE_release_objsdir)/, $(subst ./, , $(subst ../, , $(patsubst %.c, %.c.o, $(WE_cfiles)))))
WE_release_obj = $(WE_release_cpp_o) $(WE_release_cc_o) $(WE_release_c_o)
WE_LIB_DIR = ./../../Lib/crossbridge
WE_release_bin := ./../../Lib/crossbridge/libWizzardEngine_x86.a

all: check build_release build_swc

build_release: prebuild_release $(WE_release_bin)

prebuild_release:
$(WE_release_bin): $(WE_release_obj) 
	@mkdir -p `dirname ./../../Lib/crossbridge/libWizzardEngine_x86.a`
	@$(AR) rcs $(WE_release_bin) $(WE_release_obj)
	@$(ECHO) building $@ complete!

$(WE_release_cpp_o): $(WE_release_objsdir)/%.o:
	@$(ECHO) WizzardEngine: compiling release $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(WE_release_objsdir),, $@))), $(WE_cppfiles))...
	@mkdir -p $(dir $(@))
	$(CXX) $(WE_release_cppflags) -c $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(WE_release_objsdir),, $@))), $(WE_cppfiles)) -o $@

$(WE_release_cc_o): $(WE_release_objsdir)/%.o:
	@$(ECHO) WizzardEngine: compiling release $(filter %$(strip $(subst .cc.o,.cc, $(subst $(WE_release_objsdir),, $@))), $(WE_ccfiles))...
	@mkdir -p $(dir $(@))
	$(CXX) $(WE_release_cppflags) -c $(filter %$(strip $(subst .cc.o,.cc, $(subst $(WE_release_objsdir),, $@))), $(WE_ccfiles)) -o $@
	
$(WE_release_c_o): $(WE_release_objsdir)/%.o:
	@$(ECHO) WizzardEngine: compiling release $(filter %$(strip $(subst .c.o,.c, $(subst $(WE_release_objsdir),, $@))), $(WE_cfiles))...
	@mkdir -p $(dir $(@))
	$(CC) $(WE_release_cflags) -c $(filter %$(strip $(subst .c.o,.c, $(subst $(WE_release_objsdir),, $@))), $(WE_cfiles)) -o $@ 

include Makefile_SWC.mk

	
clean: clean_release clean_swc
	
clean_release:
	@$(ECHO) clean WizzardEngine release...
	@$(RMDIR) $(OBJS_DIR)
	@$(RMDIR) $(WE_release_objsdir)
	@$(RMDIR) $(WE_release_bin)
	
# Self check
check:
	@if [ -d $(FLASCC)/usr/bin ] ; then true ; \
	else echo "Couldn't locate crossBridge SDK directory, please set absoblute direcotry of FLASCC SDK in Makefile" ; exit 1 ; \
	fi




