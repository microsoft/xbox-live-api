#include "pch.h"

void InitializeAsync(_In_ async_queue_handle_t queue, _Out_ uint32_t* callbackToken);
void CleanupAsync(_In_ async_queue_handle_t queue, _In_ uint32_t callbackToken);
void DrainAsyncCompletionQueueUntilEmpty(_In_ async_queue_handle_t queue);
bool DrainAsyncCompletionQueue(_In_ async_queue_handle_t queue, _In_ uint32_t maxItemsToDrain);
bool DrainAsyncCompletionQueueWithTimeout(_In_ async_queue_handle_t queue, _In_ double stopAfterMilliseconds);