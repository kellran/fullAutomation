import subprocess
import os
from config import GHIDRA_PATH, GHIDRA_SCRIPT, PROJECT_DIR, OUTPUT_DIR

def run_ghidra_decompile(binary_path):
    output_file = os.path.join(
        OUTPUT_DIR,
        os.path.basename(binary_path) + "_pseudocode.c"
    )

    os.makedirs(PROJECT_DIR, exist_ok=True)

    cmd = [
        GHIDRA_PATH,
        PROJECT_DIR,
        "temp_project",
        "-import", os.path.abspath(binary_path),
        "-scriptPath", os.path.dirname(GHIDRA_SCRIPT),
        "-postScript", os.path.basename(GHIDRA_SCRIPT),
        "-deleteProject"
    ]

    subprocess.run(cmd, check=True)

    with open(output_file, "r", encoding="utf-8") as f:
        return f.read()
