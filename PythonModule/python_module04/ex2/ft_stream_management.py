#!/usr/bin/env python3
import sys


def main() -> None:
    """Cyber Archives Stream Management Process for ex2."""
    if len(sys.argv) != 2:
        sys.stderr.write(f"[STDERR] Usage: {sys.argv[0]} <file>\n")
        sys.exit(1)

    filename = sys.argv[1]

    sys.stdout.write("=== Cyber Archives Recovery & Preservation ===\n")
    sys.stdout.write(f"Accessing file '{filename}'\n")

    try:
        f = open(filename, "r", encoding="utf-8")
        content = f.read()
        sys.stdout.write("---\n")
        sys.stdout.write(content)
        sys.stdout.write("---\n")
        f.close()
        sys.stdout.write(f"File '{filename}' closed.\n")
    except Exception as e:
        sys.stderr.write(f"[STDERR] Error opening file '{filename}': {e}\n")
        sys.exit(1)

    sys.stdout.write("Transform data:\n")
    sys.stdout.write("---\n")

    lines = content.splitlines()
    new_lines = []
    for line in lines:
        new_lines.append(f"{line}#")

    new_content = "\n".join(new_lines) + "\n"
    sys.stdout.write(new_content)
    sys.stdout.write("---\n")

    sys.stdout.write("Enter new file name (or empty): ")
    sys.stdout.flush()

    raw_input = sys.stdin.readline()
    new_filename = raw_input.rstrip("\n")

    if new_filename == "":
        sys.stdout.write("Not saving data.\n")
    else:
        sys.stdout.write(f"Saving data to '{new_filename}'\n")
        try:
            new_f = open(new_filename, "w", encoding="utf-8")
            new_f.write(new_content)
            new_f.close()
            sys.stdout.write(f"Data saved in file '{new_filename}'.\n")
        except Exception as e:
            sys.stderr.write(
                f"[STDERR] Error opening file '{new_filename}': {e}\n"
            )
            sys.stdout.write("Data not saved.\n")
            sys.exit(1)


if __name__ == "__main__":
    main()
