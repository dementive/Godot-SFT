//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#include "summator.hpp"
#include "tests/sft_example_tests.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Summator::Summator()
{
    count = 0;
}

Summator::~Summator()
{
}

void Summator::add(int p_value)
{
    count += p_value;
}

void Summator::reset()
{
    count = 0;
}

int Summator::get_total() const
{
    return count;
}

void Summator::test() {
    // This function runs the SFT tests.
    // There are lots of different places you can run these tests from.
    // I reccommend creating an autoload and running them there so they can be automatically run in headless mode.
    // You have to be very careful about when and where you run the tests though as there are a LOT of wrong places
    // that you can run them in that are too early for godot to keep track of the object lifetimes properly.
    run_tests();
}


void Summator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("add", "value"), &Summator::add, DEFVAL(1));
    ClassDB::bind_method(D_METHOD("reset"), &Summator::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Summator::get_total);

    ClassDB::bind_method(D_METHOD("test"), &Summator::test);
}