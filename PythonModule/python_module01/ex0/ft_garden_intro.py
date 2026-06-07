#!/usr/bin/env python3
def print_garden(name: str, height: int, age: int) -> None:
    print("=== Welcome to My Garden ===")
    print(f"Plant: {name}")
    print(f"Height: {height}cm")
    print(f"Age: {age} days")
    print("=== End of Program ===")


if __name__ == "__main__":
    plant_name = "Rose"
    plant_height = 25
    plant_age = 30

    print_garden(plant_name, plant_height, plant_age)
