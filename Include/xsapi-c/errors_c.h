// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>
/// Enumeration values that define the Xbox Live API error conditions.
/// </summary>
/// <remarks>
/// A best practice is to test the returned XBL_RESULT.errorCondition against these error conditions.
/// For more detail about std::error_code vs std::error_condition, see 
/// http://en.cppreference.com/w/cpp/error/error_condition 
/// </remarks>
typedef enum XBL_ERROR_CONDITION
{
    /// <summary>
    /// No error.
    /// </summary>
    XBL_ERROR_CONDITION_NO_ERROR = 0,

    /// <summary>
    /// A generic error condition.
    /// </summary>
    XBL_ERROR_CONDITION_GENERIC_ERROR,

    /// <summary>
    /// An error condition related to an object being out of range.
    /// </summary>
    XBL_ERROR_CONDITION_GENERIC_OUT_OF_RANGE,

    /// <summary>
    /// An error condition related to attempting to authenticate.
    /// </summary>
    XBL_ERROR_CONDITION_AUTH,

    /// <summary>
    /// An error condition related to network connectivity.
    /// </summary>
    XBL_ERROR_CONDITION_NETWORK,

    /// <summary>
    /// An error condition related to an HTTP method call.
    /// </summary>
    XBL_ERROR_CONDITION_HTTP,

    /// <summary>
    /// The requested resource was not found.
    /// </summary>
    XBL_ERROR_CONDITION_HTTP_404_NOT_FOUND,

    /// <summary>
    /// The precondition given in one or more of the request-header fields evaluated
    /// to false when it was tested on the server.
    /// </summary>
    XBL_ERROR_CONDITION_HTTP_412_PRECONDITION_FAILED,

    /// <summary>
    /// Client is sending too many requests
    /// </summary>
    XBL_ERROR_CONDITION_HTTP_429_TOO_MANY_REQUESTS,

    /// <summary>
    /// The service timed out while attempting to process the request.
    /// </summary>
    XBL_ERROR_CONDITION_HTTP_SERVICE_TIMEOUT,

    /// <summary>
    /// An error related to real time activity.
    /// </summary>
    XBL_ERROR_CONDITION_RTA
} XBL_ERROR_CONDITION;

/// <summary>
/// These are XSAPI specific error codes.
/// Only the HTTP codes and errors that are commonly seen when calling Xbox LIVE are called out with guidance about the cause.
/// The best practice to test for and react to using the xbox_live_error_condition enum instead of these error codes.
/// </summary>
typedef enum XBL_ERROR_CODE
{
    /// <summary>
    /// <b>0</b>
    /// No error
    /// </summary>
    XBL_ERROR_CODE_NO_ERROR = 0,

    //////////////////////////////////////////////////////////////////////////
    // HTTP errors
    //////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// The 204 response indicates that the content data was not found.
    /// This code is returned when you are trying to access or write to a session that has been deleted.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_204_RESOURCE_DATA_NOT_FOUND = 204,

    /// <summary>
    /// <b>0x8019012C</b>
    /// The 300 response indicates there are multiple choices.  Not returned by Xbox Live services
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_300_MULTIPLE_CHOICES = 300,

    /// <summary>
    /// <b>0x8019012D</b>
    /// The 301 response indicates that the request should be directed at a new URI
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_301_MOVED_PERMANENTLY = 301,

    /// <summary>
    /// <b>0x8019012E</b>
    /// The 302 response indicates found.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_302_FOUND = 302,

    /// <summary>
    /// <b>0x8019012F</b>
    /// The 303 response indicates see other.  Not returned by Xbox Live services
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_303_SEE_OTHER = 303,

    /// <summary>
    /// <b>0x80190130</b>
    /// The 304 response indicates resource has not been modified.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_304_NOT_MODIFIED = 304,

    /// <summary>
    /// <b>0x80190131</b>
    /// The 305 response indicates you must a use proxy.  Not typically returned by Xbox Live services
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_305_USE_PROXY = 305,

    /// <summary>
    /// <b>0x80190133</b>
    /// The 307 response indicates a temporary redirect.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_307_TEMPORARY_REDIRECT = 307,

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
    XBL_ERROR_CODE_HTTP_STATUS_400_BAD_REQUEST = 400,

    /// <summary>
    /// <b>0x80190191</b>
    /// The 401 response typically indicates that authorization has been refused for provided credentials.
    /// This code is returned when the user(s) or the device is not authorized to access the requested data 
    /// or perform the requested action.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_401_UNAUTHORIZED = 401,

    /// <summary>
    /// <b>0x80190192</b>
    /// 402 Payment Required
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_402_PAYMENT_REQUIRED = 402,

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
    XBL_ERROR_CODE_HTTP_STATUS_403_FORBIDDEN = 403,

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
    XBL_ERROR_CODE_HTTP_STATUS_404_NOT_FOUND = 404,

    /// <summary>
    /// <b>0x80190195</b>
    /// The method specified in the Request-Line is not allowed for the resource identified by the Request-URI. 
    /// For example, the request is a POST where a PUT is needed.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_405_METHOD_NOT_ALLOWED = 405,

    /// <summary>
    /// <b>0x80190196</b>
    /// The resource identified by the request is only capable for generating response entities which have 
    /// content characteristics not acceptable according to the accept headers sent in the Request.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_406_NOT_ACCEPTABLE = 406,

    /// <summary>
    /// <b>0x80190197</b>
    /// This code is similar to HTTP 401 (Unauthorized), but indicates that the client must first 
    /// authenticate itself with the proxy.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED = 407,

    /// <summary>
    /// <b>0x80190198</b>
    /// The client did not produce a request within the time the server was prepared to wait. The client MAY 
    /// repeat the request without modifications at a later time.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_408_REQUEST_TIMEOUT = 408,

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
    XBL_ERROR_CODE_HTTP_STATUS_409_CONFLICT = 409,

    /// <summary>
    /// <b>0x8019019A</b>
    /// This typically indicates that the requested resource is no longer available at the server and no forwarding address 
    /// is known. The expectation is that this condition is considered to be permanent.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_410_GONE = 410,

    /// <summary>
    /// <b>0x8019019B</b>
    /// The server refuses to accept the request without a defined Content-Length. The client MAY repeat the request if it 
    /// adds a valid Content-Length header field containing the length of the message-body in the request.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_411_LENGTH_REQUIRED = 411,

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
    XBL_ERROR_CODE_HTTP_STATUS_412_PRECONDITION_FAILED = 412,

    /// <summary>
    /// <b>0x8019019D</b>
    /// The server is refusing to process a request because the request entity is larger than the server is willing, or able, 
    /// to process. The server MAY close the connection to prevent the client from continuing with the request.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE = 413,

    /// <summary>
    /// <b>0x8019019E</b>
    /// The server is refusing to service the request because the Request-URI is longer than the server is willing to interpret. 
    /// This rare condition is only likely to occur when a client has improperly converted a POST Request to a GET Request with long query information.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_414_REQUEST_URI_TOO_LONG = 414,

    /// <summary>
    /// <b>0x8019019F</b>
    /// The server is refusing to service the request because the entity of the request is in a format not supported by the 
    /// requested resource for the requested method.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE = 415,

    /// <summary>
    /// <b>0x801901A0</b>
    /// A server SHOULD return a response with this status code if a requested included in a Range request-header field, and none of 
    /// the range-specifier values in this field overlap the current extent of the selected resource, and the request did not 
    /// include an If-Range request-header field.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIABLE = 416,

    /// <summary>
    /// <b>0x801901A1</b>
    /// Expect-request header failure
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_417_EXPECTATION_FAILED = 417,

    /// <summary>
    /// <b>0x801901A5</b>
    /// The request was misdirected 
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_421_MISDIRECTED_REQUEST = 421,

    /// <summary>
    /// <b>0x801901A6</b>
    /// The request was not processable
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_422_UNPROCESSABLE_ENTITY = 422,

    /// <summary>
    /// <b>0x801901A7</b>
    /// The resource was locked 
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_423_LOCKED = 423,

    /// <summary>
    /// <b>0x801901A8</b>
    /// The request failed due to dependency
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_424_FAILED_DEPENDENCY = 424,

    /// <summary>
    /// <b>0x801901AA</b>
    /// The client should upgrade to later protocol
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_426_UPGRADE_REQUIRED = 426,

    /// <summary>
    /// <b>0x801901AC</b>
    /// The server requires a precondition
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_428_PRECONDITION_REQUIRED = 428,

    /// <summary>
    /// <b>0x801901AD</b>
    /// Client is sending too many requests
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_429_TOO_MANY_REQUESTS = 429,

    /// <summary>
    /// <b>0x801901AF</b>
    /// The request headers are too large
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,

    /// <summary>
    /// <b>0x801901C1</b>
    /// The request should be retried after doing the appropriate action
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_449_RETRY_WITH = 449,

    /// <summary>
    /// <b>0x801901C3</b>
    /// The request was unavailable for legal reasons
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_451_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

    /// <summary>
    /// <b>0x801901F4</b>
    /// Corresponds to HTTP 500 Internal Server Error.This error can occur when invalid parameters are provided to the web service 
    /// via a RESTful API call, or if the web service encounters a crash or other unexpected error state. Using fiddler to examine 
    /// the failing HTTP request will often help root cause the issue.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_500_INTERNAL_SERVER_ERROR = 500,

    /// <summary>
    /// <b>0x801901F5</b>
    /// The requested service is not implemented.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_501_NOT_IMPLEMENTED = 501,

    /// <summary>
    /// <b>0x801901F6</b>
    /// The request got an invalid response to the gateway
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_502_BAD_GATEWAY = 502,

    /// <summary>
    /// <b>0x801901F7</b>
    /// The requested service is not available.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_503_SERVICE_UNAVAILABLE = 503,

    /// <summary>
    /// <b>0x801901F8</b>
    /// The HTTP gateway has timed out.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_504_GATEWAY_TIMEOUT = 504,

    /// <summary>
    /// <b>0x801901F9</b>
    /// This version of HTTP is not supported by the endpoint.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED = 505,

    /// <summary>
    /// <b>0x801901FA</b>
    /// Internal configuration error
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_506_VARIANT_ALSO_NEGOTIATES = 506,

    /// <summary>
    /// <b>0x801901FB</b>
    /// The service was unable complete the request due to storage
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_507_INSUFFICIENT_STORAGE = 507,

    /// <summary>
    /// <b>0x801901FC</b>
    /// The service detected an infinite loop while processing
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_508_LOOP_DETECTED = 508,

    /// <summary>
    /// <b>0x801901FE</b>
    /// The request requires more extensions to complete
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_510_NOT_EXTENDED = 510,

    /// <summary>
    /// <b>0x801901FF</b>
    /// The client needs to authenticate to gain network access.  Not used by Xbox Live services.
    /// </summary>
    XBL_ERROR_CODE_HTTP_STATUS_511_NETWORK_AUTHENTICATION_REQUIRED = 511,

    //////////////////////////////////////////////////////////////////////////
    // Errors from exception enabled components such as Casablanca
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// <b>0x8007000e</b>
    /// xbox_live_error_code 1000
    /// Bad alloc
    /// </summary>
    XBL_ERROR_CODE_BAD_ALLOC = 1000,

    /// <summary>
    /// <b>0x80004002</b>
    /// xbox_live_error_code 1001
    /// Bad cast
    /// </summary>
    XBL_ERROR_CODE_BAD_CAST,

    /// <summary>
    /// <b>0x80070057</b>
    /// xbox_live_error_code 1002
    /// Invalid argument
    /// </summary>
    XBL_ERROR_CODE_INVALID_ARGUMENT,

    /// <summary>
    /// <b>0x8000000b</b>
    /// xbox_live_error_code 1003
    /// Out of range
    /// </summary>
    XBL_ERROR_CODE_OUT_OF_RANGE,

    /// <summary>
    /// <b>0x80070018</b>
    /// xbox_live_error_code 1004
    /// Length error
    /// </summary>
    XBL_ERROR_CODE_LENGTH_ERROR,

    /// <summary>
    /// <b>0x8000000b</b>
    /// xbox_live_error_code 1005
    /// Range error
    /// </summary>
    XBL_ERROR_CODE_RANGE_ERROR,

    /// <summary>
    /// <b>0x8000ffff</b>
    /// xbox_live_error_code 1006
    /// Logic error
    /// </summary>
    XBL_ERROR_CODE_LOGIC_ERROR,

    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 1007
    /// Runtime error
    /// </summary>
    XBL_ERROR_CODE_RUNTIME_ERROR,

    /// <summary>
    /// <b>0x83750007</b>
    /// xbox_live_error_code 1008
    /// JSON error
    /// </summary>
    XBL_ERROR_CODE_JSON_ERROR,

    /// <summary>
    /// <b>0x83750005</b>
    /// xbox_live_error_code 1009
    /// Websocket error
    /// </summary>
    XBL_ERROR_CODE_WEBSOCKET_ERROR,

    /// <summary>
    /// <b>0x83750005</b>
    /// xbox_live_error_code 1010
    /// URI error
    /// </summary>
    XBL_ERROR_CODE_URI_ERROR,

    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 1011
    /// Generic error
    /// </summary>
    XBL_ERROR_CODE_GENERIC_ERROR,

    //////////////////////////////////////////////////////////////////////////
    // RTA errors
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 1500
    /// RTA generic error
    /// </summary>
    XBL_ERROR_CODE_RTA_GENERIC_ERROR = 1500,

    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 1501
    /// RTA subscription limit reached
    /// </summary>
    XBL_ERROR_CODE_RTA_SUBSCRIPTION_LIMIT_REACHED,

    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 1502
    /// RTA access denied
    /// </summary>
    XBL_ERROR_CODE_RTA_ACCESS_DENIED,

    //////////////////////////////////////////////////////////////////////////
    // Auth errors
    //////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 2000
    /// Unknown auth error
    /// </summary>
    XBL_ERROR_CODE_AUTH_UNKNOWN_ERROR = 2000,

    /// <summary>
    /// <b>0x8086000c</b>
    /// xbox_live_error_code 2001
    /// User interaction required
    /// </summary>
    XBL_ERROR_CODE_AUTH_USER_INTERACTION_REQUIRED,

    /// <summary>
    /// <b>0x80070525</b>
    /// xbox_live_error_code 2002
    /// User interaction required
    /// </summary>
    XBL_ERROR_CODE_AUTH_USER_SWITCHED,

    /// <summary>
    /// <b>0x80070525</b>
    /// xbox_live_error_code 2003
    /// User cancelled
    /// </summary>
    XBL_ERROR_CODE_AUTH_USER_CANCEL,

    /// <summary>
    /// <b>0x80070525</b>
    /// xbox_live_error_code 2004
    /// User not signed in
    /// </summary>
    XBL_ERROR_CODE_AUTH_USER_NOT_SIGNED_IN,

    /// <summary>
    /// <b>0x80004005</b>
    /// xbox_live_error_code 2005
    /// Auth runtime error
    /// </summary>
    XBL_ERROR_CODE_AUTH_RUNTIME_ERROR,

    /// <summary>
    /// <b>0x80070525</b>
    /// xbox_live_error_code 2006
    /// Auth no token error
    /// </summary>
    XBL_ERROR_CODE_AUTH_NO_TOKEN_ERROR,

    //////////////////////////////////////////////////////////////////////////
    // Xbox Live SDK errors
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// <b>0x8007064a</b>
    /// xbox_live_error_code 3000
    /// Could not read the xboxservices.config.  Ensure it is deployed in the package at
    /// Windows::ApplicationModel::Package::Current->InstalledLocation + "\xboxservices.config"
    /// </summary>
    XBL_ERROR_CODE_INVALID_CONFIG = 3000,

    /// <summary>
    /// <b>0x80004001</b>
    /// xbox_live_error_code 3001
    /// API is not supported
    /// </summary>
    XBL_ERROR_CODE_UNSUPPORTED = 3001,

    //////////////////////////////////////////////////////////////////////////
    // Xbox Live SDK flat C errors 
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// xbox_live_error_code 4000
    /// XblGlobalInitialize has already been called and an operation that must be called
    /// beforehand was called (i.e. XblMemSetFunctions)
    /// </summary>
    XBL_ERROR_CODE_ALREADYINITITIALIZED = 4000,

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
    XBL_ERROR_CODE_HR_ERROR_INTERNET_TIMEOUT = (int)0x80072EE2,

    /// <summary>
    /// <b>0x87DD0003</b>
    /// XASD returned an unexpected response.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XASD_UNEXPECTED = (int)0x87DD0003,

    /// <summary>
    /// <b>0x87DD0004</b>
    /// XASU returned an unexpected response.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XASU_UNEXPECTED = (int)0x87DD0004,

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
    XBL_ERROR_CODE_AM_E_XAST_UNEXPECTED = (int)0x87DD0005,

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
    XBL_ERROR_CODE_AM_E_XSTS_UNEXPECTED = (int)0x87DD0006,

    /// <summary>
    /// <b>0x87DD0007</b>
    /// XDevice returned an unexpected response.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XDEVICE_UNEXPECTED = (int)0x87DD0007,

    /// <summary>
    /// <b>0x87DD0008</b>
    /// The console is not authorized to enable development mode.
    /// </summary>
    XBL_ERROR_CODE_AM_E_DEVMODE_NOT_AUTHORIZED = (int)0x87DD0008,

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
    XBL_ERROR_CODE_AM_E_NOT_AUTHORIZED = (int)0x87DD0009,

    /// <summary>
    /// <b>0x87DD000A</b>
    /// The operation was forbidden. The server responded with a 403, and a more detailed error is not available.
    /// </summary>
    XBL_ERROR_CODE_AM_E_FORBIDDEN = (int)0x87DD000A,

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
    XBL_ERROR_CODE_AM_E_UNKNOWN_TARGET = (int)0x87DD000B,

    /// <summary>
    /// <b>0x87DD000C</b>
    /// There were problems with the JSON data downloaded from the server.
    /// </summary>
    XBL_ERROR_CODE_AM_E_INVALID_NSAL_DATA = (int)0x87DD000C,

    /// <summary>
    /// <b>0x87DD000D</b>
    /// The title has not yet been successfully authenticated.
    /// Cause: This will be returned when a title token is required, but has not been cached. For example, 
    /// attempting to retrieve an X token with title claims will fail with this error if AuthenticateTitle 
    /// hasn't been called successfully.
    /// 
    /// Resolution: Check your TitleID, SOCID, and SandboxID, or check your title's configuration on XDP.
    /// </summary>
    XBL_ERROR_CODE_AM_E_TITLE_NOT_AUTHENTICATED = (int)0x87DD000D,

    /// <summary>
    /// <b>0x87DD000E</b>
    /// XAST returned 401 when attempting to retrieve the T token.  Double-check that your device is set to the
    /// proper development sandbox and that the user has access to the sandbox.
    ///
    /// For more information, please see the "Troubleshooting Sign-in" article in the Xbox Live documentation.
    /// </summary>
    XBL_ERROR_CODE_AM_E_TITLE_NOT_AUTHORIZED = (int)0x87DD000E,

    /// <summary>
    /// <b>0x87DD0011</b>
    /// The user hash value for the specified user hasn't been recorded, so a valid token can't be generated.
    /// </summary>
    XBL_ERROR_CODE_AM_E_USER_HASH_MISSING = (int)0x87DD0011,

    /// <summary>
    /// <b>0x87DD0013</b>
    /// The Authentication Manager can't find the user for which an authentication token is being retrieved. 
    /// One possible scenario is that the User signed out, but the title still has a reference to the User object.
    /// </summary>
    XBL_ERROR_CODE_AM_E_USER_NOT_FOUND = (int)0x87DD0013,

    /// <summary>
    /// <b>0x87DD0015</b>
    /// The environment configured or specified is not valid.
    /// </summary>
    XBL_ERROR_CODE_AM_E_INVALID_ENVIRONMENT = (int)0x87DD0015,

    /// <summary>
    /// <b>0x87DD0016</b>
    /// The XASD authentication server has timed out.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XASD_TIMEOUT = (int)0x87DD0016,

    /// <summary>
    /// <b>0x87DD0017</b>
    /// The XASU authentication server has timed out.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XASU_TIMEOUT = (int)0x87DD0017,

    /// <summary>
    /// <b>0x87DD0018</b>
    /// The XAST authentication server has timed out.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XAST_TIMEOUT = (int)0x87DD0018,

    /// <summary>
    /// <b>0x87DD0019</b>
    /// The XSTS authentication server has timed out.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XSTS_TIMEOUT = (int)0x87DD0019,

    /// <summary>
    /// <b>0x87DD001A</b>
    /// Title authentication failed because a connection to Xbox Live is required, by policy, and none is present.
    /// </summary>
    XBL_ERROR_CODE_AM_E_LIVE_CONNECTION_REQUIRED = (int)0x87DD001A,

    /// <summary>
    /// <b>0x87dd001e</b>
    /// There is no network connection.
    /// </summary>
    XBL_ERROR_CODE_AM_E_NO_NETWORK = (int)0x87dd001e,

    /// <summary>
    /// <b>0x87dd0020</b>
    /// The Network Security Authorization List(NSAL) returned an unexpected response.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XTITLE_UNEXPECTED = (int)0x87dd0020,

    /// <summary>
    /// <b>0x87dd0021</b>
    /// The endpoint does not require an authorization token, but the application is attempting to 
    /// retrieve a token via GetTokenAndSignatureAsync.
    /// </summary>
    XBL_ERROR_CODE_AM_E_NO_TOKEN_REQUIRED = (int)0x87dd0021,

    /// <summary>
    /// <b>0x87dd0022</b>
    /// Timeouts were received from the various authorization servers.
    /// </summary>
    XBL_ERROR_CODE_AM_E_XTITLE_TIMEOUT = (int)0x87dd0022,

    /// <summary>
    /// <b>0x8015DC00</b>
    /// Developer mode is not authorized for the client device.
    /// </summary>
    XBL_ERROR_CODE_XO_E_DEVMODE_NOT_AUTHORIZED = (int)0x8015DC00,

    /// <summary>
    /// <b>0x8015DC01</b>
    /// A system update is required before this action can be performed.
    /// </summary>
    XBL_ERROR_CODE_XO_E_SYSTEM_UPDATE_REQUIRED = (int)0x8015DC01,

    /// <summary>
    /// <b>0x8015DC02</b>
    /// A content update is required before this action can be performed.
    /// </summary>
    XBL_ERROR_CODE_XO_E_CONTENT_UPDATE_REQUIRED = (int)0x8015DC02,

    /// <summary>
    /// <b>0x8015DC03</b>
    /// The device or user was banned.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ENFORCEMENT_BAN = (int)0x8015DC03,

    /// <summary>
    /// <b>0x8015DC04</b>
    /// The device or user was banned.
    /// </summary>
    XBL_ERROR_CODE_XO_E_THIRD_PARTY_BAN = (int)0x8015DC04,

    /// <summary>
    /// <b>0x8015DC05</b>
    /// Access to this resource has been parentally restricted.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_PARENTALLY_RESTRICTED = (int)0x8015DC05,

    /// <summary>
    /// <b>0x8015DC08</b>
    /// Access to this resource requires that the account billing information
    /// is updated.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED = (int)0x8015DC08,

    /// <summary>
    /// <b>0x8015DC0A</b>
    /// The user has not accepted the terms of use for this resource.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED = (int)0x8015DC0A,

    /// <summary>
    /// <b>0x8015DC0B</b>
    /// This resource is not available in the country associated with the user.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED = (int)0x8015DC0B,

    /// <summary>
    /// <b>0x8015DC0C</b>
    /// Access to this resource requires age verification.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED = (int)0x8015DC0C,

    /// <summary>
    /// <b>0x8015DC0D</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_CURFEW = (int)0x8015DC0D,

    /// <summary>
    /// <b>0x8015DC0E</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_CHILD_NOT_IN_FAMILY = (int)0x8015DC0E,

    /// <summary>
    /// <b>0x8015DC0F</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED = (int)0x8015DC0F,

    /// <summary>
    /// <b>0x8015DC09</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_CREATION_REQUIRED = (int)0x8015DC09,

    /// <summary>
    /// <b>0x8015DC10</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_MAINTENANCE_REQUIRED = (int)0x8015DC10,

    /// <summary>
    /// <b>0x8015DC11</b>
    /// The call was blocked because there was a conflict with the sandbox, console, application, or 
    /// your account.Verify your account, console and title settings in XDP, and check the current 
    /// Sandbox on the device.
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_TYPE_NOT_ALLOWED = (int)0x8015DC11,

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
    XBL_ERROR_CODE_XO_E_CONTENT_ISOLATION = (int)0x8015DC12,

    /// <summary>
    /// <b>0x8015DC13</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_NAME_CHANGE_REQUIRED = (int)0x8015DC13,

    /// <summary>
    /// <b>0x8015DC14</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_DEVICE_CHALLENGE_REQUIRED = (int)0x8015DC14,

    /// <summary>
    /// <b>0x8015DC16</b>
    /// The account was signed in on another device.
    /// </summary>
    XBL_ERROR_CODE_XO_E_SIGNIN_COUNT_BY_DEVICE_TYPE_EXCEEDED = (int)0x8015DC16,

    /// <summary>
    /// <b>0x8015DC17</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_PIN_CHALLENGE_REQUIRED = (int)0x8015DC17,

    /// <summary>
    /// <b>0x8015DC18</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_RETAIL_ACCOUNT_NOT_ALLOWED = (int)0x8015DC18,

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
    XBL_ERROR_CODE_XO_E_SANDBOX_NOT_ALLOWED = (int)0x8015DC19,

    /// <summary>
    /// <b>0x8015DC1A</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_ACCOUNT_SERVICE_UNAVAILABLE_UNKNOWN_USER = (int)0x8015DC1A,

    /// <summary>
    /// <b>0x8015DC1B</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_GREEN_SIGNED_CONTENT_NOT_AUTHORIZED = (int)0x8015DC1B,

    /// <summary>
    /// <b>0x8015DC1C</b>
    /// </summary>
    XBL_ERROR_CODE_XO_E_CONTENT_NOT_AUTHORIZED = (int)0x8015DC1C,

    //////////////////////////////////////////////////////////////////////////
    // Generic errors
    //////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// <b>0x800C0002</b>
    /// The URL is invalid.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_INVALID_URL = (int)0x800C0002,

    /// <summary>
    /// <b>0x800C0003</b>
    /// No session.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_NO_SESSION = (int)0x800C0003,

    /// <summary>
    /// <b>0x800C0004</b>
    /// WinINet cannot connect to the requested resource.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_CANNOT_CONNECT = (int)0x800C0004,

    /// <summary>
    /// <b>0x800C0005</b>
    /// The requested resource was not found.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_RESOURCE_NOT_FOUND = (int)0x800C0005,

    /// <summary>
    /// <b>0x800C0006</b>
    /// The requested resource was not found.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_OBJECT_NOT_FOUND = (int)0x800C0006,

    /// <summary>
    /// <b>0x800C0007</b>
    /// The requested resource was not found.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_DATA_NOT_AVAILABLE = (int)0x800C0007,

    /// <summary>
    /// <b>0x800C0008</b>
    /// Operation restricted by the current inability to discover or join the multiplayer session, or 
    /// the player does not have the multiplayer privilege.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_DOWNLOAD_FAILURE = (int)0x800C0008,

    /// <summary>
    /// <b>0x800C0009</b>
    /// Authentication is required to access this resource.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_AUTHENTICATION_REQUIRED = (int)0x800C0009,

    /// <summary>
    /// <b>0x800C000A</b>
    /// No valid media
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_NO_VALID_MEDIA = (int)0x800C000A,

    /// <summary>
    /// <b>0x800C000B</b>
    /// The connection has timed out.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_CONNECTION_TIMEOUT = (int)0x800C000B,

    /// <summary>
    /// <b>0x800C000C</b>
    /// Invalid request
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_INVALID_REQUEST = (int)0x800C000C,

    /// <summary>
    /// <b>0x800C000D</b>
    /// The requested protocol is unknown.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_UNKNOWN_PROTOCOL = (int)0x800C000D,

    /// <summary>
    /// <b>0x800C000E</b>
    /// Security problem
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_SECURITY_PROBLEM = (int)0x800C000E,

    /// <summary>
    /// <b>0x800C000F</b>
    /// Cannot load data
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_CANNOT_LOAD_DATA = (int)0x800C000F,

    /// <summary>
    /// <b>0x800C0010</b>
    /// Cannot instantiate object
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_CANNOT_INSTANTIATE_OBJECT = (int)0x800C0010,

    /// <summary>
    /// <b>0x800C0019</b>
    /// The certificate presented for the request is invalid.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_INVALID_CERTIFICATE = (int)0x800C0019,

    /// <summary>
    /// <b>0x800C0014</b>
    /// The redirect to a different endpoint has failed.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_REDIRECT_FAILED = (int)0x800C0014,

    /// <summary>
    /// <b>0x800C0015</b>
    /// A resource request has been directed to a directory rather than an individual resource.
    /// </summary>
    XBL_ERROR_CODE_HR_INET_E_REDIRECT_TO_DIR = (int)0x800C0015,

    /// <summary>
    /// <b>0x800704cf</b>
    /// The network location cannot be reached.
    /// </summary>
    XBL_ERROR_CODE_HR_ERROR_NETWORK_UNREACHABLE = (int)0x800704cf,

    //////////////////////////////////////////////////////////////////////////
    // LibHttpClient errors
    //////////////////////////////////////////////////////////////////////////
    XBL_ERROR_CODE_HC_FAIL = -1,
    XBL_ERROR_CODE_HC_POINTER = -2,
    XBL_ERROR_CODE_HC_INVALIDARG = -3,
    XBL_ERROR_CODE_HC_OUTOFMEMORY = -4,
    XBL_ERROR_CODE_HC_BUFFERTOOSMALL = -5,
    XBL_ERROR_CODE_HC_NOTINITIALIZED = -6,
    XBL_ERROR_CODE_HC_FEATURENOTPRESENT = -7,
    XBL_ERROR_CODE_HC_PERFORMALREADYCALLED = -8,
    XBL_ERROR_CODE_HC_ALREADYINITIALISED = -9,

} XBL_ERROR_CODE;

typedef struct XBL_RESULT
{
    XBL_ERROR_CONDITION errorCondition;
    // TODO rename
    XBL_ERROR_CODE _errorCode;
} XBL_RESULT;

const XBL_RESULT XBL_RESULT_OK { XBL_ERROR_CONDITION_NO_ERROR, XBL_ERROR_CODE_NO_ERROR };
const XBL_RESULT XBL_RESULT_INVALID_ARG{ XBL_ERROR_CONDITION_GENERIC_ERROR, XBL_ERROR_CODE_INVALID_ARGUMENT };

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)