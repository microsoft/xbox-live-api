/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
const std::wstring statValueDocumentResponse = LR"({
    "ver": 1, 
    "envelope": {
        "serverVersion": 1,
        "clientVersion": 1,
        "clientId": "feb26f16-2348-48f1-8161-fb5723bffdce"
    },
    "timestamp": "2016-10-07T23:04:08Z",
    "stats": {
        "contextualKeys": {},
        "title": {
            "headshots": { "value": 7, "op": "add" },
            "fastestRound": { "value": 7, "op": "min" },
            "longestJump": { "value": 9.5, "op": "max" },
            "strangeStat": { "value": "foo", "op": "replace" }
        }
    }
})";

const std::wstring statValueDocumentResponseWithContext = LR"({
    "ver": 1, 
    "envelope": {
        "serverVersion": 1,
        "clientVersion": 1,
        "clientId": "feb26f16-2348-48f1-8161-fb5723bffdce"
    },
    "timestamp": "2016-10-07T23:04:08Z",
    "stats": {
        "contextualKeys": { "race": "human", "class": "wizard" },
        "title": {
            "headshots": { "value": 7, "op": "add" },
            "fastestRound": { "value": 7, "op": "min" },
            "longestJump": { "value": 9.5, "op": "max" },
            "strangeStat": { "value": "foo", "op": "replace" }
        }
    }
})";