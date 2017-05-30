// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
const std::wstring statValueDocumentResponse = LR"({
    "revision": 1,
    "timestamp": "2016-10-07T23:04:08Z",
    "stats": {
        "title": {
            "headshots": { "value": 7 },
            "fastestRound": { "value": 7 },
            "longestJump": { "value": 9.5 },
            "strangeStat": { "value": "foo" }
        }
    }
})";

const std::wstring statValueDocumentResponseWithContext = LR"({
    "revision": 1,
    "timestamp": "2016-10-07T23:04:08Z",
    "stats": {
        "title": {
            "headshots": { "value": 7, "op": "replace" },
            "fastestRound": { "value": 7, "op": "replace" },
            "longestJump": { "value": 9.5, "op": "replace" },
            "strangeStat": { "value": "foo", "op": "replace" }
        }
    }
})";

const std::wstring defaultLeaderboardData =
LR"({
    "pagingInfo": {
        "continuationToken": "3",
        "totalItems": 6
    },
    "leaderboardInfo": {
        "displayName": "jumps",
        "totalCount": 6,
        "columns": [{
            "displayName": "Total Jumps",
            "statName": "jumps",
            "type": "Double"
        }]
    },
    "userList": [{
        "gamertag": "2 Dev 240641524",
        "xuid": "2814670835245896",
        "percentile": 0.16666666666666666,
        "rank": 1,
        "values": ["96"]
    },
    {
        "gamertag": "2 Dev 109176772",
        "xuid": "2814648765577694",
        "percentile": 0.33333333333333331,
        "rank": 2,
        "values": ["20"]
    }]
})";