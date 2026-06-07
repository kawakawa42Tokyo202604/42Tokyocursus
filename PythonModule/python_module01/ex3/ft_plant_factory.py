#!/usr/bin/env python3

class Plant:
    def __init__(self, name: str, height: float, age: int) -> None:
        self.name = name
        self.height = float(height)
        self.age = age

    def show(self) -> None:
        print(f"{self.name}: {self.height:.1f}cm, {self.age} days old")


def ft_init_plant() -> None:
    rose = Plant("Rose", 25.0, 30)
    oak = Plant("Oak", 200.0, 30)
    cactus = Plant("Cactus", 5.0, 90)
    sunflower = Plant("Sunflower", 80.0, 45)
    fern = Plant("Fern", 15.0, 120)
    garden = [rose, oak, cactus, sunflower, fern]
    targets = ["Rose", "Fern"]
    for plant in garden:
        if plant.name in targets:
            plant.show()


if __name__ == "__main__":
    ft_init_plant()
