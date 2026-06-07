#!/usr/bin/env python3
import typing
from ex1 import (
    HealingCreatureFactory,
    TransformCreatureFactory,
    HealCapability,
    TransformCapability,
)


def main() -> None:
    """Execute the creature factory test scenarios."""
    # 1. Healing Creature Scenario
    print("Testing Creature with healing capability")
    print("base:")
    healing_factory = HealingCreatureFactory()
    base_heal = healing_factory.create_base()
    print(base_heal.describe())
    print(base_heal.attack())
    print(typing.cast(HealCapability, base_heal).heal())

    print("evolved:")
    evolved_heal = healing_factory.create_evolved()
    print(evolved_heal.describe())
    print(evolved_heal.attack())
    print(typing.cast(HealCapability, evolved_heal).heal())
    print()

    print("Testing Creature with transform capability")
    print("base:")
    transform_factory = TransformCreatureFactory()
    base_trans = transform_factory.create_base()
    print(base_trans.describe())
    print(base_trans.attack())
    t_base = typing.cast(TransformCapability, base_trans)
    print(t_base.transform())
    print(base_trans.attack())
    print(t_base.revert())

    print("evolved:")
    evolved_trans = transform_factory.create_evolved()
    print(evolved_trans.describe())
    print(evolved_trans.attack())
    t_evolved = typing.cast(TransformCapability, evolved_trans)
    print(t_evolved.transform())
    print(evolved_trans.attack())
    print(t_evolved.revert())


if __name__ == "__main__":
    main()
