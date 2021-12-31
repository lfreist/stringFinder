// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <iostream>

#include "../src/RotatingReader.h"

// ____________________________________________________________________________
TEST(RotatingReaderTest, bufferConstructorTest) {
  {
		Buffer buf;
		ASSERT_FALSE(buf._read);
		ASSERT_FALSE(buf._wrote);
		ASSERT_FALSE(buf._last);
		ASSERT_EQ(buf._next, nullptr);
  }
	{
		String str = String("Test");
		Buffer buf = Buffer(&str);
		ASSERT_FALSE(buf._read);
		ASSERT_TRUE(buf._wrote);
		ASSERT_EQ(buf._next, nullptr);
		ASSERT_TRUE(*buf._content == str);
	}
}

// ____________________________________________________________________________
TEST(RotatingReaderTest, bufferMethodsTest) {
  {
		Buffer buf;
		ASSERT_FALSE(buf.getRead());
		ASSERT_FALSE(buf.getWrote());
		String str = String("Test");
		buf.write(&str);
		ASSERT_FALSE(buf.getRead());
		ASSERT_TRUE(buf.getWrote());
		ASSERT_TRUE(*buf.read() == str);
		ASSERT_TRUE(buf.getRead());
		ASSERT_FALSE(buf.getWrote());
		ASSERT_EQ(buf.getNext(), nullptr);
  }
	{
		Buffer* buf1 = new Buffer();
		Buffer* buf2 = new Buffer(true);
		buf1->setNext(buf2);
		ASSERT_EQ(buf1->getNext(), buf2);
		ASSERT_EQ(buf2->getNext(), nullptr);
		buf2->setNext(buf1);
		ASSERT_EQ(buf1->getNext(), buf2);
		ASSERT_EQ(buf2->getNext(), buf1);
	}
}

// ____________________________________________________________________________
TEST(RotatingReaderTest, RotatingReaderConstructorTest) {
  {
		RotatingReader rr = RotatingReader(4);
		ASSERT_EQ(rr._nBuffers, 4);
		ASSERT_EQ(rr._readBuffer,  rr._writeBuffer);
  }
	{
		RotatingReader rr = RotatingReader(0);
		ASSERT_EQ(rr._nBuffers, 0);
		ASSERT_EQ(rr._writeBuffer, rr._readBuffer);
  }
}

// ____________________________________________________________________________
TEST(RotatingReaderTest, RotatingReaderMethodsTest) {
  {
		RotatingReader rr = RotatingReader(4);
		Buffer* writeBuffer = rr.getNextWriteBuffer();
		String str = String("Test");
		writeBuffer->write(&str);
		ASSERT_TRUE(*rr.getNextReadBuffer()->read() == str);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
