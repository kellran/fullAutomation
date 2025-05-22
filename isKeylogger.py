import re
import json
import requests
from datetime import datetime
import sys
import os

# === CONFIGURATION ===
LLM_BASE_URL = "https://api.openai.com/v1/chat/completions"
LLM_API_KEY = "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"
LLM_MODEL_NAME = "gpt-4"
PROMPT_FILE = "prompts.json"
YES_THRESHOLD = 3


def load_prompts(prompt_file):
    with open(prompt_file, "r") as f:
        data = json.load(f)
    return data["keylogger_prompts"]


def extract_functions(decompiled_text):
    pattern = r"// Function: (.*?)\n\n(.*?)(?=(// Function:|\Z))"
    matches = re.findall(pattern, decompiled_text, re.DOTALL)
    return [{"name": name.strip(), "code": code.strip()} for name, code, _ in matches]


def query_llm(prompt_text, code):
    headers = {
        "Authorization": f"Bearer {LLM_API_KEY}",
        "Content-Type": "application/json"
    }

    session_context = (
        "You are an expert reverse engineer assisting in static binary analysis. "
        "You are identifying suspicious behaviors associated with keyloggers. "
        "You must evaluate the following code in isolation and respond with 'yes' or 'no' only. "
        "Answer 'yes' if there is any indication of the described behavior, even if the evidence is subtle or partial."
    )

    payload = {
        "model": LLM_MODEL_NAME,
        "messages": [
            {"role": "system", "content": session_context},
            {"role": "user", "content": prompt_text.replace("{code}", code)}
        ],
        "temperature": 0
    }

    response = requests.post(LLM_BASE_URL, headers=headers, json=payload)
    response.raise_for_status()
    content = response.json()["choices"][0]["message"]["content"].strip().lower()
    return content.startswith("yes")


def analyze(decompiled):
    source_name = os.path.basename(sys.argv[1]) if len(sys.argv) > 1 else "unknown"
    with open(PROMPT_FILE, "r") as f:
        prompt_data = json.load(f)
    prompts = prompt_data["keylogger_prompts"]
    functions = extract_functions(decompiled)

    category_to_behavior = {
        "Input Capture": "input_capture",
        "Logging": "logging",
        "Exfiltration": "exfiltration",
        "Obfuscation": "obfuscation",
        "Suspicious Globals": "globals",
        "Behavior Context": "looping_behavior",
    }

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open("analysis_log.txt", "a") as log_file:
        log_file.write(f"\n=== Analysis of '{source_name}' started at {timestamp} ===\n")

        threat_levels = {"high": [], "medium": [], "low": []}

        for func in functions:
            if not func["name"].startswith("FUN") and func["name"] != "main":
                continue

            behaviors_triggered = set()

            for prompt in prompts:
                try:
                    result = query_llm(prompt["prompt"], func["code"])
                except Exception as e:
                    msg = f"[{timestamp}] Error querying LLM for function {func['name']}: {e}"
                    print(msg)
                    log_file.write(msg + "\n")
                    continue

                if result:
                    behavior = category_to_behavior.get(prompt["category"], prompt["category"])
                    behaviors_triggered.add(behavior)

            if behaviors_triggered:
                level = "low"
                if len(behaviors_triggered) >= 3:
                    level = "high"
                elif len(behaviors_triggered) == 2:
                    level = "medium"
                threat_levels[level].append({
                    "function": func["name"],
                    "behaviors": list(behaviors_triggered)
                })

        for level in ["high", "medium", "low"]:
            funcs = threat_levels[level]
            if funcs:
                log_file.write(f"\n{level.upper()} THREAT FUNCTIONS:\n")
                print(f"\n{level.upper()} THREAT FUNCTIONS:")
                for entry in funcs:
                    line = f"- {entry['function']} (behaviors: {', '.join(entry['behaviors'])})"
                    print(line)
                    log_file.write(line + "\n")

        log_file.write(f"=== Analysis of '{source_name}' ended at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')} ===\n")
        print(f"\nAnalysis of '{source_name}' completed.\n")

    return len(threat_levels["high"]) > 0 or len(threat_levels["medium"]) > 0
