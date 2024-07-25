#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Initialize counters
total_tests=0
passed_tests=0

# Function to run tests and compare results
run_tests() {
    local test_dir="$1"
    local dir_total=0
    local dir_passed=0

    # Iterate over all test input files
    for test_input in "$test_dir"/*.in; do
        ((dir_total++))
        ((total_tests++))
        test_number=$(basename "$test_input" .in)
        test_output="${test_dir}/${test_number}.out"
        test_result="${test_dir}/${test_number}.in.res"
        test_diff="${test_dir}/${test_number}.diff"
        
        # Run the test
        ./hw3 < "$test_input" > "$test_result"  # Redirect stdout only
        
        # Compare the results
        if diff "$test_result" "$test_output" > /dev/null; then
            echo -e "Test ${test_number} ${GREEN}PASSED${NC}"
            ((dir_passed++))
            ((passed_tests++))
        else
            echo -e "Test ${test_number} ${RED}FAILED${NC}"
            diff "$test_result" "$test_output" > "$test_diff"
        fi
    done

    echo "Directory results: $dir_passed/$dir_total tests passed"
}

# Directory containing the tests
TEST_DIR="tests"

echo "----------------- Starting tests -----------------"
run_tests "$TEST_DIR"

# Directory containing additional tests
TEST_DIR2="tests2"

echo "----------------- Starting tests2 -----------------"
run_tests "$TEST_DIR2"

# Print final results
echo "----------------- Final Results -----------------"
echo "Total tests passed: $passed_tests/$total_tests"