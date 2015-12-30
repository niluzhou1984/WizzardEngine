#!/usr/bin/make

IPATH_SWC :=
IPATH_SWC += ../../../Public/Include

LPATH_SWC :=
LPATH_SWC += ./../../../../PhysXSDK/Lib/crossbridge
LPATH_SWC += ./../../../Lib/crossbridge


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
CFLAGS_SWC += -pthread -Werror -Wno-write-strings -Wno-trigraphs -fexceptions -fPIC -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden
CFLAGS_SWC += $(addprefix -D, $(DEFINES_SWC))
CFLAGS_SWC += $(addprefix -I, $(IPATH_SWC))
LFLAGS_SWC :=
LFLAGS_SWC += $(addprefix -L, $(LPATH_SWC))
LFLAGS_SWC += -Wl,--start-group $(addprefix -l, $(LIBRARIES_SWC)) -Wl,--end-group

build_swc:
	$(FLASCC)/usr/bin/swig -as3 -package org.WizzardEngine -D__CYGWIN__ -D__CROSSBRIDGE__ -c++ -module WizzardEngine WeAsWrapper.i
	@mv WizzardEngine.as $(OBJS_DIR)
	@mv WeAsWrapper_wrap.cxx $(OBJS_DIR)
	
	@java -jar $(call nativepath, $(FLASCC)/usr/lib/asc2.jar) -merge -md -parallel -abcfuture -AS3  \
			-import $(call nativepath, $(FLASCC)/usr/lib/builtin.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/playerglobal.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/BinaryData.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/ISpecialFile.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/IBackingStore.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/IVFS.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/PlayerKernel.abc) \
			-import $(call nativepath, $(FLASCC)/usr/lib/PlayerCreateWorker.abc)  $(OBJS_DIR)/WizzardEngine.as
	@mv $(OBJS_DIR)/WizzardEngine.as $(OBJS_DIR)/WizzardEngine.as3
	@cd $(OBJS_DIR) && $(FLASCC)/usr/bin/g++ $(CFLAGS_SWC) -O1 -emit-llvm  -L./../../../Lib/crossbridge WeAsWrapper_wrap.cxx -c
	@cp -f exports.txt $(OBJS_DIR)/
	@chmod u+rw $(OBJS_DIR)/exports.txt
	@$(FLASCC)/usr/bin/nm $(OBJS_DIR)/WeAsWrapper_wrap.o | grep " T " | awk '{print $$3}' | sed 's/__/_/' >> $(OBJS_DIR)/exports.txt
	cd $(OBJS_DIR) && $(FLASCC)/usr/bin/g++ $(CFLAGS_SWC) -O1 WeAsWrapper_wrap.o WizzardEngine.abc  $(LFLAGS_SWC) -emit-swc=org.WizzardEngine -flto-api=exports.txt -o WizzardEngine.swc
	@mv $(OBJS_DIR)/WizzardEngine.swc $(WE_LIB_DIR)/
	@echo "Build WizzardEngine.swc Completed!"
	
clean_swc:
	@echo clean WeAsWrapper release...
	@rm -rf $(OBJS_DIR)
	@rm -f $(WE_LIB_DIR)/WizzardEngine.swc
	