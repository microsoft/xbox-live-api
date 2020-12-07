```cpp
void Test_XblCallRoutedHandler(
    _In_ XblServiceCallRoutedArgs,
    _In_ void*)
{
}
// CODE SNIPPET START: XblCallRoutedHandler


int XblAddServiceCallRoutedHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsAddServiceCallRoutedHandler
    XblFunctionContext fn = XblAddServiceCallRoutedHandler(Test_XblCallRoutedHandler, nullptr);
```
