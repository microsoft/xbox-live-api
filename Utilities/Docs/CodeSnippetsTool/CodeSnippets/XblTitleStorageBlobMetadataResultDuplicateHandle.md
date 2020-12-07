```cpp
XblTitleStorageBlobMetadataResultHandle handle = blobMetadataResultHandle;
XblTitleStorageBlobMetadataResultHandle duplicatedHandle;

HRESULT hr = XblTitleStorageBlobMetadataResultDuplicateHandle(handle, &duplicatedHandle);
```
