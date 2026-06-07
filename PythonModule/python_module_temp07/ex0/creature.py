#!/usr/bin/env python3
from abc import ABC, abstractmethod


class Creature(ABC):
    def __init__(self, name: str, type: str) -> None:
        self.name = name
        self.type = type

    def describe(self) -> str:
        return f"{self.name} is a {self.type} type Creature"

    @abstractmethod
    def attack(self) -> str:
        pass


class Flameling(Creature):
    def __init__(self, name: str) -> None:
        super().__init__(name, "Fire")

    def attack(self) -> str:
        return f"{self.name} uses Ember!"


class Pyrodon(Creature):
    def __init__(self, name: str) -> None:
        super().__init__(name, "Fire/Flying")

    def attack(self) -> str:
        return f"{self.name} uses Flamethrower!"


class Aquabub(Creature):
    def __init__(self, name: str) -> None:
        super().__init__(name, "Water")

    def attack(self) -> str:
        return f"{self.name} uses Water Gun!"


class Torragon(Creature):

    def __init__(self, name: str) -> None:
        super().__init__(name, "Water")

    def attack(self) -> str:
        return f"{self.name} uses Hydro Pump!"
