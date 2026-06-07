#!/usr/bin/env python3
import alchemy


def main() -> None:
    print("=== Alembic 4 ===")
    print("Accessing the alchemy module using 'import alchemy'")
    print(f"Testing create_air: {alchemy.create_air()}")
    print("Now show that not all functions can be reached")
    print("This will raise an exception!")

    print("Testing the hidden create_earth: ", end="")
    print(f"{alchemy.create_earth()}")  # type: ignore[attr-defined]


if __name__ == "__main__":
    main()
