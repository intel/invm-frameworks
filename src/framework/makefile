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
# Makefile for the Intel CLI Frameowork
#

# ---- BUILD ENVIRONMENT ---------------------------------------------------------------------------
ROOT_DIR = ../..
# sets up standard build variables
include $(ROOT_DIR)/build.mk

OBJECT_MODULE_DIR = $(OBJECT_DIR)/framework

# ---- COMPONENTS ----------------------------------------------------------------------------------
UNITTEST = unittest

# ---- FILES ---------------------------------------------------------------------------------------
SRC = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

EXTERNAL_HEADERS = CliFrameworkTypes.h CommandSpec.h DuplicateTokenErrorResult.h \
	ErrorResult.h FeatureBase.h FeatureRef.h Framework.h HelpFeature.h Logger.h \
	NoInputErrorResult.h NotImplementedErrorResult.h \
	ObjectListResult.h Parser.h PropertyListResult.h ResultBase.h \
	SimpleListResult.h SimpleResult.h SyntaxErrorBadValueResult.h SyntaxErrorMissingValueResult.h \
	SyntaxErrorUnexpectedValueResult.h SyntaxErrorResult.h Trace.h
	 
OBJS = $(patsubst %.cpp,%.o,$(SRC))
#add the resource file on windows
ifdef BUILD_WINDOWS
	OBJS += cli_framework_resources.o
endif
OBJNAMES = $(addprefix $(OBJECT_MODULE_DIR)/, $(OBJS))

# pull in any previously generated source dependency declarations (.d files)
# (hyphen preceeding the "include" keyword allows MAKE to continue if a .d file is not found) 
-include $(addprefix $(OBJECT_MODULE_DIR)/, $(patsubst %.cpp,%.d,$(SRC)))

# Target library 'linker name'
TARGETBASE = $(LIB_BASENAME).$(LIB_SUFFIX)
# Target library 'soname'
TARGETSO = $(TARGETBASE).$(VERSION_MAJOR)
# Target library 'real name'
ifdef BUILD_WINDOWS
	TARGET = $(TARGETBASE)
else
	TARGET = $(TARGETSO).$(VERSION_MINOR).0
endif 
# Target library 'real name', prefixed with its output location
# This library is packaged as its own library, therefore output to build directory 
TARGETNAME = $(addprefix $(BUILD_DIR)/, $(TARGET))

# ---- COMPILER PARAMETERS -------------------------------------------------------------------------
LIBS = 	-L$(BUILD_DIR)

ifdef BUILD_WINDOWS
	LIBS += -lws2_32 -lmswsock
else ifdef BUILD_LINUX
	LIBS += -ldl
else
	LIBS += -ldl -lm
endif

ifdef INTEL_I18N
	INCS = -I$(EXTERN_DIR)/intel_i18n/include
	LIBS += -l$(I18N_LIB_NAME)
else
ifdef BUILD_WINDOWS
	LIBS += -llibintl
endif
endif

# ---- RECIPES -------------------------------------------------------------------------------------
all : targets
ifndef SKIP_UNITTESTS 
	$(MAKE) $(UNITTEST)
endif

targets: $(OBJECT_MODULE_DIR) $(BUILD_DIR) $(TARGETNAME) headers

headers : $(HEADER_DIR)
	$(COPY) --parents $(EXTERNAL_HEADERS) $(HEADER_DIR)
ifndef SKIP_UNITTESTS
	$(MKDIR) $(HEADER_DIR)/$(UNITTEST)
	$(COPY) $(UNITTEST)/GTestExtensions.h $(HEADER_DIR)/$(UNITTEST)
	sed -i 's/<framework\/NotImplementedErrorResult.h/<libinvm-cli\/NotImplementedErrorResult.h/g' $(HEADER_DIR)/$(UNITTEST)/GTestExtensions.h
	$(MAKE) $(HEADER_DIR)/$(UNITTEST)
endif


$(OBJECT_MODULE_DIR) $(BUILD_DIR) $(HEADER_DIR) $(HEADER_DIR)/$(UNITTEST) :
	$(MKDIR) $@

$(TARGETNAME) : $(OBJNAMES)
ifdef BUILD_WINDOWS
	$(CPP) $(CPPFLAGS) -shared $^ $(LIBS) -o $@ 
else
	$(CPP) $(CPPFLAGS) -shared $^ $(LIBS) -Wl,--as-needed,-soname,$(TARGETSO) -o $@
	cd $(BUILD_DIR); $(RM) $(TARGETSO); $(SOFTLINK) $(TARGET) $(TARGETSO)
	cd $(BUILD_DIR); $(RM) $(TARGETBASE); $(SOFTLINK) $(TARGET) $(TARGETBASE)
endif

# suffix rule for .cpp -> .o
$(OBJECT_MODULE_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(INCS) -c $< -o $@

# suffix rule for .rc -> .o
$(OBJECT_MODULE_DIR)/%.o : %.rc
	$(RC) $(RCFLAGS) $(INCS) $< -o $@
	
$(UNITTEST):
ifndef BUILD_ESX
	$(MAKE) -C $(UNITTEST) all
endif

test:
	$(MAKE) -C $(UNITTEST) test

i18n: 
	$(GETTEXT) *.cpp
	$(GETTEXT) *.h	

clean:
	$(RMDIR) $(OBJECT_MODULE_DIR)	
	$(MAKE) -C $(UNITTEST) clean

clobber : clean
	rm -f $(BUILD_DIR)/$(HEADER_DIRECTORY)*

sourcedrop :
	$(MKDIR) $(SOURCEDROP_DIR)/src/framework
	$(COPY) *.* $(SOURCEDROP_DIR)/src/framework/
	$(COPY) makefile $(SOURCEDROP_DIR)/src/framework/
ifndef SKIP_UNITTESTS
	$(MKDIR) $(SOURCEDROP_DIR)/src/framework/$(UNITTEST)
	$(COPY) $(UNITTEST)/GTestExtensions.h $(SOURCEDROP_DIR)/src/framework/$(UNITTEST)
	sed -i 's/<framework\/NotImplementedErrorResult.h/<libinvm-cli\/NotImplementedErrorResult.h/g' $(SOURCEDROP_DIR)/src/framework/$(UNITTEST)/GTestExtensions.h
endif

.PHONY : all targets headers unittest test clean clobber sourcedrop i18n
