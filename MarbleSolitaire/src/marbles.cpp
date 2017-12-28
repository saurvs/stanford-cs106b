#include "grid.h"
#include "vector.h"
#include "marbletypes.h"
#include "marbleutil.h"
#include "set.h"
#include "compression.h"

using namespace std;

bool solve(Grid<Marble>&, int, Vector<Move>&, Set<BoardEncoding>);

bool solvePuzzle(Grid<Marble>& board, int marbleCount, Vector<Move>& moveHistory) {
    Set<BoardEncoding> boardHistory;
    return solve(board, marbleCount, moveHistory, boardHistory);
}

bool solve(Grid<Marble>& board, int marbleCount, Vector<Move>& moveHistory, Set<BoardEncoding> boardHistory) {
    if (marbleCount == 1) {
        return true;
    }

    BoardEncoding encoding = compressBoard(board);
    if (boardHistory.contains(encoding)) {
        return false;
    }

    Vector<Move> possibleMoves = findPossibleMoves(board);
    for (auto& move : possibleMoves) {
        makeMove(move, board);
        moveHistory.add(move);
        if (solve(board, marbleCount - 1, moveHistory, boardHistory))
            return true;
        moveHistory.remove(moveHistory.size() - 1);
        undoMove(move, board);
        boardHistory.add(encoding);
    }

    return false;
}
