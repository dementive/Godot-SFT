// SFT - Stupid Fucking Tests

#include "ForEachMacro.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/node.hpp>

#ifndef SFT_H
#define SFT_H

// Uncomment this so clangd can actually work in the ifdefs...make sure to redisable it before compiling!
//#define TESTS_ENABLED

#ifdef TESTS_ENABLED

// Defines to use in your test conditions and checks
#define VAR_CHECK(first, second) first == godot::Variant(second)
#define STRING_CHECK(first, second) first == godot::StringName(second)
#define NULL_CHECK(first) first != nullptr
#define NUM_CHECK(first, second) first == second

// Defines that can be tweaked to change the output
#define TEST_PRINT_FUNCTION godot::UtilityFunctions::print_rich
#define PRINT_TEST(message, name, file, line, condition) TEST_PRINT_FUNCTION(godot::vformat(message, name, file, line, condition));
#define TEST_FAIL_MESSAGE "%-55s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - [code]%s[/code]"
#define TEST_PASS_MESSAGE "%-55s | [color=green]Passed[/color] %s%s%s"
#define TEST_CASE_NAME_MESSAGE "[color=blue]%-20s[/color]"
#define SCENE_TEST_FAIL_MESSAGE(scene_path, message)                                                                                                                          \
    TEST_PRINT_FUNCTION(godot::vformat("\n[color=blue]Scene Test[/color]: %-43s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - %s %s.",                          \
            scene_path.replace("res://", ""), __FILE__, __LINE__, scene_path, message))
#define SCENE_TEST_PASS_MESSAGE(scene_path)                                                                                                                                   \
    TEST_PRINT_FUNCTION(godot::vformat("\n[color=blue]Scene Test[/color]: %-43s | [color=green]Passed[/color]", scene_path.replace("res://", "")))

#define TEST_MESSAGE(condition) condition ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_FILE(condition) condition ? "" : __FILE__
#define TEST_LINE(condition) condition ? "" : godot::vformat("%s", __LINE__)
#define TEST_CONDITION(condition) condition ? "" : #condition
inline int SFT_check_number = 1;

// This uses a do-while loop to avoid if/else nesting hell while also not using a return statement to allow multiple TEST_SCENE calls in a single function.
#define TEST_SCENE(scene_path, root_node_class_name)                                                                                                                          \
    do {                                                                                                                                                                      \
        if (!ResourceLoader::get_singleton()->exists(scene_path)) {                                                                                                           \
            SCENE_TEST_FAIL_MESSAGE(String(scene_path), "does not exist");                                                                                                    \
            break;                                                                                                                                                            \
        }                                                                                                                                                                     \
        Ref<PackedScene> scene_ref = ResourceLoader::get_singleton()->load(scene_path);                                                                                       \
        if (!scene_ref->can_instantiate()) {                                                                                                                                  \
            SCENE_TEST_FAIL_MESSAGE(String(scene_path), "could not be instantiated");                                                                                         \
            break;                                                                                                                                                            \
        }                                                                                                                                                                     \
        Node *node = scene_ref->instantiate();                                                                                                                                \
        root_node = Object::cast_to<root_node_class_name>(node);                                                                                                              \
        if (root_node == nullptr) {                                                                                                                                           \
            SCENE_TEST_FAIL_MESSAGE(String(#root_node_class_name), "node is a nullptr");                                                                                      \
            break;                                                                                                                                                            \
        }                                                                                                                                                                     \
        SCENE_TEST_PASS_MESSAGE(String(scene_path));                                                                                                                          \
    } while (0);

#define NAMED_TESTS(test_case_name, ...)                                                                                                                                      \
    TEST_PRINT_FUNCTION(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                                              \
    FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define TESTS(test_case_name, ...)                                                                                                                                            \
    SFT_check_number = 1;                                                                                                                                                     \
    TEST_PRINT_FUNCTION(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                                              \
    FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define CHECK(test_case_name, condition)                                                                                                                                      \
    PRINT_TEST(TEST_MESSAGE(condition), SFT_check_number, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))                                              \
    SFT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                                    \
    PRINT_TEST(TEST_MESSAGE(condition), check_name, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))

#endif // TESTS_ENABLED

#endif // SFT_H
