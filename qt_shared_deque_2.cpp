// qt_shared_deque_2.cpp
// Producer: repeatedly prompts for integers and enqueues them
// into the shared ring buffer as long as there is space.
#include <iostream>
#include <limits>

#include "shared_ring_buffer.h"

int main(int argc, char *argv[])
{
  // Must match the size the consumer requests in qt_shared_deque_1.cpp
  constexpr long int kCapacity = 100;

  SharedRingBuffer<int> buffer;

  if (!buffer.create(kCapacity)) {
    std::cerr << "Could not create or attach to shared memory" << std::endl;
    return 1;
  }

  // keep prompting as long as there is space in the buffer
  while (buffer.getCount() < kCapacity) {
    std::cout << "$>" << std::flush;

    int value;
    if (!(std::cin >> value)) {
      if (std::cin.eof()) {
        // Ctrl+D: stop cleanly so the destructor detaches from the segment
        std::cout << std::endl << "End of input, exiting." << std::endl;
        return 0;
      }
      // not a number: clear the error and discard the rest of the line
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Not an integer, try again." << std::endl;
      continue;
    }

    if (buffer.enqueue(value)) {
      std::cout << "Enqueued: " << value << std::endl;
    }
    else {
      std::cout << "Buffer full, " << value << " was not enqueued." << std::endl;
    }
  }

  std::cout << "Buffer is full, no more space." << std::endl;
  return 0;
}