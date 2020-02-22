#pragma once

#include "quill/detail/misc/Attributes.h"
#include <atomic>
#include <cstddef>
#include <cstdint>

template <typename TBaseObject, size_t Capacity>
class SPSCQueue
{
public:
  SPSCQueue()
    : _producer_pos(_storage), _end_of_recorded_space(_storage + Capacity), _min_free_space(Capacity)
  {
  }

  template <typename TInsertedObject, typename... Args>
  QUILL_NODISCARD_ALWAYS_INLINE_HOT bool try_emplace(Args&&... args) noexcept
  {
    auto pos = reserveProducerSpace(sizeof(TInsertedObject));

    // Get the pointer to the beginning of the buffer
    // copy construct the Message there
    new (pos) TInsertedObject{std::forward<Args>(args)...};

    // update the head
    _producer_pos.store(pos + sizeof(TInsertedObject), std::memory_order_release);
    return true;
    ;
  }

private:
  inline unsigned char* reserveProducerSpace(size_t nbytes)
  {

    // Fast in-line path
    if (nbytes < _min_free_space)
    {
      return _producer_pos.load(std::memory_order_relaxed);
    }

    // Slow allocation
    return reserveSpaceInternal(nbytes);
  }

  unsigned char* reserveSpaceInternal(size_t nbytes)
  {
    const unsigned char* endOfBuffer = _storage + Capacity;

    // There's a subtle point here, all the checks for remaining
    // space are strictly < or >, not <= or => because if we allow
    // the record and print positions to overlap, we can't tell
    // if the buffer either completely full or completely empty.
    // Doing this check here ensures that == means completely empty.
    while (_min_free_space <= nbytes)
    {
      // Since consumerPos can be updated in a different thread, we
      // save a consistent copy of it here to do calculations on
      unsigned char* cachedConsumerPos = _consumer_pos.load(std::memory_order_relaxed);
      unsigned char* producerPos = _producer_pos.load(std::memory_order_relaxed);

      // Avoid wrapping around second time
      if (cachedConsumerPos <= producerPos)
      {
        _min_free_space = endOfBuffer - producerPos;

        if (_min_free_space > nbytes)
        {
          break;
        }

        // Not enough space at the end of the buffer; wrap around
        _end_of_recorded_space = producerPos;

        // Prevent the roll over if it overlaps the two positions because
        // that would imply the buffer is completely empty when it's not.
        if (cachedConsumerPos != _storage)
        {
          // prevents producerPos from updating before endOfRecordedSpace

          _producer_pos.store(_storage, std::memory_order_relaxed);
          _min_free_space = cachedConsumerPos - producerPos;
        }
      }
      else
      {
        // we had already rolled over once, so just update
        _min_free_space = cachedConsumerPos - producerPos;
      }
    }
    return _producer_pos.load(std::memory_order_relaxed);
  }

private:
  // Position within storage[] where the producer may place new data
  std::atomic<unsigned char*> _producer_pos;

  // Marks the end of valid data for the consumer. Set by the producer
  // on a roll-over
  std::atomic<unsigned char*> _end_of_recorded_space;

  // Lower bound on the number of bytes the producer can allocate w/o
  // rolling over the producerPos or stalling behind the consumer
  uint64_t _min_free_space;

  // Position within the storage buffer where the consumer will consume
  // the next bytes from. This value is only updated by the consumer.
  std::atomic<unsigned char*> _consumer_pos;

  // Backing store used to implement the circular queue
  unsigned char _storage[Capacity];
};