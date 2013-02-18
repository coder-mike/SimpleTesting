/*
 * simple_testing.h
 *
 *  Created on: Dec 21, 2012
 *      Author: michael.hunter
 */

#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#ifndef SIMPLE_TESTING_H_
#define SIMPLE_TESTING_H_

class SimpleTest {
public:
	SimpleTest(const char* testName, const char* fileName, int line);
	static int runAllTests();
	virtual ~SimpleTest() { }
protected:
	virtual void run() = 0;
	void checkMemory(int line);
	void assert(bool proposition, const char* msg, int line);
	template <typename T1, typename T2>
	void assert_equal(const T1& v1, const T2& v2, const char* msg, int line);
	template <typename T1>
	void assert_equal(const T1& v1, const char* v2, const char* msg, int line);
	template <typename T1, typename T2>
	void assert_not_equal(const T1& v1, const T2& v2, const char* msg, int line);
	void pass(const char* passText);
	void fail(const char* failText, int line);
private:
	void executeTest();
	static std::vector<SimpleTest*>* allTests;
	bool isFailed_;
	std::string testName_;
	std::string fileName_;
	int testStartLine_;
	int passCount_;
	int failCount_;
	static std::string* lastFileName_;
};

#define ASSERT(PROPOSITION) assert(PROPOSITION, #PROPOSITION, __LINE__)
#define ASSERT_EQUAL(V1, V2) assert_equal(V1, V2, #V1, __LINE__)
#define ASSERT_NOT_EQUAL(V1, V2) assert_not_equal(V1, V2, #V1, __LINE__)
#define PASS(PASS_TEXT) pass(PASS_TEXT)
#define FAIL(FAIL_TEXT) fail(FAIL_TEXT, __LINE__)
#define TEST(TEST_NAME) \
	namespace { \
		class TEST_NAME##Test: public SimpleTest { \
		public: \
			TEST_NAME##Test(const char* file, int line) : SimpleTest(#TEST_NAME, file, line) { } \
			virtual void run() override; \
		} TEST_NAME##_test(__FILE__, __LINE__); \
	} \
	void TEST_NAME##Test::run() \


template <typename T1, typename T2>
void SimpleTest::assert_equal(const T1& v1, const T2& v2, const char* msg, int line)
{
	if (v1 == v2)
		pass(msg);
	else
	{
		std::stringstream s;
		s << "expected \"" << v2 << "\" but got \"" << v1 << "\"";
		fail(s.str().c_str(), line);
	}
}

template <typename T1>
void SimpleTest::assert_equal(const T1& v1, const char* v2, const char* msg, int line)
{
	std::stringstream s;
	s << v1;
	if (strcmp(s.str().c_str(), v2) == 0)
		pass(msg);
	else
	{
		std::stringstream msg;
		msg << "expected \"" << v2 << "\" but got \"" << s.str() << "\"";
		fail(msg.str().c_str(), line);
	}
}

template <typename T1, typename T2>
void SimpleTest::assert_not_equal(const T1& v1, const T2& v2, const char* msg, int line)
{
	if (v1 != v2)
		pass(msg);
	else
	{
		std::stringstream s;
		s << msg << " is \"" << v2 << "\"";
		fail(s.str().c_str(), line);
	}
}


#endif /* SIMPLE_TESTING_H_ */
