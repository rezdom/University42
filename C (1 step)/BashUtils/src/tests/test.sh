#!/bin/bash

TEST_DIR="./test_files"

CAT_FLAGS=("" "-b" "-e" "-n" "-s" "-t" "--number-nonblank" "-E" "--number" "--squeeze-blank" "-T")
GREP_FLAGS=("" "-i" "-v" "-c" "-l" "-n")

assert_equal() {
    local expected="$1"
    local actual="$2"
    local message="$3"

    if diff -q <(echo "$expected") <(echo "$actual") >/dev/null; then
        echo "✅ PASS: $message"
    else
        echo "❌ FAIL: $message"
        echo "--- Expected:"
        echo "$expected"
        echo "--- Actual:"
        echo "$actual"
        exit 1
    fi
}

echo "Testing cat..."
for file in "$TEST_DIR"/*.txt; do
    for flag in "${CAT_FLAGS[@]}"; do
        expected=$(cat $flag "$file")
        actual=$(../cat/s21_cat $flag "$file")
        assert_equal "$expected" "$actual" "my_cat $flag $file"
    done
done

echo "Testing grep..."
pattern="Hello"
for file in "$TEST_DIR"/*.txt; do
    for flag in "${GREP_FLAGS[@]}"; do
        expected=$(grep $flag "$pattern" "$file")
        actual=$(../grep/s21_grep $flag "$pattern" "$file")
        assert_equal "$expected" "$actual" "my_grep $flag $pattern $file"
    done
    expected=$(grep -e Hello "$file")
    actual=$(../grep/s21_grep -e Hello "$file")
    assert_equal "$expected" "$actual" "my_grep -e Hello $file"
done

echo "All tests completed."