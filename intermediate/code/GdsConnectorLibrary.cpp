#include "GdsConnectorLibrary.hpp"
#include "Arp/System/Core/CommonTypeName.hxx"
#include "GdsConnectorComponent.hpp"

namespace PxceTcs { namespace Mqtt
{

GdsConnectorLibrary::GdsConnectorLibrary(AppDomain& appDomain)
    : MetaLibraryBase(appDomain, ARP_VERSION_CURRENT, typeDomain)
    , typeDomain(CommonTypeName<GdsConnectorLibrary>().GetNamespace())
{
    this->componentFactory.AddFactoryMethod(CommonTypeName<::PxceTcs::Mqtt::GdsConnectorComponent>(), &::PxceTcs::Mqtt::GdsConnectorComponent::Create);
    this->InitializeTypeDomain();
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
