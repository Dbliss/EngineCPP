import json
import os
import random
import re
import subprocess
import tempfile


def load_fens():
    with open('Chess Engine.cpp', 'r') as f:
        text = f.read()
    m = re.search(r"const char\* fenArray\[] = \{([^}]*)\};", text, re.S)
    return re.findall(r"\"([^\"]+)\"", m.group(1)) if m else []


def load_default_tables():
    arrays = [
        'pawn_pcsq',
        'knight_pcsq',
        'bishop_pcsq',
        'king_pcsq',
        'king_pcsq_black',
        'king_endgame_pcsq'
    ]
    tables = {}
    with open('engine.cpp', 'r') as f:
        text = f.read()
    for name in arrays:
        m = re.search(rf"{name}\s*\[64\]\s*=\s*\{{([^}}]+)\}};", text, re.S)
        if not m:
            continue
        nums = [int(x) for x in re.findall(r'-?\d+', m.group(1))]
        tables[name] = nums
    return tables


def selfplay_score(tables, games=5):
    fens = load_fens()[:games]
    with tempfile.NamedTemporaryFile('w', delete=False) as f:
        json.dump(tables, f)
        temp_path = f.name
    score = 0
    for fen in fens:
        res = subprocess.run(['./training/selfplay', fen, temp_path],
                             capture_output=True, text=True)
        outcome = res.stdout.strip()
        if outcome == 'engine1':
            score += 1
        elif outcome == 'engine2':
            score -= 1
    os.remove(temp_path)
    return score


def mutate(tables, step=5):
    new = {}
    for k, v in tables.items():
        new[k] = [x + random.randint(-step, step) for x in v]
    return new


def train(iterations=10, games=5):
    tables = load_default_tables()
    best = selfplay_score(tables, games)
    for i in range(iterations):
        cand = mutate(tables)
        score = selfplay_score(cand, games)
        if score > best:
            tables, best = cand, score
            print(f'Improved to {best} at iteration {i}')
    with open('pcsq_tables.json', 'w') as f:
        json.dump(tables, f, indent=2)
    print('Saved tables to pcsq_tables.json')


if __name__ == '__main__':
    train()
