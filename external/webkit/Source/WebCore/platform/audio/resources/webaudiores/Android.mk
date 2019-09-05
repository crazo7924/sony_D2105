#<40175-sueye-20140625, [5865_KK][All][]Remove webaudiores.apk>
ifdef Remove_webaudiores
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PACKAGE_NAME := webaudiores

LOCAL_MODULE_TAGS := debug

include $(BUILD_PACKAGE)
endif
#<40175-sueye-20140625>
