// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <thread>

#include "../src/utils/ThreadSafeQueue.h"
#include "../src/utils/FileChunk.h"

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, Constructor) {
	TSQueue<FileChunk*> tsq;
	ASSERT_TRUE(tsq._queue.empty());
}

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, methodsTest) {
  {
    FileChunk str1("hello");
    FileChunk str2("bye");
    TSQueue<FileChunk*> tsq;
    tsq.push(&str1);
    ASSERT_FALSE(tsq.empty());
    ASSERT_EQ(tsq._queue.size(), 1);
    tsq.push(&str2);
    ASSERT_EQ(tsq._queue.size(), 2);
    FileChunk* fst = tsq.pop();
    ASSERT_EQ(tsq._queue.size(), 1);
    FileChunk* snd = tsq.pop();
    ASSERT_TRUE(tsq.empty());
    ASSERT_TRUE(*fst == str1);
    ASSERT_TRUE(*snd == str2);
  }
  {
    auto pushIntoQueue = [](TSQueue<FileChunk*>* q, FileChunk* elem) {
      sleep(1);
      q->push(elem);
    };
    auto popFromQueue = [](TSQueue<FileChunk*>* q, FileChunk* expectElem) {
      ASSERT_TRUE(q->empty());
      FileChunk* elem = q->pop();
      ASSERT_EQ(elem, expectElem);
    };
    TSQueue<FileChunk*> tsq;
    FileChunk str("element");
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
