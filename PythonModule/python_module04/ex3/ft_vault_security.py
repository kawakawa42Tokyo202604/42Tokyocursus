#!/usr/bin/env python3
def secure_archive(
        filename: str, action: str = "r", content: str = ""
        ) -> tuple[bool, str]:
    if action == "r":
        try:
            with open(filename, "r", encoding="utf-8") as f:
                data = f.read()
            return True, data
        except Exception as e:
            return (False, str(e))

    elif action == "w":
        try:
            with open(filename, "w", encoding="utf-8") as f:
                f.write(content)
            return (True, "Write successful")
        except Exception as e:
            return (False, str(e))

    else:
        return (False, f"Unsupported action mode: '{action}'")


def main() -> None:
    print("=== Cyber Archives Security ===")
    print("Using 'secure_archive' to read from a nonexistent file:")
    res = secure_archive("/not/existing/file")
    print(res)

    print("Using 'secure_archive' to read from an inaccessible file:")
    res = secure_archive("/etc/master.passwd")
    print(res)

    print("Using 'secure_archive' to read from a regular file:")
    res = secure_archive("test.txt")
    print(res)

    print("Using 'secure_archive' to write previous content")
    if res[0]:
        secure_archive("new_file", "w", res[1])


if __name__ == "__main__":
    main()
