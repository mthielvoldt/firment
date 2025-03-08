#include "queue.h"
#include <crit_section_port.h>
#include <memory.h>

// Storage

void initQueue(
    size_t itemSize, 
    uint32_t length, 
    queue_t *queue, 
    uint8_t *itemsStorage, 
    uint32_t highestSenderPriority)
{
  // utilizes auto-zeroing of unspecified elements.
  *queue = (queue_t){
    .highestSenderPriority = highestSenderPriority,
    .items = itemsStorage,
    .itemSize = itemSize,
    .maxNumItems = length,
  };
}

bool enqueueBack(queue_t *queue, const void *src)
{
  bool success = false;
  int_fast16_t indexToWrite;

  disableLowPriorityInterrupts(queue->highestSenderPriority);
  if (queue->numSlotsClaimed < queue->maxNumItems)
  {
    indexToWrite = queue->front + queue->numSlotsClaimed;
    queue->numSlotsClaimed++;
    success = true;
  }
  enableAllInterrupts();

  if (success)
  {
    if (indexToWrite >= queue->maxNumItems)
    {
      indexToWrite -= queue->maxNumItems;
    }
    uint8_t *dest = queue->items + (indexToWrite * queue->itemSize);
    memcpy(dest, src, queue->itemSize);

    // A barrier to keep the compiler from re-ordering the next line.
    __asm volatile("" ::: "memory");

    // Signal to any context wanting to dequeue this item that it's ready.
    queue->numItemsWaiting = queue->numSlotsClaimed;
  }
  return success;
}

bool enqueueFront(queue_t *queue, const void *src)
{
  bool success = false;
  return success;
}

bool peekFront(queue_t *queue, void *result)
{
  bool success = false;
  return success;
}

bool dequeueFront(queue_t *queue, void *result)
{
  bool success = false;

  // Do I need a critical section for reading from queue?
  // Maybe you won't have more than one read thread.
  // disableLowPriorityInterrupts(queue->highestSenderPriority);
  if (queue->numItemsWaiting > 0)
  {
    success = true;
    queue->numItemsWaiting--;

    memcpy(
        result,
        queue->items + (queue->itemSize * queue->front),
        queue->itemSize);

    if (++queue->front == queue->maxNumItems)
      queue->front = 0;
    
    // A barrier to keep the compiler from re-ordering the next line.
    __asm volatile("" ::: "memory");
    // And, finally, mark this slot as free for writing.  This must be last so
    // enqueue doesn't start writing this last slot before we're done. 
    queue->numSlotsClaimed--;
  }
  return success;
}

bool dequeueBack(queue_t *queue, void *result)
{
  bool success = false;
  return success;
}

uint32_t numItemsInQueue(queue_t *queue)
{
  return queue->numItemsWaiting;
}

uint32_t emptySpacesInQueue(queue_t *queue)
{
  return queue->maxNumItems - queue->numSlotsClaimed;
}