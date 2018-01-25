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

LOCAL_PATH:= $(call my-dir)

src_files := FontManager.c

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= $(src_files)
LOCAL_SHARED_LIBRARIES := libicuuc liblog
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libminipanel
LOCAL_C_INCLUDES += \
    external/harfbuzz_ng/src \
    external/freetype/include

LOCAL_SHARED_LIBRARIES := \
    libharfbuzz_ng \
    libft2 \
    liblog \
    libpng \
    libz \
    libicuuc \
    libutils

include $(BUILD_STATIC_LIBRARY)

#
# Build for the test.
#

include $(CLEAR_VARS)
LOCAL_SRC_FILES += FontManagerTest.c
LOCAL_C_INCLUDES += \
    external/freetype/include

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

include $(BUILD_EXECUTABLE)
