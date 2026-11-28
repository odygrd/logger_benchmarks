#pragma once

#include "quill/core/FrontendOptions.h"

// Every option not set here keeps Quill's default, including the queue capacity of the default
// unbounded queue and the immediate flush check
struct CustomFrontendOptions : quill::FrontendOptions
{
#ifdef QUILL_USE_BOUNDED_DROPPING_QUEUE
  static constexpr quill::QueueType queue_type = quill::QueueType::BoundedDropping;

  // A bounded queue never grows, so it is sized for the load, as an application would: 1 MiB per
  // thread, the default of fmtlog's fixed-size queue. Quill's default of 128 KiB holds about six
  // bursts of the complex types payload and drops messages whenever the backend thread is
  // descheduled for longer, which happens on a machine without isolated CPUs. A run that drops a
  // message is not published, and a dropped call skips the copy, so it would not be comparable.
  static constexpr size_t initial_queue_capacity = 1024u * 1024u;
#endif

#ifdef QUILL_USE_HUGE_PAGES
  static constexpr quill::HugePagesPolicy huge_pages_policy = quill::HugePagesPolicy::Always;
#endif
};
