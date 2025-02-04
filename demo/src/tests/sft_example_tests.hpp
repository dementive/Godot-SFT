#pragma once

#include "summator.hpp"
#include "SFT.hpp"

/*
These tests will result in the following output:

---------
Summator Scene Tests
---------

Scene Test: main.tscn                                   | Passed
Main scene tests
exists                                                  | Passed 
get_name                                                | Passed 
---------
Summator Class Tests
---------
Summator tests
add 10                                                  | Passed 
Unnamed Summator tests
1                                                       | Passed 
2                                                       | Failed [src/tests/sft_example_tests.hpp:30] - test_summator_node->get_total() == 100
*/

inline void run_tests() {
	// You can use command line args here so the tests only run when certain args are passed.
	// if (!OS::get_singleton()->get_cmdline_args().has("--dotests"))
	// 	return;

	UtilityFunctions::print_rich("---------\n[color=orange]Summator Scene Tests[/color]\n---------");
	TEST_SCENE("res://main.tscn", Node, root_node)
	NAMED_TESTS(
		"Main scene tests",
		"exists", root_node != nullptr,
		"get_name", "Main" == root_node->get_name()
	)

	TEST_SCENE_END(root_node)

	UtilityFunctions::print_rich("---------\n[color=orange]Summator Class Tests[/color]\n---------");
	TEST_OBJECT(Summator, test_summator_node)

	test_summator_node->add(10);
	NAMED_TESTS(
		"Summator tests",
		"add 10", test_summator_node->get_total() == 10 // all SFT checks must return a boolean value.
	)

	test_summator_node->add(40);
	TESTS(
		"Unnamed Summator tests",
		test_summator_node->get_total() == 50,
		test_summator_node->get_total() == 100 // this test will fail.
	)
	TEST_OBJECT_END(test_summator_node)
}
