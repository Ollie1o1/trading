# Trading Project Setup for Mac M2

## Prerequisites
- macOS with Apple M2 chip
- Python 3.9+ (recommended: install via [Homebrew](https://brew.sh/) or [pyenv](https://github.com/pyenv/pyenv))
- pip (Python package manager)

## Setup Instructions

1. Clone this repository:
```bash
git clone https://github.com/Ollie1o1/trading.git
cd trading
```

2. Create and activate a virtual environment:
```bash
python3 -m venv venv
source venv/bin/activate
```

3. Install dependencies:
```bash
pip install -r requirements.txt
```

## Project Structure
```
trading/
├── data/            # Contains market data files
├── notebooks/       # Jupyter notebooks for analysis
├── results/         # Output files and logs
├── strategies/      # Trading strategy implementations
├── requirements.txt # Python dependencies
```

## Running the Project

1. For Jupyter notebooks:
```bash
jupyter notebook
```
Then open `notebooks/backtest_sma.ipynb`

2. For Python scripts:
```bash
python strategies/sma_crossover.py
```

## Basic Workflow
1. Add your market data files to the `data/` directory
2. Create new notebooks in `notebooks/` for analysis
3. Implement trading strategies in `strategies/`
4. Review results in `results/`

## Notes
- The specific trading strategy implementations may change frequently
- Always run in a virtual environment to avoid dependency conflicts
- For M2 chip optimization, ensure you're using native ARM64 Python builds
