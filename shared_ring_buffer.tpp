// shared_ring_buffer.tpp
#include "shared_ring_buffer.h"

#include <QDebug>

// for placement new
#include <new> 

template <typename T>
SharedRingBuffer<T>::SharedRingBuffer(): sharedMemory("SharedRingBuffer")
{
  shm_data = nullptr;
}

template <typename T>
SharedRingBuffer<T>::~SharedRingBuffer()
{
  sharedMemory.detach();
}

template <typename T>
bool SharedRingBuffer<T>::create(std::size_t requestedLength)
{
  std::size_t byteSize = sizeof(SharedData) + sizeof(T)*requestedLength;

  // try to create
  // typical race condition here
  // which process reaches this point first?
  // and how to we tell for sure?
  bool createdByThisProcess = false;
  if (!sharedMemory.create(byteSize)) {
    // failed, check if the segment already exists
    if(sharedMemory.error() == QSharedMemory::AlreadyExists){
      qDebug() << "Already exists: " << sharedMemory.nativeKey();
      // try to attach
      if(!sharedMemory.attach()){
	qDebug() << "but could not attach";
	return false;
      }
      byteSize = sharedMemory.size();
    }
    else{
      qDebug() << "Unable to create shared memory segment: "
	       << sharedMemory.errorString();
      return false;
    }
  }
  else{
    createdByThisProcess = true;
  }
  
  sharedMemory.lock();
    
  // this is the start of the shared memory block
  void * shm_ptr = static_cast<void *>(sharedMemory.data());

  shm_data = new(shm_ptr) SharedData;

  if(createdByThisProcess){
    shm_data->queueLength = requestedLength;
    shm_data->front = 0;
    shm_data->back = requestedLength-1;
    shm_data->count = 0;
  }

  data = reinterpret_cast<T *> (static_cast<char*>(shm_ptr) + sizeof(SharedData));

  sharedMemory.unlock();
  
  return true;
}

template <typename T>
bool SharedRingBuffer<T>::enqueue(const T& item)
{
  bool rv = false;
  
  sharedMemory.lock();
  
  if(shm_data->count != shm_data->queueLength){
    shm_data->back = (shm_data->back + 1) % shm_data->queueLength;
    *(data + shm_data->back) = item;
    ++(shm_data->count);
    rv = true;
  }
  
  sharedMemory.unlock();
  
  return rv;
}

template <typename T>
T SharedRingBuffer<T>::dequeue()
{
  T value;
  
  sharedMemory.lock();
  
  value = *(data + shm_data->front);
  shm_data->front = (shm_data->front +1) % shm_data->queueLength;
  --(shm_data->count);

  sharedMemory.unlock();
  
  return value;
}

template <typename T>
long int SharedRingBuffer<T>::getCount()
{
  long int count;
  
  sharedMemory.lock();
  count = shm_data->count;
  sharedMemory.unlock();

  return count;
}
