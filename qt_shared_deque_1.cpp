// qt_shared_deque_1.cpp
#include <iostream>
#include <cassert>

#include "shared_ring_buffer.h"

int main(int argc, char *argv[])
{
  SharedRingBuffer<int> buffer;

  buffer.create(100);

  while(true){
    if(buffer.getCount() > 0){
      int value = buffer.dequeue();
      std::cout << "Dequeued: " << value << std::endl;
      break;
    }
  }
  
  return 0;
}
