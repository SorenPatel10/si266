#!/bin/bash

CC="gcc"
CFLAGS="-Wall -Wextra"
DRIVER="test_driver"
STUDENT="exam.c"

# Uncolored pass/fail formatters
pass() { echo "[PASS] $1"; }
fail() { echo "[FAIL] $1"; }

# Compilation Function
compile_code() {
    # Attempt to compile, sending all standard output/errors to null
    if $CC $CFLAGS -o $DRIVER test_driver.c $STUDENT >/dev/null 2>&1; then
        # Only print the pass message if we called it in "verbose" mode (for 'all')
        if [ "$1" == "verbose" ]; then
            pass "Compilation successful"
        fi
        return 0
    else
        fail "Compilation failed. Grade capped at 50% (manual review). Halting."
        exit 1
    fi
}

# --- Test Functions ---

test_e1() {
    OUT=$(timeout 2s ./$DRIVER e1 2>&1)
    if [[ "$OUT" == *"RESULT:5"* ]]; then
        pass "E1: Character counter found correct number of characters."
    else
        fail "E1: Character counter failed."
    fi
}

test_e2() {
    OUT=$(timeout 2s ./$DRIVER e2 2>&1)
    if [[ "$OUT" == *"HELLO"* && "$OUT" == *"RETURN:0"* ]]; then
        pass "E2: Offset reader skipped 10 bytes and printed correct 5 bytes."
    else
        fail "E2: Offset reader failed or returned incorrect status."
    fi
}

test_e3() {
    OUT=$(timeout 2s ./$DRIVER e3 2>&1)
    if [[ "$OUT" =~ Child\ exited\ with\ code\ [0-9]+ ]]; then
        pass "E3: Dynamic status correctly extracted and printed."
    else
        fail "E3: Dynamic status missing or incorrectly formatted."
    fi
}

test_e4() {
    OUT=$(timeout 2s ./$DRIVER e4 2>&1)
    if [[ "$OUT" == *"SUCCESS"* && "$OUT" != *"This should not print"* ]]; then
        pass "E4: Dynamic exec replaced process successfully."
    else
        fail "E4: Dynamic exec failed or 'This should not print' was executed."
    fi
}

test_m1() {
    OUT=$(timeout 3s ./$DRIVER m1 2>&1)
    RET=$?
    if [ $RET -eq 124 ]; then
        fail "M1: TIMEOUT! You likely created an infinite loop or fork bomb."
    else
        SHIPS=$(echo "$OUT" | grep -c "Ship .* reporting")
        
        # Get the last line and strip any carriage returns for safety
        LAST_LINE=$(echo "$OUT" | tail -n 1 | tr -d '\r')
        
        # Check that we have 3 ships AND the last line is exactly "Fleet assembled"
        if [[ $SHIPS -eq 3 && "$LAST_LINE" == "Fleet assembled" ]]; then
            pass "M1: Fleet commander successfully spawned and waited for 3 ships."
        else
            fail "M1: Fleet commander logic failed or output was out of order."
        fi
    fi
}
test_m2() {
    OUT=$(timeout 2s ./$DRIVER m2 2>&1)
    RET=$?
    if [ $RET -eq 124 ]; then
        fail "M2: TIMEOUT! You forgot to close a pipe end causing a deadlock."
    elif [[ "$OUT" == *"Result: 42"* ]]; then
        pass "M2: Number doubler successfully passed binary data through pipe."
    else
        fail "M2: Number doubler output incorrect."
    fi
}

test_m3() {
    OUT=$(timeout 2s ./$DRIVER m3 2>&1)
    RET=$?
    if [ $RET -eq 124 ]; then
        fail "M3: TIMEOUT! Deadlock detected."
    elif [[ "$OUT" == *"RELAY_WORKS"* ]]; then
        pass "M3: File relay correctly redirected child, waited, and read file."
    else
        fail "M3: File relay failed to output command execution."
    fi
}

# --- Dispatcher ---
# Routes the command line argument to the appropriate function
case "$1" in
    compile) compile_code verbose ;;
    e1) compile_code quiet && test_e1 ;;
    e2) compile_code quiet && test_e2 ;;
    e3) compile_code quiet && test_e3 ;;
    e4) compile_code quiet && test_e4 ;;
    m1) compile_code quiet && test_m1 ;;
    m2) compile_code quiet && test_m2 ;;
    m3) compile_code quiet && test_m3 ;;
    all)
        compile_code verbose
        test_e1
        test_e2
        test_e3
        test_e4
        test_m1
        test_m2
        test_m3
        ;;
    *)
        echo "Usage: $0 {compile|e1|e2|e3|e4|m1|m2|m3|all}"
        exit 1
        ;;
esac
