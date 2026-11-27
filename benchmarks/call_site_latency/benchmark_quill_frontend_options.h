#pragma once

#include "quill/core/FrontendOptions.h"

// Every option not set here keeps Quill's default, including the queue capacity and the
// immediate flush check
struct CustomFrontendOptions : quill::FrontendOptions
{
#ifdef QUILL_USE_BOUNDED_DROPPING_QUEUE
  static constexpr quill::QueueType queue_type = quill::QueueType::BoundedDropping;
#endif

#ifdef QUILL_USE_HUGE_PAGES
  static constexpr quill::HugePagesPolicy huge_pages_policy = quill::HugePagesPolicy::Always;
#endif
};
