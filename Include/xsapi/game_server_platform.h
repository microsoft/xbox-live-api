// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

namespace xbox { namespace services {
    class xbox_live_context_impl;
    /// <summary>
    /// Classes for managing and referencing server resources allocated by the game server platform.
    /// </summary>
    namespace game_server_platform {

/// <summary>Defines a set of values used to indicate the the fulfillment state.</summary>
enum class game_server_fulfillment_state
{
    /// <summary>The fulfillment state is unknown.</summary>
    unknown,

    /// <summary>Indicates that the allocate cluster request is fulfilled and the cluster is available.</summary>
    fulfilled,

    /// <summary>Indicates that the allocate cluster request is queued.</summary>
    queued,

    /// <summary>Indicates that the cluster request was aborted. This will only be returned if "abortIfQueued" is set to true.</summary>
    aborted
};

/// <summary>Defines a set of values used to indicate the host state.</summary>
enum class game_server_host_status
{
    /// <summary>The fulfillment state is unknown.</summary>
    unknown,

    /// <summary>Indicates that the cluster request is active.</summary>
    active,

    /// <summary>Indicates that the cluster request is queued.</summary>
    queued,

    /// <summary>Indicates that the cluster request was aborted. This will only be returned if "abortIfQueued" is set to true.</summary>
    aborted,

    /// <summary>Indicates that the cluster request had an error.</summary>
    error
};

/// <summary>
/// Represents a mapping between the port numbers requested for the game server to use, and the port numbers exposed by load-balancing hardware.
/// </summary>
class game_server_port_mapping
{
    // Example:
    //    "portMappings": {
    //        "GameData1": {
    //            "internal": 8347,
    //            "external": 30002
    //        },
    //        "GameData2": {
    //            "internal": 8348,
    //            "external": 31002
    //        },
    //        "Microsoft.WindowsAzure.Plugins.RemoteAccess.Rdp": {
    //            "internal": 3389,
    //            "external": 3389
    //        },
    //        "Microsoft.WindowsAzure.Plugins.RemoteForwarder.RdpInput": {
    //            "internal": 20000,
    //            "external": 3389
    //        }
    //    }

public:
    game_server_port_mapping();

    /// <summary>
    /// Internal function
    /// </summary>
    game_server_port_mapping(
        _In_ string_t portName,
        _In_ uint32_t internalPortNumber,
        _In_ uint32_t externalPortNumber
        );

    /// <summary>
    /// The name of the port, which is specified in your Azure project cscfg.
    /// </summary>
    _XSAPIIMP const string_t& port_name() const;

    /// <summary>
    /// The internal port number for the named port.  This is the port that the server will open using the socket API.
    /// </summary>
    _XSAPIIMP uint32_t internal_port_number() const;

    /// <summary>
    /// The external port number clients should use when connecting.  This port is mapped by networking hardware to the internal port number.
    /// </summary>
    _XSAPIIMP uint32_t external_port_number() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<game_server_port_mapping> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_portName;
    uint32_t m_internalPortNumber;
    uint32_t m_externalPortNumber;
};

/// <summary>
/// Represents a game server cluster result.
/// </summary>
class cluster_result
{    
public:
    cluster_result();

    /// <summary>
    /// Internal function
    /// </summary>
    cluster_result(
        _In_ std::chrono::milliseconds pollInterval,
        _In_ game_server_fulfillment_state fulfillmentState,
        _In_ string_t hostName,
        _In_ string_t region,
        _In_ std::vector< game_server_port_mapping > portMappings,
        _In_ string_t secureDeviceAddress
        );

    /// <summary>
    /// Recommended interval to poll for completion. Note that this is not an estimate of when 
    /// the cluster will be ready, but rather a recommendation for how frequently the caller should 
    /// poll for a session status update.
    /// </summary>
    _XSAPIIMP const std::chrono::milliseconds& poll_interval() const;

    /// <summary>
    /// Indicates if the cluster is available if or the caller should wait or if the request was aborted.
    /// </summary>
    _XSAPIIMP game_server_fulfillment_state fulfillment_state() const;

    /// <summary>
    /// The host name of the cluster.
    /// </summary>
    _XSAPIIMP const string_t& host_name() const;

    /// <summary>
    /// The region of the cluster.
    /// </summary>
    _XSAPIIMP const string_t& region() const;

    /// <summary>
    /// The port mappings of the cluster.
    /// </summary>
    _XSAPIIMP const std::vector< game_server_port_mapping >& port_mappings() const;

    /// <summary>
    /// The secure context which is the secure device address of the cluster
    /// </summary>
    _XSAPIIMP const string_t& secure_device_address() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<cluster_result> _Deserialize(_In_ const web::json::value& json);

private:
    static game_server_fulfillment_state convert_string_to_fulfillment_state(_In_ const string_t& value);

    std::chrono::milliseconds m_pollInterval;
    game_server_fulfillment_state m_fulfillmentState;
    string_t m_secureDeviceAddress;
    string_t m_hostName;
    string_t m_region;
    std::vector< game_server_port_mapping > m_portMappings;
};

/// <summary>
/// Represents status information for the allocation request previously sent to the game server.
/// </summary>
class game_server_ticket_status
{
    // Example:
    // {
    //    "ticketId": "12341234-1234-1234-1234-123412341234",
    //    "clusterId": "12341234-1234-1234-1234-123412341234",
    //    "titleId": "1234",
    //    "hostName": "cloudapp.net",
    //    "status": "Active",
    //    "description": "",
    //    "secureContext": "",
    //    "portMappings": {},
    //    "gameHostId": "abc.Cloud.Gaming.Bootstrap_IN_0.0"
    //    "region": "Central US"
    // }

public:
    /// <summary>
    /// Internal function
    /// </summary>
    game_server_ticket_status();

    /// <summary>
    /// Internal function
    /// </summary>
    game_server_ticket_status(
        _In_ string_t ticket_id,
        _In_ string_t cluster_id,
        _In_ uint32_t title_id,
        _In_ string_t host_name,
        _In_ game_server_host_status status,
        _In_ string_t description,
        _In_ string_t secure_context,
        _In_ std::vector< game_server_port_mapping > port_mappings,
        _In_ string_t game_host_id,
        _In_ string_t region
        );

    /// <summary>
    /// The ID of the ticket whose status is being checked.
    /// </summary>
    _XSAPIIMP const string_t& ticket_id() const;

    /// <summary>
    /// The cluster ID of the server allocated for this ticket.
    /// </summary>
    _XSAPIIMP const string_t& cluster_id() const; 

    /// <summary>
    /// The title ID of the requesting title.
    /// </summary>
    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// The host name of the server which was allocated to support this request.
    /// </summary>
    _XSAPIIMP const string_t& host_name() const;

    /// <summary>
    /// The status of the server.
    /// </summary>
    _XSAPIIMP game_server_host_status status() const;

    /// <summary>
    /// Description of the allocation request.
    /// </summary>
    _XSAPIIMP const string_t& description() const;

    /// <summary>
    /// The secure device address information for the assigned server.
    /// </summary>
    _XSAPIIMP const string_t& secure_context() const;

    /// <summary>
    /// The list of GameServerPortMapping for this server.
    /// </summary>
    _XSAPIIMP std::vector< game_server_port_mapping > port_mappings() const;

    /// <summary>
    /// The ID of the game host.
    /// </summary>
    _XSAPIIMP const string_t& game_host_id() const;

    /// <summary>
    /// The region of the game host.
    /// </summary>
    _XSAPIIMP const string_t& region() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<game_server_ticket_status> _Deserialize(_In_ const web::json::value& json);
    
private:
    static game_server_host_status convert_string_to_host_status(_In_ const string_t& value);

    string_t m_ticketId;
    string_t m_clusterId;
    uint32_t m_titleId;
    string_t m_hostName;
    game_server_host_status m_status;
    string_t m_description;
    string_t m_secureContext;
    std::vector< game_server_port_mapping > m_portMappings;
    string_t m_gameHostId;
    string_t m_region;
};

/// <summary>
/// Represents a game variant schema.
/// </summary>
class game_variant_schema
{
    // Example:
    //        {
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //            "schemaContent":"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n <xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\r\n<xs:element name=\"testSchema\">\r\n </xs:element> \r\n </xs:schema> ",
    //            "name":"Example Variant"
    //        }

public:
    /// <summary>
    /// Internal function
    /// </summary>
    game_variant_schema();

    /// <summary>
    /// Internal function
    /// </summary>
    game_variant_schema(
        _In_ string_t schema_id,
        _In_ string_t schema_content,
        _In_ string_t schema_name
        );

    /// <summary>
    /// ID of a schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_id() const;

    /// <summary>
    /// Content of the schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_content() const;

    /// <summary>
    /// Name of the schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_name() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<game_variant_schema> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_schemaId;
    string_t m_schemaContent;
    string_t m_schemaName;
};

/// <summary>
/// Represents a game variant.
/// </summary>
class game_variant
{
    // Example:
    //        {
    //            "gamevariantId":"041aae97-d359-4244-9b93-2d23ba27cd19",
    //            "name":"Example Variant",
    //            "rank":"100",
    //            "isPublisher":"False"
    //            "gameVariantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //        }

public:
    /// <summary>
    /// Internal function
    /// </summary>
    game_variant();

    /// <summary>
    /// Internal function
    /// </summary>
    game_variant(
        _In_ string_t id,
        _In_ string_t name,
        _In_ bool isPublisher,
        _In_ uint64_t rank,
        _In_ string_t schemaId
        );

    /// <summary>
    /// ID of a variant.
    /// </summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>
    /// Name of variant.
    /// </summary>
    _XSAPIIMP const string_t& name() const;

    /// <summary>
    /// A boolean field to indicate whether the variant was user-created or publisher-created.
    /// </summary>
    _XSAPIIMP bool is_publisher() const;

    /// <summary>
    /// A boolean field to indicate whether the variant was user-created or publisher-created.
    /// </summary>
    _XSAPIIMP uint64_t rank() const;

    /// <summary>
    /// Content of the variant's schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_content() const;

    /// <summary>
    /// Name of the variant's schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_name() const;

    /// <summary>
    /// ID of the variant's schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<game_variant> _Deserialize(_In_ const web::json::value& json);

    void _Set_schema(_In_ game_variant_schema schema);

private:
    string_t m_id;
    string_t m_name;
    bool m_isPublisher;
    uint64_t m_rank;
    game_variant_schema m_schema;
    string_t m_schemaId;
};

/// <summary>
/// Represents a set of game server images.
/// </summary>
class game_server_image_set
{
    // Example:
    //        {
    //            "gsiSetId":"7efdf3a7-9ce4-4d28-b889-4aeea98727c1",
    //            "minRequiredPlayers":"2",
    //            "maxAllowedPlayers":"50",
    //            "selectionOrder":"1",
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806"
    //            "tags": 
    //            [
    //                {
    //                    "name": "tagName1",
    //                    "value": "tagValue1"
    //                },
    //                {
    //                    "name": "tagName2",
    //                    "value": "tagValue2"
    //                }
    //            ]
    //        }

public:
    /// <summary>
    /// Internal function
    /// </summary>
    game_server_image_set();

    /// <summary>
    /// Internal function
    /// </summary>
    game_server_image_set(
        _In_ uint64_t minPlayers,
        _In_ uint64_t maxPlayers,
        _In_ string_t id,
        _In_ string_t name,
        _In_ uint64_t selectionOrder,
        _In_ string_t schemaId,
        _In_ std::map<string_t, string_t> tags
        );

    /// <summary>
    /// The minimum number of players for the variant.
    /// </summary>
    _XSAPIIMP const uint64_t min_players() const;

    /// <summary>
    /// The maximum number of players for the variant.
    /// </summary>
    _XSAPIIMP uint64_t max_players() const;

    /// <summary>
    /// The set ID of the variant.
    /// </summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>
    /// The set Name of the variant.
    /// </summary>
    _XSAPIIMP const string_t& name() const;

    /// <summary>
    /// Selection order for this set.
    /// </summary>
    _XSAPIIMP uint64_t selection_order() const;

    /// <summary>
    /// Content of this set's schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_content() const;

    /// <summary>
    /// Name of this set's schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_name() const;

    /// <summary>
    /// ID of this set's schema.
    /// </summary>
    _XSAPIIMP const string_t& schema_id() const;

    /// <summary>
    /// Tags are key value metadata on game variant that can be set at the time of ingestion and then use them as filters or additional metadata.
    /// </summary>
    _XSAPIIMP const std::map<string_t, string_t>& tags() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<game_server_image_set> _Deserialize(_In_ const web::json::value& json);

    void _Set_schema(_In_ game_variant_schema schema);

private:
    uint64_t m_minPlayers;
    uint64_t m_maxPlayers;
    string_t m_id;
    string_t m_name;
    uint64_t m_selectionOrder;
    string_t m_schemaId;
    game_variant_schema m_schema;
    std::map<string_t, string_t> m_tags;
};

/// <summary>
/// Represents information on game server image sets and game variants.
/// </summary>
class game_server_metadata_result
{
    // Example:
    //{
    //    "variants" :
    //    [
    //        {
    //            "gamevariantId":"041aae97-d359-4244-9b93-2d23ba27cd19",
    //            "name":"Example Variant",
    //            "rank":"100",
    //            "isPublisher":"False"
    //            "gameVariantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //        }
    //    ],
    //
    //    "variantSchemas" :
    //    [
    //        {
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //            "schemaContent":"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n <xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\r\n<xs:element name=\"testSchema\">\r\n </xs:element> \r\n </xs:schema> ",
    //            "name":"Example Variant"
    //        }
    //    ],
    //
    //    "gsiSets" :
    //    [
    //        {
    //            "gsiSetId":"7efdf3a7-9ce4-4d28-b889-4aeea98727c1",
    //            "minRequiredPlayers":"2",
    //            "maxAllowedPlayers":"50",
    //            "selectionOrder":"1",
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806"
    //            "tags": 
    //            [
    //                {
    //                    "name": "tagName1",
    //                    "value": "tagValue1"
    //                },
    //                {
    //                    "name": "tagName2",
    //                    "value": "tagValue2"
    //                }
    //            ]
    //        }
    //    ]
    //}

public:
    /// <summary>
    /// Internal function
    /// </summary>
    game_server_metadata_result();

    /// <summary>
    /// Internal function
    /// </summary>
    game_server_metadata_result(
        _In_ std::vector< game_variant > gameVariants,
        _In_ std::vector< game_variant_schema > gameVariantSchemas,
        _In_ std::vector< game_server_image_set > gameServerImageSets
        );

    /// <summary>
    /// The collection of game variants. 
    /// </summary>
    _XSAPIIMP const std::vector< game_variant >& game_variants();

    /// <summary>
    /// The collection of game server image sets.
    /// </summary>        
    _XSAPIIMP const std::vector< game_server_image_set >& game_server_image_sets() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<game_server_metadata_result> _Deserialize(_In_ const web::json::value& json);

private:
    std::vector< game_variant > m_gameVariants;
    std::vector< game_variant_schema > m_gameVariantSchemas;
    std::vector< game_server_image_set > m_gameServerImageSets;
};

/// <summary>
/// Represents the server used to measure network quality of service (QoS) for the session.
/// </summary>
class quality_of_service_server
{
    // Example:
    //
    // {
    //      "qosServers" :
    //      [
    //          {
    //              "serverFqdn":"microsoft.com",
    //              "serverSecureDeviceAddress":"r5Y=",
    //              "targetLocation":"North Central US"
    //          },
    //          {
    //              "serverFqdn":"microsoft.com",
    //              "serverSecureDeviceAddress":"r5Y=",
    //              "targetLocation":"West US"
    //          }
    //      ]
    //  }

public:
    /// <summary>
    /// Internal function
    /// </summary>
    quality_of_service_server();

    /// <summary>
    /// Internal function
    /// </summary>
    quality_of_service_server(
        _In_ string_t serverFullQualifiedDomainName,
        _In_ string_t secureDeviceAddressBase64,
        _In_ string_t targetLocation
        );

    /// <summary>
    /// The full qualified domain name of the server.
    /// </summary>
    _XSAPIIMP const string_t& server_full_qualified_domain_name() const;

    /// <summary>
    /// The secure device address in base64 of the server.
    /// </summary>
    _XSAPIIMP const string_t& secure_device_address_base64() const;

    /// <summary>
    /// The target location.
    /// </summary>
    _XSAPIIMP const string_t& target_location() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<quality_of_service_server> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_serverFullQualifiedDomainName;
    string_t m_secureDeviceAddressBase64;
    string_t m_targetLocation;
};


class allocation_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    allocation_result();

    /// <summary>
    /// Internal function
    /// </summary>
    allocation_result(
        _In_ game_server_fulfillment_state fulfillmentState,
        _In_ string_t hostName,
        _In_ string_t sessionHostId,
        _In_ string_t region,
        _In_ std::vector< game_server_port_mapping > portMappings,
        _In_ string_t secureDeviceAddress
        );

    /// <summary>
    /// Indicates if the allocation is available 
    /// </summary>
    _XSAPIIMP game_server_fulfillment_state fulfillment_state() const;

    /// <summary>
    /// The host name of the session host
    /// </summary>
    _XSAPIIMP const string_t& host_name() const;

    /// <summary>
    /// The ID of the session host.  It is responsible for hosting the session. 
    /// A session host can only host one session at a time, but over its life time, will host several sessions.   
    /// </summary>
    _XSAPIIMP const string_t& session_host_id() const;

    /// <summary>
    /// The region of the session host
    /// </summary>
    _XSAPIIMP const string_t& region() const;

    /// <summary>
    /// The port mappings of the session host
    /// </summary>
    _XSAPIIMP const std::vector<game_server_port_mapping>& port_mappings() const;

    /// <summary>
    /// The secure device address of the session host
    /// </summary>
    _XSAPIIMP const string_t& secure_device_address() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<allocation_result> _Deserialize(_In_ const web::json::value& json);

private:
    static game_server_fulfillment_state convert_string_to_fulfillment_state(_In_ const string_t& value);

    game_server_fulfillment_state m_fulfillmentState;
    string_t m_hostName;
    string_t m_sessionHostId;
    string_t m_region;
    std::vector< game_server_port_mapping > m_portMappings;
    string_t m_secureDeviceAddress;
};


class game_server_platform_service
{
public:
    /// <summary>
    /// Allocates a new Game Server Cluster using an existing Game Server Image.
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server.</param>
    /// <param name="serviceConfigurationId">Service configuration ID (SCID).  This can come from the MatchTicket.</param>
    /// <param name="sessionTemplateName">Session template name.  This can come from the MatchTicket.</param>
    /// <param name="sessionName">Session name.  This can come from the MatchTicket.</param>
    /// <param name="abortIfQueued">Indicates if the allocate should aborted if the operation is queued.</param>
    /// <returns>ClusterResult object</returns>
    /// <remarks>Calls V1 POST /titles/{gameServerTitleId}/clusters.</remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<cluster_result>> allocate_cluster(
        _In_ uint32_t gameServerTitleId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sessionTemplateName,
        _In_ const string_t& sessionName,
        _In_ bool abortIfQueued
        );

    /// <summary>
    /// Allocates a new Game Server Cluster using an existing Game Server Image, without interacting with MPSD.
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server.</param>
    /// <param name="serviceConfigurationId">Service configuration ID (SCID).</param>
    /// <param name="sandboxId">The sandbox you wish the cluster to be allocated in.</param>
    /// <param name="ticketId">Unique id for this request.  The ticket id should be the same for all members of the group.</param>
    /// <param name="gsiSetId">The GSI Set id to use for this request.  Comes from GetGameServerMetadataAsync.</param>
    /// <param name="gameVariantId">Which game variant to use.  Comes from GetGameServerMetadataAsync.</param>
    /// <param name="maxAllowedPlayers">The maximum players allowed by this variant.</param>
    /// <param name="location">Which datacenter to use for allocation.  This information should come from QoS probes.</param>
    /// <param name="abortIfQueued">Indicates if the allocate should aborted if the operation is queued.</param>
    /// <returns>ClusterResult object</returns>
    /// <remarks>Calls V2 POST /titles/{gameServerTitleId}/inlineclusters.</remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<cluster_result>> allocate_cluster_inline(
        _In_ uint32_t gameServerTitleId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sandboxId,
        _In_ const string_t& ticketId,
        _In_ const string_t& gsiSetId,
        _In_ const string_t& gameVariantId,
        _In_ uint64_t maxAllowedPlayers,
        _In_ const string_t& location,
        _In_ bool abortIfQueued
        );

    /// <summary>
    /// Allocates a new Game Server Cluster using an existing Game Server Image, without interacting with MPSD.
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server.</param>
    /// <param name="ticketId">Unique id for this request.  The ticket id should be the same for all members of the group.</param>
    /// <returns>GameServerTicketStatus object</returns>
    /// <remarks>Calls V6 GET titles/{titleId}/tickets/{ticketId}/status.</remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<game_server_ticket_status>> get_ticket_status(
        _In_ uint32_t gameServerTitleId,
        _In_ const string_t& ticketId
        );

    /// <summary>
    /// Retrieves a list of game variants for the specified title Id. 
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="maxAllowedPlayers">The maximum players the variant allows.</param>
    /// <param name="publisherOnly">Indicates whether to filter on publisher-created variants; otherwise user-created variants.</param>
    /// <param name="maxVariants">The maximum number of variants to return.</param>
    /// <param name="locale">The locale of variants to return. (Optional)</param>
    /// <param name="filterTags">Tags are key value metadata on game variant, that a game developer sets at the time of ingestion and then use it as filters or 
    /// additional metadata.  The developer can use this value to filter which game variants they want to see. (Optional)</param>
    /// <returns>GameServerMetadataResult object</returns>
    /// <remarks>Calls V1 POST /titles/{titleId}/variants.</remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<game_server_metadata_result>> get_game_server_metadata(
        _In_ uint32_t titleId,
        _In_ uint32_t maxAllowedPlayers,
        _In_ bool publisherOnly,
        _In_ uint32_t maxVariants,
        _In_ const std::map<string_t, string_t>& filterTags
        );

    /// <summary>
    /// Retrieves a collection of Quality of Service (QoS) servers. 
    /// </summary>
    /// <returns>A collection of QualityOfServiceServer object</returns>
    /// <remarks>Calls V1 GET /qosservers/ </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<quality_of_service_server>>> get_quality_of_service_servers();

    /// <summary>
    /// Allocates a new session host
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server</param>
    /// <param name="locations">The ordered list of preferred location you wish the session host to be allocated from.</param>
    /// <param name="sessionId">This is the caller specified identifier.It is assigned to the session host that is allocated and returned.Later on you can reference the specific sessionhost by this identifier.It must be globally unique(i.e.GUID).</param>
    /// <param name="cloudGameId">The cloud game identifier (GUID), otherwise known as the GSI Set ID.</param>
    /// <param name="gameModeId">The game mode identifier otherwise known as game variant IDs.</param>
    /// <param name="sessionCookie">This is a caller specified opaque string.It too is assign to the sessionhost and can be referenced in your game code.Use this to pass a small amount of information from the client to the server.Max size is 32KB</param>
    /// <returns>allocation_result object</returns>
    /// <remarks>Calls V8 POST /titles/{gameServerTitleId}/allocationStatus</remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<allocation_result>> allocate_session_host(
        _In_ uint32_t gameServerTitleId,
        _In_ const std::vector<string_t>& locations,
        _In_ const string_t& sessionId,
        _In_ const string_t& cloudGameId,
        _In_opt_ const string_t& gameModeId,
        _In_opt_ const string_t& sessionCookie
        );

    /// <summary>
    /// Get allocation status for a previous allocate session host request
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server</param>
    /// <param name="sessionId">This is the caller specified identifier.It is assigned to the session host that is allocated and returned.Later on you can reference the specific sessionhost by this identifier.It must be globally unique(i.e.GUID).</param>
    /// <returns>allocation_result object</returns>
    /// <remarks>Calls V8 POST /titles/{gameServerTitleId}/sessionhosts</remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<allocation_result>> get_session_host_allocation_status(
        _In_ uint32_t gameServerTitleId,
        _In_ const string_t& sessionId
        );

private:
    game_server_platform_service() {}

    game_server_platform_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    static string_t pathandquery_game_server_create_cluster_subpath(
        _In_ uint32_t gameServerTitleId,
        _In_ bool inlineAlloc
        );

    static string_t pathandquery_game_server_gamevariant_list_subpath(
        _In_ uint32_t titleId
        );

    static string_t pathandquery_game_server_ticket_status_subpath(
        _In_ uint32_t titleId,
        _In_ const string_t& ticketId
        );

    static string_t game_server_session_id_path(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sessionTemplateName,
        _In_ const string_t& sessionName
        );

    static string_t pathandquery_game_server_allocate_session_host_subpath(
        _In_ uint32_t titleId
        );

    static string_t pathandquery_game_server_allocation_status_subpath(
        _In_ uint32_t titleId,
        _In_ const string_t& sessionId
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend xbox_live_context_impl;
};

}}}
