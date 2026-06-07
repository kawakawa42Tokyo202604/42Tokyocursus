#!/usr/bin/env python3
from ex0 import FlameFactory, AquaFactory, CreatureFactory


def verify_factory_generation(factory: CreatureFactory) -> None:
    print("Testing factory")
    base = factory.create_base()
    evolved = factory.create_evolved()

    print(base.describe())
    print(base.attack())
    print(evolved.describe())
    print(evolved.attack())
    print()


def battle_base_creatures(
    factory1: CreatureFactory, factory2: CreatureFactory
) -> None:
    print("Testing battle")
    c1 = factory1.create_base()
    c2 = factory2.create_base()

    print(c1.describe())
    print("vs.")
    print(c2.describe())
    print("fight!")
    print(c1.attack())
    print(c2.attack())


def main() -> None:
    flame_factory = FlameFactory()
    aqua_factory = AquaFactory()

    verify_factory_generation(flame_factory)
    verify_factory_generation(aqua_factory)

    battle_base_creatures(flame_factory, aqua_factory)


if __name__ == "__main__":
    main()
