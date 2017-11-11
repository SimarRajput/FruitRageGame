#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <chrono>
#include <algorithm>

using namespace std;

struct BestMoveVector
{
	int row = -1;
	int column = -1;
};

struct BestMove
{
public:
	int boardScore = 0;
	vector<BestMoveVector> bestMoveV;
};

short GRID_SIZE = 0;
short NO_OF_FRUIT_TYPES = 0;
float TIME_REMAINING = 0.0;
short MINIMAX_DEPTH = 0;
int MAX_CON_COMP = 0;
vector<vector<char>> FRUIT_BOARD;
BestMove BEST_MOVE;

bool Ascending(BestMove& i, BestMove& j)
{
	return i.boardScore > j.boardScore;
}
bool Descending(BestMove& i, BestMove& j)
{
	return i.boardScore < j.boardScore;
}

void ReadFile()
{
	string argumnets;
	ifstream inputFile;
	inputFile.open("input.txt");
	int lineIndex = 0;
	if (inputFile.is_open())
	{
		while (!inputFile.eof())
		{
			getline(inputFile, argumnets);
			if (argumnets != "")
			{
				if (lineIndex == 0)
				{
					GRID_SIZE = stoi(argumnets);
					FRUIT_BOARD.resize(GRID_SIZE, vector<char>(GRID_SIZE));
					lineIndex += 1;
				}
				else if (lineIndex == 1)
				{
					NO_OF_FRUIT_TYPES = stoi(argumnets);
					lineIndex += 1;
				}
				else if (lineIndex == 2)
				{
					TIME_REMAINING = stof(argumnets);
					lineIndex += 1;
				}
				else
				{
					for (int j = 0; j < GRID_SIZE; j++)
					{
						if (lineIndex - 3 < GRID_SIZE)
						{
							FRUIT_BOARD[lineIndex - 3][j] = argumnets[j];
						}
					}
					lineIndex += 1;
				}
			}
		}
	}
	inputFile.close();
}

vector<vector<char>> RunGravity(vector<vector<char>> bestMove)
{
	for (int row = GRID_SIZE - 1; row > 0; row--)
	{
		for (int column = 0; column < GRID_SIZE; column++)
		{
			if (bestMove[row][column] == '*')
			{
				int cell = row - 1;
				while (bestMove[cell][column] == '*' && cell >= 0)
				{
					if (cell == 0)
						break;
					cell--;
				}
				bestMove[row][column] = bestMove[cell][column];
				bestMove[cell][column] = '*';
			}
		}
	}
	return bestMove;
}

vector<vector<char>> GenerateFruitGrid(vector<vector<char>> fruitGrid, BestMoveVector bestMoveV)
{
	queue<BestMoveVector> traverseLocations;
	BestMoveVector nextMove;
	vector<vector<bool>> vFruits(GRID_SIZE, vector<bool>(GRID_SIZE));
	BestMoveVector newNextMove;

	traverseLocations.push(bestMoveV);

	while (traverseLocations.size() != 0)
	{
		nextMove = traverseLocations.front();
		traverseLocations.pop();

		char fruitType = fruitGrid[nextMove.row][nextMove.column];

		if (nextMove.row + 1 < GRID_SIZE && (fruitGrid[nextMove.row + 1][nextMove.column] == fruitType && !vFruits[nextMove.row + 1][nextMove.column]))
		{
			newNextMove.row = nextMove.row + 1;
			newNextMove.column = nextMove.column;
			traverseLocations.push(newNextMove);
			vFruits[newNextMove.row][newNextMove.column] = 1;
		}

		if (nextMove.row - 1 >= 0 && (fruitGrid[nextMove.row - 1][nextMove.column] == fruitType && !vFruits[nextMove.row - 1][nextMove.column]))
		{
			newNextMove.row = nextMove.row - 1;
			newNextMove.column = nextMove.column;
			traverseLocations.push(newNextMove);
			vFruits[newNextMove.row][newNextMove.column] = 1;
		}

		if (nextMove.column + 1 < GRID_SIZE && (fruitGrid[nextMove.row][nextMove.column + 1] == fruitType && !vFruits[nextMove.row][nextMove.column + 1]))
		{
			newNextMove.row = nextMove.row;
			newNextMove.column = nextMove.column + 1;
			traverseLocations.push(newNextMove);
			vFruits[newNextMove.row][newNextMove.column] = 1;
		}

		if (nextMove.column - 1 >= 0 && (fruitGrid[nextMove.row][nextMove.column - 1] == fruitType && !vFruits[nextMove.row][nextMove.column - 1]))
		{
			newNextMove.row = nextMove.row;
			newNextMove.column = nextMove.column - 1;
			traverseLocations.push(newNextMove);
			vFruits[newNextMove.row][newNextMove.column] = 1;
		}
		vFruits[nextMove.row][nextMove.column] = 1;
		fruitGrid[nextMove.row][nextMove.column] = '*';
	}
	fruitGrid = RunGravity(fruitGrid);
	return fruitGrid;
}

vector<BestMove> ComputeConnectedComponents(BestMove bestMove, bool maxPlayer)
{
	vector<vector<char>> iFBoard = FRUIT_BOARD;
	vector<BestMove> childBestMoveVector;
	vector<vector<bool>> vFruits(GRID_SIZE, vector<bool>(GRID_SIZE));;
	BestMoveVector newNextMove;
	int count = 0;

	for (int i = 0; i < bestMove.bestMoveV.size(); i++)
	{
		if (bestMove.bestMoveV[i].row >= 0 && bestMove.bestMoveV[i].column >= 0)
		{
			iFBoard = GenerateFruitGrid(iFBoard, bestMove.bestMoveV[i]);
		}
	}
	for (int row = 0; row < GRID_SIZE; row++)
	{
		for (int column = 0; column < GRID_SIZE; column++)
		{
			if (iFBoard[row][column] == '*')
				vFruits[row][column] = 1;

			if (vFruits[row][column] != 1)
			{
				int score = 0;
				BestMove newBestMove;
				BestMoveVector bestMoveV;
				bestMoveV.row = row;
				bestMoveV.column = column;

				queue<BestMoveVector> nextMoves;

				newBestMove.boardScore = 1;
				newBestMove.bestMoveV.push_back(bestMoveV);

				childBestMoveVector.push_back(newBestMove);
				nextMoves.push(bestMoveV);

				vFruits[row][column] = 1;

				while (nextMoves.size() != 0)
				{
					score++;
					char fruitType = iFBoard[row][column];
					BestMoveVector nextMove = nextMoves.front();
					nextMoves.pop();

					if (nextMove.row + 1 < GRID_SIZE && (iFBoard[nextMove.row + 1][nextMove.column] == fruitType && !vFruits[nextMove.row + 1][nextMove.column]))
					{
						newNextMove.row = nextMove.row + 1;
						newNextMove.column = nextMove.column;
						nextMoves.push(newNextMove);
						vFruits[nextMove.row + 1][nextMove.column] = 1;
					}

					if (nextMove.row - 1 >= 0 && (iFBoard[nextMove.row - 1][nextMove.column] == fruitType && !vFruits[nextMove.row - 1][nextMove.column]))
					{
						newNextMove.row = nextMove.row - 1;
						newNextMove.column = nextMove.column;
						nextMoves.push(newNextMove);
						vFruits[nextMove.row - 1][nextMove.column] = 1;
					}

					if (nextMove.column + 1 < GRID_SIZE && (iFBoard[nextMove.row][nextMove.column + 1] == fruitType && !vFruits[nextMove.row][nextMove.column + 1]))
					{
						newNextMove.row = nextMove.row;
						newNextMove.column = nextMove.column + 1;
						nextMoves.push(newNextMove);
						vFruits[nextMove.row][nextMove.column + 1] = 1;
					}

					if (nextMove.column - 1 >= 0 && (iFBoard[nextMove.row][nextMove.column - 1] == fruitType && !vFruits[nextMove.row][nextMove.column - 1]))
					{
						newNextMove.row = nextMove.row;
						newNextMove.column = nextMove.column - 1;
						nextMoves.push(newNextMove);
						vFruits[nextMove.row][nextMove.column - 1] = 1;
					}
				}
				childBestMoveVector[count].boardScore = score * score;
				count++;
			}
		}
	}
	return childBestMoveVector;
}

vector<BestMove> AppendMovesAndCalCumScore(BestMove bestMove, vector<BestMove> bestMoveVector, bool maxPlayer)
{
	for (int i = 0; i < bestMoveVector.size(); i++)
	{
		BestMove newBestMove;
		newBestMove.bestMoveV = bestMove.bestMoveV;
		newBestMove.boardScore = bestMove.boardScore;

		if (maxPlayer)
			newBestMove.boardScore += bestMoveVector[i].boardScore;
		else
			newBestMove.boardScore -= bestMoveVector[i].boardScore;

		if (bestMoveVector[i].bestMoveV.size() > 0)
			newBestMove.bestMoveV.push_back(bestMoveVector[i].bestMoveV[0]);

		bestMoveVector[i] = newBestMove;
	}
	return bestMoveVector;
}

int RunMiniMaxAlgorithm(int depth, bool maxPlayer, BestMove bestMove, int alpha, int beta)
{
	if (depth == 0)
		return bestMove.boardScore;

	vector<BestMove> allBestMoves = ComputeConnectedComponents(bestMove, maxPlayer);

	if (allBestMoves.size() == 0)
		return bestMove.boardScore;

	allBestMoves = AppendMovesAndCalCumScore(bestMove, allBestMoves, maxPlayer);

	for (int i = 0; i < allBestMoves.size(); i++)
	{
		if (maxPlayer)
		{
			sort(allBestMoves.begin(), allBestMoves.end(), Ascending);

			int currentScore = RunMiniMaxAlgorithm(depth - 1, false, allBestMoves[i], alpha, beta);
			if (currentScore > alpha)
			{
				alpha = currentScore;
				if (depth == MINIMAX_DEPTH)
				{
					BEST_MOVE = allBestMoves[i];
					BEST_MOVE.boardScore = currentScore;
				}
			}
			if (alpha >= beta) {
				break;
			}
		}
		else
		{
			sort(allBestMoves.begin(), allBestMoves.end(), Descending);

			int currentScore = RunMiniMaxAlgorithm(depth - 1, true, allBestMoves[i], alpha, beta);
			beta = min(currentScore, beta);

			if (alpha >= beta) {
				break;
			}
		}
	}

	if (maxPlayer)
		return alpha;
	else
		return beta;
}

void StartPlayingGame()
{
	BestMove initialBestMove;
	RunMiniMaxAlgorithm(MINIMAX_DEPTH, true, initialBestMove, INT_MIN, INT_MAX);
}
void WriteOutputFile()
{
	ofstream file;
	file.open("output.txt");
	int bestRow = BEST_MOVE.bestMoveV[0].row;
	int bestColumn = BEST_MOVE.bestMoveV[0].column;
	vector<vector<char>> finalBest = GenerateFruitGrid(FRUIT_BOARD, BEST_MOVE.bestMoveV[0]);
	file << (char)((bestColumn % 26) + 'A') << bestRow + 1 << endl;

	for (int i = 0; i < finalBest.size(); i++)
	{
		for (int j = 0; j < finalBest.size(); j++)
		{
			file << finalBest[i][j];
		}
		file << endl;
	}
	file << endl;
}

int FindDepth()
{
	int depth = 3;
	if (TIME_REMAINING < 1)
	{
		return 1;
	}
	else if (TIME_REMAINING < 2)
	{
		return 2;
	}
	else
	{
		if (GRID_SIZE < 6) {
			depth = 6;
		}
		else if (GRID_SIZE < 17) {
			if (MAX_CON_COMP <= 100) {
				depth = 5;
			}
			else if (MAX_CON_COMP <= 200) {
				depth = 4;
			}
		}
		else if (GRID_SIZE < 22) {
			if (MAX_CON_COMP < 60) {
				depth = 5;
			}
			else if (MAX_CON_COMP < 100) {
				depth = 4;
			}
		}
		else if (GRID_SIZE < 27) {
			if (MAX_CON_COMP < 40) {
				depth = 5;
			}
			else if (MAX_CON_COMP < 80) {
				depth = 4;
			}
		}
	}
	return depth;
}

int main()
{
	ReadFile();
	BestMove initialBestMove;
	MAX_CON_COMP = ComputeConnectedComponents(initialBestMove, true).size();
	MINIMAX_DEPTH = FindDepth();
	StartPlayingGame();
	WriteOutputFile();
}