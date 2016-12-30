SimpleTesting
=============

    Author: Michael Hunter (elec.mike@gmail.com)
    Version: 1.0.0

A minimalist unit testing framework for C++.

Features:
* Compiles under GCC (other compilers not tested).
* Only 4 files, and a handful of functions.
* Includes basic memory leak and heap corruption checking.
* Outputs errors in a format similar to GCC, which may be used to integrate into IDEs as a post-build step (Code::Blocks IDE tested).

Overview
--------

The files `debug_new_and_delete.cpp` and `debug_new_and_delete.h` are used internally for heap checking so you shouldn't need open them or `#include` them. The file `simple_testing.h` is the file contains the test macros that you use, as well as the class `SimpleTest` which you shouldn't need to worry about unless you extend the framework. The file `simple_testing.cpp` includes a definition of `main()`, as well as the list of registered tests.

To Use
------

Simply clone the repository into your project folder, and add the files `debug_new_and_delete.cpp` and `simple_testing.cpp` to your project or make file under a new build configuration. `simple_testing.cpp` includes a definition of `main()`, and so you can't use `simple_testing.cpp` with your primary application build.

To create a new test file, create a file with the extension ".tests.cpp". By convention I create one test file per cpp. So test "MyClass.cpp" I create a test file called "MyClass.tests.cpp".

The framework includes a few simple macros for testing:
* `TEST(testName) { /*...*/ }`: Declares and registers a new test method
* `ASSERT(proposition)` : Fails if the proposition is false
* `ASSERT_EQUAL(value, expected)`: Fails if the value does not have the expected value.
* `PASS("Pass message")`: Note that a custom test passed (only needed in rare circumstances)
* `FAIL("Fail message")`: Note that a custom test failed (only needed in rare circumstances)

The test file should `#include` the `simple_testing.h` header. It can contain any C++ code, and in addition use the test macros. For example:

    // MyCalculator.tests.cpp
    #include "SimpleTesting.h"
    #include "MyCalculator.h"

    TEST(Addition)
    {
    	int x, y, r;
    	MyCalculator myCalculator;
    	x = 1;
    	y = 2;
    	r = myCalculator.add(x, y);
    	ASSERT_EQUALS(r, 3);
    	x = -1;
    	y = -2;
    	r = myCalculator.add(x, y);
    	ASSERT_EQUALS(r, -3);
    }

    TEST(Subtraction)
    {
    	// ...
    }

## Visual C++ Integration

To run the unit test output each time after they are built, do the followin.

In the project settings in VC++, look for "Build Events", and modify the "Post-Build Event" so that the command line is `$(TargetPath)`.
