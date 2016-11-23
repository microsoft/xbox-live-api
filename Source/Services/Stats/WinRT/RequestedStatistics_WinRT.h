//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

/// <summary>
/// Contains requested statistics.
/// </summary>
public ref class RequestedStatistics sealed
{
public:
    /// <summary>
    /// Constructor for a RequestedStatistics object.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID to use.</param>
    /// <param name="statistics">A collection of statistics.</param>
    RequestedStatistics( 
        Platform::String^ serviceConfigurationId,
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ statistics
        );

    /// <summary>
    /// The service configuration ID in use.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// A collection of statistics.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Statistics
    { 
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); 
    }

internal:
    const xbox::services::user_statistics::requested_statistics& GetCppObj() const;

private:
    xbox::services::user_statistics::requested_statistics m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_statistics;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END