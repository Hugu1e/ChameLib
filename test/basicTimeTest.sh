#!/bin/bash

directories=(
    "ABE_test"
    "scheme_test/CH_test"
    "scheme_test/IBCH_test"
    "scheme_test/PBCH_test"
)

#  repeat test times
REPEAT=1000

for dir in "${directories[@]}"; do
    if [ -d "$dir" ]; then
        echo "Contents of $dir:"
        ls "$dir"
        echo "--------------------------------------------------------------------------------"
        # run test
        for file in "$dir"/*; do
            if [ -f "$file" ]; then
                TEST_NAME=$(basename "$file" | sed 's/\.[^.]*$//')
                cd ../build
                ./bin/"$TEST_NAME" --repeat ${REPEAT} --checktime
                cd - > /dev/null
            fi
        done
    else
        echo "Directory $dir does not exist."
    fi
done