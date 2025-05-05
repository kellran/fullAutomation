from java.io import FileWriter
import os

# Match the OUTPUT_DIR from config.py
output_dir = "/home/kelran/Documents/fullAutomation/outputs"
filename = os.path.basename(currentProgram.getExecutablePath())
output_path = os.path.join(output_dir, filename + "_pseudocode.c")

from ghidra.util.task import ConsoleTaskMonitor
from ghidra.app.decompiler import DecompInterface

decompiler = DecompInterface()
decompiler.openProgram(currentProgram)

writer = FileWriter(output_path)

functionManager = currentProgram.getFunctionManager()
functions = functionManager.getFunctions(True)

for function in functions:
    results = decompiler.decompileFunction(function, 30, ConsoleTaskMonitor())
    if results.decompileCompleted():
        writer.write("// Function: %s\n" % function.getName())
        writer.write(results.getDecompiledFunction().getC())
        writer.write("\n\n")

writer.close()
print("[GHIDRA SCRIPT] Decompiled pseudocode written to: " + output_path)
