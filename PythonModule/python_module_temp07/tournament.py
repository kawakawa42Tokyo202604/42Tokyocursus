#!/usr/bin/env python3
import typing
from ex0 import Creature, CreatureFactory
from ex1 import HealingCreatureFactory, TransformCreatureFactory
from ex2 import (
    BattleStrategy,
    NormalStrategy,
    AggressiveStrategy,
    DefensiveStrategy,
    InvalidStrategyException,
)


class Flameling(Creature):
    """Flameling from ex0."""

    def __init__(self) -> None:
        super().__init__("Flameling", "Fire")

    def attack(self) -> str:
        return "Flameling uses Ember!"


class Aquabub(Creature):
    """Aquabub from ex0."""

    def __init__(self) -> None:
        super().__init__("Aquabub", "Water")

    def attack(self) -> str:
        return "Aquabub uses Water Gun!"


class FlamelingFactory(CreatureFactory):
    """Factory for Flameling."""

    def create_base(self) -> Creature:
        return Flameling()

    def create_evolved(self) -> Creature:
        return Flameling()


class AquabubFactory(CreatureFactory):
    """Factory for Aquabub."""

    def create_base(self) -> Creature:
        return Aquabub()

    def create_evolved(self) -> Creature:
        return Aquabub()


def print_tournament(
        title: str,
        opponents: typing.List[typing.Tuple[CreatureFactory, BattleStrategy]],
) -> None:
    print(title)
    parts = []
    for factory, strategy in opponents:
        f_name = factory.__class__.__name__.replace(
            "CreatureFactory", ""
        ).replace("Factory", "")
        s_name = strategy.__class__.__name__.replace("Strategy", "")
        parts.append(f"({f_name}+{s_name})")
    print(f"[ {', '.join(parts)} ]")


def battle(
    opponents: typing.List[typing.Tuple[CreatureFactory, BattleStrategy]]
) -> None:
    print("*** Tournament ***")
    print(f"{len(opponents)} opponents involved")
    creatures = [factory.create_base() for factory, _ in opponents]

    for i in range(len(opponents)):
        for j in range(i + 1, len(opponents)):
            c1, s1 = creatures[i], opponents[i][1]
            c2, s2 = creatures[j], opponents[j][1]

            print()
            print("* Battle *")
            print(c1.describe())
            print("vs.")
            print(c2.describe())
            print("now fight!")

            try:
                log1 = s1.act(c1)
                print(log1)
                log2 = s2.act(c2)
                print(log2)
            except InvalidStrategyException as e:
                print(f"Battle error, aborting tournament: {e}")
                return


def main() -> None:
    flame_fac = FlamelingFactory()
    aqua_fac = AquabubFactory()
    heal_fac = HealingCreatureFactory()
    trans_fac = TransformCreatureFactory()

    normal = NormalStrategy()
    aggressive = AggressiveStrategy()
    defensive = DefensiveStrategy()

    t0_opponents = [(flame_fac, normal), (heal_fac, defensive)]
    print_tournament("Tournament 0 (basic)", t0_opponents)
    print()
    battle(t0_opponents)
    print()

    t1_opponents = [(flame_fac, aggressive), (heal_fac, defensive)]
    print_tournament("Tournament 1 (error)", t1_opponents)
    print()
    battle(t1_opponents)
    print()

    t2_opponents = [
        (aqua_fac, normal), (heal_fac, defensive), (trans_fac, aggressive)
    ]
    print_tournament("Tournament 2 (multiple)", t2_opponents)
    print()
    battle(t2_opponents)


if __name__ == "__main__":
    main()
