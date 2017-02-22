// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "InventoryItem_WinRT.h"

using namespace Microsoft::Xbox::Services::Marketplace;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

InventoryItem::InventoryItem(
    _In_ xbox::services::marketplace::inventory_item cppObj
    ):
    m_cppObj(std::move(cppObj))
{
    m_containerIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.container_ids())->GetView();
}

IVectorView<String^>^
InventoryItem::ContainerIds::get()
{
    return m_containerIds;
}

xbox::services::marketplace::inventory_item
InventoryItem::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END