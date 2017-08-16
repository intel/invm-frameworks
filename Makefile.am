ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = \
	libinvm-cim.la \
	libinvm-i18n.la \
	libinvm-cli.la

AM_CPPFLAGS =

if LINUX
AM_CPPFLAGS += \
	-DCMPI_PLATFORM_LINUX_GENERIC_GNU=1
endif
if WINDOWS
AM_CPPFLAGS += \
	-DCMPI_PLATFORM_WIN32_IX86_MSVC=1
endif

libinvm_cim_la_SOURCES = \
	external/invm-cim/rapidxml-1.13/rapidxml.hpp \
	external/invm-cim/rapidxml-1.13/rapidxml_iterators.hpp \
	external/invm-cim/rapidxml-1.13/rapidxml_print.hpp \
	external/invm-cim/rapidxml-1.13/rapidxml_utils.hpp \
	invm-cim/src/cimom/CimomAdapter.cpp \
	invm-cim/src/cimom/cmpi/CmpiAdapter.cpp \
	invm-cim/src/cimom/cmpi/CmpiAdapter.h \
	invm-cim/src/cimom/cmpi/IntelCmpiProvider.cpp \
	invm-cim/src/cimom/cmpi/IntelToCmpi.cpp \
	invm-cim/src/cimom/cmpi/IntelToCmpi.h \
	invm-cim/src/common/common_types.h \
	invm-cim/src/common/logger/Logger.cpp \
	invm-cim/src/common/logger/logging.h \
	invm-cim/src/common/logger/Trace.cpp \
	invm-cim/src/common/logger/Trace.h \
	invm-cim/src/common/string/revision.c \
	invm-cim/src/common/string/revision.h \
	invm-cim/src/common/string/s_str.c \
	invm-cim/src/common/string/unicode_utilities.c \
	invm-cim/src/common/string/unicode_utilities.h \
	invm-cim/src/common/string/x_str.c \
	invm-cim/src/common/string/x_str.h \
	invm-cim/src/common/time/time_utilities.c \
	invm-cim/src/common/time/time_utilities.h \
	invm-cim/src/framework/AssociationFactory.cpp \
	invm-cim/src/framework/AssociationMapper.cpp \
	invm-cim/src/framework/Attribute.cpp \
	invm-cim/src/framework/CimXml.cpp \
	invm-cim/src/framework/ExceptionBadAttribute.cpp \
	invm-cim/src/framework/ExceptionBadParameter.cpp \
	invm-cim/src/framework/Exception.cpp \
	invm-cim/src/framework/ExceptionInvalidWqlQuery.cpp \
	invm-cim/src/framework/ExceptionNoMemory.cpp \
	invm-cim/src/framework/ExceptionNotSupported.cpp \
	invm-cim/src/framework/ExceptionSystemError.cpp \
	invm-cim/src/framework/IndicationService.cpp \
	invm-cim/src/framework/Instance.cpp \
	invm-cim/src/framework/InstanceFactory.cpp \
	invm-cim/src/framework/NullInstanceFactory.cpp \
	invm-cim/src/framework/NullInstanceFactory.h \
	invm-cim/src/framework/ObjectPathBuilder.cpp \
	invm-cim/src/framework/ObjectPath.cpp \
	invm-cim/src/framework/ProviderFactory.cpp \
	invm-cim/src/framework/StringUtil.cpp \
	invm-cim/src/framework/WqlConditional.cpp \
	invm-cim/src/framework/WqlQuery.cpp \
	invm-cim/src/framework/WqlQuery.h

libinvm_cimdir = $(includedir)/libinvm-cim
libinvm_cim_HEADERS = \
	invm-cim/src/cimom/CimomAdapter.h \
	invm-cim/src/cimom/cmpi/IntelCmpiProvider.h \
	invm-cim/src/common/logger/Logger.h \
	invm-cim/src/common/string/s_str.h \
	invm-cim/src/framework/AssociationFactory.h \
	invm-cim/src/framework/AssociationMapper.h \
	invm-cim/src/framework/Attribute.h \
	invm-cim/src/framework/CimXml.h \
	invm-cim/src/framework/ExceptionBadAttribute.h \
	invm-cim/src/framework/ExceptionBadParameter.h \
	invm-cim/src/framework/Exception.h \
	invm-cim/src/framework/ExceptionInvalidWqlQuery.h \
	invm-cim/src/framework/ExceptionNoMemory.h \
	invm-cim/src/framework/ExceptionNotSupported.h \
	invm-cim/src/framework/ExceptionSystemError.h \
	invm-cim/src/framework/IndicationService.h \
	invm-cim/src/framework/InstanceFactoryCreator.h \
	invm-cim/src/framework/InstanceFactory.h \
	invm-cim/src/framework/Instance.h \
	invm-cim/src/framework/ObjectPathBuilder.h \
	invm-cim/src/framework/ObjectPath.h \
	invm-cim/src/framework/ProviderFactory.h \
	invm-cim/src/framework/Strings.h \
	invm-cim/src/framework/StringUtil.h \
	invm-cim/src/framework/Types.h \
	invm-cim/src/framework/WqlConditional.h

if WINDOWS
libinvm_cim_la_SOURCES +=
	invm-cim/src/cimom/wmi/DllMain.cpp \
	invm-cim/src/cimom/wmi/IntelToWmi.cpp \
	invm-cim/src/cimom/wmi/IntelWmiProvider.cpp \
	invm-cim/src/cimom/wmi/IntelWmiProviderFactory.cpp \
	invm-cim/src/cimom/wmi/WmiAdapter.cpp \
	invm-cim/src/cimom/wmi/wmiUtilities.cpp
endif

libinvm_cim_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I $(top_srcdir)/invm-cim/src/framework \
	-I $(top_srcdir)/external/invm-cim/cmpi/include \
	-I $(top_srcdir)/external/invm-cim/rapidxml-1.13 \
	-I $(top_srcdir)/invm-cim/src/cimom \
	-I $(top_srcdir)/invm-cim/src/common \
	-I $(top_srcdir)/invm-cim/src/common/logger \
	-I $(top_srcdir)/invm-cim/src/common/string

libinvm_cim_la_LDFLAGS = \
	-version-info 1:0:0

invmincludedir = $(includedir)/libinvm-cim/cmpi
invminclude_HEADERS = \
	external/invm-cim/cmpi/include/cmpi/cmpidt.h \
	external/invm-cim/cmpi/include/cmpi/cmpift.h \
	external/invm-cim/cmpi/include/cmpi/cmpimacs.h \
	external/invm-cim/cmpi/include/cmpi/cmpios.h \
	external/invm-cim/cmpi/include/cmpi/cmpipl.h

libinvm_cli_la_SOURCES = \
	invm-cli/src/framework/CliFrameworkTypes.cpp \
	invm-cli/src/framework/CommandFilter.cpp \
	invm-cli/src/framework/CommandFilter.h \
	invm-cli/src/framework/CommandSpec.cpp \
	invm-cli/src/framework/CommandVerify.cpp \
	invm-cli/src/framework/CommandVerify.h \
	invm-cli/src/framework/cr_i18n.h \
	invm-cli/src/framework/DuplicateTokenErrorResult.cpp \
	invm-cli/src/framework/DuplicateTokenErrorResult.h \
	invm-cli/src/framework/ErrorResult.cpp \
	invm-cli/src/framework/FeatureBase.cpp \
	invm-cli/src/framework/Framework.cpp \
	invm-cli/src/framework/HelpFeature.cpp \
	invm-cli/src/framework/HelpFeature.h \
	invm-cli/src/framework/HelpResult.cpp \
	invm-cli/src/framework/HelpResult.h \
	invm-cli/src/framework/Lexer.cpp \
	invm-cli/src/framework/Lexer.h \
	invm-cli/src/framework/Logger.cpp \
	invm-cli/src/framework/NoInputErrorResult.cpp \
	invm-cli/src/framework/NoInputErrorResult.h \
	invm-cli/src/framework/NotImplementedErrorResult.cpp \
	invm-cli/src/framework/ObjectListResult.cpp \
	invm-cli/src/framework/osAdapter.cpp \
	invm-cli/src/framework/osAdapter.h \
	invm-cli/src/framework/OutputOptions.cpp \
	invm-cli/src/framework/OutputOptionsValidator.cpp \
	invm-cli/src/framework/OutputOptionsValidator.h \
	invm-cli/src/framework/ParseErrorResult.cpp \
	invm-cli/src/framework/ParseErrorResult.h \
	invm-cli/src/framework/Parser.cpp \
	invm-cli/src/framework/PropertyListResult.cpp \
	invm-cli/src/framework/ResultBase.cpp \
	invm-cli/src/framework/SimpleListResult.cpp \
	invm-cli/src/framework/SimpleResult.cpp \
	invm-cli/src/framework/SyntaxErrorBadValueResult.cpp \
	invm-cli/src/framework/SyntaxErrorMissingValueResult.cpp \
	invm-cli/src/framework/SyntaxErrorResult.cpp \
	invm-cli/src/framework/SyntaxErrorUnexpectedValueResult.cpp \
	invm-cli/src/framework/SyntaxErrorUnexpectedValueResult.h \
	invm-cli/src/framework/Trace.cpp

libinvm_clidir = $(includedir)/libinvm-cli
libinvm_cli_HEADERS = \
	invm-cli/src/framework/CliFrameworkTypes.h \
	invm-cli/src/framework/CommandSpec.h \
	invm-cli/src/framework/ErrorResult.h \
	invm-cli/src/framework/FeatureBase.h \
	invm-cli/src/framework/FeatureRef.h \
	invm-cli/src/framework/Framework.h \
	invm-cli/src/framework/Logger.h \
	invm-cli/src/framework/NotImplementedErrorResult.h \
	invm-cli/src/framework/ObjectListResult.h \
	invm-cli/src/framework/OutputOptions.h \
	invm-cli/src/framework/Parser.h \
	invm-cli/src/framework/PropertyListResult.h \
	invm-cli/src/framework/ResultBase.h \
	invm-cli/src/framework/SimpleListResult.h \
	invm-cli/src/framework/SimpleResult.h \
	invm-cli/src/framework/SyntaxErrorBadValueResult.h \
	invm-cli/src/framework/SyntaxErrorMissingValueResult.h \
	invm-cli/src/framework/SyntaxErrorResult.h \
	invm-cli/src/framework/Trace.h

libinvm_cli_la_LDFLAGS = \
	-version-info 1:0:0

libinvm_i18n_la_SOURCES = \
	invm-i18n/src/gettext.c \
	invm-i18n/src/gettext_dummy.c \
	invm-i18n/src/libIntel_i18n_local.h \
	invm-i18n/src/posix_system.c \
	invm-i18n/src/safe_str.c \
	invm-i18n/src/safe_str.h \
	invm-i18n/src/system.h \
	invm-i18n/src/textdomain.c

libinvm_i18ndir = $(includedir)/libinvm-i18n
libinvm_i18n_HEADERS = \
	invm-i18n/src/libIntel_i18n.h

if WINDOWS
libinvm_i18n_la_SOURCES += \
	invm-i18n/src/win_system.c
endif

libinvm_i18n_la_LDFLAGS = \
	-version-info 1:0:0

pkgconfig_DATA = \
	invm-cim.pc \
	invm-cli.pc \
	invm-i18n.pc

EXTRA_DIST = \
	README.md \
	LICENSE \
	external/invm-cim/rapidxml-1.13/license.txt \
	external/invm-cim/rapidxml-1.13/manual.html
