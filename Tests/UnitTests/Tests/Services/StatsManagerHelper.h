/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
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