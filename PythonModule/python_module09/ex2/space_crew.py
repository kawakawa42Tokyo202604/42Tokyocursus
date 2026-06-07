#!/usr/bin/env python3
from datetime import datetime
from enum import Enum
import sys
from typing import List
from pydantic import BaseModel, Field, ValidationError, model_validator


class Rank(str, Enum):
    CADET = "cadet"
    OFFICER = "officer"
    LIEUTENANT = "lieutenant"
    CAPTAIN = "captain"
    COMMANDER = "commander"


class CrewMember(BaseModel):
    member_id: str = Field(..., min_length=3, max_length=10)
    name: str = Field(..., min_length=2, max_length=50)
    rank: Rank
    age: int = Field(..., ge=18, le=80)
    specialization: str = Field(..., min_length=3, max_length=30)
    years_experience: int = Field(..., ge=0, le=50)
    is_active: bool = Field(default=True)


class SpaceMission(BaseModel):
    mission_id: str = Field(..., min_length=5, max_length=15)
    mission_name: str = Field(..., min_length=3, max_length=100)
    destination: str = Field(..., min_length=3, max_length=50)
    launch_date: datetime
    duration_days: int = Field(..., ge=1, le=3650)
    crew: List[CrewMember] = Field(..., min_length=1, max_length=12)
    mission_status: str = Field(default="planned")
    budget_millions: float = Field(..., ge=1.0, le=10000.0)

    @model_validator(mode="after")
    def validate_mission_safety(self) -> "SpaceMission":
        if not self.mission_id.startswith("M"):
            raise ValueError("Mission ID must start with 'M'")
        for member in self.crew:
            if not member.is_active:
                raise ValueError("All crew members must be active")
        has_leader = any(
            m.rank in (Rank.COMMANDER, Rank.CAPTAIN) for m in self.crew
        )
        if not has_leader:
            raise ValueError(
                "Mission must have at least one Commander or Captain"
            )
        if self.duration_days > 365:
            exp_crew = [m for m in self.crew if m.years_experience >= 5]
            if len(exp_crew) * 2 < len(self.crew):
                raise ValueError(
                    "Long missions (> 365 days) need 50% experienced "
                    "crew (5+ years)"
                )
        return self


def main() -> None:
    print("Space Mission Crew Validation")
    print("=========================================")

    try:
        valid_crew = [
            CrewMember(
                member_id="C001",
                name="Sarah Connor",
                rank=Rank.COMMANDER,
                age=45,
                specialization="Mission Command",
                years_experience=20,
            ),
            CrewMember(
                member_id="C002",
                name="John Smith",
                rank=Rank.LIEUTENANT,
                age=32,
                specialization="Navigation",
                years_experience=8,
            ),
            CrewMember(
                member_id="C003",
                name="Alice Johnson",
                rank=Rank.OFFICER,
                age=28,
                specialization="Engineering",
                years_experience=3,
            ),
        ]

        valid_mission = SpaceMission(
            mission_id="M2024_MARS",
            mission_name="Mars Colony Establishment",
            destination="Mars",
            launch_date=datetime.fromisoformat("2026-06-04T12:00:00"),
            duration_days=900,
            crew=valid_crew,
            budget_millions=2500.0,
        )

        print("Valid mission created:")
        print(f"Mission: {valid_mission.mission_name}")
        print(f"ID: {valid_mission.mission_id}")
        print(f"Destination: {valid_mission.destination}")
        print(f"Duration: {valid_mission.duration_days} days")
        print(f"Budget: ${valid_mission.budget_millions:.1f}M")
        print(f"Crew size: {len(valid_mission.crew)}")
        print("Crew members:")
        for m in valid_mission.crew:
            print(f"- {m.name} ({m.rank.value}) - {m.specialization}")

    except ValidationError as e:
        print(f"Unexpected error: {e}", file=sys.stderr)

    print("=========================================")
    print("Expected validation error:")

    try:
        invalid_crew = [
            CrewMember(
                member_id="C004",
                name="Bob Builder",
                rank=Rank.OFFICER,
                age=30,
                specialization="Construction",
                years_experience=6,
            )
        ]
        SpaceMission(
            mission_id="M2024_FAIL",
            mission_name="Unled Expedition",
            destination="Moon",
            launch_date=datetime.now(),
            duration_days=30,
            crew=invalid_crew,
            budget_millions=100.0,
        )
        print("[-] Failure: Invalid data bypassed validation!")
    except ValidationError as e:
        for error in e.errors():
            msg = error["msg"]
            clean_msg = msg.replace("Value error, ", "")
            print(clean_msg)


if __name__ == "__main__":
    main()
