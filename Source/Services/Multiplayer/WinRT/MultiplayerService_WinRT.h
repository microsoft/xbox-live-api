// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "xsapi/xbox_live_context_settings.h"
#include "Macros_WinRT.h"
#include "MultiplayerSessionStates_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MultiplayerSession_WinRT.h"
#include "MultiplayerSessionVisibility_WinRT.h"
#include "MultiplayerSessionWriteMode_WinRT.h"
#include "MultiplayerActivityDetails_WinRT.h"
#include "MultiplayerSessionChangeEventArgs_WinRT.h"
#include "MultiplayerGetSessionsRequest_WinRT.h"
#include "MultiplayerQuerySearchHandleRequest_WinRT.h"
#include "MultiplayerSearchHandleRequest_WinRT.h"
#include "MultiplayerSearchHandleDetails_WinRT.h"
#include "WriteSessionResult_WinRT.h"
#include "MultiplayerSubscriptionLostEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

ref class MultiplayerService;

class MultiplayerSessionChangeEventBind : public std::enable_shared_from_this<MultiplayerSessionChangeEventBind>
{
public:
    MultiplayerSessionChangeEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::multiplayer::multiplayer_service& cppObj
        );

    void AddMultiplayerSessionChangeEvent();

    void RemoveMultiplayerSessionChangeEvent(
        _In_ xbox::services::multiplayer::multiplayer_service& cppObj
        );

    void MultiplayerSessionChangeRouter(_In_ const xbox::services::multiplayer::multiplayer_session_change_event_args& sessionChangeEventArgs);

private:
    function_context m_functionContext;
    xbox::services::multiplayer::multiplayer_service m_cppObj;
    Platform::WeakReference m_setting;
};

class MultiplayerSubscriptionLostEventBind : public std::enable_shared_from_this<MultiplayerSubscriptionLostEventBind>
{
public:
    MultiplayerSubscriptionLostEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::multiplayer::multiplayer_service& cppObj
        );

    void AddMultiplayerSubscriptionLostEvent();

    void RemoveMultiplayerSubscriptionLostEvent(
        _In_ xbox::services::multiplayer::multiplayer_service& cppObj
        );

    void MultiplayerSubscriptionLostRouter();

private:
    function_context m_functionContext;
    xbox::services::multiplayer::multiplayer_service m_cppObj;
    Platform::WeakReference m_setting;
};

/// <summary>
/// Handles interactions with an Xbox Live service endpoint on a server.
/// </summary>
public ref class MultiplayerService sealed
{
public:
    /// <summary>
    /// Registers for multiplayer session change notifications.  Event handlers receive MultiplayerSessionChangeEventArgs^.
    /// </summary>
    event Windows::Foundation::EventHandler<MultiplayerSessionChangeEventArgs^>^ MultiplayerSessionChanged;

    /// <summary>
    /// Notification sent when a multiplayer connectivity via RTA is lost or stopped.
    /// Event handlers receive Windows::Networking::Sockets::RealTimeActivityMultiplayerSubscriptionsLostEventArgs^.
    /// </summary>
    event Windows::Foundation::EventHandler<MultiplayerSubscriptionLostEventArgs^>^ MultiplayerSubscriptionLost;

    /// <summary>
    /// Writes a new or updated multiplayer session to the multiplayer service.
    /// </summary>
    /// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
    /// <param name="multiplayerSessionWriteMode">An enumeration value indicating the type of write operation.</param>
    /// <returns>Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a MultiplayerSession object containing the response returned from the server.
    /// The attributes in it might be different from the attributes in the passed-in MultiplayerSession object. </returns>
    /// <remarks>Calls V102 PUT /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}.
    /// 
    /// If your client needs to pass a handle ID when writing the session, it should call the MultiplayerService.WriteSessionByHandleAsync Method.
    ///
    /// If the client calls the MultiplayerSession.Leave Method for the last member of the session, when the session is 
    /// using a sessionEmptyTimeout value of 0, the session is deleted immediately. After the call to MultiplayerSession.Leave, 
    /// the client calls WriteSessionAsync to write local changes. This call returns a null pointer with an HTTP/204 status code, 
    /// since the session has been deleted. 
    /// </remarks>
    Windows::Foundation::IAsyncOperation<MultiplayerSession^>^ WriteSessionAsync(
        _In_ MultiplayerSession^ multiplayerSession,
        _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode
        );

    /// <summary>
    /// Attempts to write a new or updated multiplayer session to the service.
    /// The passed multiplayerSession must have a valid MultiplayerSessionReference set on it. The purpose of this method is to give the caller
    /// additional details on the failure or success of the operation. 
    ///
    /// This method returns a WriteSessionResult instead of throwing an exception, but only in cases where the write failed for a reason that is expected to occur 
    /// during normal operation. 
    /// </summary>
    /// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
    /// <param name="multiplayerSessionWriteMode">An enumeration value indicating the type of write operation.</param>
    /// <returns>Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    /// 
    /// The result is an object representing a WriteSessionResult object that contains the response returned from the server, 
    /// a status with the result, as well as if the status was successful. The attributes in it may be different
    /// from the attributes in the passed in MultiplayerSessionRequest object. 
    /// </returns>
    /// <remarks>If you leave a session that you are the last member of, and the sessionEmptyTimeout is equal to 0, then the session is deleted immediately
    /// and a nullptr is returned. 
    ///
    /// Calls V105 PUT /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}.</remarks>
    Windows::Foundation::IAsyncOperation<WriteSessionResult^>^ TryWriteSessionAsync(
        _In_ MultiplayerSession^ multiplayerSession,
        _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode
        );

    /// <summary>
    /// Writes a new or updated multiplayer session to the service, using the specified handle to the session.  
    /// A handle is a service-side pointer to a session.  The handleid is a GUID identifier of the handle.  Callers will
    /// usually get the handleid either from another player's MultiplayerActivityDetails, or from a protocol
    /// activation after a user has accepted an invite.
    /// </summary>
    /// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
    /// <param name="multiplayerSessionWriteMode">An enumeration value indicating the type of write operation.</param>
    /// <param name="handleId">The ID (GUID) of the handle that should be used when writing the session.</param>
    /// <returns>Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    /// 
    /// The result is an object representing the new or updated MultiplayerSession object that contains the response returned from the server.
    /// The returned MultiplayerSession contains a MultiplayerSessionReference, so it can be used when calling WriteSessionAsync.
    /// </returns>
    /// <remarks>
    /// Use this method only if your MultiplayerSession object doesn't have a MultiplayerSessionReference, as  
    /// a handle's lifetime may be shorter than that of the session it points to.
    ///
    /// Calls V105 PUT /handles/{handleid}/session.</remarks>
    Windows::Foundation::IAsyncOperation<MultiplayerSession^>^ WriteSessionByHandleAsync(
        _In_ MultiplayerSession^ multiplayerSession,
        _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode,
        _In_ Platform::String^ handleId
        );

    /// <summary>
    /// Attempts to write a new or updated multiplayer session to the service, using the specified handle to the session.  
    /// A handle is a service-side pointer to a session.  The handleid is a GUID identifier of the handle.  Callers will
    /// usually get the handleid either from another player's MultiplayerActivityDetails, or from a protocol
    /// activation after a user has accepted an invite.
    /// The purpose of this method is to give the caller additional details on the failure or success. 
    /// 
    /// This method returns a WriteSessionResult instead of throwing an exception, but only in cases where the write failed for a reason that is expected to occur 
    /// during normal operation. 
    /// </summary>
    /// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
    /// <param name="multiplayerSessionWriteMode">An enumeration value indicating the type of write operation.</param>
    /// <returns>Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    /// 
    /// The result is an object representing a WriteSessionResult object that contains the response returned from the server,
    /// a status with the result, as well as if the status was successful. The attributes in it may be different
    /// from the attributes in the passed in MultiplayerSessionRequest object. </returns>
    /// <remarks>
    /// Use this method only if your MultiplayerSession object doesn't have a MultiplayerSessionReference, as  
    /// a handle's lifetime may be shorter than that of the session it points to.
    ///
    /// If you leave a session that you are the last member of, and the sessionEmptyTimeout is equal to 0, then the session is deleted immediately
    /// and a nullptr is returned. 
    ///
    /// Calls V105 PUT /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}</remarks>
    Windows::Foundation::IAsyncOperation<WriteSessionResult^>^ TryWriteSessionByHandleAsync(
        _In_ MultiplayerSession^ multiplayerSession,
        _In_ MultiplayerSessionWriteMode multiplayerSessionWriteMode,
        _In_ Platform::String^ handleId
        );

    /// <summary>
    /// Gets a session object, with all of its attributes, from the server.
    /// </summary>
    /// <param name="sessionReference">A MultiplayerSessionReference object that contains identifying information for the session.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a MultiplayerSession object containing the response returned from the server.
    /// </returns>
    /// <remarks>
    /// If the client tries to get a session that does not exist, this method returns an HTTP/204 status code.
    /// Additionally an exception is thrown for trying to retrieve a nonexistent session. 
    ///
    /// Calls V102 GET /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}</remarks>
    Windows::Foundation::IAsyncOperation<MultiplayerSession^>^ GetCurrentSessionAsync(
        _In_ MultiplayerSessionReference^ sessionReference
        );

    /// <summary>
    /// Gets a session object with all its attributes from the server, given a session handle id.
    /// A handle is a service-side pointer to a session.  The handleid is a GUID identifier of the handle.  Callers will
    /// usually get the handleid either from another player's MultiplayerActivityDetails, or from a protocol
    /// activation after a user has accepted an invite.
    /// </summary>
    /// <param name="handleId">A multiplayer handle id, which uniquely identifies the session.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a MultiplayerSession object that contains the response returned from the server.</returns>
    /// <remarks>
    /// If the client tries to get a session that does not exist, this method returns an HTTP/204 status code.
    /// Additionally an exception is thrown for trying to retrieve a nonexistent session. 
    ///
    /// Calls GET /handles/{handleId}/session</remarks>
    Windows::Foundation::IAsyncOperation<MultiplayerSession^>^ GetCurrentSessionByHandleAsync(
        _In_ Platform::String^ handleId
        );

    /// <summary>
    /// Retrieve a list of sessions with various filters. You can use this method to filter sessions for a single Xbox User ID,
    /// a collection of Xbox User IDs, or for a keyword.
    /// </summary>
    /// <param name="getSessionsRequest">An object that defines the search filters for retrieveing a collection of discoverable sessions.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a collection of SessionStates objects where each object contains metadata about one session.
    /// </returns>
    /// <remarks>
    /// You can use this method to filter sessions for a collection of Xbox User IDs.
    ///
    /// Calls V102 GET /serviceconfigs/{scid}/sessions or /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerSessionStates^>^>^ GetSessionsAsync(
        _In_ MultiplayerGetSessionsRequest^ getSessionsRequest
        );

    /// <summary>
    /// Retrieve a list of sessions with various filters
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration id that contains the sessions.</param>
    /// <param name="sessionTemplateNameFilter">(Optional) The name of the template for the multiplayer session to filter on.</param>
    /// <param name="xboxUserIdFilter">(Optional) An Xbox user ID to use for filtering results to sessions for that user.
    /// You must specify either xboxUserIdFilter or keywordFilter.</param>
    /// <param name="keywordFilter">(Optional) A keyword to use for filtering results to sessions with that keyword.
    /// You must specify either xboxUserIdFilter or keywordFilter.</param>
    /// <param name="visibilityFilter">An enumeration value specifying the visibilty to use for filtering results.</param>
    /// <param name="contractVersionFilter">A value specifying the major version, or less, of the contract to use for filtering results.
    /// Set to 0 to ignore the contract version.</param>
    /// <param name="includePrivateSessions">True to include private sessions in the results, and false otherwise.</param>
    /// <param name="includeReservations">True to include sessions that the user has not accepted in the results, and false otherwise.
    /// For a setting of true, xboxUserIdFilter must be specified.</param>
    /// <param name="includeInactiveSessions">True to include inactive sessions in the results, and false otherwise.
    /// For a setting of true, xboxUserIdFilter must be specified.</param>
    /// <param name="maxItems">The maximum number of sessions to return.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a collection of SessionStates objects where each object contains metadata about one session.
    /// </returns>
    /// <remarks>Calls V105 GET /serviceconfigs/{scid}/sessions or /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions</remarks>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call GetSessionsAsync(MultiplayerGetSessionsRequest^) instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerSessionStates^>^>^ GetSessionsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_opt_ Platform::String^ sessionTemplateNameFilter,
        _In_opt_ Platform::String^ xboxUserIdFilter,
        _In_opt_ Platform::String^ keywordFilter,
        _In_ MultiplayerSessionVisibility visibilityFilter,
        _In_ uint32 contractVersionFilter,
        _In_ bool includePrivateSessions,
        _In_ bool includeReservations,
        _In_ bool includeInactiveSessions,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Retrieve a list of sessions with various filters for multiple users.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration id that contains the sessions.</param>
    /// <param name="sessionTemplateNameFilter">(Optional) The name of the template for the multiplayer session to filter on.</param>
    /// <param name="xboxUserIdsFilter">(Optional) A collection of Xbox user IDs to use for filtering results to sessions containing these Ids.
    /// You must specify either xboxUserIdsFilter or keywordFilter.</param>
    /// <param name="keywordFilter">(Optional) A keyword to use for filtering results to sessions with that keyword.
    /// You must specify either xboxUserIdsFilter or keywordFilter.</param>
    /// <param name="visibilityFilter">An enumeration value specifying the visibilty to use for filtering results.</param>
    /// <param name="contractVersionFilter">A value specifying the major version, or less, of the contract to use for filtering results.
    /// Set to 0 to ignore the contract version.</param>
    /// <param name="includePrivateSessions">True to include private sessions in the results, and false otherwise.</param>
    /// <param name="includeReservations">True to include sessions that the user has not accepted in the results, and false otherwise.
    /// For a setting of true, xboxUserIdFilter must be specified.</param>
    /// <param name="includeInactiveSessions">True to include inactive sessions in the results, and false otherwise.
    /// For a setting of true, xboxUserIdFilter must be specified.</param>
    /// <param name="maxItems">The maximum number of sessions to return.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a collection of SessionStates objects where each object contains metadata about one session.
    /// </returns>
    /// <remarks>Calls V105 POST /serviceconfigs/{scid}/batch or /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/batch</remarks>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call GetSessionsAsync(MultiplayerGetSessionsRequest^) instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerSessionStates^>^>^ GetSessionsForUsersFilterAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_opt_ Platform::String^ sessionTemplateNameFilter,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIdsFilter,
        _In_opt_ Platform::String^ keywordFilter,
        _In_ MultiplayerSessionVisibility visibilityFilter,
        _In_ uint32 contractVersionFilter,
        _In_ bool includePrivateSessions,
        _In_ bool includeReservations,
        _In_ bool includeInactiveSessions,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Sets the passed session as the user's current activity, which will be displayed in Xbox 
    /// dashboard user experiences (e.g. friends and gamercard) as associated with the currently 
    /// running title.  If the session is joinable, it may also be displayed as joinable in those 
    /// user experiences.
    /// </summary>
    /// <param name="sessionReference">A MultiplayerSessionReference for the session of the activity.</param>
    /// <returns>Returns an IAsyncAction object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncAction.Completed property.</returns>
    Windows::Foundation::IAsyncAction^ SetActivityAsync(_In_ MultiplayerSessionReference^ sessionReference);

    /// <summary>
    /// Extends the access rights for the caller from the origin session to the target session.
    /// For example, in a title with a lobby session and a game session, the title could put a transfer handle 
    /// linking the lobby to the game inside the lobby session. Users invited to the lobby can use the handle to join the game session as well.
    /// The transfer handle is deleted once the target session is deleted.
    /// </summary>
    /// <param name="targetSessionReference">Target multiplayer_session_reference for the session you want to extend the access rights to.</param>
    /// <param name="originSessionReference">Origin multiplayer_session_reference for the session that grants access to the target session.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a string that contains the transfer handle ID.</returns>
    Windows::Foundation::IAsyncOperation<Platform::String^>^ SetTransferHandleAsync(
        _In_ MultiplayerSessionReference^ targetSessionReference,
        _In_ MultiplayerSessionReference^ originSessionReference
        );

    /// <summary>
    /// Creates a search handle associated with the session. The visibility of the session is dependent on its search handle. 
    /// While you can create an searchable session, it is not queryable and visible to others unless you have the associated search handle committed as well. 
    /// </summary>
    /// <param name="searchHandleRequest">A search handle request object for the associated session</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    Windows::Foundation::IAsyncAction^ SetSearchHandleAsync(
        _In_ MultiplayerSearchHandleRequest^ searchHandleRequest
        );

    /// <summary>
    /// Clears the user's current activity session for the specified service configuration ID.
    /// </summary>
    /// <param name="serviceConfigurationId">A string containing the service configuration ID in which to clear activity.</param>
    /// <returns>Returns an IAsyncAction object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncAction.Completed property.</returns>
    Windows::Foundation::IAsyncAction^ ClearActivityAsync(_In_ Platform::String^ serviceConfigurationId);

    /// <summary>
    /// Clears the search handle that is associated with the session.
    /// </summary>
    /// <param name="handleId">The handleId associated with the session to clear.</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    Windows::Foundation::IAsyncAction^ ClearSearchHandleAsync(_In_ Platform::String^ handleId);

    /// <summary>
    /// Invites the specified users to a session.
    /// </summary>
    /// <param name="sessionReference">A MultiplayerSessionReference object representing the session the target users will be invited to.</param>
    /// <param name="xboxUserIds">The list of Xbox user IDs to invite.</param>
    /// <param name="titleId">The ID of the title that the invited user will activate in order to join the session.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a vectorview of handle ID strings corresponding to the invites that have been sent.</returns>
    /// <remarks>
    /// Calling this method results in a notification being shown to each invited user.
    /// If a user accepts the notification, then the title is activated for that user.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ SendInvitesAsync(
        _In_ MultiplayerSessionReference^ sessionReference,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ uint32 titleId
        );

    /// <summary>
    /// Invites the specified users to a session.
    /// </summary>
    /// <param name="sessionReference">A MultiplayerSessionReference object representing the session the target users will be invited to.</param>
    /// <param name="xboxUserIds">The list of Xbox user IDs to invite.</param>
    /// <param name="titleId">The ID of the title that the invited user will activate in order to join the session.</param>
    /// <param name="contextStringId">
    /// The ID of the custom invite string that is displayed with the invite notification. The ID must match the ID that is
    /// assigned to the custom invite string in the title's multiplayer service configuration.
    ///
    /// The format of the parameter is "///{id}", where {id} is replaced with the ID of the custom string.
    ///
    /// For example, if the ID of the custom string "Play Capture the Flag" is 1, then you would set this parameter
    /// to "///1" in order to display the "Play Capture the Flag" custom string in the game invite.
    /// 
    /// Pass an empty string if you don't want a custom string added to the invite.
    /// </param>
    /// <param name="activationContext">The custom activation contest.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a vectorview of handle ID strings corresponding to the invites that have been sent.</returns>
    /// <remarks>
    /// Calling this method results in a notification being shown to each invited user. 
    /// If a user accepts the notification, then the title is activated for that user.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ SendInvitesAsync(
        _In_ MultiplayerSessionReference^ sessionReference,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ uint32 titleId,
        _In_ Platform::String^ contextStringId,
        _In_ Platform::String^ activationContext
        );

    /// <summary>
    /// Gets the current activity for a social group of users associated with a user that is designated as
    /// the owner of the session.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID to query for activities.</param>
    /// <param name="socialGroupOwnerXboxUserId">The Xbox user ID of the person whose social group will be used for the query.</param>
    /// <param name="socialGroup">The social group ID to use in order to get the list of users. (e.g. "friends" or "favorites").</param>
    /// <returns>The async object for notifying when the operation is completed.  
    /// This contains a vectorview of MultiplayerActivityDetails objects, containing the details of the activities of the targeted users.</returns>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a vectorview of MultiplayerActivityDetails objects, containing the details of the activities for the social group of users.</returns>
    /// <remarks>
    /// There is no paging or continuation, and the multiplayer service limit the number of items returned to 100.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerActivityDetails^>^>^ GetActivitiesForSocialGroupAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ socialGroupOwnerXboxUserId,
        _In_ Platform::String^ socialGroup
        );

    /// <summary>
    /// Gets the current activity for a set of users specified by Xbox user IDs.  
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID to query for activities.</param>
    /// <param name="xboxUserIds">The list of Xbox user IDs to find activities for.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the IAsyncOperation&lt;TResult&gt;.Completed property.
    /// When the asynchronous operation is complete, the result of the operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// The result is a vectorview of MultiplayerActivityDetails objects, containing the details of the activities for the specified users.</returns>
    /// <remarks>
    /// There is no paging or continuation, and the multiplayer service limit the number of items returned to 100.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerActivityDetails^>^>^ GetActivitiesForUsersAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
        );

    /// <summary>
    /// Queries for the all search handles that references the searchable sessions given the specific query.
    /// There is no paging or continuation, and the multiplayer service will limit the number of items returned to 100.
    /// Call GetSearchHandlesAsync(MultiplayerQuerySearchHandleRequest^ ) instead.
    /// </summary>
    /// <param name="serviceConfigurationId">The scid within which to query for search handles.</param>
    /// <param name="sessionTemplateName">The name of the template to query for search handles.</param>
    /// <param name="orderBy">This specifies the attribute to sort the search handles by.  Pass empty string to default to ordering by 'Timestamp asc' </param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="searchFilter">The query string to get the search handles for.
    /// The search query.
    /// The query syntax is an OData like syntax with only the following operators supported EQ, NE, GE, GT, LE and LT along with the logical operators of AND and OR.
    ///
    /// Example 1:
    /// To search for search handles for a specific XboxUserId use
    ///     "MemberXuids/any(d:d eq '12345678')" or "OwnerXuids/any(d:d eq '12345678')"
    ///
    /// Example 2:
    /// To search for search handles for a title defined string metadata use
    ///     "Strings/stringMetadataType eq 'value'"
    ///
    /// Example 3:
    /// To search for search handles for a title defined numbers metadata AND a tag type value use
    ///     "Numbers/numberMetadataType eq 53 AND Tags/tagType eq 'value'"
    /// </param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of MultiplayerSearchHandleDetails objects, containing the details of the search handles.</returns>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerSearchHandleDetails^>^>^ GetSearchHandlesAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ sessionTemplateName,
        _In_ Platform::String^ orderBy,
        _In_ bool orderAscending,
        _In_ Platform::String^ searchFilter
        );

    /// <summary>
    /// Queries for the all search handles that references the searchable sessions given the specific query.
    /// There is no paging or continuation, and the multiplayer service will limit the number of items returned to 100.
    /// </summary>
    /// <param name="searchHandleRequest"> A search handle request object that queries for the all search handles.</param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of MultiplayerSearchHandleDetails objects, containing the details of the search handles.</returns>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerSearchHandleDetails^>^>^ GetSearchHandlesAsync(
        _In_ MultiplayerQuerySearchHandleRequest^ searchHandleRequest
        );

    /// <summary>
    /// Starts multiplayerservice connectivity via Real Time Activity (RTA), for two purposes:
    /// 1. Subscriptions to changes on specific sessions, using the MultiplayerSession object.
    /// 2. Automatic removal of members from sessions when the collection underlying this multiplayer subscription is broken.
    ///
    /// This method does not actually make the connection, but enables the connection, and helps track its lifetime.
    /// </summary>
    /// <remarks>
    /// This method fails if called twice, unless the multiplayerconnection has been lost or stopped in the interim.  
    /// This can be detected by listening for the MultiplayerSubscriptionsLost event.
    /// </remarks>
    void EnableMultiplayerSubscriptions();

    /// <summary>
    /// Stops multiplayerservice connectivity via Real Time Activity (RTA).
    /// </summary>
    /// <remarks>
    /// When stopping multiplayer is complete, a MultiplayerSubscriptionsLost event is fired.
    /// It is not necessary to wait for this event to fire, unless you intend to call EnableMultiplayerSubscriptions() to restart it,
    /// in which case you must wait for the stop to complete.
    /// </remarks>
    void DisableMultiplayerSubscriptions();

    /// <summary>
    /// Indicates whether multiplayer subscriptions are currently enabled.  
    /// </summary>
    DEFINE_PROP_GET_OBJ(MultiplayerSubscriptionsEnabled, subscriptions_enabled, bool);

internal:
    MultiplayerService(
        _In_ xbox::services::multiplayer::multiplayer_service cppObj
        );

    void RaiseMultiplayerSessionChange(_In_ MultiplayerSessionChangeEventArgs^ args);

    void RaiseMultiplayerSubscriptionLost(_In_ MultiplayerSubscriptionLostEventArgs^ args);

private:
    ~MultiplayerService();

    WriteSessionResult^ TryWriteSessionHelper(_In_ xbox::services::xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> multiplayerSession);
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplayerSessionStates^>^>^ GetSessionsHelper(
        _In_ Platform::String^ serviceConfigurationId,
        _In_opt_ Platform::String^ sessionTemplateNameFilter,
        _In_opt_ Platform::String^ xboxUserIdFilter,
        _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIdsFilter,
        _In_opt_ Platform::String^ keywordFilter,
        _In_ MultiplayerSessionVisibility visibilityFilter,
        _In_ uint32 contractVersionFilter,
        _In_ bool includePrivateSessions,
        _In_ bool includeReservations,
        _In_ bool includeInactiveSessions,
        _In_ uint32 maxItems
        );

    xbox::services::multiplayer::multiplayer_service m_cppObj;
    std::shared_ptr<MultiplayerSessionChangeEventBind> m_sessionChangeEventBind;
    std::shared_ptr<MultiplayerSubscriptionLostEventBind> m_subscriptionLostEventBind;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END