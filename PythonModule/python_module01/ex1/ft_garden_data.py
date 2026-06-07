#!/usr/bin/env python3

class Plant:
    name: str
    height: int
    age: int

    def show(self) -> None:
        print(f"{self.name}: {self.height}cm, {self.age} days old")


def ft_print_info() -> None:
    print("=== Garden Plant Registry ===")
    rose = Plant()
    rose.name = "Rose"
    rose.height = 25
    rose.age = 30
    sunflower = Plant()
    sunflower.name = "Sunflower"
    sunflower.height = 80
    sunflower.age = 45
    cactus = Plant()
    cactus.name = "Cactus"
    cactus.height = 15
    cactus.age = 120
    rose.show()
    sunflower.show()
    cactus.show()


if __name__ == "__main__":
    ft_print_info()
