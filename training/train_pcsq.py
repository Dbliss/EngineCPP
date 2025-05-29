import json
import random
import re


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


# Board helpers

def parse_fen(fen):
    parts = fen.split()
    rows = parts[0].split('/')
    board = []
    for r in rows:
        row = []
        for c in r:
            if c.isdigit():
                row.extend(['.'] * int(c))
            else:
                row.append(c)
        board.append(row)
    turn = parts[1]
    return board, turn


def to_index(row, col):
    return (7 - row) * 8 + col


def is_white(piece):
    return piece.isupper()


def generate_moves(board, turn):
    moves = []
    dirs_knight = [(2,1),(1,2),(-1,2),(-2,1),(-2,-1),(-1,-2),(1,-2),(2,-1)]
    dirs_bishop = [(1,1),(1,-1),(-1,1),(-1,-1)]
    dirs_rook = [(1,0),(-1,0),(0,1),(0,-1)]
    dirs_king = dirs_bishop + dirs_rook
    for r in range(8):
        for c in range(8):
            p = board[r][c]
            if p == '.':
                continue
            if turn == 'w' and not is_white(p):
                continue
            if turn == 'b' and is_white(p):
                continue
            if p in 'Pp':
                direction = -1 if p == 'P' else 1
                start_row = 6 if p == 'P' else 1
                nr = r + direction
                if 0 <= nr < 8:
                    if board[nr][c] == '.':
                        moves.append(((r,c),(nr,c)))
                        if r == start_row and board[nr + direction][c] == '.' and board[nr][c] == '.':
                            moves.append(((r,c),(nr + direction,c)))
                    for dc in (-1,1):
                        nc = c + dc
                        if 0 <= nc < 8:
                            target = board[nr][nc]
                            if target != '.' and is_white(target) != is_white(p):
                                moves.append(((r,c),(nr,nc)))
            elif p in 'Nn':
                for dr,dc in dirs_knight:
                    nr, nc = r+dr, c+dc
                    if 0<=nr<8 and 0<=nc<8:
                        target = board[nr][nc]
                        if target == '.' or is_white(target) != is_white(p):
                            moves.append(((r,c),(nr,nc)))
            elif p in 'Bb':
                for dr,dc in dirs_bishop:
                    nr,nc=r+dr,c+dc
                    while 0<=nr<8 and 0<=nc<8:
                        target=board[nr][nc]
                        if target=='.':
                            moves.append(((r,c),(nr,nc)))
                        else:
                            if is_white(target)!=is_white(p):
                                moves.append(((r,c),(nr,nc)))
                            break
                        nr+=dr
                        nc+=dc
            elif p in 'Rr':
                for dr,dc in dirs_rook:
                    nr,nc=r+dr,c+dc
                    while 0<=nr<8 and 0<=nc<8:
                        target=board[nr][nc]
                        if target=='.':
                            moves.append(((r,c),(nr,nc)))
                        else:
                            if is_white(target)!=is_white(p):
                                moves.append(((r,c),(nr,nc)))
                            break
                        nr+=dr
                        nc+=dc
            elif p in 'Qq':
                for dr,dc in dirs_bishop+dirs_rook:
                    nr,nc=r+dr,c+dc
                    while 0<=nr<8 and 0<=nc<8:
                        target=board[nr][nc]
                        if target=='.':
                            moves.append(((r,c),(nr,nc)))
                        else:
                            if is_white(target)!=is_white(p):
                                moves.append(((r,c),(nr,nc)))
                            break
                        nr+=dr
                        nc+=dc
            elif p in 'Kk':
                for dr,dc in dirs_king:
                    nr,nc=r+dr,c+dc
                    if 0<=nr<8 and 0<=nc<8:
                        target=board[nr][nc]
                        if target=='.' or is_white(target)!=is_white(p):
                            moves.append(((r,c),(nr,nc)))
    return moves


def make_move(board, move):
    (r1,c1),(r2,c2) = move
    new = [row[:] for row in board]
    new[r2][c2] = new[r1][c1]
    new[r1][c1] = '.'
    return new


def evaluate(board, tables):
    score = 0
    for r in range(8):
        for c in range(8):
            p = board[r][c]
            if p == '.':
                continue
            idx = to_index(r,c)
            if p.isupper():
                if p == 'P':
                    score += tables['pawn_pcsq'][63-idx] + 100
                elif p == 'N':
                    score += tables['knight_pcsq'][63-idx] + 320
                elif p == 'B':
                    score += tables['bishop_pcsq'][63-idx] + 330
                elif p == 'R':
                    score += 500
                elif p == 'Q':
                    score += 900
                elif p == 'K':
                    score += tables['king_pcsq'][63-idx]
            else:
                if p == 'p':
                    score -= tables['pawn_pcsq'][idx] + 100
                elif p == 'n':
                    score -= tables['knight_pcsq'][idx] + 320
                elif p == 'b':
                    score -= tables['bishop_pcsq'][idx] + 330
                elif p == 'r':
                    score -= 500
                elif p == 'q':
                    score -= 900
                elif p == 'k':
                    score -= tables['king_pcsq_black'][idx]
    return score


def play_game(fen, tables, limit=40):
    board, turn = parse_fen(fen)
    for _ in range(limit):
        moves = generate_moves(board, turn)
        if not moves:
            return 1 if turn == 'b' else -1
        best = None
        best_score = -1e9 if turn == 'w' else 1e9
        for mv in moves:
            nb = make_move(board, mv)
            sc = evaluate(nb, tables)
            if turn == 'w':
                if sc > best_score:
                    best_score = sc
                    best = mv
            else:
                if sc < best_score:
                    best_score = sc
                    best = mv
        board = make_move(board, best)
        turn = 'b' if turn == 'w' else 'w'
    return 0


def selfplay_score(tables, games=5):
    fens = load_fens()[:games]
    score = 0
    for fen in fens:
        score += play_game(fen, tables)
    return score


def mutate(tables, step=5):
    new = {}
    for k,v in tables.items():
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
