///////////////////////////////////////////////////////////////////////////////
//
//  Copyright PHOENIX CONTACT Electronics GmbH
//
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Acf/ComponentBase.hpp"
#include "Arp/System/Acf/IApplication.hpp"
#include "GdsConnectorLibrary.hpp"
#include "Arp/System/Commons/Logging.h"
#include "Arp/System/Commons/Threading/WorkerThread.hpp"
#include "Arp/System/Rsc/ServiceManager.hpp"
#include "Arp/System/Rsc/Services/RscString.hxx"
#include "Arp/System/Rsc/Services/RscVariant.hxx"
#include "Arp/System/Commons/Threading/WorkerThread.hpp"
#include "Arp/Plc/Gds/Services/IDataAccessService.hpp"

#include "IMqttClientService.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

<<<<<<< HEAD
#define APP_ID "60002172000048"  // The unique ID for this app in the PLCnext Store
#define SCHEMA_FILE_PATH "$ARP_ACTIVE_APPS_DIR$/" APP_ID
#define SCHEMA_FILE_NAME "mqtt_gds.schema.json"
#define SCHEMA_FILE SCHEMA_FILE_PATH "/" SCHEMA_FILE_NAME

=======
>>>>>>> bfb9a5c... added gitattributes to avoid messing up LF
namespace PxceTcs { namespace Mqtt
{

using namespace Arp;
using namespace Arp::System::Acf;

// for convenience
using json = nlohmann::json;

//#component
class GdsConnectorComponent : public ComponentBase, private Loggable<GdsConnectorComponent>
{
public: // typedefs

public: // construction/destruction
    GdsConnectorComponent(IApplication& application, const String& name);
    virtual ~GdsConnectorComponent() = default;

public: // IComponent operations
    void LoadSettings(const String& settingsPath) override;
    void SubscribeServices() override;
    void LoadConfig() override;
    void SetupConfig() override;
    void ResetConfig() override;
    void Dispose() override;

private: // methods
    GdsConnectorComponent(const GdsConnectorComponent& arg) = delete;
    GdsConnectorComponent& operator= (const GdsConnectorComponent& arg) = delete;

public: // static factory operations
    static IComponent::Ptr Create(Arp::System::Acf::IApplication& application, const String& name);

private: // fields
    String settingsPath;
 	System::Commons::Threading::WorkerThread updateThread;
    PxceTcs::MqttClient::Services::IMqttClientService::Ptr pMqttClientService;
    Arp::Plc::Gds::Services::IDataAccessService::Ptr pDataAccessService;

    int32 mqttClientId;
    int32 mqttPublishResponse;

    json config;

private:
	void Update();  // Operation that is executed on each thread loop

public: /* Ports
           =====
           Component ports are defined in the following way:
           //#port
           //#name(NameOfPort)
           boolean portField;

           The name comment defines the name of the port and is optional. Default is the name of the field.
           Attributes which are defined for a component port are IGNORED. If component ports with attributes
           are necessary, define a single structure port where attributes can be defined foreach field of the
           structure.
        */
};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class GdsConnectorComponent
inline GdsConnectorComponent::GdsConnectorComponent(IApplication& application, const String& name)
: ComponentBase(application, ::PxceTcs::Mqtt::GdsConnectorLibrary::GetInstance(), name, ComponentCategory::Custom)
, updateThread(this, &PxceTcs::Mqtt::GdsConnectorComponent::GdsConnectorComponent::Update, Milliseconds{500}.count(), "CyclicUpdate")
{
}

inline IComponent::Ptr GdsConnectorComponent::Create(Arp::System::Acf::IApplication& application, const String& name)
{
    return IComponent::Ptr(new GdsConnectorComponent(application, name));
}

}} // end of namespace PxceTcs.Mqtt
