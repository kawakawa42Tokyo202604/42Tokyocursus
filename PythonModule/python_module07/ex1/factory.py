#!/usr/bin/env python3
from ex0 import Creature, CreatureFactory
from .creature import Sproutling, Bloomelle, Shiftling, Morphagon


class HealingCreatureFactory(CreatureFactory):
    def create_base(self) -> Creature:
        return Sproutling("Sproutling")

    def create_evolved(self) -> Creature:
        return Bloomelle("Bloomelle")


class TransformCreatureFactory(CreatureFactory):
    def create_base(self) -> Creature:
        return Shiftling("Shiftling")

    def create_evolved(self) -> Creature:
        return Morphagon("Morphagon")
