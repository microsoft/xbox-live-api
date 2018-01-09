// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/title_storage_c.h"

class XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT_IMPL
{
public:
    void update(const xbox::services::title_storage::title_storage_blob_metadata_result& cppObject);
    const XSAPI_TITLE_STORAGE_BLOB_METADATA* first_item() const;
    size_t item_count() const;
    // TODO We should be able to just return a const ref here, but the cpp get_next API needs to be marked as const first
    /* const */ xbox::services::title_storage::title_storage_blob_metadata_result& cppObject() /* const */;

private:
    std::mutex m_lock;
    std::vector<XSAPI_TITLE_STORAGE_BLOB_METADATA> m_items;
    xbox::services::title_storage::title_storage_blob_metadata_result m_cppObject;
};