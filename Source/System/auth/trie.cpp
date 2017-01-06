//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "trie.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::vector<string_t> GetSegments(string_t path)
{
    std::vector<string_t> segments;
    stringstream_t sstream(path);

    while (!sstream.eof())
    {
        string_t segment;
        std::getline(sstream, segment, _T('/'));

        if (!segment.empty())
        {
            segments.push_back(segment);
        }
    }

    return segments;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
