#!/usr/bin/env python3
class Plant:
    name: str
    height: float
    age_val: int
    temp: float

    def show(self) -> None:
        print(f"{self.name}: {self.height:.1f}cm, {self.age_val} days old")

    def grow(self) -> None:
        if self.name == "Rose":
            self.height += 0.8
        if self.name == "Sunflower":
            self.height += 2.0
        if self.name == "Cactus":
            self.height += 0.1
        self.height = round(self.height, 1)

    def age(self) -> None:
        self.age_val += 1


def ft_growth_plant() -> None:
    rose = Plant()
    rose.name = "Rose"
    rose.height = 25
    rose.age_val = 30
    rose.temp = rose.height
    sunflower = Plant()
    sunflower.name = "Sunflower"
    sunflower.height = 80
    sunflower.age_val = 45
    sunflower.temp = sunflower.height
    cactus = Plant()
    cactus.name = "Cactus"
    cactus.height = 15
    cactus.age_val = 120
    cactus.temp = cactus.height
    garden = [rose, sunflower, cactus]
    targets = ["Rose"]
    print("=== Garden Plant Growth ===")
    for plant in garden:
        if plant.name in targets:
            plant.show()
    for i in range(1, 8):
        for plant in garden:
            if plant.name in targets:
                plant.grow()
                plant.age()
        print(f"=== Day {i} ===")
        for plant in garden:
            if plant.name in targets:
                plant.show()
    for plant in garden:
        if plant.name in targets:
            total_growth = round(plant.height - plant.temp, 1)
            print(f"Growth this week: {total_growth}cm")


if __name__ == "__main__":
    ft_growth_plant()
