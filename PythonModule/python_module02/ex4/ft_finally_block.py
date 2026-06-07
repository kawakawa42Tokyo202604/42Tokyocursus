#!/usr/bin/env python3
class GardenError(Exception):
    def __init__(self, message: str = "error"):
        super().__init__(message)


class PlantError(GardenError):
    def __init__(self, message: str = "Unknown plant error"):
        super().__init__(message)


def water_plant(plant_name: str) -> None:
    if not plant_name[0].isupper():
        raise PlantError(f"Invalid plant name to water: '{plant_name}'")
    print(f"Watering {plant_name}: [OK]")


def test_watering_system(plants: list[str]) -> None:
    print("Opening watering system")
    try:
        for plant in plants:
            water_plant(plant)

    except PlantError as e:
        print(f"Caught PlantError: {e}")
        print(".. ending tests and returning to main")
        return

    finally:
        print("Closing watering system")


def ft_finally_block() -> None:
    print("=== Garden Watering System ===")
    print()
    print("Testing valid plants...")
    test_watering_system(["Tomato", "Lettuce", "Carrots"])
    print()

    print("Testing invalid plants...")
    test_watering_system(["Tomato", "lettuce", "Carrots"])
    print()
    print("Cleanup always happens, even with errors!")


if __name__ == "__main__":
    try:
        ft_finally_block()
    except Exception as e:
        print(f"{e}")
