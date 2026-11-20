LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := QuickTurnCamera
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_SRC_FILES := main.cpp \
                   mod/logger.cpp \
                   mod/config.cpp \
                   mod/menu/game_menu.cpp \
                   mod/menu/menu.cpp \
                   mod/menu/menu_items.cpp \
                   mod/menu/menu_screens.cpp \
                   mod/menu/menu_scroll.cpp

LOCAL_CFLAGS += -O2 -DNDEBUG -std=c++17
LOCAL_CPPFLAGS += -Wall -Wextra -Wno-unused-parameter -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/plugin-sdk $(LOCAL_PATH)/mod/thirdparty
LOCAL_LDLIBS += -llog
LOCAL_LDFLAGS += -Wl,--exclude-libs,ALL

include $(BUILD_SHARED_LIBRARY)
