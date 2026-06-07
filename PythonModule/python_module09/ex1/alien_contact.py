#!/usr/bin/env python3
from datetime import datetime
from enum import Enum
import sys
from typing import Optional
from pydantic import BaseModel, Field, ValidationError, model_validator


class ContactType(str, Enum):
    RADIO = "radio"
    VISUAL = "visual"
    PHYSICAL = "physical"
    TELEPATHIC = "telepathic"


class AlienContact(BaseModel):
    contact_id: str = Field(..., min_length=5, max_length=15)
    timestamp: datetime
    location: str = Field(..., min_length=3, max_length=100)
    contact_type: ContactType
    signal_strength: float = Field(..., ge=0.0, le=10.0)
    duration_minutes: int = Field(..., ge=1, le=1440)
    witness_count: int = Field(..., ge=1, le=100)
    message_received: Optional[str] = Field(default=None, max_length=500)
    is_verified: bool = Field(default=False)

    @model_validator(mode="after")
    def validate_business_rules(self) -> "AlienContact":
        if not self.contact_id.startswith("AC"):
            raise ValueError("Contact ID must start with 'AC'")

        if (
            self.contact_type == ContactType.PHYSICAL
            and not self.is_verified
        ):
            raise ValueError("Physical contact reports must be verified")

        if (
            self.contact_type == ContactType.TELEPATHIC
            and self.witness_count < 3
        ):
            raise ValueError(
                "Telepathic contact requires at least 3 witnesses"
            )

        if self.signal_strength > 7.0:
            if (
                not self.message_received
                or self.message_received.strip() == ""
            ):
                raise ValueError(
                    "Strong signals (> 7.0) should include "
                    "received messages"
                )

        return self


def main() -> None:
    print("Alien Contact Log Validation")
    print("======================================")
    try:
        valid_contact = AlienContact(
            contact_id="AC_2024_001",
            timestamp=datetime.fromisoformat("2026-06-04T12:00:00"),
            location="Area 51, Nevada",
            contact_type=ContactType.RADIO,
            signal_strength=8.5,
            duration_minutes=45,
            witness_count=5,
            message_received="Greetings from Zeta Reticuli",
        )
        if valid_contact.signal_strength.is_integer():
            sig: float | int = int(valid_contact.signal_strength)
        else:
            sig = valid_contact.signal_strength

        print("Valid contact report:")
        print(f"ID: {valid_contact.contact_id}")
        print(f"Type: {valid_contact.contact_type.value}")
        print(f"Location: {valid_contact.location}")
        print(f"Signal: {sig}/10")
        print(f"Duration: {valid_contact.duration_minutes} minutes")
        print(f"Witnesses: {valid_contact.witness_count}")
        print(f"Message: '{valid_contact.message_received}'")

    except ValidationError as e:
        print(f"Unexpected error: {e}", file=sys.stderr)

    print("======================================")
    print("Expected validation error:")
    try:
        AlienContact(
            contact_id="AC_2024_002",
            timestamp=datetime.now(),
            location="Secret Base",
            contact_type=ContactType.TELEPATHIC,
            signal_strength=4.0,
            duration_minutes=30,
            witness_count=1,
        )
        print("[-] Failure: Invalid data bypassed validation!")
    except ValidationError as e:
        for error in e.errors():
            msg = error["msg"]
            clean_msg = msg.replace("Value error, ", "")
            print(clean_msg)


if __name__ == "__main__":
    main()
