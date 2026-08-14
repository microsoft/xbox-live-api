// Build validation for the XSAPI flat C API.
//
// This is a link-time smoke test, not a functional test. It references a
// representative set of public APIs so that the linker has to resolve each one,
// which catches the case where an API is declared in the headers but is not
// actually present in the library or the DLL's export table.
//
// It builds against the static library by default. The Thunks DLL is validated
// by building the same source with XSAPI_BUILD_VALIDATION_DYNAMIC defined.
//
// The API calls are behind a condition the compiler cannot fold away, so the
// references survive optimization while nothing is invoked at runtime. That
// keeps the executable safe to launch, which is what proves the DLL's exports
// resolve when the dynamic configuration is used.

#include <cstdio>

#include <xsapi-c/services_c.h>

namespace
{
    void CALLBACK OnMuteListChanged(const XblPrivacyMuteListChangeEventArgs*, void*)
    {
    }

    void CALLBACK OnBlockListChanged(const XblPrivacyBlockListChangeEventArgs*, void*)
    {
    }

    void CALLBACK OnFriendRequestCountChanged(const XblSocialFriendRequestCountChangedEventArgs*, void*)
    {
    }

    // The four privacy handlers and the two social handlers were absent from the
    // Thunks DLL's export table while being present in the static library, so
    // they are called out explicitly here.
    void ReferenceListChangedHandlers(XblContextHandle context)
    {
        XblFunctionContext muteToken = XblPrivacyAddMuteListChangedHandler(context, OnMuteListChanged, nullptr);
        XblFunctionContext blockToken = XblPrivacyAddBlockListChangedHandler(context, OnBlockListChanged, nullptr);

        XblFunctionContext friendRequestToken{};
        XblSocialAddFriendRequestCountChangedHandler(context, OnFriendRequestCountChanged, nullptr, &friendRequestToken);

        XblPrivacyRemoveMuteListChangedHandler(context, muteToken);
        XblPrivacyRemoveBlockListChangedHandler(context, blockToken);
        XblSocialRemoveFriendRequestCountChangedHandler(context, friendRequestToken);
    }

    void ReferenceCoreApis(XblContextHandle context)
    {
        XblContextGetUser(context, nullptr);
        XblContextCloseHandle(context);
        XblCleanupAsync(nullptr);
    }
}

int main(int argc, char** argv)
{
    (void)argv;

    // Always false in practice, but not provably so at compile time.
    if (argc > 1000)
    {
        XblContextHandle context{};
        ReferenceListChangedHandlers(context);
        ReferenceCoreApis(context);
    }

#if defined(XSAPI_BUILD_VALIDATION_DYNAMIC)
    std::printf("XSAPI build validation passed (dynamic / Thunks DLL).\n");
#else
    std::printf("XSAPI build validation passed (static library).\n");
#endif

    return 0;
}
