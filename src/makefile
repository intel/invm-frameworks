#
# Copyright (c) 2015 2016, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of Intel Corporation nor the names of its contributors
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Makefile for the Intel CLI Framework and example CLI utility
#

# ---- BUILD ENVIRONMENT ---------------------------------------------------------------------------
ROOT_DIR = ..
# sets up standard build variables
include $(ROOT_DIR)/build.mk

OBJECT_MODULE_DIR = $(OBJECT_DIR)

# ---- COMPONENTS ----------------------------------------------------------------------------------
FRAMEWORK = $(BUILD_DIR)/$(LIB_BASENAME).$(LIB_SUFFIX)
FEATURES = $(BUILD_DIR)/libexfeatures.$(LIB_SUFFIX)
INTELI18N = $(BUILD_DIR)/$(I18N_LIB_BASENAME).$(LIB_SUFIX)

# ---- FILES ---------------------------------------------------------------------------------------
SRC = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRC))
OBJNAMES = $(addprefix $(OBJECT_MODULE_DIR)/, $(OBJS))

# pull in any previously generated source dependency declarations (.d files)
# (hyphen preceeding the "include" keyword allows MAKE to continue if a .d file is not found) 
-include $(addprefix $(OBJECT_MODULE_DIR)/, $(patsubst %.cpp,%.d,$(SRC)))

TARGETBASE=examplecli
ifdef BUILD_WINDOWS
	TARGET = $(TARGETBASE).exe
else
	TARGET = $(TARGETBASE)
endif 
TARGET_NAME = $(addprefix $(BUILD_DIR)/, $(TARGET))

# ---- COMPILER PARAMETERS -------------------------------------------------------------------------
INCS = 	-I$(SRC_DIR)
		
LIBS = 	-L$(BUILD_DIR) -l$(LIB_NAME)\
		-L$(BUILD_DIR) -lexfeatures \

ifdef INTEL_I18N
INCS += -I$(EXTERN_DIR)/intel_i18n/include
LIBS += -l$(I18N_LIB_NAME)
else
ifdef BUILD_WINDOWS
LIBS += -llibintl
endif
endif
		
ifdef BUILD_WINDOWS
	LIBS += -lws2_32 -lmswsock -ladvapi32 -lversion -lGdi32 -lShlwapi \
	 		-L$(BUILD_DIR) -lssp -lpthread
else ifdef BUILD_LINUX
	LIBS += -lpthread -ldl
else
	LIBS += -lpthread -ldl -lm
endif

# ---- RECIPES -------------------------------------------------------------------------------------
all : $(BUILD_DIR)  libcopy framework features $(TARGET_NAME)

$(TARGET_NAME): $(FRAMEWORK) $(FEATURES) $(OBJNAMES) 
	$(CPP) $(CPPFLAGS) $(OBJNAMES) $(LIBS) -o $@

$(BUILD_DIR):
	$(MKDIR) $@

libcopy:
	$(COPY) $(I18N_DIR)/$(OS_TYPE)_$(BUILD_TYPE)/lib* $(BUILD_DIR)

framework:
	$(MAKE) -C framework all

features:
	$(MAKE) -C features all

test :
	$(MAKE) -C framework test
	
# suffix rule for .cpp -> .o
$(OBJECT_MODULE_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(INCS) -c $< -o $@

i18n :	
	$(MAKE) -C framework i18n

clean : 
	rm -f $(OBJNAMES)
	$(MAKE) -C framework clean
	$(MAKE) -C features clean

clobber :
	rm -f $(TARGET)
	$(MAKE) -C framework clobber
	$(MAKE) -C features clobber
	
sourcedrop : 
	$(MKDIR) $(SOURCEDROP_DIR)/src
	$(COPY) makefile $(SOURCEDROP_DIR)/src
	$(MAKE) -C framework sourcedrop

.PHONY : all clean clobber framework features i18n sourcedrop
