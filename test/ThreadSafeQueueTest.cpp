// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <thread>

#include "../src/ThreadSafeQueue.h"

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, Constructor) {
	TSQueue<int> tsq;
	ASSERT_TRUE(tsq._queue.empty());
}

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, methodsTest) {
	{
		TSQueue<int> tsq;
		tsq.push(5);
		ASSERT_FALSE(tsq.empty());
		ASSERT_EQ(tsq._queue.size(), 1);
		tsq.push(8);
		ASSERT_EQ(tsq._queue.size(), 2);
		int fst = tsq.pop();
		ASSERT_EQ(tsq._queue.size(), 1);
		int snd = tsq.pop();
		ASSERT_TRUE(tsq.empty());
		ASSERT_EQ(fst, 8);
		ASSERT_EQ(snd, 5);
	}
	/*
	{
		auto pushIntoQueue = [](TSQueue<int>* q) {
			q->push(5);
		};
		auto popFromQueue = [](TSQueue<int>* q) {
			int elem = q->pop();
			ASSERT_EQ(elem, 5);
		};
		TSQueue<int> tsq;
		std::thread push(pushIntoQueue, &tsq);
		std::thread pop(popFromQueue, &tsq);
		push.join();
		pop.join();
		ASSERT_TRUE(tsq.empty());
	}
	*/
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
