#!/usr/bin/env python3

import sys
from ghidra_runner import run_ghidra_decompile
import optimizeDegpt
import isKeylogger

def main():
    if len(sys.argv) != 2:
        print("Usage: ./main.py <FileToTest>")
        sys.exit(1)

    test_file = sys.argv[1]

    decompiled = run_ghidra_decompile(test_file)
    #optimized = optimizeDegpt.process(decompiled)
    result = isKeylogger.analyze(decompiled)

    print(result)

if __name__ == "__main__":
    main()
