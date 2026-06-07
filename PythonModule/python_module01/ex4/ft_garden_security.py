#!/usr/bin/env python3
class Plant:
    def __init__(self, name: str, height: float, age: int) -> None:
        self._name = name
        self._height = float(height)
        self._age = age

    def get_height(self) -> float:
        return self._height

    def set_height(self, value: float) -> None:
        if value < 0:
            print(f"{self._name}: Error, height can't be negative")
            print("Height update rejected")
        else:
            self._height = float(value)
            print(f"Height updated: {int(self._height)}cm")

    def get_age(self) -> int:
        return self._age

    def set_age(self, value: int) -> None:
        if value < 0:
            print(f"{self._name}: Error, age can't be negative")
            print("Age update rejected")
        else:
            self._age = value
            print(f"Age updated: {self._age} days")

    def show(self) -> None:
        print(f"{self._name}: {self._height:.1f}cm, {self._age} days old")


def ft_security_garden() -> None:
    print("=== Garden Security System ===")

    rose = Plant("Rose", 15.0, 10)
    print("Plant created: ", end="")
    rose.show()

    rose.set_height(25.0)
    rose.set_age(30)

    rose.set_height(-5.0)
    rose.set_age(-1)

    print("Current state: ", end="")
    rose.show()


if __name__ == "__main__":
    ft_security_garden()
