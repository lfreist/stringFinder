// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <thread>

#include "../src/utils/ThreadSafeQueue.h"
#include "../src/utils/Buffer.h"

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, Constructor) {
	TSQueue<Buffer*> tsq;
	ASSERT_TRUE(tsq._queue.empty());
}

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, methodsTest) {
  {
    Buffer str1("hello");
    Buffer str2("bye");
    TSQueue<Buffer*> tsq;
    tsq.push(&str1);
    ASSERT_FALSE(tsq.empty());
    ASSERT_EQ(tsq._queue.size(), 1);
    tsq.push(&str2);
    ASSERT_EQ(tsq._queue.size(), 2);
    Buffer* fst = tsq.pop();
    ASSERT_EQ(tsq._queue.size(), 1);
    Buffer* snd = tsq.pop();
    ASSERT_TRUE(tsq.empty());
    ASSERT_TRUE(*fst == str1);
    ASSERT_TRUE(*snd == str2);
  }
  {
    auto pushIntoQueue = [](TSQueue<Buffer*>* q, Buffer* elem) {
      sleep(10);
      q->push(elem);
    };
    auto popFromQueue = [](TSQueue<Buffer*>* q, Buffer* expectElem) {
      ASSERT_TRUE(q->empty());
      Buffer* elem = q->pop();
      ASSERT_EQ(elem, expectElem);
    };
    TSQueue<Buffer*> tsq;
    Buffer str("element");
    std::thread push(pushIntoQueue, &tsq, &str);
    std::thread pop(popFromQueue, &tsq, &str);
    push.join();
    pop.join();
    ASSERT_TRUE(tsq.empty());
  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
