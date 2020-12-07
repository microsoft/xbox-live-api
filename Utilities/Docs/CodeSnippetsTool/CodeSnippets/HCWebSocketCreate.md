```cpp
HCWebsocketHandle websocket = nullptr;
HRESULT hr = HCWebSocketCreate(&websocket, WebsocketMessageReceived, nullptr, WebsocketClosed, nullptr);
```
