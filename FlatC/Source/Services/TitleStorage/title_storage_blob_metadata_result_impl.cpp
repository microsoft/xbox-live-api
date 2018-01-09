// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "title_storage_blob_metadata_result_impl.h"
#include "title_storage_blob_metadata_impl.h"

using namespace xbox::services::title_storage;

void XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT_IMPL::update(const title_storage_blob_metadata_result& cppObject)
{
    std::lock_guard<std::mutex> lock(m_lock);

    m_cppObject = std::move(cppObject);
    auto& cppItems = m_cppObject.items();

    for (auto& metadata : m_items)
    {
        if (metadata.pImpl != nullptr)
        {
            delete metadata.pImpl;
            metadata.pImpl = nullptr;
        }
    }

    m_items = std::vector<XSAPI_TITLE_STORAGE_BLOB_METADATA>(cppItems.size());
    for (unsigned int i = 0; i < m_items.size(); ++i)
    {
        m_items[i].pImpl = new XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL(&m_items[i], cppItems[i]);
    }
}

const XSAPI_TITLE_STORAGE_BLOB_METADATA* XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT_IMPL::first_item() const
{
    return &m_items[0];
}

size_t XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT_IMPL::item_count() const
{
    return m_items.size();
}

/* const */ title_storage_blob_metadata_result& XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT_IMPL::cppObject() /* const */
{
    return m_cppObject;
}