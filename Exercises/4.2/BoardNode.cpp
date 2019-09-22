#include "BoardNode.h"
#include <algorithm>

const BoardState * AlphaBetaDecide(const BoardState * root, int maxDepth) {
	const BoardState* choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();
	float beta = std::numeric_limits<float>::infinity();
	for (const BoardState* child : root->GetPossibleMoves(BoardState::SquareState::Red)) {
		float v = AlphaBetaMin(child, maxDepth - 1, maxValue, beta);
		if (v > maxValue) {
			maxValue = v;
			choice = child;
		}
	}
	return choice;
}

float AlphaBetaMax(const BoardState * node, int depth, float alpha, float beta) {
	if (depth == 0 || node->IsTerminal()) {
		return node->GetScore();
	}
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const BoardState* child : node->GetPossibleMoves(BoardState::SquareState::Yellow)) {
		maxValue = std::max(maxValue, AlphaBetaMin(child, depth - 1, alpha, beta));
		if (maxValue >= beta) {
			return maxValue;
		}
		alpha = std::max(maxValue, alpha);
	}
	return maxValue;
}

float AlphaBetaMin(const BoardState * node, int depth, float alpha, float beta) {
	if (depth == 0 || node->IsTerminal()) {
		return node->GetScore();
	}
	float minValue = std::numeric_limits<float>::infinity();
	for (const BoardState* child : node->GetPossibleMoves(BoardState::SquareState::Red)) {
		minValue = std::min(minValue, AlphaBetaMax(child, depth - 1, alpha, beta));
		if (minValue <= alpha) {
			return minValue;
		}
		beta = std::min(minValue, beta);
	}
	return minValue;
}
