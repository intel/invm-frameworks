/*
 * Copyright (c) 2015 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Intel Corporation nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * GTestExtensions.h
 *
 * Additional common macros for GTEST
 */

#include <intel_cli_framework/NotImplementedErrorResult.h>
/*
 * Asserts that the syntax (list of strings) is valid but the command is not implemented yet
 */

#define	ASSERT_NOT_IMPLEMENTED(expectedId, expectedType, tokens) \
{ \
	cli::framework::Framework *pFrameworkInst = cli::framework::Framework::getFramework(); \
	cli::framework::ResultBase *pResult = pFrameworkInst->execute(tokens); \
	cli::framework::NotImplementedErrorResult *pErrorResult = dynamic_cast<cli::framework::NotImplementedErrorResult *>(pResult); \
	ASSERT_NE((cli::framework::NotImplementedErrorResult *)NULL, pErrorResult) << pResult->output(); \
	EXPECT_EQ(cli::framework::ErrorResult::ERRORCODE_NOTSUPPORTED, pErrorResult->getErrorCode()); \
	EXPECT_EQ(expectedType::Name, pErrorResult->getFeatureName()); \
	EXPECT_EQ( expectedId, pErrorResult->getCommandSpecId()); \
	delete pResult;\
}

#define	EXPECT_STR_CONTAINS(expected, stringToSearch) \
{	std::string expectedStr(expected); \
	std::string l_stringToSearch(stringToSearch);\
	size_t pos = l_stringToSearch.find(expectedStr); \
	EXPECT_NE((size_t)std::string::npos, (size_t)pos) << "\"" << expected << "\" not found in \"" << stringToSearch << "\""; \
}

#define	EXPECT_STR_NOT_CONTAINS(expected, stringToSearch) \
{	std::string expectedStr(expected); \
	std::string l_stringToSearch(stringToSearch);\
	size_t pos = l_stringToSearch.find(expectedStr); \
	EXPECT_EQ((size_t)std::string::npos, (size_t)pos) << "\"" << expected << "\" not found in \"" << stringToSearch << "\""; \
}

#define ASSERT_NOT_NULL(type, pointer) ASSERT_NE((type)NULL, (pointer))
#define ASSERT_NULL(type, pointer) ASSERT_EQ((type)NULL, (pointer))
#define ASSERT_TYPE(type, pointer) ASSERT_NE((type)NULL, dynamic_cast<type>(pointer))
#define ASSERT_NOT_TYPE(type, pointer) ASSERT_EQ((type)NULL, dynamic_cast<type>(pointer))
#define EXPECT_NOT_NULL(type, pointer) EXPECT_NE((type)NULL, pointer)
#define EXPECT_NULL(type, pointer) EXPECT_EQ((type)NULL, pointer)
#define EXPECT_TYPE(type, pointer) EXPECT_NE((type)NULL, dynamic_cast<type>(pointer))
#define EXPECT_NOT_TYPE(type, pointer) EXPECT_EQ((type)NULL, dynamic_cast<type>(pointer))

#define EXPECT_OBJECTLIST_CONTAINS_KEY(expect, collection) \
		{cli::framework::propertyObjects_t::iterator iter = collection->objectsBegin();     \
		bool found = false;                                                                   \
		for (; iter != collection->objectsEnd() && !found; iter++)                       \
		{                                                                                     \
			if (iter->first == expect)                                                      \
				found = true;                                                                 \
		}\
		EXPECT_TRUE(found) << expect << " not found in collection";}
