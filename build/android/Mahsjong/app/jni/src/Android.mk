LOCAL_PATH  := $(call my-dir)
CURR_DEPTH  := ..
CUGL_OFFSET := ../../../../../../cugl

########################
#
# Main Application Entry
#
########################
CUGL_PATH := $(LOCAL_PATH)/$(CURR_DEPTH)/$(CUGL_OFFSET)
SDL2_PATH := $(CUGL_PATH)/sdlapp
PROJ_PATH := $(LOCAL_PATH)/${CURR_DEPTH}/../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(CUGL_PATH)/include
LOCAL_C_INCLUDES += $(SDL2_PATH)/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/libdatachannel/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/box2d/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/poly2tri
LOCAL_C_INCLUDES += $(PROJ_PATH)/source


# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(LOCAL_PATH)/source/MJDiscardPile.cpp \
	$(LOCAL_PATH)/source/MJMenuScene.cpp \
	$(LOCAL_PATH)/source/MJAnimationController.cpp \
	$(LOCAL_PATH)/source/MJTutorialScene.cpp \
	$(LOCAL_PATH)/source/MJPlayer.cpp \
	$(LOCAL_PATH)/source/MJPauseScene.cpp \
	$(LOCAL_PATH)/source/MJInfoScene.cpp \
	$(LOCAL_PATH)/source/MJDiscardUIScene.cpp \
	$(LOCAL_PATH)/source/MJClientScene.cpp \
	$(LOCAL_PATH)/source/MJSettingScene.cpp \
	$(LOCAL_PATH)/source/MJDiscardUINode.cpp \
	$(LOCAL_PATH)/source/MJPileUINode.cpp \
	$(LOCAL_PATH)/source/MJScoreManager.cpp \
	$(LOCAL_PATH)/source/MJGameOverScene.cpp \
	$(LOCAL_PATH)/source/MJAudioController.cpp \
	$(LOCAL_PATH)/source/MJInputController.cpp \
	$(LOCAL_PATH)/source/MJLoadingScene.cpp \
	$(LOCAL_PATH)/source/MJHostScene.cpp \
	$(LOCAL_PATH)/source/MJPile.cpp \
	$(LOCAL_PATH)/source/MJAnimatedNode.cpp \
	$(LOCAL_PATH)/source/MJMatchController.cpp \
	$(LOCAL_PATH)/source/MJGameScene.cpp \
	$(LOCAL_PATH)/source/MJTileSet.cpp \
	$(LOCAL_PATH)/source/MJNetworkController.cpp \
	$(LOCAL_PATH)/source/main.cpp \
	$(LOCAL_PATH)/source/MJApp.cpp)

# Link in SDL2
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_image
LOCAL_SHARED_LIBRARIES += SDL2_ttf
LOCAL_SHARED_LIBRARIES += SDL2_atk
LOCAL_SHARED_LIBRARIES += SDL2_app

LOCAL_STATIC_LIBRARIES := CUGL

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid
include $(BUILD_SHARED_LIBRARY)
