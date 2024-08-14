// SFT - Stupid Fucking Tests

#include "ForEachMacro.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#ifndef SFT_H
#define SFT_H

#ifdef TESTS_ENABLED

#define TEST_FAIL_MESSAGE "Test [%s : %s] - Failed"
#define TEST_PASS_MESSAGE "Test [%s : %s] - Passed"

int SFT_test_case_number = 1;

#define TESTS(test_case_name, ...)                                                                                                                             \
    SFT_test_case_number = 1;                                                                                                                                  \
    FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define NAMED_TESTS(test_case_name, ...) FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

// NOTE: print() is used to make parsing for CI/CD easier. If you are only using the tests to print in the editor printerr should be used for fail and print_rich for success.
#define CHECK(test_case_name, condition)                                                                                                                       \
    if (!(condition)) {                                                                                                                                        \
        godot::UtilityFunctions::print(godot::vformat(TEST_FAIL_MESSAGE, test_case_name, SFT_test_case_number));                                               \
    } else {                                                                                                                                                   \
        godot::UtilityFunctions::print(godot::vformat(TEST_PASS_MESSAGE, test_case_name, SFT_test_case_number));                                               \
    }                                                                                                                                                          \
    SFT_test_case_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                     \
    if (!(condition)) {                                                                                                                                        \
        godot::UtilityFunctions::print(godot::vformat(TEST_FAIL_MESSAGE, test_case_name, check_name));                                                         \
    } else {                                                                                                                                                   \
        godot::UtilityFunctions::print(godot::vformat(TEST_PASS_MESSAGE, test_case_name, check_name));                                                         \
    }

#endif // TESTS_ENABLED

#endif // SFT_H
