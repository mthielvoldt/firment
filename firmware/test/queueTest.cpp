#include <CppUTest/TestHarness.h>

extern "C"
{
#include <queue.h>
}

#define QUEUE_LENGTH 3
#define ITEM_SIZE 4
#define PRIORITY 10

extern void (*disableLowPriorityInterruptsCallback)(void);
extern void (*enableAllInterruptsCallback)(void);
const uint8_t inBuff[ITEM_SIZE] = {0xC0, 0xFF, 0xEE};
uint8_t outBuff[ITEM_SIZE];
queue_t queue[1];
bool dequeueSucceeded = false, enqueueSucceeded = false;

void enqueueHelper(void)
{
  enqueueSucceeded = enqueueBack(queue, inBuff);
}
void dequeueHelper(void)
{
  dequeueSucceeded = dequeueFront(queue, outBuff);
}

TEST_GROUP(queue)
{
  uint8_t store[QUEUE_LENGTH * ITEM_SIZE];
  bool initReturn;
  void setup()
  {
    dequeueSucceeded = enqueueSucceeded = false;
    disableLowPriorityInterruptsCallback = enableAllInterruptsCallback = NULL;
    memset(store, 0, sizeof(store));
    queue[0] = (queue_t){0};
    initReturn = initQueue(ITEM_SIZE, QUEUE_LENGTH, queue, store,
                           PRIORITY);
    //
  }
  void fillQueue(void)
  {
    for (int i = 0; i < QUEUE_LENGTH; i++)
    {
      enqueueBack(queue, inBuff);
    }
  }
};

TEST(queue, initQueue)
{
  CHECK(initReturn);
  CHECK_EQUAL_ZERO(numItemsInQueue(queue));
  CHECK_FALSE(dequeueFront(queue, outBuff));
  CHECK_FALSE(dequeueBack(queue, outBuff));
}
TEST(queue, initFailsForQueueNull)
{
  CHECK_FALSE(initQueue(ITEM_SIZE, QUEUE_LENGTH, NULL, store, PRIORITY));
}
TEST(queue, initFailsForStoreNull)
{
  CHECK_FALSE(initQueue(ITEM_SIZE, QUEUE_LENGTH, queue, NULL, PRIORITY));
}
TEST(queue, initFailsForLength0)
{
  CHECK_FALSE(initQueue(ITEM_SIZE, 0, queue, store, PRIORITY));
}
TEST(queue, enqueueDequeueWork)
{
  CHECK(enqueueBack(queue, inBuff));
  CHECK(dequeueFront(queue, outBuff));
  MEMCMP_EQUAL(inBuff, outBuff, ITEM_SIZE);
}
TEST(queue, re_initEmptiesQueue)
{
  CHECK(enqueueBack(queue, inBuff));
  CHECK(initQueue(ITEM_SIZE, QUEUE_LENGTH, queue, store, PRIORITY));
  CHECK_FALSE(dequeueFront(queue, outBuff));
}
TEST(queue, fillQueue)
{
  for (int i = 0; i < QUEUE_LENGTH; i++)
  {
    CHECK(enqueueBack(queue, inBuff));
  }
  CHECK_FALSE(enqueueBack(queue, inBuff));
}
TEST(queue, emptyFromFull)
{
  fillQueue();
  for (int i = 0; i < QUEUE_LENGTH; i++)
  {
    CHECK(dequeueFront(queue, outBuff));
  }
  CHECK_FALSE(dequeueFront(queue, outBuff));
}
TEST(queue, chaseFull)
{
  fillQueue();
  for (int i = 0; i < (QUEUE_LENGTH * 2); i++)
  {
    CHECK_FALSE(enqueueBack(queue, inBuff));
    CHECK_TRUE(dequeueFront(queue, outBuff));
    CHECK_TRUE(enqueueBack(queue, inBuff));
  }
}
TEST(queue, chaseEmpty)
{
  for (int i = 0; i < (QUEUE_LENGTH * 2); i++)
  {
    CHECK_FALSE(dequeueFront(queue, outBuff));
    CHECK_TRUE(enqueueBack(queue, inBuff));
    CHECK_TRUE(dequeueFront(queue, outBuff));
  }
}
// To test: Chase empty state around loop backward
// To test: Chase full state around loop backward

/* Q empty: start enqueue, interrupt with dequeue just after crit section. 
The dequeue should be blocked, so we should end with one item in queue.
*/
TEST(queue, empty_dequeueInterruptsEnqueueLate)
{
  enableAllInterruptsCallback = dequeueHelper;
  CHECK_TRUE(enqueueBack(queue, inBuff));
  CHECK_TRUE(dequeueSucceeded);
  CHECK_EQUAL(0, numItemsInQueue(queue));
}

// Interrupt read with a write as soon as allowed, queue full
TEST(queue, empty_dequeueInterruptsEnqueueEarly)
{
  disableLowPriorityInterruptsCallback = dequeueHelper;
  CHECK_TRUE(enqueueBack(queue, inBuff));
  CHECK_FALSE(dequeueSucceeded);
  CHECK_EQUAL(1, numItemsInQueue(queue));
}

TEST(queue, 1Item_dequeueInterruptsDequeue_late)
{
  enqueueBack(queue, inBuff);
  enableAllInterruptsCallback = dequeueHelper;
  CHECK_TRUE(dequeueFront(queue, outBuff)); // Original dequeue call wins race.
  CHECK_EQUAL(0, numItemsInQueue(queue));
  CHECK_FALSE(dequeueSucceeded);

}
TEST(queue, 1Item_dequeueInterruptsDequeue_early)
{
  enqueueBack(queue, inBuff);
  disableLowPriorityInterruptsCallback = dequeueHelper;
  CHECK_FALSE(dequeueFront(queue, outBuff));
  CHECK_EQUAL(0, numItemsInQueue(queue));
  CHECK_TRUE(dequeueSucceeded);  // Interrupting dequeue call wins race.
}

/*
TEST(queue, )
{
{

TEST(queue, )
{

}
*/