#!/usr/bin/env python3
def input_temperature(temp_str: str) -> int:
    temp = int(temp_str)
    if temp > 40:
        raise Exception(f"{temp}°C is too hot for plants (max 40°C)")
    if temp < 0:
        raise Exception(f"{temp}°C is too cold for plants (min 0°C)")
    return temp


def test_temperature() -> None:
    print("=== Garden Temperature Checker ===")
    print()
    test_inputs = ["25", "abc", "100", "-50"]
    for data in test_inputs:
        print(f"Input data is '{data}'")

        try:
            temp = input_temperature(data)
            print(f"Temperature is now {temp}°C")

        except Exception as e:
            print(f"Caught input_temperature error: {e}")

        print()

    print("All tests completed - program didn't crash!")


if __name__ == "__main__":
    try:
        test_temperature()
    except Exception as e:
        print(f"{e}")
