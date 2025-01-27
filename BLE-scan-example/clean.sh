#!/bin/bash

# Get the project directory (where the script is located)
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Change to the project directory before running functions
cd "$PROJECT_DIR"

# Define file paths relative to the project directory
MESON_FILE="./meson.build"
BRD_CFG_FILE="./.vscode/brd.cfg"

## Function to remove everything between #***_Start and #***_End in meson.build
clean_meson_file() {
  echo "Cleaning $(realpath "$MESON_FILE")"
  awk '
  BEGIN { keep=1 }
  /#.*_Start/ { print; keep=0 }
  /#.*_End/ { print; keep=1; next }
  keep { print }
  ' "$MESON_FILE" > "$MESON_FILE.tmp" && mv "$MESON_FILE.tmp" "$MESON_FILE"
  sed -i '' -e '/#.*_Line/{n;s/.*//;}' "$MESON_FILE"
  sed -i '' -e '/#.*_Lines/{n;s/.*//;n;s/.*//;}' "$MESON_FILE"
}

clean_build_folder() {
  echo "Cleaning $(realpath "./build")"
  find "./build" -type f ! -name '*.elf' ! -name '*.hex' -exec rm -f {} +
  find "./build" -type d -mindepth 1 -exec rm -rf {} +
}

# Function to remove the first line of .vscode/brd.cfg if it contains "PROGRAMMER"
clean_brd_cfg_file() {
  echo "Cleaning $(realpath "$BRD_CFG_FILE")"
  # Read the first line of the file
  first_line=$(head -n 1 "$BRD_CFG_FILE")
  
  # Check if the first line contains "PROGRAMMER"
  if [[ "$first_line" == *PROGRAMMER* ]]; then
    # Remove the first line
    sed -i '' -e '1d' "$BRD_CFG_FILE"
  fi
}

clean_psoccreator_folder() {
  echo "Cleaning folders and files in $PWD"

  # Remove directories and their contents
  rm -rf codegentemp
  rm -rf CortexM0
  rm -rf CortexM0p
  rm -rf CortexM3
  rm -rf CortexM4
  #rm -rf Generated_Source
  #rm -rf Export
  rm -rf armgcc
  rm -rf iar
  rm -rf mdk

  # Remove specific file types
  rm -f *.rpt
  rm -f *.cyfit
  rm -f *.cycdx
  rm -f *.html
  rm -f *.svd
  rm -f *.scat
  #rm -f *.ld
  rm -f *.icf
  rm -f *.cyprj.*
  rm -f *.cywrk.*
  #rm -f *system_psoc63_cm*.c
  #rm -f cy_ipc_config.*
}

# Run the functions
clean_meson_file
clean_brd_cfg_file
clean_build_folder

# Execute cleaning function in every .cydsn folder in the immediate directory
for dir in $(find "$PROJECT_DIR" -maxdepth 1 -type d -name "*.cydsn"); do
   cd "$dir"
   clean_psoccreator_folder;
done
