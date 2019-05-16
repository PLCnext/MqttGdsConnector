///////////////////////////////////////////////////////////////////////////////
//
//  Copyright PHOENIX CONTACT Electronics GmbH
//
///////////////////////////////////////////////////////////////////////////////
#include "GdsConnectorComponent.hpp"
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

namespace PxceTcs { namespace Mqtt
{

using namespace Arp::System::Rsc;
using namespace PxceTcs::MqttClient::Services;
using namespace Arp::Plc::Gds::Services;

bool IsSupported(RscType type)
{
    switch (type)
    {
        case RscType::Bool:
        case RscType::Int8:
        case RscType::Int16:
        case RscType::Int32:
        case RscType::Int64:
        case RscType::Uint8:
        case RscType::Uint16:
        case RscType::Uint32:
        case RscType::Uint64:
        case RscType::Real32:
        case RscType::Real64:
        case RscType::String:
        case RscType::DateTime:
        {
            return true;
            break;
        }

        case RscType::AnsiString:
        case RscType::Array:
        case RscType::Char:
        case RscType::Enumerator:
        case RscType::Exception:
        case RscType::None:
        case RscType::Null:
        case RscType::Object:
        case RscType::SecureString:
        case RscType::SecurityToken:
        case RscType::Stream:
        case RscType::Struct:
        case RscType::Utf16String:
        case RscType::Void:
        default:
            return false;
            break;
    }
}

size_t Sizeof(RscVariant<512> variant)
{
    switch (variant.GetType())
    {
        case RscType::Bool:
        {
            return sizeof(boolean);
            break;
        }

        case RscType::Int8:
        {
            return sizeof(int8);
            break;
        }

        case RscType::Int16:
        {
            return sizeof(int16);
            break;
        }

        case RscType::Int32:
        {
            return sizeof(int32);
            break;
        }

        case RscType::Int64:
        {
            return sizeof(int64);
            break;
        }

        case RscType::Uint8:
        {
            return sizeof(uint8);
            break;
        }

        case RscType::Uint16:
        {
            return sizeof(uint16);
            break;
        }

        case RscType::Uint32:
        {
            return sizeof(uint32);
            break;
        }

        case RscType::Uint64:
        {
            return sizeof(uint64);
            break;
        }

        case RscType::Real32:
        {
            return sizeof(float32);
            break;
        }

        case RscType::Real64:
        {
            return sizeof(float64);
            break;
        }

        case RscType::String:
        {
            // Strings are null-terminated, and the terminating
            // NULL character is sent with the MQTT payload.
            // strlen does not count the terminating NULL, so add 1.
            return strlen(variant.GetChars()) + 1;
            break;
        }

        case RscType::DateTime:
        {
            return sizeof(DateTime);
            break;
        }

        case RscType::AnsiString:
        case RscType::Array:
        case RscType::Char:
        case RscType::Enumerator:
        case RscType::Exception:
        case RscType::None:
        case RscType::Null:
        case RscType::Object:
        case RscType::SecureString:
        case RscType::SecurityToken:
        case RscType::Stream:
        case RscType::Struct:
        case RscType::Utf16String:
        case RscType::Void:
        default:
            return 0;
            break;
    }
}

void GdsConnectorComponent::LoadSettings(const String& settingsPath)
{
    // load firmware settings here
    this->settingsPath = settingsPath;
	this->log.Info("Loaded settings: Path={0}", this->settingsPath);

    // LoadSettings: Go!
    this->allSystemsGo = true;
}

void GdsConnectorComponent::SubscribeServices()
{
    if (!this->allSystemsGo) return;

    // subscribe the services used by this component here

    // TODO: TryGetService first, and log a message if it fails.
    this->pMqttClientService = ServiceManager::GetService<IMqttClientService>("PxceTcs");
    this->log.Info("Subscribed to MQTT Client Service.");

    this->pDataAccessService = ServiceManager::GetService<IDataAccessService>("Arp");
    this->log.Info("Subscribed to GDS Data Access Service.");

    // SubscribeServices: Go!
    this->allSystemsGo = true;
}

void GdsConnectorComponent::LoadConfig()
{
    if (!this->allSystemsGo) return;

    // load project config here
    // TODO: Error checking.
    std::ifstream settingsFile(this->settingsPath.GetBaseString(), ios_base::in);

    // Check for the existence of the settings file
    if(!settingsFile)
    {
        // Settings file doesn't exist.
        this->log.Error("Configuration file {0} does not exist.", this->settingsPath);
        this->allSystemsGo = false;
        return;
    }

    // Read the settings
    this->config = json::parse(settingsFile);
    this->log.Info("Loaded configuration from file: {0}", this->settingsPath);

    // Get the configuration schema file
    std::ifstream mqttSettingsSchemaFile(MQTT_SCHEMA_FILE, ios_base::in);
    std::ifstream awsSettingsSchemaFile(AWS_SCHEMA_FILE, ios_base::in);

    // Check for the existence of the schema file
    // Currently it is not possible to install both the MQTT app
    // and the AWS app in the same PLC, so this should work ...
    std::string schemaFile;
    if(mqttSettingsSchemaFile) schemaFile = MQTT_SCHEMA_FILE;
    else if(awsSettingsSchemaFile) schemaFile = AWS_SCHEMA_FILE;
    else {
        this->log.Error("Configuration schema file does not exist.");
        this->allSystemsGo = false;
        return;
    }

    // Open the schema file.
    std::ifstream settingsSchemaFile(schemaFile, ios_base::in);

    using valijson::Schema;
    using valijson::SchemaParser;
    using valijson::Validator;
    using valijson::ValidationResults;
    using valijson::adapters::NlohmannJsonAdapter;

    // Read the schema
    json configJsonSchema = json::parse(settingsSchemaFile);
    this->log.Info("Loaded configuration schema.");

    // Parse JSON schema content
    Schema configSchema;
    SchemaParser configParser;
    NlohmannJsonAdapter configSchemaAdapter(configJsonSchema);
    configParser.populateSchema(configSchemaAdapter, configSchema);

    // Validate the configuration
    Validator validator;
    ValidationResults results;
    NlohmannJsonAdapter configAdapter(this->config);
    if (!validator.validate(configSchema, configAdapter, &results))
    {
        this->log.Error("Configuration in file {0} is not valid.", this->settingsPath);

        // Report the errors
        ValidationResults::Error error;
        while (results.popError(error))
        {
            std::string context;
            std::vector<std::string>::iterator itr = error.context.begin();
            for (; itr != error.context.end(); itr++)
            {
                context += *itr;
            }
            this->log.Error("Context: {0}, Description: {1}", context, error.description);
        }
        this->allSystemsGo = false;
        return;
    }
    else
    {
        this->log.Info("Configuration is valid.");
    }

    // LoadConfig: Go!
    this->allSystemsGo = true;
}

void GdsConnectorComponent::SetupConfig()
{
    if (!this->allSystemsGo) return;

    // Read the configuration

    // TODO: Error handling - especially JSON errors.

    // Create the MQTT client and check for errors
    // TODO: Handle more than the first broker!
    json broker = this->config["brokers"][0];
    const auto host = RscString<512>(broker["host"].get<std::string>());

    // Check if Transport Layer Security will be used.
    if (char_traits<char>::compare(host.CStr(), "ssl", 3) == 0)
    {
        // If using TLS, check that SSL options have been specified.
        if (!broker.contains("connect_options"))
        {
            this->log.Error("MQTT TLS connection requires SSL options to be specified.");
            this->allSystemsGo = false;
            return;
        }
        else
        {
            if (!broker["connect_options"].contains("ssl_options"))
            {
                this->log.Error("MQTT TLS connection requires SSL options to be specified.");
                this->allSystemsGo = false;
                return;
            }
        }
    }

    // Check that the Status GDS port exists
    if (broker.contains("status_port"))
    {
        RscString<512> portName = RscString<512>(broker["status_port"].get<std::string>());
        ReadItem portData = this->pDataAccessService->ReadSingle(portName);
        if (portData.Error != DataAccessError::None)
        {
            this->log.Error("Port {0}: {1}", portName, portData.Error);
            // Delete this port from the settings.
            broker.erase("status_port");
            this->allSystemsGo = false;
            return;
        }
        else if (portData.Value.GetType() != RscType::Bool)
        {
            this->log.Error("Status port {0} is not of type Bool.", portName);
            // Delete this port from the settings.
            broker.erase("status_port");
            this->allSystemsGo = false;
            return;
        }
    }
    else
    {
        this->log.Info("No status port has been specified.");
    }

    // Check that the Reconnect GDS port exists
    if (broker.contains("reconnect_port"))
    {
        RscString<512> portName = RscString<512>(broker["reconnect_port"].get<std::string>());
        ReadItem portData = this->pDataAccessService->ReadSingle(portName);
        if (portData.Error != DataAccessError::None)
        {
            this->log.Error("Port {0}: {1}", portName, portData.Error);
            // Delete this port from the settings.
            broker.erase("reconnect_port");
            this->allSystemsGo = false;
            return;
        }
        else if (portData.Value.GetType() != RscType::Bool)
        {
            this->log.Error("Reconnect port {0} is not of type Bool.", portName);
            // Delete this port from the settings.
            broker.erase("reconnect_port");
            this->allSystemsGo = false;
            return;
        }
    }
    else
    {
        this->log.Info("No reconnect port has been specified.");
    }

    // Create the MQTT client and check for errors
    const auto clientName = RscString<512>(broker["client_name"].get<std::string>());
    this->mqttClientId = this->pMqttClientService->CreateClient(host, clientName);
    if (this->mqttClientId > 0)
    {
        this->log.Info("Created MQTT Client with ID: {0}", this->mqttClientId);
        // TODO: Store ID in config (required for multiple brokers)

        // Set the client timeout value.
        // This is a catch-all for blocking operations, so that the application doesn't
        // hang forever if one operation fails to complete.
        // The value should be different depending on the underlying type of network and connection
        // e.g. higher for a satellite phone, lower for a LAN.
        if (broker.contains("timeout"))
            this->pMqttClientService->SetTimeout(this->mqttClientId, broker["timeout"].get<int32>());
    }
    else
    {
        this->log.Error("Error creating MQTT Client with URL {0} and client name {1}", host, clientName);
        this->allSystemsGo = false;
        return;
    }

    // Create and initialise a connect options structure
    ConnectOptions opts;
    opts.keepAliveInterval = 60;
    opts.connectTimeout = 30;
    opts.isCleanSession = true;

    // Assign ConnectOptions
    if (broker.contains("connect_options"))
    {
        json connectOpts = broker["connect_options"];

        if (connectOpts.contains("keep_alive_interval"))
            opts.keepAliveInterval = connectOpts["keep_alive_interval"].get<int32>();
        if (connectOpts.contains("connect_timeout"))
            opts.connectTimeout = connectOpts["connect_timeout"].get<int32>();
        if (connectOpts.contains("user_name"))
            opts.userName = (RscString<512>)connectOpts["user_name"].get<std::string>();
        if (connectOpts.contains("password"))
            opts.password = (RscString<512>)connectOpts["password"].get<std::string>();
        if (connectOpts.contains("max_inflight"))
            opts.maxInflight = connectOpts["max_inflight"].get<int32>();

        if (broker["connect_options"].contains("will_options"))
        {
            json willOpts = broker["connect_options"]["will_options"];

            if (willOpts.contains("topic"))
                opts.willOptions.topic = (RscString<512>)willOpts["topic"].get<std::string>();
            if (willOpts.contains("payload"))
                opts.willOptions.payload = (RscString<512>)willOpts["payload"].get<std::string>();
            if (willOpts.contains("qos"))
                opts.willOptions.qos = willOpts["qos"].get<int32>();
            if (willOpts.contains("retained"))
                opts.willOptions.retained = willOpts["retained"].get<boolean>();
        }
        else
        {
            this->log.Info("No MQTT Last Will Options provided. Defaults will be used.");
        }

        if (broker["connect_options"].contains("ssl_options"))
        {
            json sslOpts = broker["connect_options"]["ssl_options"];

            if (sslOpts.contains("trust_store"))
                opts.sslOptions.trustStore = (RscString<512>)sslOpts["trust_store"].get<std::string>();
            if (sslOpts.contains("key_store"))
                opts.sslOptions.keyStore = (RscString<512>)sslOpts["key_store"].get<std::string>();
            if (sslOpts.contains("private_key"))
                opts.sslOptions.privateKey = (RscString<512>)sslOpts["private_key"].get<std::string>();
            if (sslOpts.contains("private_key_password"))
                opts.sslOptions.privateKeyPassword = (RscString<512>)sslOpts["private_key_password"].get<std::string>();
            if (sslOpts.contains("enabled_cipher_suites"))
                opts.sslOptions.enabledCipherSuites = (RscString<512>)sslOpts["enabled_cipher_suites"].get<std::string>();
            if (sslOpts.contains("enable_server_cert_auth"))
                opts.sslOptions.enableServerCertAuth = sslOpts["enable_server_cert_auth"].get<boolean>();
        }
        else
        {
            this->log.Info("No MQTT SSL/TLS Options provided. Defaults will be used.");
        }

        if (connectOpts.contains("is_clean_session"))
            opts.isCleanSession = connectOpts["is_clean_session"].get<boolean>();
        // TODO: Implement opts.servers
        if (connectOpts.contains("mqtt_version"))
            opts.mqttVersion = connectOpts["mqtt_version"].get<int32>();
        if (connectOpts.contains("automatic_reconnect"))
            opts.automaticReconnect = connectOpts["automatic_reconnect"].get<boolean>();
        if (connectOpts.contains("min_retry_interval"))
            opts.minRetryInterval = connectOpts["min_retry_interval"].get<int32>();
        if (connectOpts.contains("max_retry_interval"))
            opts.maxRetryInterval = connectOpts["max_retry_interval"].get<int32>();
    }
    else
    {
        this->log.Info("No MQTT Connect Options provided. Defaults will be used.");
    }

    // Remember some of the automatic connection options
    this->retryInterval = opts.maxRetryInterval;
    this->automaticReconnect = (this->retryInterval > 0 ? opts.automaticReconnect : false);

    // Initialise the automatic reconnect timer.
    this->secsToReconnect = this->retryInterval;

    // Connect to the MQTT client
    int32 mqttResponse = this->pMqttClientService->Connect(this->mqttClientId, opts);
    if (mqttResponse == 0)
    {
        this->log.Info("Connected to MQTT Client {0}", this->mqttClientId);
    }
    else
    {
        this->log.Error("Error connecting to MQTT Client {0}", this->mqttClientId);
        this->allSystemsGo = false;
        return;
    }

    // Check all GDS ports for existence
    // TODO: Handle more than the first broker!
    // TODO: Check for empty arrays.
    // TODO: Use the Namespace Navigator to get Port information, rather than ReadSingle!
    // Iterate through the publish_data list
    json * publishData = &this->config["brokers"][0]["publish_data"];
    for (auto it = publishData->begin(); it != publishData->end(); ++it)
    {
        json publishRecord = it.value();
        RscString<512> portName = RscString<512>(publishRecord["port"].get<std::string>());
        ReadItem portData = this->pDataAccessService->ReadSingle(portName);
        if (portData.Error != DataAccessError::None)
        {
            this->log.Error("Port {0}: {1}", portName, portData.Error);
            // Delete this port from the settings.
            publishData->erase(it);
            --it;  // because the iterator is incremented by the erase method.
            this->allSystemsGo = false;
            return;
        }
        else if (!IsSupported(portData.Value.GetType()))
        {
            this->log.Error("Port {0} data type is not currently supported.", portName);
            // Delete this port from the settings.
            publishData->erase(it);
            --it;  // because the iterator is incremented by the erase method.
            this->allSystemsGo = false;
            return;
        }
    }

    // Iterate through the subscribe_data list
    json * subscribeData = &this->config["brokers"][0]["subscribe_data"];
    for (json::iterator it = subscribeData->begin(); it != subscribeData->end(); ++it)
    {
        // Iterate through the ports that will be updated by this topic
        json * subPorts = &it.value()["ports"];
        for (json::iterator it2 = subPorts->begin(); it2 != subPorts->end(); ++it2)
        {
            RscString<512> portName = RscString<512>(it2.value());
            ReadItem portData = this->pDataAccessService->ReadSingle(portName);
            // TODO: Check that all ports are the same data type!
            if (portData.Error != DataAccessError::None)
            {
                this->log.Error("Port {0}: {1}", portName, portData.Error);
                // Delete this port from the settings.
                subPorts->erase(it2);
                --it2;  // because the iterator is incremented by the erase method.
                this->allSystemsGo = false;
                return;
            }
            else if (!IsSupported(portData.Value.GetType()))
            {
                this->log.Error("Port {0} data type is not currently supported.", portName);
                // Delete this port from the settings.
                publishData->erase(it2);
                --it2;  // because the iterator is incremented by the erase method.
                this->allSystemsGo = false;
                return;
            }
            else
            {
                // Store the type information with the port data.
                // This will be required when consuming MQTT messages.
                // Note that this overwites the previous value, or creates it if it doesn't exist.
                // TODO: Check that all port types are the same!
                it.value()["type"] = (int)portData.Value.GetType();
            }
        }
    }

    // Subscribe to all topics
    if (!Subscribe())
    {
        this->allSystemsGo = false;
        return;
    }

    // Start the worker thread
	this->updateThread.Start();
	this->log.Info("SetupConfig(): Worker thread has been started.");

    // SetupConfig: Go!
    this->allSystemsGo = true;
}

void GdsConnectorComponent::ResetConfig()
{
    // implement this inverse to SetupConfig() and LoadConfig()
	this->log.Info("ResetConfig()");
}

void GdsConnectorComponent::Dispose()
{
    // implement this inverse to SetupSettings(), LoadSettings() and Initialize()
    this->pMqttClientService->Disconnect(this->mqttClientId, 1000);
    this->pMqttClientService->DestroyClient(this->mqttClientId);
	this->updateThread.Stop();
	this->log.Info("Dispose(): Worker thread has been stopped.");
}

void GdsConnectorComponent::Update()
{
    // Calculate seconds from the cycle counter
    // This is likely to drift, but ... so what.
    int32 seconds = this->cycles / CYCLES_PER_SECOND;

    // Generate a pulse (approximately) every second
    boolean secPulse = (this->cycles % CYCLES_PER_SECOND == 0);

    this->log.Info("cycles = {0} : seconds = {1} : secPulse = {2}", this->cycles, seconds, secPulse);

    // Look for a connection drop-out
    // TODO: Handle more than the first broker!
    json broker = this->config["brokers"][0];

    // Handle disconnects and reconnects
    if (this->IsConnected && !this->pMqttClientService->IsConnected(this->mqttClientId))
    {
        // Connection has just dropped.
        this->log.Info("Connection to the server has been lost.");
    }

    if (!this->pMqttClientService->IsConnected(this->mqttClientId))
    {
        if (secPulse)
        {
            // Decrement the automatic reconnect timer and reset if necessary
            if (--(this->secsToReconnect) < 0) this->secsToReconnect = this->retryInterval;
        }
    }
    else
    {
        // Reset the automatic reconnect timer.
        this->secsToReconnect = this->retryInterval;
    }

    // Process input ports
    if (broker.contains("reconnect_port"))
    {
        RscString<512> portName = RscString<512>(broker["reconnect_port"].get<std::string>());
        ReadItem portData = this->pDataAccessService->ReadSingle(portName);
        if (portData.Error == DataAccessError::None)
        {
            portData.Value.CopyTo(this->Reconnect);
        }
        else
        {
            this->log.Warning("Port {0}: {1}", portName, portData.Error);
        }
    }

    // Check for manual or automatic reconnect attempts
    if (this->Reconnect && !this->ReconnectMemory
        || this->automaticReconnect && this->secsToReconnect == 0)
    {
        // Only try to reconnect if currently disconnected
        if (!this->pMqttClientService->IsConnected(this->mqttClientId))
        {
            this->log.Info("Attempting MQTT client reconnect.");
            // Note that if the broker can be contacted, this method blocks
            // until the reconnect is successful.
            // TODO: Understand what causes this to block ...
            //       (for precisely 5 minutes when using the Mosquitto test broker)
            this->pMqttClientService->Reconnect(this->mqttClientId);
        }
    }
    // Remember the value of the reconnect port for next time
    this->ReconnectMemory = this->Reconnect;

    // Check for successful reconnect
    if (!this->IsConnected && this->pMqttClientService->IsConnected(this->mqttClientId))
    {
        // Connection has just been restored.
        this->log.Info("Connection to the server has been restored.");

        // Subscribe to all topics again.
        // Note: If this fails, errors are logged but the app continues to run.
        Subscribe();

        // Reset the automatic reconnect timer.
        this->secsToReconnect = this->retryInterval;
    }

    // Publish all GDS data on every scan cycle.
    // TODO: Think about subscribing to GDS ports instead.
    // OR using the Read() method with Delegates to read multiple data items (including arrays and ... structs?)
    // Iterate through the publish_data list
    json * publishData = &this->config["brokers"][0]["publish_data"];
    if (this->pMqttClientService->IsConnected(this->mqttClientId))
    {
        for (auto it : publishData->items())
        {
            json publishRecord = it.value();
            RscString<512> portName = RscString<512>(publishRecord["port"].get<std::string>());
            int32 period = (publishRecord.contains("period") ? publishRecord["period"].get<int32>() : -1);
            int32 qos = publishRecord["qos"].get<int32>();
            boolean retained = publishRecord["retained"].get<boolean>();

            this->log.Info("Seconds = {0} : Period = {1} : seconds % period = {2}", seconds, period, seconds % period);
            // Publish each record when required
            if (period == -1 || (seconds % period == 0 && secPulse))
            {
                // Read the current value of the port variable
                ReadItem portData = this->pDataAccessService->ReadSingle(portName);

                // TODO: Check for DataAccessErrors

                // Iterate through the topics that the message will be published to
                json * pubTopics = &it.value()["topics"];
                for (auto it2 : pubTopics->items())
                {
                    // Publish the data
                    size_t length = Sizeof(portData.Value);
                    int32 response = this->pMqttClientService->Publish(this->mqttClientId, RscString<512>(it2.value()), portData.Value, length, qos, retained);
                    if (response != 0)
                    {
                        this->log.Warning("Could not publish to topic {0}, RscVariant, {1}, {2}, {3}", it2.value().get<std::string>(), length, qos, retained);
                    }
                }
            }
        }
    }

    // Check if any MQTT subscription data has arrived.
    // Loop until the queue is empty.
    // TODO: Put a limit on this loop!
    // TODO: Consider consolidating all messages and writing all together using the Write() method
    //    ... so that we can also write arrays and ... structs?
    Message msg;

    // Only check subscriptions if we have a connection to the broker.
    if (this->pMqttClientService->IsConnected(this->mqttClientId))
    {
        while (this->pMqttClientService->TryConsumeMessage(this->mqttClientId, msg) == 1)
        {
            // Write to each of the Ports configured for this topic.
            // Iterate through the subscribe_data list
            json * subscribeData = &this->config["brokers"][0]["subscribe_data"];
            for (auto it : subscribeData->items())
            {
                if (it.value()["topic"].get<std::string>() == msg.topic.CStr())
                {
                    // Get the RscVariant type for this topic,
                    // and assign this to the incoming message payload.
                    // TODO: CHECK THAT THE LENGTH OF THE PAYLOAD MATCHES THE DATA TYPE!
                    // TODO: CHECK THAT THIS TYPE CAST IS NECESSARY
                    msg.payload.SetType((RscType)it.value()["type"].get<int>());

                    // Iterate through the ports that will be updated by this topic.
                    json * subPorts = &it.value()["ports"];
                    for (auto it2 : subPorts->items())
                    {
                        // Write the MQTT message payload to the GDS port.
                        WriteItem writePortData;
                        writePortData.PortName = RscString<512>(it2.value());
                        writePortData.Value = RscVariant<512>(msg.payload);
                        DataAccessError result = this->pDataAccessService->WriteSingle(writePortData);
                        if (result != DataAccessError::None)
                        {
                            this->log.Warning("Could not write to port {0}: {1}", it2.value(), result);
                        }
                    }
                }
            }
        }
    }

    // Update output ports
    this->IsConnected = this->pMqttClientService->IsConnected(this->mqttClientId);
    if (broker.contains("status_port"))
    {
        WriteItem writePortData;
        writePortData.PortName = RscString<512>(broker["status_port"].get<std::string>());
        writePortData.Value = RscVariant<512>(this->IsConnected);
        DataAccessError result = this->pDataAccessService->WriteSingle(writePortData);
        if (result != DataAccessError::None)
        {
            this->log.Warning("Could not write to port {0}: {1}", broker["status_port"].get<std::string>(), result);
        }
    }

    // Increment the runtime counter and reset if necessary
    if (++(this->cycles) >= MAX_CYCLES) this->cycles = 0;
}

bool GdsConnectorComponent::Subscribe()
{
    // TODO: Handle more than just the first broker in the list!
    // Iterate through the subscribe_data list
    json * subscribeData = &this->config["brokers"][0]["subscribe_data"];
    for (json::iterator it = subscribeData->begin(); it != subscribeData->end(); ++it)
    {
        // TODO: If there are no ports in this topic - e.g. because all ports in the topic are invalid - then delete the topic and move on.
        // Otherwise, subscribe to this topic.
        RscString<512> topic = RscString<512>(it.value()["topic"].get<std::string>());
        if (this->pMqttClientService->IsConnected(this->mqttClientId))
        {
            int32 mqttResponse = this->pMqttClientService->Subscribe(this->mqttClientId, topic);
            if (mqttResponse == 0)
            {
                this->log.Info("Subscribed to MQTT topic {0}", topic);
            }
            else
            {
                this->log.Error("Error subscribing to MQTT topic {0}", topic);
                return false;
            }
        }
        else
        {
            this->log.Error("MQTT Client is not connected. Cannot subscribe topic {0}", topic);
            return false;
        }
    }
    return true;
}
}} // end of namespace PxceTcs.Mqtt
