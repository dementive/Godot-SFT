# Godot-SFT

Godot-SFT is a extremely simple testing library for Godot 4 GDExtension C++ that allows you to easily test your extension code with C++ macros.

After a lot of searching I couldn't find a single testing framework for C++ that wasn't either a massive pain to get working with gdextension or just way too complicated for no good reason. So here is my stupid simple testing "library" that does everytihng I will ever need it to do in only like 50 lines of code.

## Example Usage

```C++
// Test.cpp

#include "SFT.hpp"
#include "CustomObject.hpp"

#ifdef TESTS_ENABLED

// clang-format off
void test_dictionary() {
    godot::Dictionary map;
    map["Hello"] = 0;
    map["Hey"] = 999;
    map["this_test_will_fail"] = -1;

    TESTS(
        "dictionary_test",
        map["Hello"] == godot::Variant(0),
        map["Hey"] == godot::Variant(999),
        map["this_test_will_fail"] == godot::Variant(999),
        map.has("Howdy"),
        map.size() == 3
    )

    NAMED_TESTS(
        "Dictionary Variant Test",
        "Check equal to 0", VAR_CHECK(map["Hello"], 0),
        "Check equal to 999", VAR_CHECK(map["Hey"], 999),
        "This will always fail why even test it?", VAR_CHECK(map["this_test_will_fail"], 999),
        "Check for non-existent member", map.has("Howdy"),
        "Check if size is 3", NUM_CHECK(map.size(), 3)
    )
}

void test_custom_object() {
    CustomObject *custom_object = memnew(CustomObject());
    custom_object->set_name("CustomObjectName");

    NAMED_TESTS(
        "custom_object_tests",
        "CustomObject nullptr test", NULL_CHECK(custom_object),
        "CustomObject get_name", STRING_CHECK(custom_object->get_name(), "WrongName"),
        "CustomObject get_custom_function", VAR_CHECK(custom_object->get_custom_function(), "CustomFunctionReturn")
    )
    memdelete(custom_object);
}
// clang-format on

void do_tests() {
    test_dictionary();
    test_custom_object();
}

#endif // TESTS_ENABLED

// Somewhere else...
#ifdef TESTS_ENABLED
do_tests()
#endif
```
You can test any condition with a named test suite as the first argument and then all other arguments will be the conditions to check for the test. The conditions can be any code that returns a bool.

The `TESTS` macro can be used to check any number of conditions, the output of the `TESTS` in the above example looks like this:

```
------------------------------------------------------------------------------------------------------------------------------------------------------
dictionary_assignment
1                                | Passed
2                                | Passed
3                                | Failed [Tests.cpp:16] - map["this_test_will_fail"] == godot::Variant(999)
4                                | Failed [Tests.cpp:16] - map.has("Howdy")
5                                | Passed
```

The `NAMED_TESTS` macro can be used to check any number of conditions and also give a name to each check, the output of the `NAMED_TESTS` in the above example looks like this:

```
------------------------------------------------------------------------------------------------------------------------------------------------------
Dictionary Variant Test
Check equal to 0                                  | Passed
Check equal to 999                                | Passed
This will always fail why even test it?           | Failed [Tests.cpp:24] - map["this_test_will_fail"] == godot::Variant(999)
Check for non-existent member                     | Failed [Tests.cpp:24] - map.has("Howdy")
Check if size is 3                                | Passed
------------------------------------------------------------------------------------------------------------------------------------------------------
custom_object_tests
CustomObject nullptr test                         | Passed
CustomObject get_name                             | Failed [Tests.cpp:38] - custom_object->get_name() = StringName("WrongName")
CustomObject get_custom_function                  | Passed
```

Note that the `TESTS_ENABLED` define must be enabled at compile time to use the macros to prevent you from accidentally shipping test code. You can enable it in your Sconstruct file like this:

```python
if env["target"] == "template_debug":
	env.Append(CPPDEFINES=["TESTS_ENABLED"])
```

This way when you compile with the debug template the tests will also be compiled.

I highly reccomend you use the clangd LSP or the macros might be kind of awful to debug if you do something wrong. The clangd preprocessor handling is so awesome it's almost impossible to make mistakes with macros.

IMPORTANT NOTE: SFT uses `__VA_OPT__` for some of the macro magic, which means it requires C++20 so you'll need to add the following to your SConstruct file to compile your GDExtension with C++20

```python
# Option to use C++20 for this extension by replacing CXXFLAGS
if env.get("is_msvc", False):
   env.Replace(CXXFLAGS=["/std:c++20"])
else:
   env.Replace(CXXFLAGS=["-std=c++20"])

# Reenable CXXFLAGS removed by the above from godot-cpp/tools/godotcpp.py
# Disable exception handling. Godot doesn't use exceptions anywhere, and this
# saves around 20% of binary size and very significant build time.
if env["disable_exceptions"]:
   if env.get("is_msvc", False):
       env.Append(CPPDEFINES=[("_HAS_EXCEPTIONS", 0)])
   else:
       env.Append(CXXFLAGS=["-fno-exceptions"])
elif env.get("is_msvc", False):
   env.Append(CXXFLAGS=["/EHsc"])

# Ignore warnings from godot-cpp since it is written in C++17.
env.Append(CXXFLAGS=['-Wno-template-id-cdtor'])
```

If you really just can't stand to use C++20 you can remove the `TESTS` and `NAMED_TESTS` macros from SFT.hpp and use the `CHECK` and `CHECK_NAMED` macros directly (im not sure why you would do this).

## Usage

The tests can be run from anywhere in your gdextension code, running them from the initialize function in register_types.cpp has been working for me. This will only print the output to stdout though so if you want to have your test results print in the godot editor console you'll have to run them from somewhere else in your code that gets initialized later in the startup process.


## Automated Testing
Setting up automated testing is easy and you should do it. If your tests print to stdout (they will unless you change SFT.hpp) you can call godot with `godot --headless --quit` in your project's root to get godot to print your test results and then immediately quit.
I have provided a simple script: `automated_testing.gd` that can be run with the following command to automate tests using something like pre-commit or github actions:

```bash
touch output.txt && godot --headless --quit > output.txt && godot --headless --script ./automated_testing.gd
```

This command will check if any of the tests failed and if any of them do the return code will be -1.

## Official SFT Haiku

Testing is stupid

I hate stupid fucking tests

Why must I test it

## Screenshots

![Screenshot 1](/assets/1.png)
