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
PROMPT_FILE = "keyloggerPrompts.json"
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
    payload = {
        "model": LLM_MODEL_NAME,
        "messages": [
            {"role": "user", "content": prompt_text.replace("{code}", code)}
        ],
        "temperature": 0
    }
    response = requests.post(LLM_BASE_URL, headers=headers, json=payload)
    response.raise_for_status()
    content = response.json()["choices"][0]["message"]["content"].strip().lower()
    return content.startswith("yes")


def analyze(decompiled):
    # Try to infer filename from CLI if available
    source_name = os.path.basename(sys.argv[1]) if len(sys.argv) > 1 else "unknown"

    prompts = load_prompts(PROMPT_FILE)
    functions = extract_functions(decompiled)

    yes_count = 0
    triggered = []

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open("analysis_log.txt", "a") as log_file:
        log_file.write(f"\n=== Analysis of '{source_name}' started at {timestamp} ===\n")
        
        for func in functions:
            for prompt in prompts:
                try:
                    result = query_llm(prompt["prompt"], func["code"])
                except Exception as e:
                    msg = f"[{timestamp}] Error querying LLM for function {func['name']}: {e}"
                    print(msg)
                    log_file.write(msg + "\n")
                    continue

                if result:
                    yes_count += 1
                    triggered.append({
                        "function": func["name"],
                        "prompt_description": prompt["description"],
                        "category": prompt["category"]
                    })

        if triggered:
            msg = "\n⚠️  Suspicious behaviors detected:\n"
            print(msg.strip())
            log_file.write(msg)
            for entry in triggered:
                line = f"- Function `{entry['function']}` triggered [{entry['category']}]: {entry['prompt_description']}\n"
                print(line.strip())
                log_file.write(f"[{timestamp}] {line}")
            summary = f"\nTotal suspicious detections: {yes_count}\n"
            print(summary.strip())
            log_file.write(summary)
        else:
            msg = "\n✅ No suspicious behavior detected.\n"
            print(msg.strip())
            log_file.write(f"[{timestamp}] {msg}")

        log_file.write(f"=== Analysis of '{source_name}' ended at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')} ===\n")

    return yes_count >= YES_THRESHOLD
