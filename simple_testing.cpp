/*
 * simple_testing.cpp
 *
 *  Created on: Dec 21, 2012
 *      Author: michael.hunter
 */
#include "simple_testing.hpp"
#include "debug_new_and_delete.hpp"

#include <exception>
#include <iostream>
#include <time.h>
#include <stdio.h>

std::vector<SimpleTest*>* SimpleTest::allTests = 0;

SimpleTest::SimpleTest(const char* testName, const char* fileName, int line)
	: isFailed_(false), testName_(testName), fileName_(fileName), testStartLine_(line)
{
	if (!allTests)
		allTests = new std::vector<SimpleTest*>();
	allTests->push_back(this);
}

void SimpleTest::checkMemory(int line)
{
	switch (checkConsistency())	{
	case 1: fail("Heap corrupt", line); break;
	case 2: fail("Free header corrupt", line); break;
	case 3: fail("Free memory modified", line); break;
	}

	if (isMultipleFree)
		fail("The same pointer has been freed twice", line);
}

void SimpleTest::executeTest()
{
	try
	{
		size_t bytesBefore = bytesAllocatedCount;
		run();
		checkMemory(testStartLine_);
		if (bytesBefore != bytesAllocatedCount)
			fail("Memory leak during test", testStartLine_);
	}
	catch (std::exception& e)
	{
		fail(e.what(), testStartLine_);
	}
	catch (...)
	{
		fail("Test threw an exception", testStartLine_);
	}
}

int SimpleTest::runAllTests()
{
	if (!allTests)
	{
		std::cout << "No Tests" << std:: endl;	
		return 0;
	}

	std::cout << "Running " << allTests->size() << " tests" << std:: endl;
	for (auto test : *allTests)
	{

		test->executeTest();
		if (test->isFailed_)
			return 1;
	}
	std::cout << "All tests passed" << std::endl;
	return 0;
}

void SimpleTest::assert(bool proposition, const char* msg, int line)
{
	if (proposition)
		pass(msg);
	else
		fail(msg, line);
}

void SimpleTest::pass(const char* passText)
{
	std::cout << "Passed: " << passText << std::endl;
}

void SimpleTest::fail(const char* failText, int line)
{
	isFailed_ = true;
	std::cout << fileName_ << ":" << line << ": error: " << testName_ << " failed: " << failText << std::endl;
}

int main(int argc, char *argv[])
{
	int result = SimpleTest::runAllTests();

	const char* successFile = nullptr;
	for (int i = 0; i < argc - 1; i ++)
		if (strcmp(argv[i], "-s") == 0)
			successFile = argv[i+1];

	if (result == 0 && successFile)
	{
		struct tm *current;
		time_t now;
		time(&now);
		current = localtime(&now);
		FILE* fp = fopen(successFile, "w");
		fprintf(fp, "Last successful build: %04u-%02i-%02i %02i:%02i:%02i\n", 
			current->tm_year + 1900,
			current->tm_mon,
			current->tm_mday,
			current->tm_hour, 
			current->tm_min,
			current->tm_sec
			);
		fclose(fp);
	}

	return result;
}
