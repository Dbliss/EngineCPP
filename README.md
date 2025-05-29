# Chess Engine

This project contains a simple chess engine written in C++. Piece-square tables
control much of the evaluation. A training script is provided to experiment with
optimising these tables through self‑play.

## Training piece-square tables

The `training/train_pcsq.py` script performs a very basic self-play search over
positions defined in `Chess Engine.cpp`. Games are played by a helper program
compiled from `training/selfplay.cpp`, which uses the C++ engine for all move
generation and evaluation. The training script mutates the piece-square tables
and keeps any mutation that performs better than the current tables.

First compile the helper program:

```bash
g++ -std=c++17 training/selfplay.cpp chess.cpp engine.cpp engine2.cpp \
    zobrist.cpp -o training/selfplay
```

Then run the training script from the repository root:

```bash
python3 training/train_pcsq.py
```

When finished, a file called `pcsq_tables.json` is created in the current
directory containing the optimised values.

## Loading custom tables

`engine.cpp` and `engine2.cpp` now include a helper function
`loadPieceSquareTables(const std::string&)` which loads the piece-square arrays
from a JSON file with the same format produced by the training script. Call this
function once at start-up before searching moves:

```cpp
loadPieceSquareTables("pcsq_tables.json");
```

If loading fails, the built-in default tables are used.
