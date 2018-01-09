// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/title_storage_c.h"
#include "title_storage_blob_metadata_result_impl.h"

class title_storage_state
{
public:
    std::recursive_mutex m_lock;

    // For now we only ever cache the most recent metadata result. Might consider changing this in the future
    XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT_IMPL m_blobMetadataResultImpl;
    std::set<const XSAPI_TITLE_STORAGE_BLOB_METADATA*> m_blobMetadata;
};