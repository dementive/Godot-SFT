# Godot-SFT

Godot-SFT is a extremely simple testing library for Godot 4 GDExtension C++ that allows you to easily test your extension code with C++ macros.

After a lot of searching I couldn't find a single testing framework for C++ that wasn't either a massive pain to get working with gdextension or just way too complicated for no good reason. So here is my stupid simple testing "library" that does everything I will ever need it to do.


## Example Usage

```C++
// Test.cpp

#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

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
    )
}

void test_custom_object() {
    TEST_OBJECT(CustomObject, custom_object) // TEST_OBJECT safely tests an object in a way that will never skip any other tests (unless your test code causes a crash), see SFT.hpp for more info.
    NAMED_TESTS(
        "CustomObject",
        "get_name", STRING_CHECK(custom_object->get_name(), "WrongName"),
        "get_custom_function", VAR_CHECK(custom_object->get_custom_function(), "CustomFunctionReturn")
    )
    TEST_OBJECT_END(custom_object) // TEST_OBJECT_END must be put at the end of the thing TEST_OBJECT is testing so it can clean itself up. 
}

void test_custom_scene() {
    Control *root_node;
    TEST_SCENE("res://scenes/main_menu.tscn", Control, root_node) // Same API as TEST_OBJECT but it instantiates the scene and from it's path.

    NAMED_TESTS(
        "MainMenu Tests",
        "visibility", root_node->is_visible()
    )

    TEST_SCENE_END(root_node)  // TEST_SCENE_END must be put at the end of the thing TEST_SCENE is testing so it can clean itself up, see SFT.hpp for more info.

    // Additional checks are optional, if you don't pass in any more it will still test if the scene is possible to instantiate.
    Control *broken_root_node
    TEST_SCENE("res://scenes/broken_scene.tscn", Control, broken_root_node)
    TEST_SCENE_END(broken_root_node)
}

// clang-format on

void do_tests() {
    test_dictionary();
    test_custom_object();
    test_custom_scene();
}

#endif // TESTS_ENABLED

// Somewhere else...
#ifdef TESTS_ENABLED
do_tests()
#endif
```
You can test any condition with a named test suite as the first argument and then all other arguments will be the conditions to check for the test. The conditions can be any code that returns a bool.

The `TESTS` macro can be used to check any number of conditions.
The `NAMED_TESTS` macro can be used to check any number of conditions and also give a name to each check, the output of the `test_dictionary` and `test_custom_object` functions in the above example looks like this:

```
------------------------------------------------------------------------------------------------------------------------------------------------------
dictionary_assignment
1                                                 | Passed
2                                                 | Passed
3                                                 | Failed [Tests.cpp:16] - map["this_test_will_fail"] == godot::Variant(999)
4                                                 | Failed [Tests.cpp:16] - map.has("Howdy")
5                                                 | Passed
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
CustomObject get_name                             | Failed [Tests.cpp:41] - custom_object->get_name() = StringName("WrongName")
CustomObject get_custom_function                  | Passed
------------------------------------------------------------------------------------------------------------------------------------------------------
MainMenu Tests
visibility                                        | Passed

Scene Test: scenes/main_menu.tscn                 | Passed
Scene Test: scenes/main_menu.tscn                 | Failed [Tests.cpp:64] - res://scenes/broken_scene.tscn could not be instantiated.
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
def using_clang(env):
    return "clang" in os.path.basename(env["CC"])


def is_vanilla_clang(env):
    if not using_clang(env):
        return False
    try:
        version = (
            subprocess.check_output([env.subst(env["CXX"]), "--version"])
            .strip()
            .decode("utf-8")
        )
    except (subprocess.CalledProcessError, OSError):
        print("Couldn't parse CXX environment variable to infer compiler version.")
        return False
    return not version.startswith("Apple")


def exists(env):
    return True


# Option to use C++20 for this extension by replacing CXXFLAGS
if env.get("is_msvc", False):
    env.Replace(CXXFLAGS=["/std:c++20"])
else:
    env.Replace(CXXFLAGS=["-std=c++20"])


# Function copied directly from godot-cpp common_compiler_flags.py
def generate(env):
    # Disable exception handling. Godot doesn't use exceptions anywhere, and this
    # saves around 20% of binary size and very significant build time.
    if env["disable_exceptions"]:
        if env.get("is_msvc", False):
            env.Append(CPPDEFINES=[("_HAS_EXCEPTIONS", 0)])
        else:
            env.Append(CXXFLAGS=["-fno-exceptions"])
    elif env.get("is_msvc", False):
        env.Append(CXXFLAGS=["/EHsc"])

    if not env.get("is_msvc", False):
        if env["symbols_visibility"] == "visible":
            env.Append(CCFLAGS=["-fvisibility=default"])
            env.Append(LINKFLAGS=["-fvisibility=default"])
        elif env["symbols_visibility"] == "hidden":
            env.Append(CCFLAGS=["-fvisibility=hidden"])
            env.Append(LINKFLAGS=["-fvisibility=hidden"])

    # Set optimize and debug_symbols flags.
    # "custom" means do nothing and let users set their own optimization flags.
    if env.get("is_msvc", False):
        if env["debug_symbols"]:
            env.Append(CCFLAGS=["/Zi", "/FS"])
            env.Append(LINKFLAGS=["/DEBUG:FULL"])

        if env["optimize"] == "speed":
            env.Append(CCFLAGS=["/O2"])
            env.Append(LINKFLAGS=["/OPT:REF"])
        elif env["optimize"] == "speed_trace":
            env.Append(CCFLAGS=["/O2"])
            env.Append(LINKFLAGS=["/OPT:REF", "/OPT:NOICF"])
        elif env["optimize"] == "size":
            env.Append(CCFLAGS=["/O1"])
            env.Append(LINKFLAGS=["/OPT:REF"])
        elif env["optimize"] == "debug" or env["optimize"] == "none":
            env.Append(CCFLAGS=["/Od"])
    else:
        if env["debug_symbols"]:
            # Adding dwarf-4 explicitly makes stacktraces work with clang builds,
            # otherwise addr2line doesn't understand them.
            env.Append(CCFLAGS=["-gdwarf-4"])
            if env.dev_build:
                env.Append(CCFLAGS=["-g3"])
            else:
                env.Append(CCFLAGS=["-g2"])
        else:
            if using_clang(env) and not is_vanilla_clang(env):
                # Apple Clang, its linker doesn't like -s.
                env.Append(LINKFLAGS=["-Wl,-S", "-Wl,-x", "-Wl,-dead_strip"])
            else:
                env.Append(LINKFLAGS=["-s"])

        if env["optimize"] == "speed":
            env.Append(CCFLAGS=["-O3"])
        # `-O2` is friendlier to debuggers than `-O3`, leading to better crash backtraces.
        elif env["optimize"] == "speed_trace":
            env.Append(CCFLAGS=["-O2"])
        elif env["optimize"] == "size":
            env.Append(CCFLAGS=["-Os"])
        elif env["optimize"] == "debug":
            env.Append(CCFLAGS=["-Og"])
        elif env["optimize"] == "none":
            env.Append(CCFLAGS=["-O0"])


generate(env)

# Ignore warnings from godot-cpp since it is written in C++17.
if not using_clang(env):
    env.Append(CXXFLAGS=["-Wno-template-id-cdtor"])
```

If you really just can't stand to use C++20 you can remove the `TESTS` and `NAMED_TESTS` macros from SFT.hpp and use the `CHECK` and `CHECK_NAMED` macros directly (im not sure why you would do this).

## Usage

The tests can be run from anywhere in your gdextension code, running them from the initialize function in register_types.cpp has been working for me. Some tests I run from the `_ready` function of the root node in my project and only in the editor because instantiating certain things in register_types can sometimes lead to crashes if your test code needs to allocate something the engine can't allocate yet.  This will only print the output to stdout though so if you want to have your test results print in the godot editor console you'll have to run them from somewhere else in your code that gets initialized later in the startup process.


## Automated Testing
Setting up automated testing is easy and you should do it. It only takes a few minutes to setup and can go a long way in preventing regressions in your code. If your tests print to stdout (they will unless you change SFT.hpp) you can call godot with `godot -e --headless --quit` in your project's root to get godot to print your test results and then immediately quit.
I have provided 2 simple scripts (you only need 1) `automated_testing.gd` and `automated_testing.sh` that can be run with the following commands to automate tests using something like pre-commit or github actions:

With the gdscript:
```bash
touch output.txt && godot -e --headless --quit > output.txt && godot --headless --script ./automated_testing.gd
```

With the bash script:
```bash
touch output.txt && godot -e --headless --quit > output.txt && ./automated_testing.sh
```

Or if you don't want to save a seperate script you can do it with this totally not disgusting bash one liner:
```bash
bash -c 'touch output.txt && /home/dm/Documents/GameDev/godot/bin/godot.linuxbsd.editor.x86_64 -e --headless --quit &> output.txt && output_file_path="output.txt"; grep "Failed" "$output_file_path" > failed_tests.txt; rm $output_file_path; if [[ -s failed_tests.txt ]]; then echo "Failed tests:"; cat failed_tests.txt; rm failed_tests.txt; exit 1; else rm failed_tests.txt; exit 0; fi;'
```

I also provide a `.pre-commit-config.yaml` file that you can use as a template to quickly setup [pre-commit](https://pre-commit.com/). It will format and lint all the common components of any godot project in these steps:

1. [ruff](https://github.com/astral-sh/ruff) to lint and format all your scons/python code.
2. [gdscript linter and formatter](https://github.com/Scony/godot-gdscript-toolkit) to lint and format all your gdscript code.
3. [clang-format](https://github.com/llvm/llvm-project/tree/main/clang/tools/clang-format) to format all your C++ code (you will need to change the `find` command in the pre-commit config unless you somehow organize your code exactly like I do)
4. Finally it will run all your SFT tests.

pre-commit will run all this before you make any commits so any code you commit will have to be formatted and tested before it even gets committed.

To add this to your project:
1. Install pre-commit with `sudo pacman -S pre-commit`
2. Copy the `.pre-commit-config.yaml` from this repo into the root of your repository
3. Run `pre-commit install`. Now pre-commit will run all the checks before you make any new commits! You can test if it is working with `pre-commit run --all-files`

This setup for pre-commit I've provided will only work on Linux, if you are on Windows you'll have to figure out how to setup the automated testing on your own.

## Official SFT Haiku

Testing is stupid

I hate stupid fucking tests

Why must I test it
