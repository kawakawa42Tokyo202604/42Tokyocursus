#!/usr/bin/env python3
def input_temperature(temp_str: str) -> int:
    return int(temp_str)


def test_temperature() -> None:
    print("=== Garden Temperature ===")
    print()
    test_inputs = ["25", "abc"]
    for data in test_inputs:
        print(f"Input data is '{data}'")

        try:
            temp = input_temperature(data)
            print(f"Temperature is now {temp}°C")

        except ValueError as e:
            print(f"Caught input_temperature error: {e}")

        print()

    print("All tests completed - program didn't crash!")


if __name__ == "__main__":
    try:
        test_temperature()
    except Exception as e:
        print(f"{e}")
