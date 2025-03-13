#include "queue.h"
#include <crit_section_port.h>
#include <memory.h>

#define FAIL_IF_PARAMS_IMPROPER       \
  if (!queue || !itemsStorage || !length) \
  return false

bool initQueue(
    size_t itemSize,
    uint32_t length,
    queue_t *queue,
    uint8_t *itemsStorage,
    uint32_t highestSenderPriority)
{
  FAIL_IF_PARAMS_IMPROPER;
  // utilizes auto-zeroing of unspecified elements.
  *queue = (queue_t){
      .highestSenderPriority = highestSenderPriority,
      .items = itemsStorage,
      .itemSize = itemSize,
      .maxNumItems = length,
  };
  return true;
}

bool enqueueBack(queue_t *queue, const void *src)
{
  bool success = false;

  disableLowPriorityInterrupts(queue->highestSenderPriority);
  if (queue->numItemsWaiting < queue->maxNumItems)
  {
    success = true;
    uint8_t *dest = queue->items + (queue->back * queue->itemSize);
    memcpy(dest, src, queue->itemSize);
    
    if (++queue->back == queue->maxNumItems)
      queue->back = 0;
    queue->numItemsWaiting++;
  }
  enableAllInterrupts();
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
  disableLowPriorityInterrupts(queue->highestSenderPriority);
  if (queue->numItemsWaiting > 0)
  {
    success = true;
    memcpy(
        result,
        queue->items + (queue->itemSize * queue->front),
        queue->itemSize);

    if (++queue->front == queue->maxNumItems)
      queue->front = 0;
    queue->numItemsWaiting--;
  }
  enableAllInterrupts();
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
  return queue->maxNumItems - queue->numItemsWaiting;
}