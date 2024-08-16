#!/bin/bash

# Create an empty output file for Godot's output
touch output.txt

# Run Godot in headless mode and redirect both stdout and stderr to output.txt
usr/bin/godot -e --headless --quit &> output.txt
output_file_path="output.txt"

# Search the output file for lines containing "Failed" and redirect them to failed_tests.txt
grep "Failed" "$output_file_path" > failed_tests.txt
rm $output_file_path

# Check if the failed_tests.txt file contains any lines indicating failures
if [[ -s failed_tests.txt ]]; then
    # If there are failed tests, print a message and display the contents of failed_tests.txt
    echo "Failed tests:"
    cat failed_tests.txt
    rm failed_tests.txt
    exit 1
else
    rm failed_tests.txt
    exit 0
fi
