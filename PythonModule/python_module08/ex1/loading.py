#!/usr/bin/env python3
import importlib
import importlib.metadata
import sys
import types

_HAS_PANDAS = False
_HAS_NUMPY = False
_HAS_REQUESTS = False
_HAS_MATPLOTLIB = False

_PANDAS_VER = ""
_NUMPY_VER = ""
_REQUESTS_VER = ""
_MATPLOTLIB_VER = ""

pd: types.ModuleType | None = None
np: types.ModuleType | None = None
plt: types.ModuleType | None = None

try:
    _PANDAS_VER = importlib.metadata.version("pandas")
    pd = importlib.import_module("pandas")
    _HAS_PANDAS = True
except (importlib.metadata.PackageNotFoundError, ModuleNotFoundError):
    pass

try:
    _NUMPY_VER = importlib.metadata.version("numpy")
    np = importlib.import_module("numpy")
    _HAS_NUMPY = True
except (importlib.metadata.PackageNotFoundError, ModuleNotFoundError):
    pass

try:
    _REQUESTS_VER = importlib.metadata.version("requests")
    importlib.import_module("requests")
    _HAS_REQUESTS = True
except (importlib.metadata.PackageNotFoundError, ModuleNotFoundError):
    pass

try:
    _MATPLOTLIB_VER = importlib.metadata.version("matplotlib")
    plt = importlib.import_module("matplotlib.pyplot")
    _HAS_MATPLOTLIB = True
except (importlib.metadata.PackageNotFoundError, ModuleNotFoundError):
    pass


def check_dependencies() -> bool:
    print("Checking dependencies:")
    if _HAS_PANDAS:
        print(f"[OK] pandas ({_PANDAS_VER}) - Data manipulation ready")
    else:
        print("[MISSING] pandas - Data manipulation ready")

    if _HAS_NUMPY:
        print(f"[OK] numpy ({_NUMPY_VER}) - Numerical computation ready")
    else:
        print("[MISSING] numpy - Numerical computation ready")

    if _HAS_REQUESTS:
        print(f"[OK] requests ({_REQUESTS_VER}) - Network access ready")

    if _HAS_MATPLOTLIB:
        print(f"[OK] matplotlib ({_MATPLOTLIB_VER}) - Visualization ready")
    else:
        print("[MISSING] matplotlib - Visualization ready")

    return bool(_HAS_PANDAS and _HAS_NUMPY and _HAS_MATPLOTLIB)


def run_analysis() -> None:
    print("Analyzing Matrix data...")
    print("Processing 1000 data points...")
    print("Generating visualization...")

    if pd is None or np is None or plt is None:
        print("[-] Fatal: Core modules are not loaded.", file=sys.stderr)
        return

    try:
        np.random.seed(42)
        sync_rates = np.random.uniform(0.0, 100.0, 1000)
        residual_speeds = np.random.normal(50.0, 15.0, 1000)

        df = pd.DataFrame({
            "Sync_Rate": sync_rates,
            "Residual_Speed": residual_speeds
        })
        plt.figure(figsize=(8, 5))
        plt.scatter(df["Sync_Rate"], df["Residual_Speed"],
                    color="red", alpha=0.5, s=10)
        plt.title("Matrix Analysis: Program Loading Metrics")
        plt.xlabel("Sync Rate (%)")
        plt.ylabel("Residual Speed (mach)")
        plt.grid(True, linestyle="--", alpha=0.5)

        output_file = "matrix_analysis.png"
        plt.savefig(output_file)
        plt.close()

        print("Analysis complete!")
        print(f"Results saved to: {output_file}")

    except Exception as e:
        print(f"[-] Data pipeline failure: {e}", file=sys.stderr)


def print_msg() -> None:
    print("\n====================================================")
    print("        DEPENDENCY MANAGEMENT: PIP VS POETRY        ")
    print("====================================================")
    print("[pip (requirements.txt)]")
    print("  - Installs packages flatly. Potential dependency drift.")
    print("[Poetry (pyproject.toml)]")
    print("  - Deterministic locking via poetry.lock. Isolated envs.")

    print("\n[-] ERROR: Missing required dependencies.")
    print("To install packages, choose one of the following methods:\n")
    print("Installing with pip:")
    print("  $> pip install -r requirements.txt")
    print("  $> python3 loading.py\n")
    print("Installing with Poetry:")
    print("  $> poetry install")
    print("  $> poetry run python loading.py")


def main() -> None:
    print("LOADING STATUS: Loading programs...")
    if check_dependencies():
        run_analysis()
    else:
        print_msg()


if __name__ == "__main__":
    main()
