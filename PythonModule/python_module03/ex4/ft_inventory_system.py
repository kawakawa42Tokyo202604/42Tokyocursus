#!/usr/bin/env python3
import sys


def main() -> None:
    print("=== Inventory System Analysis ===")
    if len(sys.argv) < 2:
        print("No arguments provided!")
        sys.exit(1)
    inventory = {}
    for arg in sys.argv[1:]:
        if ":" not in arg:
            print(f"Error - invalid parameter '{arg}'")
            continue

        parts = arg.split(":", 1)
        item = parts[0]
        count = parts[1]
        if item in inventory:
            print(f"Redundant item '{item}' - discarding")
            continue

        try:
            quantity = int(count)
            inventory[item] = quantity
        except ValueError as e:
            print(f"Quantity error for '{item}': {e}")

    if not inventory:
        print("Got inventory: {}")
        return

    print(f"Got inventory: {inventory}")
    item_list = list(inventory.keys())
    print(f"Item list: {item_list}")

    total_count = sum(inventory.values())
    print(f"Total quantity of the {len(inventory)} items: {total_count}")

    for item, qty in inventory.items():
        percentage = (qty / total_count) * 100
        print(f"Item {item} represents {percentage:.1f}%")

    most_item = None
    most_qty = -1
    least_item = None
    least_qty = float("inf")

    for item, qty in inventory.items():
        if qty > most_qty:
            most_qty = qty
            most_item = item
        if qty < least_qty:
            least_qty = qty
            least_item = item

    print(f"Item most abundant: {most_item} with {most_qty}")
    print(f"Item least abundant: {least_item} with {least_qty}")
    inventory["magic_item"] = 1
    print(f"Updated inventory: {inventory}")


if __name__ == "__main__":
    main()
