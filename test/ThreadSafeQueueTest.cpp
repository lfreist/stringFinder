// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <thread>

#include "../src/ThreadSafeQueue.h"
#include "../src/String.h"

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, Constructor) {
	TSQueue<String*> tsq;
	ASSERT_TRUE(tsq._queue.empty());
}

// ____________________________________________________________________________________________________________________
TEST(TSQueueTest, methodsTest) {
	{
		String str1("hello");
		String str2("bye");
		TSQueue<String*> tsq;
		tsq.push(&str1);
		ASSERT_FALSE(tsq.empty());
		ASSERT_EQ(tsq._queue.size(), 1);
		tsq.push(&str2);
		ASSERT_EQ(tsq._queue.size(), 2);
		String* fst = tsq.pop();
		ASSERT_EQ(tsq._queue.size(), 1);
		String* snd = tsq.pop();
		ASSERT_TRUE(tsq.empty());
		ASSERT_TRUE(*fst == str1);
		ASSERT_TRUE(*snd == str2);
	}
	{
		auto pushIntoQueue = [](TSQueue<String*>* q, String* elem) {
			sleep(1);
			q->push(elem);
		};
		auto popFromQueue = [](TSQueue<String*>* q, String* expectElem) {
			ASSERT_TRUE(q->empty());
			String* elem = q->pop();
			ASSERT_EQ(elem, expectElem);
		};
		TSQueue<String*> tsq;
		String str("element");
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
