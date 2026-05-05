#!/bin/sh

set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT_DIR"

fail() {
    echo "FAIL: $1" >&2
    exit 1
}

assert_contains() {
    file=$1
    text=$2
    label=$3

    if ! grep -F "$text" "$file" >/dev/null; then
        echo "Missing expected text: $text" >&2
        echo "--- output ---" >&2
        sed -n '1,220p' "$file" >&2
        fail "$label"
    fi
}

run_game() {
    input=$1
    output=$2
    printf "%b" "$input" | ./yukon > "$output"
}

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"; rm -f decks/codextest.txt codexstate.txt best_time.txt' EXIT

make >/dev/null

run_game "QQ\n" "$TMP_DIR/initial.txt"
assert_contains "$TMP_DIR/initial.txt" "LAST Command: " "initial last command line"
assert_contains "$TMP_DIR/initial.txt" "Message: " "initial message line"
assert_contains "$TMP_DIR/initial.txt" "INPUT > Goodbye!" "startup prompt should be INPUT"

run_game "P\n" "$TMP_DIR/no_deck_play.txt"
assert_contains "$TMP_DIR/no_deck_play.txt" "Message: Error: No deck loaded." "P must require a loaded deck"
assert_contains "$TMP_DIR/no_deck_play.txt" "Goodbye!" "EOF should exit cleanly"

run_game "LD missingfile\nQQ\n" "$TMP_DIR/missing_deck.txt"
assert_contains "$TMP_DIR/missing_deck.txt" "Error: Deck file does not exist: decks/missingfile.txt" "missing LD file must be rejected"
assert_contains "$TMP_DIR/missing_deck.txt" "Message: Error: Could not load deck: missingfile" "missing LD message"
if [ -f decks/missingfile.txt ]; then
    fail "LD missingfile must not create a deck file"
fi

run_game "LD dup\nQQ\n" "$TMP_DIR/duplicate_deck.txt"
assert_contains "$TMP_DIR/duplicate_deck.txt" "Duplicate card detected: 'AH'" "duplicate deck must be rejected"
assert_contains "$TMP_DIR/duplicate_deck.txt" "Message: Error: Could not load deck: dup" "duplicate LD message"

run_game "LD default.txt\nSW\nQQ\n" "$TMP_DIR/show_default.txt"
assert_contains "$TMP_DIR/show_default.txt" "LAST Command: SW" "SW command should run"
assert_contains "$TMP_DIR/show_default.txt" "| AC | 2C | 3C | 4C | 5C | 6C | 7C |" "SW should show loaded cards face up in row-wise startup view"
assert_contains "$TMP_DIR/show_default.txt" "Message: OK" "SW should report OK"

run_game "LD\nP\nQ\nQQ\n" "$TMP_DIR/play_layout.txt"
assert_contains "$TMP_DIR/play_layout.txt" "| AC | [] | [] | [] | [] | [] | [] |  F1 [    ]" "P row 1 layout"
assert_contains "$TMP_DIR/play_layout.txt" "|    | 8C | [] | [] | [] | [] | [] |  F2 [    ]" "P must deal row-wise into C2"
assert_contains "$TMP_DIR/play_layout.txt" "|    | AD | 2D | [] | [] | [] | [] |  F3 [    ]" "P must deal row-wise into C3"
assert_contains "$TMP_DIR/play_layout.txt" "LAST Command: P" "P command tracked"
assert_contains "$TMP_DIR/play_layout.txt" "Message: OK" "P should report OK"
assert_contains "$TMP_DIR/play_layout.txt" "Time: " "timer should print during PLAY"

run_game "LD\nP\nLD\nQ\nQQ\n" "$TMP_DIR/play_phase_command.txt"
assert_contains "$TMP_DIR/play_phase_command.txt" "Message: Command not available in the PLAY phase." "startup commands unavailable during PLAY"

run_game "LD\nP\nC5:AS->F1\nQ\nQQ\n" "$TMP_DIR/foundation_bottom.txt"
assert_contains "$TMP_DIR/foundation_bottom.txt" "Only the bottom card in a column can move to a foundation." "foundation move must use bottom column card"
assert_contains "$TMP_DIR/foundation_bottom.txt" "Message: Move input was invalid or not allowed." "invalid foundation move message"

run_game "LD\nSI 10\nQQ\n" "$TMP_DIR/split_shuffle.txt"
assert_contains "$TMP_DIR/split_shuffle.txt" "| AC | JC | 2C | QC | 3C | KC | 4C |  F1 [    ]" "SI must honor supplied split"
assert_contains "$TMP_DIR/split_shuffle.txt" "LAST Command: SI 10" "SI split command tracked"

run_game "LD\nSD codextest.txt\nQQ\n" "$TMP_DIR/save_deck.txt"
assert_contains "$TMP_DIR/save_deck.txt" "Deck saved to file: decks/codextest.txt" "SD should save exact .txt filename"
assert_contains "$TMP_DIR/save_deck.txt" "Message: OK" "SD should report OK"
if [ ! -f decks/codextest.txt ]; then
    fail "SD did not create decks/codextest.txt"
fi

run_game "LD\nP\nC1->F1\nU\nR\nQ\nQQ\n" "$TMP_DIR/undo_redo.txt"
assert_contains "$TMP_DIR/undo_redo.txt" "LAST Command: C1->F1" "baseline legal move should execute"
assert_contains "$TMP_DIR/undo_redo.txt" "|    | [] | [] | [] | [] | [] | [] |  F1 [ AC ]" "move should place AC on F1"
assert_contains "$TMP_DIR/undo_redo.txt" "LAST Command: U" "undo command tracked"
assert_contains "$TMP_DIR/undo_redo.txt" "| AC | [] | [] | [] | [] | [] | [] |  F1 [    ]" "undo should restore AC to C1"
assert_contains "$TMP_DIR/undo_redo.txt" "LAST Command: R" "redo command tracked"

run_game "LD\nP\nC1->F1\nS codexstate\nQ\nL codexstate\nQ\nQQ\n" "$TMP_DIR/save_load_state.txt"
assert_contains "$TMP_DIR/save_load_state.txt" "Game state saved to file: codexstate.txt" "S should save full game state"
assert_contains "$TMP_DIR/save_load_state.txt" "Game state loaded from file: codexstate.txt" "L should load full game state"
assert_contains "$TMP_DIR/save_load_state.txt" "|    | [] | [] | [] | [] | [] | [] |  F1 [ AC ]" "loaded state should restore foundation"
if [ ! -f codexstate.txt ]; then
    fail "S did not create codexstate.txt"
fi

printf "LD\nP\n" | ./yukon > "$TMP_DIR/eof_play.txt" &
pid=$!
sleep 1
if kill -0 "$pid" 2>/dev/null; then
    kill "$pid" 2>/dev/null || true
    fail "program did not exit on EOF during PLAY"
fi
wait "$pid" || fail "EOF during PLAY exited with failure"
assert_contains "$TMP_DIR/eof_play.txt" "INPUT > Goodbye!" "EOF during PLAY should exit cleanly"

echo "All requirements smoke tests passed."
