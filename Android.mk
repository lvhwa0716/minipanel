# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


#

LOCAL_PATH:= $(call my-dir)

src_files := \
	FontManager.c \
	MicroPanelGui.c \
	MicroPanelService.cpp \
	MicroPanelServiceClient.cpp \
	MicroPanelHttpDebug.cpp

src_files += OledDriver_intfApp.c

local_define := -DMICROPANEL_BPP=8

local_define += -DDEBUG_LOG

local_debug_static_lib := libcurl_i029
local_debug_shared_lib :=  libcrypto libssl libz

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= $(src_files)
LOCAL_SHARED_LIBRARIES := libicuuc liblog
#LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libminipanel
LOCAL_C_INCLUDES += \
    external/harfbuzz_ng/src \
    external/freetype/include

LOCAL_CFLAGS += $(local_define)

LOCAL_SHARED_LIBRARIES += \
    libharfbuzz_ng \
    libft2 \
    liblog \
    libpng \
    libz \
    libicuuc \
    libutils


#debug post framebuffer to server
LOCAL_STATIC_LIBRARIES += $(local_debug_static_lib)
LOCAL_C_INCLUDES += vendor/i029/display/curl/include


include $(BUILD_STATIC_LIBRARY)


#
# Build for the MicroPanelService.
#

include $(CLEAR_VARS)
LOCAL_SRC_FILES += MicroPanelServiceMain.cpp
LOCAL_C_INCLUDES += \
    external/freetype/include

LOCAL_CFLAGS += $(local_define)

LOCAL_SHARED_LIBRARIES := \
    libharfbuzz_ng \
    libft2 \
    liblog \
    libpng \
    libz \
    libicuuc \
    libutils \
    libbinder \
    libcutils \


LOCAL_STATIC_LIBRARIES := libminipanel
#LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := minipanelservice

LOCAL_STATIC_LIBRARIES += $(local_debug_static_lib)
LOCAL_SHARED_LIBRARIES += $(local_debug_shared_lib)

include $(BUILD_EXECUTABLE)

#
# Build for the test.
#

include $(CLEAR_VARS)

LOCAL_MODULE := com.i029.minipanel.IMicroPanelService
#LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, java) $(call all-Iaidl-files-under, java)


#LOCAL_PROGUARD_FLAG_FILES := proguard.flags
#LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res-keyguard $(LOCAL_PATH)/res

include $(BUILD_STATIC_JAVA_LIBRARY)

#
# Build for the test.
#

include $(CLEAR_VARS)
LOCAL_SRC_FILES += FontManagerTest.c
LOCAL_C_INCLUDES += \
    external/freetype/include

LOCAL_CFLAGS += $(local_define)

LOCAL_SHARED_LIBRARIES := \
    libharfbuzz_ng \
    libft2 \
    liblog \
    libpng \
    libz \
    libicuuc \
    libutils
LOCAL_STATIC_LIBRARIES := libminipanel
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := minipaneltest
LOCAL_STATIC_LIBRARIES += $(local_debug_static_lib)
LOCAL_SHARED_LIBRARIES += $(local_debug_shared_lib)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := FrameBufferDump.c

LOCAL_CFLAGS += $(local_define)

LOCAL_SHARED_LIBRARIES := \
    libharfbuzz_ng \
    libft2 \
    liblog \
    libpng \
    libz \
    libicuuc \
    libutils

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := minipaneldump

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := OledDriverTest.c OledDriver_intfApp.c

LOCAL_CFLAGS += $(local_define)

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := minipanelDriverTest

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := MicroPanelServiceClient.cpp ServicesTest.cpp

LOCAL_CFLAGS += $(local_define)

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libbinder \
    libcutils

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := minipanelServiceTest

include $(BUILD_EXECUTABLE)




