///////////////////////////////////////////////////////////////////////////////
//
//  Copyright PHOENIX CONTACT Electronics GmbH
//
///////////////////////////////////////////////////////////////////////////////
#include "GdsConnectorLibrary.hpp"
#include "GdsConnectorComponent.hpp"

namespace PxceTcs { namespace Mqtt
{

GdsConnectorLibrary::GdsConnectorLibrary(AppDomain& appDomain)
    : LibraryBase(appDomain, ARP_VERSION_CURRENT)
{
    this->componentFactory.AddFactoryMethod(CommonTypeName<::PxceTcs::Mqtt::GdsConnectorComponent>(), &::PxceTcs::Mqtt::GdsConnectorComponent::Create);
}

void GdsConnectorLibrary::Main(AppDomain& appDomain)
{
    SingletonBase::CreateInstance(appDomain);
}

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain)
{
    GdsConnectorLibrary::Main(appDomain);
    return  GdsConnectorLibrary::GetInstance();
}

}} // end of namespace PxceTcs.Mqtt
