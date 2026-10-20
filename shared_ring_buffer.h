// shared_ring_buffer.h
// implement a deque as a ring buffer
//  in a Qt shared memory segment

#ifndef _SHARED_RING_BUFFER_H_
#define _SHARED_RING_BUFFER_H_

#include <QSharedMemory>

#include <type_traits>

// The type T must be POD
template <typename T>
class SharedRingBuffer
{
  static_assert(std::is_pod<T>::value, "T must be POD");
  
public:
  SharedRingBuffer();
  ~SharedRingBuffer();

  bool create(std::size_t requestedLength);
  
  bool enqueue(const T& item);
  T dequeue();
  long int getCount();
  
private:
  // per-process variable
  QSharedMemory sharedMemory;
  T* data;
    
  // shared variables
  struct SharedData
  {
    std::size_t queueLength;
    long int front;
    long int back;
    long int count;
  };

  SharedData * shm_data;
};

#include "shared_ring_buffer.tpp"

#endif // _SHARED_RING_BUFFER_H_
