#!/usr/bin/env python3
import alchemy.transmutation


def main() -> None:
    print("=== Transmutation 1 ===")
    print("Import transmutation module directly")
    res = alchemy.transmutation.lead_to_gold()
    print(f"Testing lead to gold: {res}")


if __name__ == "__main__":
    main()
