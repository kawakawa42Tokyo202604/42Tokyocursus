#!/usr/bin/env python3

class Plant:
    class _Stats:
        def __init__(self) -> None:
            self.grow_count: int = 0
            self.age_count: int = 0
            self.show_count: int = 0

        def display(self) -> None:
            print(
                f"Stats: {self.grow_count} grow, "
                f"{self.age_count} age, {self.show_count} show"
            )

    def __init__(self, name: str, height: float, age_val: int) -> None:
        self.name: str = name
        self.height: float = height
        self.age_val: int = age_val
        self._stats: Plant._Stats = self._Stats()

    @staticmethod
    def check_year(year: int) -> None:
        print(f"Is {year} days more than a year? -> ", end="")
        if year > 365:
            print("True")
        else:
            print("False")

    @classmethod
    def create_anonymous(cls) -> "Plant":
        return cls("Unknown plant", 0.0, 0)

    def show(self) -> None:
        print(f"{self.name}: {self.height:.1f}cm, {self.age_val} days old")
        self._stats.show_count += 1

    def grow(self, amount: float) -> None:
        self.height += amount
        self._stats.grow_count += 1

    def age(self, days: int) -> None:
        self.age_val += days
        self._stats.age_count += 1

    def display_stats(self) -> None:
        print(f"[statistics for {self.name}]")
        self._stats.display()


class Flower(Plant):
    def __init__(self, name: str, height: float, age_val: int, colour: str
                 ) -> None:
        super().__init__(name, height, age_val)
        self.colour: str = colour
        self.is_blooming: bool = False

    def bloom(self) -> None:
        self.is_blooming = True

    def show(self) -> None:
        super().show()
        print(f"colour: {self.colour}")
        if self.is_blooming:
            print(f"{self.name} is blooming beautifully!")
        else:
            print(f"{self.name} has not bloomed yet")


class Seed(Flower):
    def __init__(self, name: str, height: float, age_val: int, colour: str
                 ) -> None:
        super().__init__(name, height, age_val, colour)
        self.seeds_count: int = 0

    def show(self) -> None:
        super().show()
        print(f"Seeds: {self.seeds_count}")


class Tree(Plant):
    class _TreeStats(Plant._Stats):
        def __init__(self) -> None:
            super().__init__()
            self.shade_count: int = 0

        def display(self) -> None:
            super().display()
            print(f"{self.shade_count} shade")

    _stats: _TreeStats

    def __init__(
        self, name: str, height: float, age_val: int, trunk_diameter: float
    ) -> None:
        super().__init__(name, height, age_val)
        self.trunk_diameter = float(trunk_diameter)
        self._stats = self._TreeStats()

    def produce_shade(self) -> None:
        print(f"Tree {self.name} now produces a shade of ", end="")
        print(f"{self.height:.1f}cm long and ", end="")
        print(f"{self.trunk_diameter:.1f}cm wide.")
        self._stats.shade_count += 1

    def show(self) -> None:
        super().show()
        print(f"Trunk diameter: {self.trunk_diameter:.1f}cm")


def display_any_plant_statistics(plant: Plant) -> None:
    plant.display_stats()


def ft_plant_types() -> None:
    print("=== Garden statistics ===")
    print("=== Check year-old")
    Plant.check_year(30)
    Plant.check_year(400)

    print("\n=== Flower")
    rose = Flower("Rose", 15.0, 10, "red")
    rose.show()
    display_any_plant_statistics(rose)

    print("[asking the rose to grow and bloom]")
    rose.grow(8.0)
    rose.bloom()
    rose.show()
    display_any_plant_statistics(rose)

    print("\n=== Tree")
    oak = Tree("Oak", 200.0, 365, 5.0)
    oak.show()
    display_any_plant_statistics(oak)
    print("[asking the oak to produce shade]")
    oak.produce_shade()
    display_any_plant_statistics(oak)

    print("\n=== Seed")
    sunflower = Seed("Sunflower", 80.0, 45, "yellow")
    sunflower.show()
    print("[make sunflower grow, age and bloom]")
    sunflower.grow(30.0)
    sunflower.age(20)
    sunflower.bloom()
    sunflower.seeds_count = 42
    sunflower.show()
    display_any_plant_statistics(sunflower)

    print("\n=== Anonymous")
    anon = Plant.create_anonymous()
    anon.show()
    display_any_plant_statistics(anon)


if __name__ == "__main__":
    ft_plant_types()
