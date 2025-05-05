# config.py

import os

# Paths to tools/scripts
GHIDRA_PATH = "/home/ghidra/support/analyzeHeadless"
SCRIPT_DIR = os.path.dirname(__file__)
GHIDRA_SCRIPT = os.path.join(SCRIPT_DIR, "ghidra_decompile.py")

# Project structure
PROJECT_DIR = "/tmp/ghidra_proj"
OUTPUT_DIR = os.path.join(SCRIPT_DIR, "outputs")
BINARY_DIR = os.path.join(SCRIPT_DIR, "binaries")

# Ensure output dirs exist (optional: call from main script)
os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(BINARY_DIR, exist_ok=True)
