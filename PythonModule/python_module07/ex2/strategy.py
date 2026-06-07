#!/usr/bin/env python3
import typing
from abc import ABC, abstractmethod
from ex0 import Creature
from ex1 import HealCapability, TransformCapability


class InvalidStrategyException(Exception):
    pass


class BattleStrategy(ABC):
    @abstractmethod
    def is_valid(self, creature: Creature) -> bool:
        pass

    @abstractmethod
    def act(self, creature: Creature) -> str:
        pass


class NormalStrategy(BattleStrategy):
    def is_valid(self, creature: Creature) -> bool:
        return True

    def act(self, creature: Creature) -> str:
        if not self.is_valid(creature):
            raise InvalidStrategyException(
                f"Invalid Creature '{creature.name}' for this normal strategy"
            )
        return creature.attack()


class AggressiveStrategy(BattleStrategy):
    def is_valid(self, creature: Creature) -> bool:
        return isinstance(creature, TransformCapability)

    def act(self, creature: Creature) -> str:
        if not self.is_valid(creature):
            raise InvalidStrategyException(
                f"Invalid Creature '{creature.name}' for this "
                f"aggressive strategy"
            )
        t_creature = typing.cast(TransformCapability, creature)
        s1 = t_creature.transform()
        s2 = creature.attack()
        s3 = t_creature.revert()
        return f"{s1}\n{s2}\n{s3}"


class DefensiveStrategy(BattleStrategy):
    def is_valid(self, creature: Creature) -> bool:
        return isinstance(creature, HealCapability)

    def act(self, creature: Creature) -> str:
        if not self.is_valid(creature):
            raise InvalidStrategyException(
                f"Invalid Creature '{creature.name}' for this "
                f"defensive strategy"
            )
        h_creature = typing.cast(HealCapability, creature)
        s1 = creature.attack()
        s2 = h_creature.heal()
        return f"{s1}\n{s2}"
