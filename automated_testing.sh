#!/bin/bash

output_file_path="output.txt"
failed_tests=()
content=$(cat "$output_file_path")
for line in $content; do
    if [[ $line == *"Failed"* ]]; then
        failed_tests+=("$line")
    fi
done

rm "$output_file_path"

if (( ${#failed_tests[@]} > 0 )); then
    exit 1
else
    exit 0
fi
