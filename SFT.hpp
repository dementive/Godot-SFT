// SFT - Stupid Fucking Tests

#include "ForEachMacro.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#ifndef SFT_H
#define SFT_H

// Uncomment this so clangd can actually work in the ifdefs...make sure to redisable it before compiling!
// #define TESTS_ENABLED

#ifdef TESTS_ENABLED

#define TEST_FAIL_MESSAGE "%-55s | [color=red]Failed:[/color] [b]%s[/b]"
#define TEST_PASS_MESSAGE "%-55s | [color=green]Passed[/color] %s"
#define TEST_CASE_NAME_MESSAGE "[color=blue]%-20s[/color]"
#define ONE_HUNDRED_FIFTY_DASHES                                                                                                                                    \
    "------------------------------------------------------------------------------------------------------------------------------------------------------"

inline int SFT_test_case_number = 1;

#define VAR_CHECK(first, second) first == godot::Variant(second)
#define STRING_CHECK(first, second) first == godot::StringName(second)
#define NULL_CHECK(first) first != nullptr
#define NUM_CHECK(first, second) first == second

#define NAMED_TESTS(test_case_name, ...)                                                                                                                            \
    godot::UtilityFunctions::print(ONE_HUNDRED_FIFTY_DASHES);                                                                                                       \
    godot::UtilityFunctions::print_rich(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                    \
    FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define TESTS(test_case_name, ...)                                                                                                                                  \
    SFT_test_case_number = 1;                                                                                                                                       \
    godot::UtilityFunctions::print(ONE_HUNDRED_FIFTY_DASHES);                                                                                                       \
    godot::UtilityFunctions::print_rich(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                    \
    FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define CHECK(test_case_name, condition)                                                                                                                            \
    godot::UtilityFunctions::print_rich(godot::vformat(condition ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE, #condition, condition ? "" : #condition));                \
    SFT_test_case_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                          \
    godot::UtilityFunctions::print_rich(godot::vformat(condition ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE, check_name, condition ? "" : #condition));

#endif // TESTS_ENABLED

#endif // SFT_H
