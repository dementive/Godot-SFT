// SFT - Stupid Fucking Tests

#include "ForEachMacro.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/node.hpp>

#ifndef SFT_H
#define SFT_H

// Uncomment this so clangd can actually work in the ifdefs...make sure to redisable it before compiling!
// #define TESTS_ENABLED

#ifdef TESTS_ENABLED

// Defines that can be tweaked to change the output
#define TEST_PRINT_FUNCTION godot::UtilityFunctions::print_rich
#define PRINT_TEST(message, name, file, line, condition) TEST_PRINT_FUNCTION(godot::vformat(message, name, file, line, condition));
#define TEST_FAIL_MESSAGE "%-55s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - [code]%s[/code]"
#define TEST_PASS_MESSAGE "%-55s | [color=green]Passed[/color] %s%s%s"
#define TEST_CASE_NAME_MESSAGE "[color=blue]%s[/color]"

#define SCENE_TEST_FAIL_MESSAGE(scene_path, message)                                                                                                                          \
    TEST_PRINT_FUNCTION(godot::vformat("\n[color=blue]Scene Test[/color]: %-43s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - %s %s.",                          \
            scene_path.replace("res://", ""), __FILE__, __LINE__, scene_path, message))
#define SCENE_TEST_PASS_MESSAGE(scene_path)                                                                                                                                   \
    TEST_PRINT_FUNCTION(godot::vformat("\n[color=blue]Scene Test[/color]: %-43s | [color=green]Passed[/color]", scene_path.replace("res://", "")))

#define TEST_PASS(test_name) TEST_PRINT_FUNCTION(godot::vformat("%-55s | [color=green]Passed[/color]", test_name));
#define TEST_FAIL(test_name, message)                                                                                                                                         \
    TEST_PRINT_FUNCTION(godot::vformat("%-55s | [color=red]Failed[/color] [[color=yellow]%s:%s[/color]] - %s", test_name, __FILE__, __LINE__, message));

// Defines to use in your test conditions and checks
#define VAR_CHECK(first, second) first == godot::Variant(second)
#define STRING_CHECK(first, second) first == godot::StringName(second)
#define NUM_CHECK(first, second) first == second

/*
Checks if a pointer is null, if it is report the check as failed and goto the null_##first label with goto.

Why use goto here? It seems to be the only way to:
1. Perform the null check
2. Allow tests on that pointer after the check without crashing (so all the tests that are possible to run are actually run and are done in a safe way)
3. Prevent returning if it is a nullptr. I do not want to ever use a return statement in SFT so all tests can be put in a single function.
4. Do this without using exceptions (godot does not use exceptions)

This effectively makes it so:
1. All tests will always be run even if some tests that rely on a valid pointer fail.
2. The program won't crash because it is trying to run tests on a nullptr.

So I think this is a phenomenal use case for goto. Clangd will also let you know if the label doesn't exist so it's pretty hard to mess up and easy to debug.
*/
#define NULL_CHECK(object)                                                                                                                                                    \
    if (object == nullptr) {                                                                                                                                                  \
        TEST_FAIL(godot::vformat("%s nullptr check", #object), godot::vformat("%s is a nullptr!", #object))                                                                   \
        goto null_##object;                                                                                                                                                   \
    }

/*
Make sure to use TEST_POINTER_END on object_name after this at some point or it won't compile.
Note that because of the goto usage in NULL_CHECK if you declare any variables after TEST_POINTER and before TEST_POINTER_END they will have to be wrapped in a scoped block
"{}"" or the goto will not compile. More info here: https://stackoverflow.com/a/14274292
Example:
_ALWAYS_INLINE_ void test_stellar_body() {
    TEST_POINTER(Control, main_menu)
    {
        int x = 0; // Declaring this variable makes it so you have to do a scoped block :(
        main_menu->get_name();
        NAMED_TESTS(
            "main_menu_tests",
            "MainMenu get_name", STRING_CHECK(main_menu->get_name(), "Menu"),
        )
    }
    TEST_POINTER_END(main_menu)

    // If you do the same thing without any variable declarations you don't need the scoped block.
    TEST_POINTER(Control, loading_screen)
    loading_screen->get_name();
    NAMED_TESTS(
        "loading_screen_tests",
        "LoadingScreen get_name", STRING_CHECK(star->loading_screen(), "LoadingScreen"),
    )
    TEST_POINTER_END(loading_screen)
}
*/
#define TEST_POINTER(class_name, object_name)                                                                                                                                 \
    class_name *object_name = memnew(class_name());                                                                                                                           \
    NULL_CHECK(object_name)

#define TEST_POINTER_END(object_name) null_##object_name : memdelete(object_name);

// Defines you or I will never need to change (hopefully)
#define TEST_MESSAGE(condition) condition ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_FILE(condition) condition ? "" : __FILE__
#define TEST_LINE(condition) condition ? "" : godot::vformat("%s", __LINE__)
#define TEST_CONDITION(condition) condition ? "" : #condition
inline int SFT_check_number = 1;

// This uses a do-while loop to avoid if/else nesting hell while also not using a return statement to allow multiple TEST_SCENE calls in a single function.
// The nullptr check does return though to prevent root_node_variable_name access crashing the program.
#define TEST_SCENE(scene_path, root_node_class_name, root_node_variable_name)                                                                                                 \
    do {                                                                                                                                                                      \
        if (!ResourceLoader::get_singleton()->exists(scene_path)) {                                                                                                           \
            SCENE_TEST_FAIL_MESSAGE(String(scene_path), "does not exist");                                                                                                    \
            break;                                                                                                                                                            \
        }                                                                                                                                                                     \
        Ref<PackedScene> scene_ref##_root_node_variable_name = ResourceLoader::get_singleton()->load(scene_path);                                                             \
        if (!scene_ref##_root_node_variable_name->can_instantiate()) {                                                                                                        \
            SCENE_TEST_FAIL_MESSAGE(String(scene_path), "could not be instantiated");                                                                                         \
            break;                                                                                                                                                            \
        }                                                                                                                                                                     \
        Node *node##_root_node_variable_name = scene_ref##_root_node_variable_name->instantiate();                                                                            \
        root_node_variable_name = Object::cast_to<root_node_class_name>(node##_root_node_variable_name);                                                                      \
        if (root_node_variable_name == nullptr) {                                                                                                                             \
            SCENE_TEST_FAIL_MESSAGE(String(#root_node_class_name), "node is a nullptr");                                                                                      \
            break;                                                                                                                                                            \
        }                                                                                                                                                                     \
        SCENE_TEST_PASS_MESSAGE(String(scene_path));                                                                                                                          \
    } while (0);

// The actual tests
#define NAMED_TESTS(test_case_name, ...)                                                                                                                                      \
    TEST_PRINT_FUNCTION(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                                              \
    FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define TESTS(test_case_name, ...)                                                                                                                                            \
    SFT_check_number = 1;                                                                                                                                                     \
    TEST_PRINT_FUNCTION(godot::vformat(TEST_CASE_NAME_MESSAGE, test_case_name));                                                                                              \
    FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

// The condition checks that the tests perform
#define CHECK(test_case_name, condition)                                                                                                                                      \
    PRINT_TEST(TEST_MESSAGE(condition), SFT_check_number, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))                                              \
    SFT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                                    \
    PRINT_TEST(TEST_MESSAGE(condition), check_name, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))

#endif // TESTS_ENABLED

#endif // SFT_H
