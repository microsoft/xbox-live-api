// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "internal_errors.h"
#include "async_helpers.h"

// Regression guard for AB#63480128.
//
// Result<T> and AsyncContext<T> are the shared plumbing every XSAPI service response travels
// through. Both used to take their payload by value and then copy it again into storage / into
// the callback, so a single service response could be duplicated several times before reaching
// the caller. That is invisible for small payloads, but Social Manager moves a Vector of
// XblSocialManagerUser through this path, and that struct is ~5.7KB each. For a 500-friend graph
// the payload is ~2.7MB, and every redundant copy added another 2.7MB to peak working set.
//
// These tests pin the move behavior with a payload that counts its own copies, so the copies
// cannot be silently reintroduced by a later refactor.

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace
{

// Counts copies and moves made of it. Copy/move counts are global rather than per-instance so a
// count survives the instance it was taken from being destroyed mid-chain.
struct CopyCounter
{
    static size_t s_copies;
    static size_t s_moves;

    static void Reset() noexcept
    {
        s_copies = 0;
        s_moves = 0;
    }

    CopyCounter() = default;
    CopyCounter(const CopyCounter&) noexcept { ++s_copies; }
    CopyCounter(CopyCounter&&) noexcept { ++s_moves; }
    CopyCounter& operator=(const CopyCounter&) noexcept { ++s_copies; return *this; }
    CopyCounter& operator=(CopyCounter&&) noexcept { ++s_moves; return *this; }
};

size_t CopyCounter::s_copies{ 0 };
size_t CopyCounter::s_moves{ 0 };

} // anonymous namespace

DEFINE_TEST_CLASS(AsyncPayloadTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(AsyncPayloadTests)

    // Result<T>'s payload constructor takes T by value. Constructing a Result from an rvalue should
    // therefore cost moves only - the by-value parameter is move-constructed from the caller's
    // rvalue, and the member is move-constructed from the parameter. Before the fix the member was
    // copy-initialized from the (named, and therefore lvalue) parameter, so this cost a full copy.
    DEFINE_TEST_CASE(TestResultConstructionFromRvalueDoesNotCopy)
    {
        TEST_LOG(L"Test starting: TestResultConstructionFromRvalueDoesNotCopy");

        CopyCounter::Reset();

        Result<CopyCounter> result{ CopyCounter{} };

        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, CopyCounter::s_copies);
        VERIFY_IS_TRUE(CopyCounter::s_moves > 0);
    }

    // Same guarantee for the std::error_code overload, which had the identical defect.
    DEFINE_TEST_CASE(TestResultErrorCodeConstructionFromRvalueDoesNotCopy)
    {
        TEST_LOG(L"Test starting: TestResultErrorCodeConstructionFromRvalueDoesNotCopy");

        CopyCounter::Reset();

        Result<CopyCounter> result{ CopyCounter{}, std::error_code{} };

        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, CopyCounter::s_copies);
        VERIFY_IS_TRUE(CopyCounter::s_moves > 0);
    }

    // ExtractPayload returns T&& so the caller can take ownership. Verify it does not copy, since
    // this is the step that hands a Peoplehub response to the social graph.
    DEFINE_TEST_CASE(TestExtractPayloadDoesNotCopy)
    {
        TEST_LOG(L"Test starting: TestExtractPayloadDoesNotCopy");

        Result<CopyCounter> result{ CopyCounter{} };

        CopyCounter::Reset();
        CopyCounter extracted{ result.ExtractPayload() };

        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, CopyCounter::s_copies);
        VERIFY_ARE_EQUAL_INT(size_t{ 1 }, CopyCounter::s_moves);
    }

    // AsyncContext::Complete takes its arguments by value, so they are private to the call and can
    // be moved into the callback. Before the fix they were passed as lvalues into a
    // Function<void(Args...)> that also takes by value, costing another full copy of the payload.
    DEFINE_TEST_CASE(TestAsyncContextCompleteDoesNotCopyPayload)
    {
        TEST_LOG(L"Test starting: TestAsyncContextCompleteDoesNotCopyPayload");

        size_t observedCopiesAtCallback{ 0 };

        AsyncContext<CopyCounter> context{
            [&](CopyCounter)
            {
                observedCopiesAtCallback = CopyCounter::s_copies;
            }
        };

        CopyCounter::Reset();
        context.Complete(CopyCounter{});

        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, observedCopiesAtCallback);
        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, CopyCounter::s_copies);
    }

    // The end-to-end shape of the Peoplehub path: a large payload is built, wrapped in a Result,
    // delivered through an AsyncContext, and extracted by the handler. This whole chain must not
    // copy the payload even once. Uses a real Vector so a regression shows up as a vector buffer
    // copy, which is exactly the allocation that drove peak memory in the reported bug.
    DEFINE_TEST_CASE(TestResultThroughAsyncContextDoesNotCopyPayload)
    {
        TEST_LOG(L"Test starting: TestResultThroughAsyncContextDoesNotCopyPayload");

        constexpr size_t c_payloadSize{ 500 };

        Vector<CopyCounter> extractedPayload;

        AsyncContext<Result<Vector<CopyCounter>>> context{
            [&](Result<Vector<CopyCounter>> result)
            {
                extractedPayload = result.ExtractPayload();
            }
        };

        Vector<CopyCounter> payload(c_payloadSize);

        CopyCounter::Reset();
        context.Complete(Result<Vector<CopyCounter>>{ std::move(payload) });

        VERIFY_ARE_EQUAL_INT(c_payloadSize, extractedPayload.size());
        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, CopyCounter::s_copies);

        // Moving a Vector steals its buffer, so none of its elements should be touched at all.
        VERIFY_ARE_EQUAL_INT(size_t{ 0 }, CopyCounter::s_moves);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
