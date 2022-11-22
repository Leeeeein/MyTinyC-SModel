SOURCE_C_FILE += $(shell find . -name "*.c")

TARGET_PATH += .
COMPILE_OPTION := -fPIC -shared -Wno-deprecated -Wno-parentheses
MACRO := -D_REENTRAN

DEFAULT_LINK := -pthread -lm -ldl
ifeq ($(shell uname), Linux)
DEFAULT_LINK += -lrt
endif

COMPILER := gcc

DEBUG_TARGET := $(TARGET_PATH)/libComponentsDebug.so
RELEASE_TARGET := $(TARGET_PATH)/libComponents.so

all:

debug:
	$(COMPILER) $(MACRO) -D_DEBUG -g $(COMPILE_OPTION) $(SOURCE_C_FILE) -o $(DEBUG_TARGET) $(DEFAULT_LINK)
	
release:
	$(COMPILER) $(MACRO) -DNDEBUG -O1 $(COMPILE_OPTION) $(SOURCE_C_FILE) -o $(RELEASE_TARGET) $(DEFAULT_LINK)    
