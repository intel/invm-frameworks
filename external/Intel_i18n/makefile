#
# Copyright (c) 2015, Intel Corporation
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
# Makefile for Common/gettext library
#

# ---- BUILD ENVIRONMENT ---------------------------------------------------------------------------
ROOT_DIR = ../..
# sets up standard build variables
include $(ROOT_DIR)/build.mk

# defines the location of this module's compiled objects
OBJECT_MODULE_DIR = $(OBJECT_DIR)/Intel_i18n

# ---- FILES ---------------------------------------------------------------------------------------
# defines list of objects to create from source
ifdef BUILD_WINDOWS
	SRC_EXCLUDE = $(wildcard posix_*.c)
else ifdef BUILD_LINUX
	SRC_EXCLUDE = $(wildcard win_*.c)
else ifdef BUILD_ESX
	SRC_EXCLUDE = $(wildcard win_*.c)
endif

EXTERNAL_HEADERS = libIntel_i18n.h

# Target library 'linker name'
TARGETBASE = libIntel_i18n.$(LIB_SUFFIX)
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

EXCLUDES =
ifdef BUILD_LINUX
	EXCLUDES = win*.*
else ifdef BUILD_ESX
	EXCLUDES = win*.*
endif

# ---- DEFINES --------------------------------------------------------------------------------------
ifdef BUILD_WINDOWS
# Windows Vista and after
	WINVER = 0x0600
endif

SRC = $(filter-out $(SRC_EXCLUDE),$(wildcard *.c))

OBJS = $(patsubst %.c,%.o,$(SRC))
# defines the location of the object files
OBJNAMES = $(addprefix $(OBJECT_MODULE_DIR)/, $(OBJS))

# pull in any previously generated source dependency declarations (.d files)
# (hyphen preceeding the "include" keyword allows MAKE to continue if a .d file is not found)
-include $(addprefix $(OBJECT_MODULE_DIR)/, $(patsubst %.c,%.d,$(SRC)))

# compiled objects not archived into static library for this module.
# they will be compiled into libcommon.a, a level above this
 
# ---- COMPILER PARAMETERS -------------------------------------------------------------------------
INCS = -I$(SRC_DIR)
LIBS = 

ifdef BUILD_LINUX
	CFLAGS += -D_GNU_SOURCE
else ifdef BUILD_ESX
	CFLAGS += -D_GNU_SOURCE
else ifdef BUILD_WINDOWS
	CFLAGS += -DWINVER=$(WINVER) -D_WIN32_WINNT=$(WINVER)
endif 

# ---- RECIPES -------------------------------------------------------------------------------------
all : targets

targets: $(BUILD_DIR) $(OBJECT_MODULE_DIR) $(TARGETNAME) $(I18N_HEADER_DIR) headers

$(TARGETNAME) : $(OBJNAMES)
ifdef BUILD_WINDOWS
	$(CPP) $(CPPFLAGS) -shared $^ $(LIBS) -o $@ 
else
	$(CPP) $(CPPFLAGS) -shared $^ $(LIBS) -Wl,-soname,$(TARGETSO) -o $@
	cd $(BUILD_DIR); $(RM) $(TARGETSO); $(SOFTLINK) $(TARGET) $(TARGETSO)
	cd $(BUILD_DIR); $(RM) $(TARGETBASE); $(SOFTLINK) $(TARGET) $(TARGETBASE)
endif
ifndef SKIP_UNITTESTS
	$(MAKE) -C unittest
endif

$(I18N_HEADER_DIR) :
	$(MKDIR) $@
	
headers :
	$(COPY) $(EXTERNAL_HEADERS) $(I18N_HEADER_DIR)
		
$(OBJECT_MODULE_DIR) :
	$(MKDIR) $@

$(BUILD_DIR) :
	$(MKDIR) $@

# suffix rule for .c -> .o
$(OBJECT_MODULE_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	
test:
	$(MAKE) -C unittest test
	
cstyle :
	$(CSTYLE) $(SRC)

clean :
	$(RM) $(OBJNAMES)

clobber : clean
	$(RMDIR) $(OBJECT_MODULE_DIR)

sourcedrop: 
	$(MKDIR) $(SOURCEDROP_DIR)/external/Intel_i18n/
	$(COPY) *.* $(SOURCEDROP_DIR)/external/Intel_i18n/
	$(COPY) makefile $(SOURCEDROP_DIR)/external/Intel_i18n/
	for exclude in $(EXCLUDES); do \
		$(RM) $(SOURCEDROP_DIR)/external/Intel_i18n/$${exclude}; \
	done
.PHONY : all  cstyle clean clobber sourcedrop
