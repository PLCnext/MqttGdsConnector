///////////////////////////////////////////////////////////////////////////////
//
//  Copyright PHOENIX CONTACT Electronics GmbH
//
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Core/AppDomain.hpp"
#include "Arp/System/Core/Singleton.hxx"
#include "Arp/System/Acf/LibraryBase.hpp"
#include "Arp/System/Core/Library.h"

namespace PxceTcs { namespace Mqtt
{

using namespace Arp::System::Acf;

class GdsConnectorLibrary : public LibraryBase, public Arp::Singleton<GdsConnectorLibrary>
{
public: // typedefs
    typedef Singleton<GdsConnectorLibrary> SingletonBase;

public: // construction/destruction
    GdsConnectorLibrary(Arp::AppDomain& appDomain);
    virtual ~GdsConnectorLibrary() = default;

public: // static operations (called through reflection)
    static void Main(Arp::AppDomain& appDomain);

private: // deleted methods
    GdsConnectorLibrary(const GdsConnectorLibrary& arg) = delete;
    GdsConnectorLibrary& operator= (const GdsConnectorLibrary& arg) = delete;
};

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(Arp::AppDomain& appDomain);

///////////////////////////////////////////////////////////////////////////////
// inline methods of class GdsConnectorLibrary

}} // end of namespace PxceTcs.Mqtt
