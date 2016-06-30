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
# makefile - Top level makefile for the Intel CLI Framework component and example CLI utility
#

# ---- BUILD ENVIRONMENT ---------------------------------------------------------------------------
ROOT_DIR = .
# sets up standard build variables
include $(ROOT_DIR)/build.mk
SKIP_UNITTESTS ?= 1

FLAGS := SKIP_UNITTESTS=$(SKIP_UNITTESTS)

# ---- FILES ---------------------------------------------------------------------------------------
# Linux Install Files
LIB_DIR ?= /usr/lib64
# files that get installed into /usr/lib64
LIB_FILES = $(LIB_BASENAME).so* 
INCLUDE_DIR ?= /usr/include
# files that get installed into /usr/include/
FRAMEWORK_INCLUDE_DIR = $(HEADER_DIRECTORY)
I18N_INCLUDE_DIR = $(I18N_HEADER_DIRECTORY)

# ---- RECIPES -------------------------------------------------------------------------------------

all: $(BUILD_DIR) 
ifdef BUILD_WINDOWS
	$(MAKE) -C src libcopy $(FLAGS)
endif
	$(MAKE) -C src framework $(FLAGS)
	$(MAKE) i18n 

$(BUILD_DIR):
	$(MKDIR) $@

# Create internationalization files
i18n:	
	$(MAKE) -C src i18n $(FLAGS)
ifndef RELEASE	
	# an example translation file
	$(MKDIR) $(LOCALE_DIR) # where translation must go
	$(MSGMERGE) $(ROOT_DIR)/lang/en_US.po $(GETTEXT_OUTPUTFILE) -o $(LOCALE_DIR)/$(LOCALE_DOMAIN).po
	# Any new translations should happen before the .mo file is created
	$(MSGFMT) $(LOCALE_DIR)/$(LOCALE_DOMAIN).po -o $(LOCALE_DIR)/$(LOCALE_DOMAIN).mo
endif

clean :
	$(MAKE) -C src clean $(FLAGS)

clobber :
	$(MAKE) -C src clobber $(FLAGS)
	$(RM) $(BUILD_DIR)/cyrstalridge.pot
	$(RM) -r $(LOCALE_DIR)
	$(RM) $(GETTEXT_OUTPUTFILE)

install :
	# complete the paths for the files to be installed
	$(eval LIB_FILES := $(addprefix $(BUILD_DIR)/, $(LIB_FILES)))
	$(eval FRAMEWORK_INCLUDE_DIR := $(addprefix $(BUILD_DIR)/include/, $(FRAMEWORK_INCLUDE_DIR)))
	# install files into lib directory
	$(MKDIR) $(RPM_ROOT)$(LIB_DIR)
	$(COPY) $(LIB_FILES) $(RPM_ROOT)$(LIB_DIR)
	
	# install files into include directory
	$(MKDIR) $(RPM_ROOT)$(INCLUDE_DIR)
	$(COPY) $(FRAMEWORK_INCLUDE_DIR) $(RPM_ROOT)$(INCLUDE_DIR)
uninstall : 

	$(eval LIB_FILES := $(addprefix $(RPM_ROOT)$(LIB_DIR)/, $(LIB_FILES)))
	$(eval FRAMEWORK_INCLUDE_DIR := $(addprefix $(RPM_ROOT)$(INCLUDE_DIR)/, $(FRAMEWORK_INCLUDE_DIR)))
	
	$(RM) $(LIB_FILES)
	$(RMDIR) $(FRAMEWORK_INCLUDE_DIR)

rpm :
	#Make the Directories
	$(MKDIR) $(RPMBUILD_DIR) $(RPMBUILD_DIR)/BUILD $(RPMBUILD_DIR)/SOURCES $(RPMBUILD_DIR)/RPMS \
				$(RPMBUILD_DIR)/SRPMS $(RPMBUILD_DIR)/SPECS $(RPMBUILD_DIR)/BUILDROOT \
				$(RPMBUILD_DIR)/BUILD/$(HEADER_DIRECTORY)-$(BUILDNUM)

	#Copy Spec File
	$(COPY) install/linux/$(LINUX_DIST)-release/*.spec $(RPMBUILD_DIR)/SPECS/$(LIB_BASENAME).spec
	#Update the Spec file
	$(SED) -i 's/^%define build_version .*/%define build_version $(BUILDNUM)/g' $(RPMBUILD_DIR)/SPECS/$(LIB_BASENAME).spec

	#Archive the directory
	git archive --format=tar --prefix="$(LIB_BASENAME)-$(BUILDNUM)/" HEAD | gzip -c > $(RPMBUILD_DIR)/SOURCES/$(LIB_BASENAME)-$(BUILDNUM).tar.gz
	#rpmbuild
	$(RPMBUILD) -ba $(RPMBUILD_DIR)/SPECS/$(LIB_BASENAME).spec --define "_topdir $(RPMBUILD_DIR)" --define "cflag $(CFLAGS_EXTERNAL)"

.PHONY : all qb_standard src i18n test clean clobber install uninstall sourcedrop rpm
