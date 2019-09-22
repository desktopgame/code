#pragma once
#include "Board.h"

const BoardState* AlphaBetaDecide(const BoardState* root, int maxDepth);

float AlphaBetaMax(const BoardState* node, int depth, float alpha, float beta);

float AlphaBetaMin(const BoardState* node, int depth, float alpha, float beta);