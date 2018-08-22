// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <system_error>

#ifndef _NOEXCEPT
#define _NOEXCEPT noexcept
#endif

namespace xbox {
    /// <summary>
    /// Contains classes, enumerations, and namespaces used to indicate error conditions for Xbox Live service components.
    /// </summary>
    namespace services {
            /// <summary>
            /// Enumeration values that define the Xbox Live API error conditions.
            /// </summary>
            /// <remarks>
            /// A best practice is to test the returned std::error_code against these error conditions.
            /// For more detail about std::error_code vs std::error_condition, see 
            /// http://en.cppreference.com/w/cpp/error/error_condition 
            /// </remarks>
            /// <example>
            /// For example:
            /// <code>
            /// if( result.err() == xbox::services::xbox_live_error_condition::auth )
            /// { 
            ///     // ...
            /// } 
            /// </code>
            /// or
            /// <code>
            /// switch (result.err().default_error_condition().value())
            /// {
            ///   case xbox::services::xbox_live_error_condition::auth:
            ///     // ...
            ///     break;
            /// }
            /// </code>
            /// </example>
            enum class xbox_live_error_condition
            {
                /// <summary>
                /// No error.
                /// </summary>
                no_error = 0,

                /// <summary>
                /// A generic error condition.
                /// </summary>
                generic_error,

                /// <summary>
                /// An error condition related to an object being out of range.
                /// </summary>
                generic_out_of_range,

                /// <summary>
                /// An error condition related to attempting to authenticate.
                /// </summary>
                auth,

                /// <summary>
                /// An error condition related to network connectivity.
                /// </summary>
                network,

                /// <summary>
                /// An error condition related to an HTTP method call.
                /// </summary>
                http,

                /// <summary>
                /// The requested resource was not found.
                /// </summary>
                http_404_not_found,

                /// <summary>
                /// The precondition given in one or more of the request-header fields evaluated
                /// to false when it was tested on the server.
                /// </summary>
                http_412_precondition_failed,

                /// <summary>
                /// Client is sending too many requests
                /// </summary>
                http_429_too_many_requests,

                /// <summary>
                /// The service timed out while attempting to process the request.
                /// </summary>
                http_service_timeout,
                
                /// <summary>
                /// An error related to real time activity.
                /// </summary>
                rta
            };

            /// <summary>
            /// These are XSAPI specific error codes.
            /// Only the HTTP codes and errors that are commonly seen when calling Xbox LIVE are called out with guidance about the cause.
            /// The best practice to test for and react to using the xbox_live_error_condition enum instead of these error codes.
            /// </summary>
            enum class xbox_live_error_code
            {
                /// <summary>
                /// <b>0</b>
                /// No error
                /// </summary>
                no_error = 0,

                //////////////////////////////////////////////////////////////////////////
                // HTTP errors
                //////////////////////////////////////////////////////////////////////////

                /// <summary>
                /// The 204 response indicates that the content data was not found.
                /// This code is returned when you are trying to access or write to a session that has been deleted.
                /// </summary>
                http_status_204_resource_data_not_found = 204,

                /// <summary>
                /// <b>0x8019012C</b>
                /// The 300 response indicates there are multiple choices.  Not returned by Xbox Live services
                /// </summary>
                http_status_300_multiple_choices = 300,

                /// <summary>
                /// <b>0x8019012D</b>
                /// The 301 response indicates that the request should be directed at a new URI
                /// </summary>
                http_status_301_moved_permanently = 301,

                /// <summary>
                /// <b>0x8019012E</b>
                /// The 302 response indicates found.
                /// </summary>
                http_status_302_found = 302,

                /// <summary>
                /// <b>0x8019012F</b>
                /// The 303 response indicates see other.  Not returned by Xbox Live services
                /// </summary>
                http_status_303_see_other = 303,

                /// <summary>
                /// <b>0x80190130</b>
                /// The 304 response indicates resource has not been modified.
                /// </summary>
                http_status_304_not_modified = 304,

                /// <summary>
                /// <b>0x80190131</b>
                /// The 305 response indicates you must a use proxy.  Not typically returned by Xbox Live services
                /// </summary>
                http_status_305_use_proxy = 305,

                /// <summary>
                /// <b>0x80190133</b>
                /// The 307 response indicates a temporary redirect.
                /// </summary>
                http_status_307_temporary_redirect = 307,

                /// <summary>
                /// <b>0x80190190</b>
                /// The request could not be understood by the server due to malformed syntax. 
                /// The client should not repeat this request without modification.
                ///
                /// This code is returned by the following methods:
                /// Social services
                /// GetUserProfileAsync:
                /// The list of XUIDs returned from a leaderboard read operation and passed to this method has failed
                /// due to one or more invalid XUID values.
                ///
                /// Data services
                /// GetSingleUserStatisticsAsync: 
                /// Corresponds to a general HTTP error code: HTTP 400 Bad Request.
                ///
                /// Server Platform services
                /// AllocateClusterAsync: 
                /// Matchmaking request failure, typically due to QoS (where the requirement is all-or-nothing).
                ///
                /// Marketplace services
                /// GetCatalogItemDetailsAsync: 
                /// The Details service can only support 10 CatalogItems per request (MAX_DETAILS_ITEMS).
                /// BrowseCatalogAsync: Typically an error of setting maxItems too large. The catalog service only supports a max of 
                /// 25 items per call.
                /// </summary>
                http_status_400_bad_request = 400,

                /// <summary>
                /// <b>0x80190191</b>
                /// The 401 response typically indicates that authorization has been refused for provided credentials.
                /// This code is returned when the user(s) or the device is not authorized to access the requested data 
                /// or perform the requested action.
                /// </summary>
                http_status_401_unauthorized = 401,

                /// <summary>
                /// <b>0x80190192</b>
                /// 402 Payment Required
                /// </summary>
                http_status_402_payment_required = 402,

                /// <summary>
                /// <b>0x80190193</b>
                /// Corresponds to general HTTP error code HTTP 403 - Forbidden.
                /// The server understood the request but is refusing to fulfill. Authorization will not resolve the issue
                /// and the request should not be repeated.
                ///
                /// General
                /// This is typically a service configuration issue or a malformed HTTP request. To resolve, make sure that 
                /// your service configuration is correct, that your console is in the right sandbox, and that you are using 
                /// the correct title id and SCID. Using fiddler to examine the failing HTTP request will often help root cause the issue.
                ///
                /// Multiplayer services
                /// The user doesn't have multiplayer privileges, or the session is private/reserved and the user isn't a member.
                /// </summary>
                http_status_403_forbidden = 403,

                /// <summary>
                /// <b>0x80190194</b>
                /// This typically indicates that the server has not found anything matching the provided URI.
                /// The root cause of the 404 will depend on the API you are using. 
                /// For example many of the "Get Leaderboard" 
                /// APIs return a 404 error if you request a stat that does not exist, or if the leader board is empty, 
                /// CreateSessionAsync returns a 404 error if you call it with an invalid Session Template name, and 
                /// GetPartyViewAsync fails with a 404 if the multiplayer session times out. Using fiddler to examine the failing 
                /// HTTP request will often help root cause the issue.
                /// </summary>
                http_status_404_not_found = 404,

                /// <summary>
                /// <b>0x80190195</b>
                /// The method specified in the Request-Line is not allowed for the resource identified by the Request-URI. 
                /// For example, the request is a POST where a PUT is needed.
                /// </summary>
                http_status_405_method_not_allowed = 405,

                /// <summary>
                /// <b>0x80190196</b>
                /// The resource identified by the request is only capable for generating response entities which have 
                /// content characteristics not acceptable according to the accept headers sent in the Request.
                /// </summary>
                http_status_406_not_acceptable = 406,

                /// <summary>
                /// <b>0x80190197</b>
                /// This code is similar to HTTP 401 (Unauthorized), but indicates that the client must first 
                /// authenticate itself with the proxy.
                /// </summary>
                http_status_407_proxy_authentication_required = 407,

                /// <summary>
                /// <b>0x80190198</b>
                /// The client did not produce a request within the time the server was prepared to wait. The client MAY 
                /// repeat the request without modifications at a later time.
                /// </summary>
                http_status_408_request_timeout = 408,

                /// <summary>
                /// <b>0x80190199</b>
                /// This typically indicates that the request could not be completed due to a conflict with the current 
                /// state of the resource. This code is only allowed in situations where it is expected that a user might 
                /// be able to resolve the conflict and re-submit the request.
                ///
                /// Conflicts are most likely to occur in response to a PUT request. For example, if versioning were being 
                /// used and the entity being PUT included changes to a resource which conflict with those made by an earlier 
                /// (third-party) request, the server might use the 409 response to indicate that it can't complete the request. 
                /// In this case, the response entity would likely contain a list of the differences between the two versions in a 
                /// format defined by the Response-ContentType.
                ///
                /// Multiplayer services:
                /// Incorrect session or matchmaking query syntax or re-setting properties that are already pre-defined in the session template.
                /// The session couldn't be updated because the request is incompatible with the session. For example:
                /// Constants in the request conflict with constants in the session or session template.
                /// Members other than the caller are added to, or removed, from a large session.
                /// </summary>
                http_status_409_conflict = 409,

                /// <summary>
                /// <b>0x8019019A</b>
                /// This typically indicates that the requested resource is no longer available at the server and no forwarding address 
                /// is known. The expectation is that this condition is considered to be permanent.
                /// </summary>
                http_status_410_gone = 410,

                /// <summary>
                /// <b>0x8019019B</b>
                /// The server refuses to accept the request without a defined Content-Length. The client MAY repeat the request if it 
                /// adds a valid Content-Length header field containing the length of the message-body in the request.
                /// </summary>
                http_status_411_length_required = 411,

                /// <summary>
                /// <b>0x8019019C</b>
                /// The precondition given in one or more of the request-header fields evaluated to false when it was tested on the 
                /// server. This response code allows the client to place preconditions on the current resource meta information 
                /// (header field data) to prevent the requested method from being applied to a resource other than the one intended.
                /// 
                /// Multiplayer services
                /// The If-Match header, or (other than on a GET) the If-None-Match header couldn't be satisfied.
                /// The If-Match header couldn't be satisfied on a PUT or DELETE to an existing session. The current state of the session 
                /// is returned along with the current ETag.
                /// </summary>
                http_status_412_precondition_failed = 412,

                /// <summary>
                /// <b>0x8019019D</b>
                /// The server is refusing to process a request because the request entity is larger than the server is willing, or able, 
                /// to process. The server MAY close the connection to prevent the client from continuing with the request.
                /// </summary>
                http_status_413_request_entity_too_large = 413,

                /// <summary>
                /// <b>0x8019019E</b>
                /// The server is refusing to service the request because the Request-URI is longer than the server is willing to interpret. 
                /// This rare condition is only likely to occur when a client has improperly converted a POST Request to a GET Request with long query information.
                /// </summary>
                http_status_414_request_uri_too_long = 414,

                /// <summary>
                /// <b>0x8019019F</b>
                /// The server is refusing to service the request because the entity of the request is in a format not supported by the 
                /// requested resource for the requested method.
                /// </summary>
                http_status_415_unsupported_media_type = 415,

                /// <summary>
                /// <b>0x801901A0</b>
                /// A server SHOULD return a response with this status code if a requested included in a Range request-header field, and none of 
                /// the range-specifier values in this field overlap the current extent of the selected resource, and the request did not 
                /// include an If-Range request-header field.
                /// </summary>
                http_status_416_requested_range_not_satisfiable = 416,
                
                /// <summary>
                /// <b>0x801901A1</b>
                /// Expect-request header failure
                /// </summary>
                http_status_417_expectation_failed = 417,

                /// <summary>
                /// <b>0x801901A5</b>
                /// The request was misdirected 
                /// </summary>
                http_status_421_misdirected_request = 421,

                /// <summary>
                /// <b>0x801901A6</b>
                /// The request was not processable
                /// </summary>
                http_status_422_unprocessable_entity = 422,

                /// <summary>
                /// <b>0x801901A7</b>
                /// The resource was locked 
                /// </summary>
                http_status_423_locked = 423,

                /// <summary>
                /// <b>0x801901A8</b>
                /// The request failed due to dependency
                /// </summary>
                http_status_424_failed_dependency = 424,

                /// <summary>
                /// <b>0x801901AA</b>
                /// The client should upgrade to later protocol
                /// </summary>
                http_status_426_upgrade_required = 426,

                /// <summary>
                /// <b>0x801901AC</b>
                /// The server requires a precondition
                /// </summary>
                http_status_428_precondition_required = 428,

                /// <summary>
                /// <b>0x801901AD</b>
                /// Client is sending too many requests
                /// </summary>
                http_status_429_too_many_requests = 429,

                /// <summary>
                /// <b>0x801901AF</b>
                /// The request headers are too large
                /// </summary>
                http_status_431_request_header_fields_too_large = 431,

                /// <summary>
                /// <b>0x801901C1</b>
                /// The request should be retried after doing the appropriate action
                /// </summary>
                http_status_449_retry_with = 449,

                /// <summary>
                /// <b>0x801901C3</b>
                /// The request was unavailable for legal reasons
                /// </summary>
                http_status_451_unavailable_for_legal_reasons = 451,

                /// <summary>
                /// <b>0x801901F4</b>
                /// Corresponds to HTTP 500 Internal Server Error.This error can occur when invalid parameters are provided to the web service 
                /// via a RESTful API call, or if the web service encounters a crash or other unexpected error state. Using fiddler to examine 
                /// the failing HTTP request will often help root cause the issue.
                /// </summary>
                http_status_500_internal_server_error = 500,

                /// <summary>
                /// <b>0x801901F5</b>
                /// The requested service is not implemented.
                /// </summary>
                http_status_501_not_implemented = 501,

                /// <summary>
                /// <b>0x801901F6</b>
                /// The request got an invalid response to the gateway
                /// </summary>
                http_status_502_bad_gateway = 502,

                /// <summary>
                /// <b>0x801901F7</b>
                /// The requested service is not available.
                /// </summary>
                http_status_503_service_unavailable = 503,

                /// <summary>
                /// <b>0x801901F8</b>
                /// The HTTP gateway has timed out.
                /// </summary>
                http_status_504_gateway_timeout = 504,

                /// <summary>
                /// <b>0x801901F9</b>
                /// This version of HTTP is not supported by the endpoint.
                /// </summary>
                http_status_505_http_version_not_supported = 505,

                /// <summary>
                /// <b>0x801901FA</b>
                /// Internal configuration error
                /// </summary>
                http_status_506_variant_also_negotiates = 506,

                /// <summary>
                /// <b>0x801901FB</b>
                /// The service was unable complete the request due to storage
                /// </summary>
                http_status_507_insufficient_storage = 507,

                /// <summary>
                /// <b>0x801901FC</b>
                /// The service detected an infinite loop while processing
                /// </summary>
                http_status_508_loop_detected = 508,

                /// <summary>
                /// <b>0x801901FE</b>
                /// The request requires more extensions to complete
                /// </summary>
                http_status_510_not_extended = 510,

                /// <summary>
                /// <b>0x801901FF</b>
                /// The client needs to authenticate to gain network access.  Not used by Xbox Live services.
                /// </summary>
                http_status_511_network_authentication_required = 511,

                //////////////////////////////////////////////////////////////////////////
                // Errors from exception enabled components such as Casablanca
                //////////////////////////////////////////////////////////////////////////
                /// <summary>
                /// <b>0x8007000e</b>
                /// xbox_live_error_code 1000
                /// Bad alloc
                /// </summary>
                bad_alloc = 1000,

                /// <summary>
                /// <b>0x80004002</b>
                /// xbox_live_error_code 1001
                /// Bad cast
                /// </summary>
                bad_cast,

                /// <summary>
                /// <b>0x80070057</b>
                /// xbox_live_error_code 1002
                /// Invalid argument
                /// </summary>
                invalid_argument,

                /// <summary>
                /// <b>0x8000000b</b>
                /// xbox_live_error_code 1003
                /// Out of range
                /// </summary>
                out_of_range,

                /// <summary>
                /// <b>0x80070018</b>
                /// xbox_live_error_code 1004
                /// Length error
                /// </summary>
                length_error,

                /// <summary>
                /// <b>0x8000000b</b>
                /// xbox_live_error_code 1005
                /// Range error
                /// </summary>
                range_error,

                /// <summary>
                /// <b>0x8000ffff</b>
                /// xbox_live_error_code 1006
                /// Logic error
                /// </summary>
                logic_error,

                /// <summary>
                /// <b>0x89235200</b>
                /// xbox_live_error_code 1007
                /// Runtime error
                /// </summary>
                runtime_error,

                /// <summary>
                /// <b>0x83750007</b>
                /// xbox_live_error_code 1008
                /// JSON error
                /// </summary>
                json_error,

                /// <summary>
                /// <b>0x83750005</b>
                /// xbox_live_error_code 1009
                /// Websocket error
                /// </summary>
                websocket_error,

                /// <summary>
                /// <b>0x83750005</b>
                /// xbox_live_error_code 1010
                /// URI error
                /// </summary>
                uri_error,

                /// <summary>
                /// <b>0x80004005</b>
                /// xbox_live_error_code 1011
                /// Generic error
                /// </summary>
                generic_error,

                //////////////////////////////////////////////////////////////////////////
                // RTA errors
                //////////////////////////////////////////////////////////////////////////
                /// <summary>
                /// <b>0x89235201</b>
                /// xbox_live_error_code 1500
                /// RTA generic error
                /// </summary>
                rta_generic_error = 1500,

                /// <summary>
                /// <b>0x89235202</b>
                /// xbox_live_error_code 1501
                /// RTA subscription limit reached
                /// </summary>
                rta_subscription_limit_reached,

                /// <summary>
                /// <b>0x89235203</b>
                /// xbox_live_error_code 1502
                /// RTA access denied
                /// </summary>
                rta_access_denied,

                //////////////////////////////////////////////////////////////////////////
                // Auth errors
                //////////////////////////////////////////////////////////////////////////

                /// <summary>
                /// <b>0x89235204</b>
                /// xbox_live_error_code 2000
                /// Unknown auth error
                /// </summary>
                auth_unknown_error = 2000,

                /// <summary>
                /// <b>0x8086000c</b>
                /// xbox_live_error_code 2001
                /// User interaction required
                /// </summary>
                auth_user_interaction_required,

                /// <summary>
                /// <b>0x80070525</b>
                /// xbox_live_error_code 2002
                /// User interaction required
                /// </summary>
                auth_user_switched,

                /// <summary>
                /// <b>0x80070525</b>
                /// xbox_live_error_code 2003
                /// User cancelled
                /// </summary>
                auth_user_cancel,

                /// <summary>
                /// <b>0x80070525</b>
                /// xbox_live_error_code 2004
                /// User not signed in
                /// </summary>
                auth_user_not_signed_in,

                /// <summary>
                /// <b>0x89235205</b>
                /// xbox_live_error_code 2005
                /// Auth runtime error
                /// </summary>
                auth_runtime_error,

                /// <summary>
                /// <b>0x89235206</b>
                /// xbox_live_error_code 2006
                /// Auth no token error
                /// </summary>
                auth_no_token_error,

                //////////////////////////////////////////////////////////////////////////
                // Xbox Live SDK errors
                //////////////////////////////////////////////////////////////////////////
                /// <summary>
                /// <b>0x8007064a</b>
                /// xbox_live_error_code 3000
                /// Could not read the xboxservices.config.  Ensure it is deployed in the package at
                /// Windows::ApplicationModel::Package::Current->InstalledLocation + "\xboxservices.config"
                /// </summary>
                invalid_config = 3000,

                /// <summary>
                /// <b>0x80004001</b>
                /// xbox_live_error_code 3001
                /// API is not supported
                /// </summary>
                unsupported = 3001,

                //////////////////////////////////////////////////////////////////////////
                // xbox live auth errors
                //////////////////////////////////////////////////////////////////////////
                /// <summary>
                /// <b>0x80072EE2</b>
                /// This error is typically returned by Xbox::Services APIs, GetTokenAndSignatureAsync, 
                /// and CheckLicense. It indicates that the Internet connection or a server response was 
                /// interrupted; possibly the result of a incorrectly configured NSAL. Pending requests 
                /// should be retried when this error is encountered.
                /// </summary>
                HR_ERROR_INTERNET_TIMEOUT = (int)0x80072EE2,

                /// <summary>
                /// <b>0x87DD0003</b>
                /// XASD returned an unexpected response.
                /// </summary>
                AM_E_XASD_UNEXPECTED = (int)0x87DD0003,

                /// <summary>
                /// <b>0x87DD0004</b>
                /// XASU returned an unexpected response.
                /// </summary>
                AM_E_XASU_UNEXPECTED = (int)0x87DD0004,

                /// <summary>
                /// <b>0x87DD0005</b>
                /// XAST returned an unexpected response.
                /// Cause: Your NSAL configuration is set up correctly for your service, however the 
                /// Relying Party certificate is not yet trusted by Xbox Live.
                ///
                /// Resolution : Relying Party configuration on XDP and provide the Relying Party 
                /// certificate in the XSTS token certificate for your endpoint.
                /// If you are using the NSAL.json file, double check that the Relying Party name is 
                /// correct in the 'target' field of your NSAL.json file on the console and ensure that 
                /// the certificate has been specified in XDP.
                ///
                /// If you are developing a UWP title and performing service configuration in XDP, then
                /// you may have not completed the Application ID binding step correctly.  For more
                /// information, please see the "Troubleshooting Sign-in" article in the Xbox Live documentation.
                /// </summary>
                AM_E_XAST_UNEXPECTED = (int)0x87DD0005,

                /// <summary>
                /// <b>0x87DD0006</b>
                /// Cause : Your NSAL configuration is set up correctly for your service, however the 
                /// Relying Party certificate is not yet trusted by Xbox Live.
                /// 
                /// Resolution : Relying Party configuration on XDP and provide the Relying Party 
                /// certificate in the XSTS token certificate for your endpoint.
                /// If you are using the NSAL.json file, double check that the Relying Party name is 
                /// correct in the 'target' field of your NSAL.json file on the console and ensure 
                /// that the certificate has been specified in XDP.
                /// </summary>
                AM_E_XSTS_UNEXPECTED = (int)0x87DD0006,

                /// <summary>
                /// <b>0x87DD0007</b>
                /// XDevice returned an unexpected response.
                /// </summary>
                AM_E_XDEVICE_UNEXPECTED = (int)0x87DD0007,

                /// <summary>
                /// <b>0x87DD0008</b>
                /// The console is not authorized to enable development mode.
                /// </summary>
                AM_E_DEVMODE_NOT_AUTHORIZED = (int)0x87DD0008,

                /// <summary>
                /// <b>0x87DD0009</b>
                /// The operation was not authorized.
                /// Cause: The user is not authorized to get an XSTS token with the current TitleID or 
                /// SandboxID setup on the console for the provided URL.
                /// 
                /// Resolution:
                /// Ensure that your console is in the proper SandboxID for the TitleID you are using.
                /// See this forum thread for instructions on changing your SandboxID and which SandboxID 
                /// to use for samples.
                ///
                /// Ensure that the test account you are using is partitioned to a group in XDP that has 
                /// access to the TitleID and SandboxID for your title(Note that all accounts are able to 
                /// access the samples while in the sample SandboxID).
                /// </summary>
                AM_E_NOT_AUTHORIZED = (int)0x87DD0009,

                /// <summary>
                /// <b>0x87DD000A</b>
                /// The operation was forbidden. The server responded with a 403, and a more detailed error is not available.
                /// </summary>
                AM_E_FORBIDDEN = (int)0x87DD000A,

                /// <summary>
                /// <b>0x87DD000B</b>
                /// The URL specified does not match a known target.
                ///
                /// Cause: 
                /// The URL you are attempting to get a token for was not found within your NSAL configuration.
                /// 
                /// Resolution:
                /// Ensure that the Relying Party and server name are properly set up as an endpoint 
                /// on your console and in your NSAL.
                ///
                /// Try calling a known Xbox Service such as https ://social.xboxlive.com/users/xuid(user's xuid)/people 
                /// to ensure that you can get tokens for the Xbox services. If you are using the NSAL.json file, make sure 
                /// that the 'target' field in the file has your Relying Party name ending with '/' and 
                /// that the server name is set up correctly.
                /// </summary>
                AM_E_UNKNOWN_TARGET = (int)0x87DD000B,

                /// <summary>
                /// <b>0x87DD000C</b>
                /// There were problems with the JSON data downloaded from the server.
                /// </summary>
                AM_E_INVALID_NSAL_DATA = (int)0x87DD000C,

                /// <summary>
                /// <b>0x87DD000D</b>
                /// The title has not yet been successfully authenticated.
                /// Cause: This will be returned when a title token is required, but has not been cached. For example, 
                /// attempting to retrieve an X token with title claims will fail with this error if AuthenticateTitle 
                /// hasn't been called successfully.
                /// 
                /// Resolution: Check your TitleID, SOCID, and SandboxID, or check your title's configuration on XDP.
                /// </summary>
                AM_E_TITLE_NOT_AUTHENTICATED = (int)0x87DD000D,

                /// <summary>
                /// <b>0x87DD000E</b>
                /// XAST returned 401 when attempting to retrieve the T token.  Double-check that your device is set to the
                /// proper development sandbox and that the user has access to the sandbox.
                ///
                /// For more information, please see the "Troubleshooting Sign-in" article in the Xbox Live documentation.
                /// </summary>
                AM_E_TITLE_NOT_AUTHORIZED = (int)0x87DD000E,

                /// <summary>
                /// <b>0x87DD0011</b>
                /// The user hash value for the specified user hasn't been recorded, so a valid token can't be generated.
                /// </summary>
                AM_E_USER_HASH_MISSING = (int)0x87DD0011,

                /// <summary>
                /// <b>0x87DD0013</b>
                /// The Authentication Manager can't find the user for which an authentication token is being retrieved. 
                /// One possible scenario is that the User signed out, but the title still has a reference to the User object.
                /// </summary>
                AM_E_USER_NOT_FOUND = (int)0x87DD0013,

                /// <summary>
                /// <b>0x87DD0015</b>
                /// The environment configured or specified is not valid.
                /// </summary>
                AM_E_INVALID_ENVIRONMENT = (int)0x87DD0015,

                /// <summary>
                /// <b>0x87DD0016</b>
                /// The XASD authentication server has timed out.
                /// </summary>
                AM_E_XASD_TIMEOUT = (int)0x87DD0016,

                /// <summary>
                /// <b>0x87DD0017</b>
                /// The XASU authentication server has timed out.
                /// </summary>
                AM_E_XASU_TIMEOUT = (int)0x87DD0017,

                /// <summary>
                /// <b>0x87DD0018</b>
                /// The XAST authentication server has timed out.
                /// </summary>
                AM_E_XAST_TIMEOUT = (int)0x87DD0018,

                /// <summary>
                /// <b>0x87DD0019</b>
                /// The XSTS authentication server has timed out.
                /// </summary>
                AM_E_XSTS_TIMEOUT = (int)0x87DD0019,

                /// <summary>
                /// <b>0x87DD001A</b>
                /// Title authentication failed because a connection to Xbox Live is required, by policy, and none is present.
                /// </summary>
                AM_E_LIVE_CONNECTION_REQUIRED = (int)0x87DD001A,

                /// <summary>
                /// <b>0x87dd001e</b>
                /// There is no network connection.
                /// </summary>
                AM_E_NO_NETWORK = (int)0x87dd001e,

                /// <summary>
                /// <b>0x87dd0020</b>
                /// The Network Security Authorization List(NSAL) returned an unexpected response.
                /// </summary>
                AM_E_XTITLE_UNEXPECTED = (int)0x87dd0020,

                /// <summary>
                /// <b>0x87dd0021</b>
                /// The endpoint does not require an authorization token, but the application is attempting to 
                /// retrieve a token via GetTokenAndSignatureAsync.
                /// </summary>
                AM_E_NO_TOKEN_REQUIRED = (int)0x87dd0021,

                /// <summary>
                /// <b>0x87dd0022</b>
                /// Timeouts were received from the various authorization servers.
                /// </summary>
                AM_E_XTITLE_TIMEOUT = (int)0x87dd0022,

                /// <summary>
                /// <b>0x8015DC00</b>
                /// Developer mode is not authorized for the client device.
                /// </summary>
                XO_E_DEVMODE_NOT_AUTHORIZED = (int)0x8015DC00,

                /// <summary>
                /// <b>0x8015DC01</b>
                /// A system update is required before this action can be performed.
                /// </summary>
                XO_E_SYSTEM_UPDATE_REQUIRED = (int)0x8015DC01,

                /// <summary>
                /// <b>0x8015DC02</b>
                /// A content update is required before this action can be performed.
                /// </summary>
                XO_E_CONTENT_UPDATE_REQUIRED = (int)0x8015DC02,

                /// <summary>
                /// <b>0x8015DC03</b>
                /// The device or user was banned.
                /// </summary>
                XO_E_ENFORCEMENT_BAN = (int)0x8015DC03,

                /// <summary>
                /// <b>0x8015DC04</b>
                /// The device or user was banned.
                /// </summary>
                XO_E_THIRD_PARTY_BAN = (int)0x8015DC04,

                /// <summary>
                /// <b>0x8015DC05</b>
                /// Access to this resource has been parentally restricted.
                /// </summary>
                XO_E_ACCOUNT_PARENTALLY_RESTRICTED = (int)0x8015DC05,

                /// <summary>
                /// <b>0x8015DC08</b>
                /// Access to this resource requires that the account billing information
                /// is updated.
                /// </summary>
                XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED = (int)0x8015DC08,

                /// <summary>
                /// <b>0x8015DC0A</b>
                /// The user has not accepted the terms of use for this resource.
                /// </summary>
                XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED = (int)0x8015DC0A,

                /// <summary>
                /// <b>0x8015DC0B</b>
                /// This resource is not available in the country associated with the user.
                /// </summary>
                XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED = (int)0x8015DC0B,

                /// <summary>
                /// <b>0x8015DC0C</b>
                /// Access to this resource requires age verification.
                /// </summary>
                XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED = (int)0x8015DC0C,
                
                /// <summary>
                /// <b>0x8015DC0D</b>
                /// </summary>
                XO_E_ACCOUNT_CURFEW = (int)0x8015DC0D,
                
                /// <summary>
                /// <b>0x8015DC0E</b>
                /// </summary>
                XO_E_ACCOUNT_CHILD_NOT_IN_FAMILY = (int)0x8015DC0E,

                /// <summary>
                /// <b>0x8015DC0F</b>
                /// </summary>
                XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED = (int)0x8015DC0F,

                /// <summary>
                /// <b>0x8015DC09</b>
                /// </summary>
                XO_E_ACCOUNT_CREATION_REQUIRED = (int)0x8015DC09,

                /// <summary>
                /// <b>0x8015DC10</b>
                /// </summary>
                XO_E_ACCOUNT_MAINTENANCE_REQUIRED = (int)0x8015DC10,

                /// <summary>
                /// <b>0x8015DC11</b>
                /// The call was blocked because there was a conflict with the sandbox, console, application, or 
                /// your account.Verify your account, console and title settings in XDP, and check the current 
                /// Sandbox on the device.
                /// </summary>
                XO_E_ACCOUNT_TYPE_NOT_ALLOWED = (int)0x8015DC11,

                /// <summary>
                /// <b>0x8015DC12</b>
                /// Your device does not have access to the Sandbox it is set to, or the account you are signed 
                /// in with does not have access to the Sandbox.Check that you are using the correct Sandbox.
                ///
                /// Note: All XDK samples use XDKS.1 SandboxID, which allow all user accounts to access and run 
                /// the samples.SandboxID's are case sensitive- Not matching the case of your SandboxID exactly may 
                /// result in errors. If you are still having issues running the sample, please work with your 
                /// Developer Account Manager and provide a fiddler trace to help with troubleshooting.
                ///
                /// For more information on handling this error, please see the "Troubleshooting Sign-in" article
                /// in the Xbox Live documentation
                /// </summary>
                XO_E_CONTENT_ISOLATION = (int)0x8015DC12,

                /// <summary>
                /// <b>0x8015DC13</b>
                /// </summary>
                XO_E_ACCOUNT_NAME_CHANGE_REQUIRED = (int)0x8015DC13,

                /// <summary>
                /// <b>0x8015DC14</b>
                /// </summary>
                XO_E_DEVICE_CHALLENGE_REQUIRED = (int)0x8015DC14,

                /// <summary>
                /// <b>0x8015DC16</b>
                /// The account was signed in on another device.
                /// </summary>
                XO_E_SIGNIN_COUNT_BY_DEVICE_TYPE_EXCEEDED = (int)0x8015DC16,

                /// <summary>
                /// <b>0x8015DC17</b>
                /// </summary>
                XO_E_PIN_CHALLENGE_REQUIRED = (int)0x8015DC17,

                /// <summary>
                /// <b>0x8015DC18</b>
                /// </summary>
                XO_E_RETAIL_ACCOUNT_NOT_ALLOWED = (int)0x8015DC18,
                
                /// <summary>
                /// <b>0x8015DC19</b>
                /// The current sandbox is not allowed to access the SCID.  Please ensure that your current
                /// sandbox is set to your development sandbox.  If you are running on a Windows 10 PC, then
                /// you can change your current sandbox using the SwitchSandbox.cmd script in the Xbox Live SDK
                /// tools directory.  If you are using an Xbox One, you can switch the sandbox using Xbox One
                /// Manager.
                ///
                /// For more information on handling this error, please see the "Troubleshooting Sign-in" article
                /// in the Xbox Live documentation.
                /// </summary>
                XO_E_SANDBOX_NOT_ALLOWED = (int)0x8015DC19,

                /// <summary>
                /// <b>0x8015DC1A</b>
                /// </summary>
                XO_E_ACCOUNT_SERVICE_UNAVAILABLE_UNKNOWN_USER = (int)0x8015DC1A,

                /// <summary>
                /// <b>0x8015DC1B</b>
                /// </summary>
                XO_E_GREEN_SIGNED_CONTENT_NOT_AUTHORIZED = (int)0x8015DC1B,

                /// <summary>
                /// <b>0x8015DC1C</b>
                /// </summary>
                XO_E_CONTENT_NOT_AUTHORIZED = (int)0x8015DC1C,

                //////////////////////////////////////////////////////////////////////////
                // Generic errors
                //////////////////////////////////////////////////////////////////////////

                /// <summary>
                /// <b>0x800C0002</b>
                /// The URL is invalid.
                /// </summary>
                HR_INET_E_INVALID_URL = (int)0x800C0002,

                /// <summary>
                /// <b>0x800C0003</b>
                /// No session.
                /// </summary>
                HR_INET_E_NO_SESSION = (int)0x800C0003,

                /// <summary>
                /// <b>0x800C0004</b>
                /// WinINet cannot connect to the requested resource.
                /// </summary>
                HR_INET_E_CANNOT_CONNECT = (int)0x800C0004,

                /// <summary>
                /// <b>0x800C0005</b>
                /// The requested resource was not found.
                /// </summary>
                HR_INET_E_RESOURCE_NOT_FOUND = (int)0x800C0005,

                /// <summary>
                /// <b>0x800C0006</b>
                /// The requested resource was not found.
                /// </summary>
                HR_INET_E_OBJECT_NOT_FOUND = (int)0x800C0006,

                /// <summary>
                /// <b>0x800C0007</b>
                /// The requested resource was not found.
                /// </summary>
                HR_INET_E_DATA_NOT_AVAILABLE = (int)0x800C0007,

                /// <summary>
                /// <b>0x800C0008</b>
                /// Operation restricted by the current inability to discover or join the multiplayer session, or 
                /// the player does not have the multiplayer privilege.
                /// </summary>
                HR_INET_E_DOWNLOAD_FAILURE = (int)0x800C0008,

                /// <summary>
                /// <b>0x800C0009</b>
                /// Authentication is required to access this resource.
                /// </summary>
                HR_INET_E_AUTHENTICATION_REQUIRED = (int)0x800C0009,

                /// <summary>
                /// <b>0x800C000A</b>
                /// No valid media
                /// </summary>
                HR_INET_E_NO_VALID_MEDIA = (int)0x800C000A,

                /// <summary>
                /// <b>0x800C000B</b>
                /// The connection has timed out.
                /// </summary>
                HR_INET_E_CONNECTION_TIMEOUT = (int)0x800C000B,

                /// <summary>
                /// <b>0x800C000C</b>
                /// Invalid request
                /// </summary>
                HR_INET_E_INVALID_REQUEST = (int)0x800C000C,

                /// <summary>
                /// <b>0x800C000D</b>
                /// The requested protocol is unknown.
                /// </summary>
                HR_INET_E_UNKNOWN_PROTOCOL = (int)0x800C000D,

                /// <summary>
                /// <b>0x800C000E</b>
                /// Security problem
                /// </summary>
                HR_INET_E_SECURITY_PROBLEM = (int)0x800C000E,

                /// <summary>
                /// <b>0x800C000F</b>
                /// Cannot load data
                /// </summary>
                HR_INET_E_CANNOT_LOAD_DATA = (int)0x800C000F,

                /// <summary>
                /// <b>0x800C0010</b>
                /// Cannot instantiate object
                /// </summary>
                HR_INET_E_CANNOT_INSTANTIATE_OBJECT = (int)0x800C0010,

                /// <summary>
                /// <b>0x800C0019</b>
                /// The certificate presented for the request is invalid.
                /// </summary>
                HR_INET_E_INVALID_CERTIFICATE = (int)0x800C0019,

                /// <summary>
                /// <b>0x800C0014</b>
                /// The redirect to a different endpoint has failed.
                /// </summary>
                HR_INET_E_REDIRECT_FAILED = (int)0x800C0014,

                /// <summary>
                /// <b>0x800C0015</b>
                /// A resource request has been directed to a directory rather than an individual resource.
                /// </summary>
                HR_INET_E_REDIRECT_TO_DIR = (int)0x800C0015,

                /// <summary>
                /// <b>0x800704cf</b>
                /// The network location cannot be reached.
                /// </summary>
                HR_ERROR_NETWORK_UNREACHABLE = (int)0x800704cf
            };

            /// <summary>
            /// Category error type for XSAPI error codes.
            /// </summary>
            class xbox_services_error_code_category_impl : public std::error_category
            {
            public:
                /// <summary>
                /// Gets the error category name.
                /// </summary>
                /// <returns>A string identifying the category.</returns>
                _XSAPIIMP const char* name() const _NOEXCEPT override { return "XBL"; }

                /// <summary>
                /// Converts an error value into a string that describes the error.
                /// </summary>
                /// <returns>A string that describes the error.</returns>
                _XSAPIIMP std::string message(_In_ int errorCode) const _NOEXCEPT override;
            };

            /// <summary>
            /// Category error type for XSAPI error conditions.
            /// </summary>
            class xbox_services_error_condition_category_impl : public std::error_category
            {
            public:
                /// <summary>
                /// Gets the error category name.
                /// </summary>
                /// <returns>A string identifying the category.</returns>
                _XSAPIIMP const char* name() const _NOEXCEPT override { return "XBL CONDITION"; }

                /// <summary>
                /// Converts an error value into a string that describes the error.
                /// </summary>
                /// <returns>A string that describes the error.</returns>
                _XSAPIIMP std::string message(_In_ int errorCode) const _NOEXCEPT override;

                /// <summary>
                /// Used to establish equivalence between arbitrary error_codes in
                /// the current category with arbitrary error_conditions.
                /// </summary>
                /// <param name="arbitraryErrorCode">An error code.</param>
                /// <param name="xboxLiveErrorCondition">An error condition.</param>
                /// <returns>True if the error code and the error condition are related; otherwise false.</returns>
                _XSAPIIMP bool equivalent(
                    _In_ const std::error_code& arbitraryErrorCode,
                    _In_ int xboxLiveErrorCondition) const _NOEXCEPT override;
            };

            /// <summary>
            /// Gets the one global instance of the error code category.
            /// </summary>
            /// <returns>An error category instance.</returns>
            _XSAPIIMP const xbox_services_error_code_category_impl& xbox_services_error_code_category();

            /// <summary>
            /// Gets the one global instance of the error condition category.
            /// </summary>
            /// <returns>An error category instance.</returns>
            _XSAPIIMP const xbox_services_error_condition_category_impl& xbox_services_error_condition_category();
    }
};


#if XSAPI_U
    NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    inline std::error_code make_error_code(xbox::services::xbox_live_error_code e)
    {
        return std::error_code(static_cast<int>(e), xbox::services::xbox_services_error_code_category());
    }

    inline std::error_condition make_error_condition(xbox::services::xbox_live_error_condition e)
    {
        return std::error_condition(static_cast<int>(e), xbox::services::xbox_services_error_condition_category());
    }
    NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif


namespace std
{
    inline std::error_code make_error_code(xbox::services::xbox_live_error_code e)
    {
        return std::error_code(static_cast<int>(e), xbox::services::xbox_services_error_code_category());
    }

    inline std::error_condition make_error_condition(xbox::services::xbox_live_error_condition e)
    {
        return std::error_condition(static_cast<int>(e), xbox::services::xbox_services_error_condition_category());
    }

    template <>
    struct is_error_code_enum<xbox::services::xbox_live_error_code> : public true_type{};

    template <>
    struct is_error_condition_enum<xbox::services::xbox_live_error_condition> : public true_type{};
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

template<typename T>
class xbox_live_result
{
public:
    xbox_live_result();
    xbox_live_result(_In_ T payload);
    xbox_live_result(_In_ std::error_code errorCode, _In_ std::string errorMessage = std::string());
    xbox_live_result(_In_ T payload, _In_ std::error_code errorCode, _In_ std::string errorMessage = std::string());

    xbox_live_result(_In_ const xbox_live_result&);
    xbox_live_result& operator=(_In_ const xbox_live_result&);
    xbox_live_result(_In_ xbox_live_result&& other);
    xbox_live_result& operator=(_In_ xbox_live_result&& other);

    /// <summary>
    /// The payload of the result.  It may be empty if there was an error
    /// </summary>
    _XSAPIIMP T& payload();

    /// <summary>
    /// The payload of the result.  It may be empty if there was an error
    /// </summary>
    _XSAPIIMP const T& payload() const;

    /// <summary>
    /// Sets the payload.
    /// </summary>
    _XSAPIIMP void set_payload(T payload);

    /// <summary>
    /// The error returned by the operation.
    /// To test for and potentially react to in your code, use the values in xbox_error_condition
    /// To see the specific error value returned by the operation, use err_code().value() but testing for these values is discouraged.
    /// For more detail about std::error_code vs std::error_condition, see 
    /// http://en.cppreference.com/w/cpp/error/error_condition
    /// </summary>
    _XSAPIIMP const std::error_code& err() const;

    /// <summary>
    /// Sets the error.
    /// To test for and potentially react to in your code, use the values in xbox_error_condition
    /// To see the specific error value returned by the operation, use err_code().value() but testing for these values is discouraged.
    /// For more detail about std::error_code vs std::error_condition, see 
    /// http://en.cppreference.com/w/cpp/error/error_condition
    /// </summary>
    void _Set_err(std::error_code errc);

    /// <summary>
    /// Returns call specific debug information.  
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    _XSAPIIMP const std::string& err_message() const;

    /// <summary>
    /// Sets error code debug information.  
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    void _Set_err_message(std::string errMessage);

private:
    T m_payload;
    std::error_code m_errorCode;
    std::string m_errorMessage;
};

template<>
class xbox_live_result <void>
{
public:
    xbox_live_result()
        : m_errorMessage(std::string())
    {
        m_errorCode = std::make_error_code(xbox_live_error_code::no_error);
    }

    xbox_live_result(_In_ std::error_code errorCode, _In_ std::string errorMessage = std::string()) :
        m_errorCode(std::move(errorCode)),
        m_errorMessage(std::move(errorMessage))
    {
    }

    xbox_live_result(_In_ const xbox_live_result& other)
    {
        *this = other;
    }

    xbox_live_result& operator=(_In_ const xbox_live_result& other)
    {
        m_errorCode = other.m_errorCode;
        m_errorMessage = other.m_errorMessage;
        return *this;
    }

    xbox_live_result(_In_ xbox_live_result&& other)
    {
        *this = std::move(other);
    }

    xbox_live_result& operator=(_In_ xbox_live_result&& other)
    {
        if (this != &other)
        {
            m_errorCode = std::move(other.m_errorCode);
            m_errorMessage = std::move(other.m_errorMessage);
        }

        return *this;
    }

    /// <summary>
    /// The error returned by the operation.
    /// To test for and potentially react to in your code, use the values in xbox_error_condition
    /// To see the specific error value returned by the operation, use err_code().value() but testing for these values is discouraged.
    /// For more detail about std::error_code vs std::error_condition, see 
    /// http://en.cppreference.com/w/cpp/error/error_condition
    /// </summary>
    _XSAPIIMP const std::error_code& err() const
    {
        return m_errorCode;
    }

    /// <summary>
    /// Returns call specific debug information.  
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    _XSAPIIMP const std::string& err_message() const
    {
        return m_errorMessage;
    }

    /// <summary>
    /// Returns call specific debug information.  
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    void _Set_err(std::error_code errCode)
    {
        m_errorCode = std::move(errCode);
    }

    /// <summary>
    /// Sets error code debug information.  
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    void _Set_err_message(std::string errMessage)
    {
        m_errorMessage = std::move(errMessage);
    }

private:
    std::error_code m_errorCode;
    std::string m_errorMessage;
};

template<typename T>
xbox_live_result<T>::xbox_live_result() :
    m_errorCode(xbox_live_error_code::no_error),
    m_errorMessage(std::string())
{
}

template<typename T>
xbox_live_result<T>::xbox_live_result(_In_ T payload) :
    m_payload(std::move(payload)),
    m_errorCode(xbox_live_error_code::no_error),
    m_errorMessage(std::string())
{
}

template<typename T>
xbox_live_result<T>::xbox_live_result(
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage
    ) :
    m_errorCode(std::move(errorCode)),
    m_errorMessage(std::move(errorMessage))
{
}

template<typename T>
xbox_live_result<T>::xbox_live_result(
    _In_ T payload,
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage
    ) :
    m_payload(std::move(payload)),
    m_errorCode(std::move(errorCode)),
    m_errorMessage(std::move(errorMessage))
{
}

template<typename T>
T& xbox_live_result<T>::payload()
{
    return m_payload;
}

template<typename T>
const T& xbox_live_result<T>::payload() const
{
    return m_payload;
}

template<typename T>
void xbox_live_result<T>::set_payload(T payload)
{
    m_payload = std::move(payload);
}

template<typename T>
const std::error_code& xbox_live_result<T>::err() const
{
    return m_errorCode;
}

template<typename T>
void xbox_live_result<T>::_Set_err(std::error_code errCode)
{
    m_errorCode = std::move(errCode);
}

template<typename T>
const std::string& xbox_live_result<T>::err_message() const
{
    return m_errorMessage;
}

template<typename T>
void xbox_live_result<T>::_Set_err_message(std::string errorMsg)
{
    m_errorMessage = std::move(errorMsg);
}

template<typename T>
xbox_live_result<T>::xbox_live_result(_In_ const xbox_live_result& other)
{
    *this = other;
}

template<typename T>
xbox_live_result<T>&
xbox_live_result<T>::operator=(_In_ const xbox_live_result& other)
{
    m_payload = other.m_payload;
    m_errorCode = other.m_errorCode;
    m_errorMessage = other.m_errorMessage;
    return *this;
}

template<typename T>
xbox_live_result<T>::xbox_live_result(_In_ xbox_live_result&& other)
{
    *this = std::move(other);
}

template<typename T>
xbox_live_result<T>& xbox_live_result<T>::operator=(_In_ xbox_live_result&& other)
{
    if (this != &other)
    {
        m_payload = std::move(other.m_payload);
        m_errorCode = std::move(other.m_errorCode);
        m_errorMessage = std::move(other.m_errorMessage);
    }

    return *this;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
