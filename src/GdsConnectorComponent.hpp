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
#include <string.h>
#include <fstream>
#include <nlohmann/json.hpp>

#define MQTT_SCHEMA_FILE "/opt/plcnext/apps/60002172000048/mqtt_gds.schema.json"
#define AWS_SCHEMA_FILE "/opt/plcnext/apps/60002172000053/mqtt_gds.schema.json"

#define CYCLE_TIME_MS 500
#define CYCLES_PER_SECOND 2
#define MAX_CYCLES 1209600 // 86400 sec/day * 2 cycles/sec * 7 days, i.e. 1 week at 500 ms cycle

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

    void MqttConnect();

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

    boolean automaticReconnect = false;
    int32 retryInterval = 0;
    int32 secsToReconnect;

    // Output port indicating the connection status
    boolean IsConnected = false;

    // Input port that forces a reconnect attempt
    boolean Reconnect = false;
    // ... and this for edge detection
    boolean ReconnectMemory;

    // Flag that indicates the Update method is blocked
    boolean IsBlocked = false;

    // Cycle counter
    // Used to publish data periodically
    int32 cycles = 0;

private:
	void Update();  // Operation that is executed on each thread loop
};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class GdsConnectorComponent
inline GdsConnectorComponent::GdsConnectorComponent(IApplication& application, const String& name)
: ComponentBase(application, ::PxceTcs::Mqtt::GdsConnectorLibrary::GetInstance(), name, ComponentCategory::Custom)
, updateThread(this, &PxceTcs::Mqtt::GdsConnectorComponent::GdsConnectorComponent::Update, Milliseconds{CYCLE_TIME_MS}.count(), "CyclicUpdate")
{
}

inline IComponent::Ptr GdsConnectorComponent::Create(Arp::System::Acf::IApplication& application, const String& name)
{
    return IComponent::Ptr(new GdsConnectorComponent(application, name));
}

}} // end of namespace PxceTcs.Mqtt
