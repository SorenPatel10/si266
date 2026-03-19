#!/bin/bash

# Clean up old test files
rm -f student_out_1.txt student_out_4.txt valgrind_out.txt

echo "========================================"
echo " Starting pthreads Autograder..."
echo "========================================"

# Make sure they actually compiled it
if [ ! -f "./crack" ]; then
    make crack || { echo "[FATAL] Failed to compile crack.c"; exit 1; }
fi

# Test 1: Concurrency Check (1 vs 4 threads)
echo "[INFO] Running with 1 thread..."
START_1=$(date +%s)
./crack hashes.digest 1 > student_out_1.txt 2>&1
END_1=$(date +%s)
TIME_1=$((END_1 - START_1))

echo "[INFO] Running with 4 threads..."
START_4=$(date +%s)
./crack hashes.digest 4 > student_out_4.txt 2>&1
END_4=$(date +%s)
TIME_4=$((END_4 - START_4))

# Ensure 4 threads is actually faster than 1 thread (allow 30% margin of error for overhead)
EXPECTED_MAX_TIME=$(( TIME_1 * 70 / 100 ))

if [ "$TIME_4" -gt "$EXPECTED_MAX_TIME" ]; then
    echo "[FAIL] Concurrency Check!"
    echo "Your 4-thread run ($TIME_4 s) wasn't significantly faster than 1-thread ($TIME_1 s)."
    echo "Did you accidentally put pthread_join() inside your thread creation loop?"
    exit 1
fi
echo "[PASS] Concurrency Check."

# Test 2: Correctness Check (Checking the final main loop output)
SUCCESS_COUNT=$(grep -i "Index [0-9]: " student_out_4.txt | wc -l)
if [ "$SUCCESS_COUNT" -ne 10 ]; then
    echo "[FAIL] Correctness Check!"
    echo "Expected 10 printed passwords in main, found $SUCCESS_COUNT."
    echo "Check the remainder math in your array partitioning."
    exit 1
fi
echo "[PASS] Correctness Check."

# Test 3: Disjoint Memory Check (No shared argument trap)
# We expect the output in main to look like "Index 0: apple", "Index 1: basic", etc.
UNIQUE_PASSWORDS=$(grep -i "Index [0-9]:" student_out_4.txt | awk '{print $3}' | sort -u | wc -l)

if [ "$UNIQUE_PASSWORDS" -ne 10 ]; then
    echo "[FAIL] Disjoint Memory Check!"
    echo "Multiple indices returned the exact same password."
    echo "Your memory boundaries overlapped, or you suffered from the Shared Argument pointer trap!"
    exit 1
fi
echo "[PASS] Disjoint Memory Check."

# Test 4: Valgrind Check (Using easy.digest to bypass the time trap)
echo "[INFO] Running Valgrind memory check on easy.digest..."
# We use --show-leak-kinds=definite to ignore OpenSSL's global setup memory caches
valgrind --leak-check=full --show-leak-kinds=definite --error-exitcode=1 ./crack easy.digest 4 > /dev/null 2> valgrind_out.txt
if [ $? -ne 0 ]; then
    echo "[FAIL] Memory Leak Detected!"
    echo "Did you remember to free() the array of structs you dynamically allocated in main?"
    echo "--- Valgrind Output ---"
    grep "definitely lost:" valgrind_out.txt
    exit 1
fi
echo "[PASS] Valgrind Memory Check."

echo "========================================"
echo "[SUCCESS] All threaded cracking tests passed!"
exit 0
