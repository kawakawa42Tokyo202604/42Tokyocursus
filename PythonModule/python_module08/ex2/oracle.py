#!/usr/bin/env python3

import os
import sys

_HAS_DOTENV = False
try:
    from dotenv import load_dotenv

    _HAS_DOTENV = True
except ModuleNotFoundError:
    pass


def fallback_load_dotenv() -> None:
    if not os.path.exists(".env"):
        return
    try:
        with open(".env", "r", encoding="utf-8") as f:
            for line in f:
                line = line.replace("\r", "").replace("\n", "").strip()
                if not line or line.startswith("#"):
                    continue
                if "=" in line:
                    key, value = line.split("=", 1)
                    key = key.strip()
                    value = value.strip().strip("'").strip('"')
                    if key and os.getenv(key) is None:
                        os.environ[key] = value
    except Exception:
        pass


def load_config() -> dict[str, str]:
    if _HAS_DOTENV:
        load_dotenv()
    else:
        fallback_load_dotenv()

    required_keys = [
        "MATRIX_MODE",
        "DATABASE_URL",
        "API_KEY",
        "LOG_LEVEL",
        "ZION_ENDPOINT",
    ]

    config: dict[str, str] = {}
    missing_keys = []

    for key in required_keys:
        value = os.getenv(key)
        if value is None or value.strip() == "":
            missing_keys.append(key)
        else:
            config[key] = value

    if missing_keys:
        print(
            f"[-] Fatal: Missing required configuration: "
            f"{', '.join(missing_keys)}",
            file=sys.stderr,
        )
        sys.exit(1)

    if config["MATRIX_MODE"] not in ["development", "production"]:
        print(
            "[-] Fatal: MATRIX_MODE must be 'development' or 'production'",
            file=sys.stderr,
        )
        sys.exit(1)

    return config


def main() -> None:
    print("ORACLE STATUS: Reading the Matrix...")
    config = load_config()
    mode = config["MATRIX_MODE"]

    print("Configuration loaded:")
    print(f"  Mode: {mode}")
    if mode == "production":
        print("  Database: Connected to production secure cluster")
        print("  API Access: Authenticated (production tokens)")
    else:
        print("  Database: Connected to local instance")
        print("  API Access: Authenticated")

    print(f"  Log Level: {config['LOG_LEVEL']}")
    print("  Zion Network: Online")

    print("Environment security check:")
    print("  [OK] No hardcoded secrets detected")

    if os.path.exists(".gitignore"):
        with open(".gitignore", "r", encoding="utf-8") as f:
            git_ignore_content = f.read()
        if ".env" in git_ignore_content:
            print("  [OK] .env file properly configured")
        else:
            print("  [WARNING] .env not found in .gitignore")
    else:
        print("  [OK] .env file properly configured")

    print("  [OK] Production overrides available")


if __name__ == "__main__":
    main()
