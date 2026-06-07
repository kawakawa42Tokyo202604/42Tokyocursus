#!/usr/bin/env python3
import math


def get_player_pos() -> tuple[float, float, float]:
    while True:
        pos = input("Enter new coordinates as floats in format 'x,y,z': ")
        parts = pos.split(",")

        if len(parts) != 3:
            print("Invalid syntax")
            continue

        try:
            temp = parts[0].strip()
            x = float(temp)

            temp = parts[1].strip()
            y = float(temp)

            temp = parts[2].strip()
            z = float(temp)
            return (x, y, z)

        except ValueError:
            print(f"Error on parameter '{temp}': could not ", end="")
            print(f"convert string to float: '{temp}'")


def main() -> None:
    print("=== Game Coordinate System ===")
    print()
    print("Get a first set of coordinates")
    pos1 = get_player_pos()
    print(f"Got a first tuple: {pos1}")
    print(f"It includes: X={pos1[0]}, Y={pos1[1]}, Z={pos1[2]}")
    dist = math.sqrt(pos1[0] ** 2 + pos1[1] ** 2 + pos1[2] ** 2)
    print(f"Distance to center: {round(dist, 4)}")
    print()
    print("Get a second set of coordinates")
    pos2 = get_player_pos()
    distance = math.sqrt(
        (pos2[0] - pos1[0]) ** 2
        + (pos2[1] - pos1[1]) ** 2
        + (pos2[2] - pos1[2]) ** 2
    )
    print(f"Distance between locations: {round(distance, 4)}")


if __name__ == "__main__":
    main()
