#!/bin/bash

# Path to the interpreter executable
INTERPRETER="./interpreter"

# Output results file
RESULTS_FILE="results.txt"

# Clear the results file if it already exists
> $RESULTS_FILE

# Loop through test files
for i in {1..11}; do
    TEST_FILE="test${i}.txt"
    echo "Running ${TEST_FILE}..." >> $RESULTS_FILE

    # Run the interpreter and append the output to the results file
    if [ -f "$TEST_FILE" ]; then
        $INTERPRETER $TEST_FILE >> $RESULTS_FILE 2>&1
        echo "Finished ${TEST_FILE}" >> $RESULTS_FILE
        echo "------------------------------------" >> $RESULTS_FILE
    else
        echo "File ${TEST_FILE} not found!" >> $RESULTS_FILE
    fi
done

echo "All tests completed. Results saved in ${RESULTS_FILE}."