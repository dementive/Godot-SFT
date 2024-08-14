// SFT - Stupid Fucking Tests

#include "ForEachMacro.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#ifndef SFT_H
#define SFT_H

// Uncomment this so clangd can actually work in the ifdefs...make sure to redisable it before compiling!
// #define TESTS_ENABLED

#ifdef TESTS_ENABLED

#define VAR_CHECK(first, second) first == godot::Variant(second)
#define STRING_CHECK(first, second) first == godot::StringName(second)
#define NULL_CHECK(first) first != nullptr
#define NUM_CHECK(first, second) first == second

#define TEST_FAIL_MESSAGE "%-55s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - [code]%s[/code]"
#define TEST_PASS_MESSAGE "%-55s | [color=green]Passed[/color] %s%s%s"
#define TEST_CASE_NAME_MESSAGE "[color=blue]%-20s[/color]"
#define TEST_MESSAGE(condition) condition ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_FILE(condition) condition ? "" : __FILE__
#define TEST_LINE(condition) condition ? "" : godot::vformat("%s", __LINE__)
#define TEST_CONDITION(condition) condition ? "" : #condition
#define TWO_HUNDRED_DASHES "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
inline int SFT_check_number = 1;

#define NAMED_TESTS(test_case_name, ...)                                                                                                                                      \
    godot::UtilityFunctions::print(TWO_HUNDRED_DASHES);                                                                                                                       \
    godot::UtilityFunctions::print_rich(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                              \
    FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define TESTS(test_case_name, ...)                                                                                                                                            \
    SFT_check_number = 1;                                                                                                                                                     \
    godot::UtilityFunctions::print(TWO_HUNDRED_DASHES);                                                                                                                       \
    godot::UtilityFunctions::print_rich(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                              \
    FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define CHECK(test_case_name, condition)                                                                                                                                      \
    godot::UtilityFunctions::print_rich(godot::vformat(TEST_MESSAGE(condition), SFT_check_number, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition)));    \
    SFT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                                    \
    godot::UtilityFunctions::print_rich(godot::vformat(TEST_MESSAGE(condition), check_name, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition)));

#endif // TESTS_ENABLED

#endif // SFT_H
