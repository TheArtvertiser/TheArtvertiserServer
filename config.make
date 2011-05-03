# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../../..


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

USER_CFLAGS = -Isrc -Isrc/ofxHttpServer/src -Isrc/ofxHttpServer/libs/microhttpd/include 
USER_CFLAGS += -I../artvertiserCommon/gui -I../artvertiserCommon -I../artvertiserCommon/ferns_demo-1.1 -I../artvertiserCommon/FAST -I../artvertiserCommon/persistance -I../artvertiserCommon/cv -I../artvertiserCommon/ofxGeoLocation -I../artvertiserCommon/ofxMD5 -I../artvertiserCommon/ofxMD5/libs/md5-1.3.0

# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LD_FLAGS = libs/libawesomelib.a

USER_LDFLAGS = src/ofxHttpServer/libs/microhttpd/lib/linux64/libmicrohttpd.a


# use this to add system libraries for example:
# USER_LIBS = -lpango

USER_LIBS = -lgcrypt


# change this to add different compiler optimizations to your project

USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os

ANDROID_COMPILER_OPTIMIZATION = -Os


# you shouldn't need to change this for usual OF apps, it allows to include code from other directories
# useful if you need to share a folder with code between 2 apps. The makefile will search recursively
# you can only set 1 path here

USER_SOURCE_DIR = ../artvertiserCommon
