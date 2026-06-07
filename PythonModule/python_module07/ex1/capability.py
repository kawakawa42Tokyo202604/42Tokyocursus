#!/usr/bin/env python3
import typing
from abc import ABC, abstractmethod


class HealCapability(ABC):

    @abstractmethod
    def heal(self, target: typing.Optional[str] = None) -> str:
        pass


class TransformCapability(ABC):
    is_transformed: bool

    @abstractmethod
    def transform(self) -> str:
        pass

    @abstractmethod
    def revert(self) -> str:
        pass
