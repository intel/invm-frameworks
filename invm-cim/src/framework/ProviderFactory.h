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
 */

/*
 * This class serves as an abstract base class for a factory that creates
 * InstanceFactories for given CIM classes.
 */

#ifndef WBEM_FRAMEWORK_PROVIDERFACTORY_H_
#define WBEM_FRAMEWORK_PROVIDERFACTORY_H_

#include <string>
#include <vector>
#include "InstanceFactory.h"
#include "AssociationFactory.h"
#include "IndicationService.h"

namespace wbem
{
namespace framework
{

class ProviderFactory : public InstanceFactoryCreator
{
public:
	ProviderFactory();
	virtual ~ProviderFactory();

	/*
	 * Each provider is expected to set its singleton on library load. It is up to the
	 * provider to delete the singleton when the process is complete.
	 *
	 * Setting the singleton will cause the old singleton, if any, to be automatically
	 * deleted.
	 */
	static ProviderFactory *getSingleton();
	static void setSingleton(ProviderFactory *pProviderFactory);
	static void deleteSingleton();

	/*
	 * Perform any provider initialization that needs to be done for each action
	 */
	virtual void InitializeProvider() {};

	/*
	 * Clean up provider after each action
	 */
	virtual void CleanUpProvider() {};

	/*
	 * Fetches the default CIM namespace for this set of CIM providers.
	 */
	std::string getDefaultCimNamespace();

	/*
	 * Gets the singleton and fetches the appropriate InstanceFactory. Returns
	 * NULL if either singleton or factory is NULL.
	 */
	static InstanceFactory *getInstanceFactoryStatic(const std::string &className);

	/*
	 * Gets the singleton and fetches the appropriate InstanceFactories for associations.
	 * Returns empty list if either singleton or factory is NULL.
	 */
	static std::vector<InstanceFactory *> getAssociationFactoriesStatic(
			Instance *pInstance,
			const std::string &associationClassName,
			const std::string &resultClassName,
			const std::string &roleName,
			const std::string &resultRoleName);

	/*
	 * Implement this method to return an appropriate InstanceFactory for the given
	 * CIM class name.
	 */
	virtual InstanceFactory *getInstanceFactory(const std::string &className) = 0;

	/*
	 * Implement this method to return an appropriate AssociationFactory list
	 * for the request.
	 */
	virtual std::vector<InstanceFactory *> getAssociationFactories(
			Instance *pInstance,
			const std::string &associationClassName,
			const std::string &resultClassName,
			const std::string &roleName,
			const std::string &resultRoleName) = 0;

	virtual IndicationService *getIndicationService() = 0;

protected:
	static ProviderFactory *m_pSingleton;
	std::string m_defaultCimNamespace;

	/*
	 * Sets the default CIM namespace for this set of CIM providers.
	 */
	void setDefaultCimNamespace(const std::string &cimNamespace);
};

} /* namespace framework */
} /* namespace wbem */

#endif /* WBEM_FRAMEWORK_PROVIDERFACTORY_H_ */
