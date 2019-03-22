#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Core/AppDomain.hpp"
#include "Arp/System/Core/Singleton.hxx"
#include "Arp/System/Core/Library.h"
#include "Arp/Plc/Commons/Meta/MetaLibraryBase.hpp"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeDomain.hpp"

namespace PxceTcs { namespace Mqtt
{

using namespace Arp::System::Acf;
using namespace Arp::Plc::Commons::Meta;
using namespace Arp::Plc::Commons::Meta::TypeSystem;

class GdsConnectorLibrary : public MetaLibraryBase, public Singleton<GdsConnectorLibrary>
{
public: // typedefs
    typedef Singleton<GdsConnectorLibrary> SingletonBase;

public: // construction/destruction
    GdsConnectorLibrary(AppDomain& appDomain);
    virtual ~GdsConnectorLibrary() = default;

public: // static operations (called through reflection)
    static void Main(AppDomain& appDomain);

private: // methods
    void InitializeTypeDomain();

private: // deleted methods
    GdsConnectorLibrary(const GdsConnectorLibrary& arg) = delete;
    GdsConnectorLibrary& operator= (const GdsConnectorLibrary& arg) = delete;

private:  // fields
    TypeDomain typeDomain;
};

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain);

///////////////////////////////////////////////////////////////////////////////
// inline methods of class GdsConnectorLibrary

}} // end of namespace PxceTcs.Mqtt
