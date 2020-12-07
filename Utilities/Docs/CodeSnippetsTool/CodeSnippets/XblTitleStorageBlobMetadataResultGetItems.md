```cpp
XblTitleStorageBlobMetadataResultHandle handle = blobMetadataResultHandle;

const XblTitleStorageBlobMetadata* items;
size_t itemsSize;

HRESULT hr = XblTitleStorageBlobMetadataResultGetItems(handle, &items, &itemsSize);
```
