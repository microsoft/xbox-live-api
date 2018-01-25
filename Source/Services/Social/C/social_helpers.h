// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class xbl_xbox_social_relationship_result_wrapper
{
public:
    xbl_xbox_social_relationship_result_wrapper(xbox_social_relationship_result socialRelationshipResult);
    ~xbl_xbox_social_relationship_result_wrapper();

    const XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT* xbl_xbox_social_relationship_result() const;

private:
    XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT m_xblXboxSocialRelationshipResult;
    xsapi_internal_vector<XBL_XBOX_SOCIAL_RELATIONSHIP> m_items;

    xbox_social_relationship_result m_socialRelationshipResult;
};

class xbl_social_relationship_change_event_args_wrapper
{
public:
    xbl_social_relationship_change_event_args_wrapper(social_relationship_change_event_args eventArgs);
    ~xbl_social_relationship_change_event_args_wrapper();

    const XBL_SOCIAL_RELATIONSHIP_CHANGE_EVENT_ARGS& xbl_social_relationship_change_event_args() const;

private:
    XBL_SOCIAL_RELATIONSHIP_CHANGE_EVENT_ARGS m_xblSocialRelationshipChangeEventArgs;
    social_relationship_change_event_args m_socialRelationshipChangeEventArgs;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END