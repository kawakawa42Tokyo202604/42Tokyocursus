#!/usr/bin/env python3
import sys


def main() -> None:
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <file>")
        sys.exit(1)
    filename = sys.argv[1]
    print("=== Cyber Archives Recovery ===")
    print(f"Accessing file '{filename}'")
    try:
        f = open(filename, "r", encoding="utf-8")
        content = f.read()
        print("---")
        print(content, end="")
        print("---")
        f.close()
        print(f"File '{filename}' closed.")
    except Exception as e:
        print(f"Error opening file '{filename}': {e}")
        sys.exit(1)

    print("Transform data:")
    print("---")
    print()
    lines = content.splitlines()
    new_lines = []
    for line in lines:
        new_lines.append(f"{line}#")
    new_content = "\n".join(new_lines) + "\n"
    print(new_content, end="")
    print("---")

    new_filename = input("Enter new file name (or empty): ")
    if new_filename == "":
        print("Not saving data.")
    else:
        print(f"Saving data to '{new_filename}'")
        try:
            new_f = open(new_filename, "w", encoding="utf-8")
            new_f.write(new_content)
            new_f.close()
            print(f"Data saved in file '{new_filename}'.")
        except Exception as e:
            print(f"Error '{new_filename}': {e}")


if __name__ == "__main__":
    main()
