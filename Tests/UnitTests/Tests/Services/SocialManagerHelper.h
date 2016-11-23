/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "xbox_live_context_impl.h"

const std::wstring peoplehubResponse =
LR"(
{
    "people": [
        {
            "xuid": "2814664990767463",
            "isFavorite": true,
            "isFollowingCaller": true,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 123410299",
            "realName": "Dev Account",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KIwuPiuIs6TLDV4WsQAGzSwnTHQB9h_UfPa19pe4OAgFTWAPsXVneopydpL6qncU1N&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 123410299",
            "gamerScore": "0",
            "xboxOneRep": "GoodPlayer",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        },
        {
            "xuid": "2814654081790887",
            "isFavorite": true,
            "isFollowingCaller": false,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 554865546",
            "realName": "",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KI6TRSO7kB1LAmJSmUVW5wJqn2n6xd9r5UHGMgD0o0KoOwHN61vlgC862huSRkTjhm&background=0xababab&mode=Padding&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 554865546",
            "gamerScore": "0",
            "xboxOneRep": "GoodPlayer",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        },
        {
            "xuid": "2814613569642996",
            "isFavorite": false,
            "isFollowingCaller": false,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 417471033",
            "realName": "",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KI6TRSO7kB1LAmJSmUVW5wJqn2n6xd9r5UHGMgD0o0KoOwHN61vlgC862huSRkTjhm&background=0xababab&mode=Padding&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 417471033",
            "gamerScore": "0",
            "xboxOneRep": "InDanger",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        },
        {
            "xuid": "2814671404555632",
            "isFavorite": false,
            "isFollowingCaller": true,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 766909125",
            "realName": "",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KI6TRSO7kB1LAmJSmUVW5wJqn2n6xd9r5UHGMgD0o0KoOwHN61vlgC862huSRkTjhm&background=0xababab&mode=Padding&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 766909125",
            "gamerScore": "0",
            "xboxOneRep": "AvoidMe",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": {
                "TitleName": "Forza Horizon 2",
                "TitleId": "446059611",
                "LastTimePlayed": "2015-01-26T22:54:54.6630000Z"
            },
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            },{
                "IsBroadcasting": false,
                "Device": "Xbox",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        }
    ]
})";

const std::wstring peoplehubOversizedResponse =
LR"(
{
    "people": [
        {
            "xuid": "281466499076746312341234",
            "isFavorite": true,
            "isFollowingCaller": true,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 123410299 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            "realName": "Dev Account 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KIwuPiuIs6TLDV4WsQAGzSwnTHQB9h_UfPa19pe4OAgFTWAPsXVneopydpL6qncU1N&format=png=fffffffffffffffffffffffffffffffffffff",
            "useAvatar": true,
            "gamertag": "2 Dev 123410299 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            "gamerScore": "0",
            "xboxOneRep": "GoodPlayer",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c1023452345",
                "secondaryColor": "05d21e2345",
                "tertiaryColor": "299c102345"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null
        }
    ]
})";


class SocialManagerHelper
{
public:
    static xbox::services::social::manager::peoplehub_service GetPeoplehubService()
    {
        auto xblContext = GetMockXboxLiveContext_WinRT();
        auto xblContextImpl = std::make_shared<xbox::services::xbox_live_context_impl>(xblContext->User);
        xblContextImpl->init();

        return xbox::services::social::manager::peoplehub_service(
            xblContextImpl->user_context(),
            xblContextImpl->settings(),
            xblContextImpl->application_config()
        );
    }
};