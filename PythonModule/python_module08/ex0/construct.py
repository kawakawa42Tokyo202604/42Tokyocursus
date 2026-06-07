#!/usr/bin/env python3
import os
import site
import sys


def is_virtual_env() -> bool:
    try:
        return sys.prefix != sys.base_prefix or "VIRTUAL_ENV" in os.environ
    except Exception as e:
        print(f"Error detecting environment: {e}", file=sys.stderr)
        return False


def main() -> None:
    try:
        current_python: str = sys.executable
        if is_virtual_env():
            env_path: str = sys.prefix
            env_name: str = os.path.basename(env_path)
            site_packages: list[str] = site.getsitepackages()
            pkg_path: str = site_packages[0] if site_packages else "Unknown"

            print("MATRIX STATUS: Welcome to the construct")
            print()
            print(f"Current Python: {current_python}")
            print(f"Virtual Environment: {env_name}")
            print()
            print(f"Environment Path: {env_path}")
            print("SUCCESS: You're in an isolated environment!")
            print("Safe to install packages without affecting")
            print("the global system.")
            print()
            print("Package installation path:")
            print(f"{pkg_path}")
        else:
            print("MATRIX STATUS: You're still plugged in")
            print()
            print(f"Current Python: {current_python}")
            print("Virtual Environment: None detected")
            print()
            print("WARNING: You're in the global environment!")
            print("The machines can see everything you install.")
            print()
            print("To enter the construct, run:")
            print("python -m venv matrix_env")
            print("source matrix_env/bin/activate # On Unix")
            print(r"matrix_env\Scripts\activate # On Windows")
            print()
            print("Then run this program again.")
    except Exception as e:
        print(f"An unexpected system failure occurred: {e}", file=sys.stderr)


if __name__ == "__main__":
    main()
