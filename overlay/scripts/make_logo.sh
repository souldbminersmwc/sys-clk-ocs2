#!/bin/bash
set -e

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

DEST="$CURRENT_DIR/../data/logo_rgba.bin"
FONT="$CURRENT_DIR/../../manager/resources/fira/FiraSans-Medium-rnx.ttf"
FONT_SIZE="24"
TEXT="sys-clk-ocs2"

function winpath() {
    if command -v cygpath >/dev/null 2>&1; then
        cygpath -w "$1"
    else
        echo "$1"
    fi
}

function render() {
    local font=$(winpath "$1")
    local out="$4"

    # If output starts with RGBA:, strip it and convert the path only
    if [[ "$out" == RGBA:* ]]; then
        local rawpath="${out#RGBA:}"
        rawpath=$(winpath "$rawpath")
        out="RGBA:$rawpath"
    else
        out=$(winpath "$out")
    fi

    convert -background none -colorspace RGB -depth 8 \
            -fill white -font "$font" -pointsize "$2" "label:$3" "$out"
}

# Debug (optional)
# render "$FONT" "$FONT_SIZE" "$TEXT" txt:-

# Final output
render "$FONT" "$FONT_SIZE" "$TEXT" "RGBA:$DEST"
