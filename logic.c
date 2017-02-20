/*
 ============================================================================
 Name        : logic.c
 Author      : Iain Hill
 Version     : 1
 Copyright   : GPL
 Description : naughts and crosses / tic tac toe
 ============================================================================
 */
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "ticTacToe.h"

// The size of the playing board, the 'hash'.
#define MATRIX	9
#define M_SQRT	3

#define NEXT	2
#define TO_WIN	3
#define EMPTY	0

#define RESET	0
#define AUGMENT	1
#define VALUE	2

#define HORIZONTAL	1
#define VERTICAL	0
#define DIAGONAL1	0
#define DIAGONAL2	0

static int moves[3][3];
static int nextMoves[2][3][3];
static int playerStatus[2][9];

/*
 * A temporary measure of nothingness.
 */
static int level;
static int myRandom;

/*
 * This function returns the total number of moves made, augments that number
 * or resets it to zero, dependant upon the int that is entered when the
 * function is called.
 *
 * 0 -> RESET
 * 1 -> augment
 * 2 -> VALUE return the value of movesMade
 *
 */
int keepCount(int option)
{
	static int movesMade = 0;

	if (option == 0) {
		movesMade = 0;
		return 0;
	} else if (option == 1) {
		movesMade++;
		return 0;
	} else if (option == 2) {
		return movesMade;
	}
	return -1;
}

/*
 * Resets the moves array to zero and then calls the function that writes the
 * new status quo to the grid.
 */
void resetMoves()
{
	for (int i = 0; i < MATRIX; i++)
		*(*moves+i) = 0;
	writeMoves(*moves);

	if (myRandom)
		level = coinToss(5)+1;

}

int setLevel(int newLevel)
{
	if (newLevel == 48) {
		myRandom = 1;
		return 1;
	}
	if (newLevel > 48 && newLevel < 54) {
		myRandom = 0;
		level = newLevel - 48;
		return 1;
	}
	return 0;
}

/*
 * Translates the char values entered by the user into the integer values that
 * correspond to them, and then writes those values to the moves array.
 */
int traslateCharForMove(int x, int y, int player)
{
	if 	(x == 97)
		x = 0;
	else if (x == 98)
		x = 1;
	else if (x == 99)
		x = 2;

	if	(y == 49)
		y = 0;
	else if (y == 50)
		y = 1;
	else if (y == 51)
		y = 2;

	// !!! `x` here is `j` and `y` is `i` !!!
	if (moves[y][x] == 1 || moves[y][x] == 2)
		return 0;
	else {
		moves[y][x] = player;
		return 1;
	}
}

/*
 * ACME debugging tool for ticTacToe. Is this perhaps the embryo of a unit test?
 * See the header file, to activate -> DEBUG 1
 */
void printDebugMoves(int player)
{
	int player1 = PLAYER1 - 1;
	int player2 = PLAYER2 - 1;

	/* Above the debug squares */
	if (myRandom)
		puts("Random mode on.");
	else
		puts("Random mode off.");
	printf("Level= %d\n", level);
	printf("Moves made = %d\n", keepCount(VALUE));
	printf("Player 1 status -> %d\n", playerStatus[player1][0]);
	printf("Player 2 status -> %d\n\n", playerStatus[player2][0]);
	/* The two matrices */
	for (int i = 0; i < M_SQRT; i++) {
		/* Player one */
		printf(" |");
		for (int j = 0; j < M_SQRT; j++) {
			printf("%2d", moves[i][j]);
		}
		printf("|%2d\t", playerStatus[player1][i+1]);
		/* Player two */
		printf(" |");
		for (int j = 0; j < M_SQRT; j++) {
			printf("%2d", moves[i][j]);
		}
		printf("|%2d", playerStatus[player2][i+1]);

		/* Player one */
		printf(" |");
		for (int j = 0; j < M_SQRT; j++) {
			printf("%2d", nextMoves[player1][i][j]);
		}
		/* Player two */
		printf(" |");
		for (int j = 0; j < M_SQRT; j++) {
			printf("%2d", nextMoves[player2][i][j]);
		}
		puts("");
	}
	/* Beneath the two matrices */
	printf("  ------   \t  ------ \n");
	/* P1 status array */
	printf("%d ", playerStatus[player1][8]);
	for (int i = 0; i < 3; i++)
		printf("%2d", playerStatus[player1][i+5]);
	printf(" %2d\t", playerStatus[player1][4]);
	/* P1 status array */
	printf("%d ", playerStatus[player2][8]);
	for (int i = 0; i < 3; i++)
		printf("%2d", playerStatus[player2][i+5]);
	printf(" %2d\t", playerStatus[player2][4]);
	/* End of the line */
	puts("\n");
}

/*
 * Refresh recalculate redraw the grid, wash rinse and hang out to dry.
 */
int updateGame(int player)
{
	int status;

	if (player) {
		status = calculateStatus(player);
		writeMoves(*moves);
	} else {
		clearStatusArrays(player);
		resetMoves();
	}
	drawGrid(player);

	return status;
}

/*
 * Ask player to input either heads or tails, an ancient form of divination
 * used by tribes that depend upon counting tokens as their major deities.
 * This rite predicts the nature of the act that follow in an uncannily precise
 * manor.
 */
int headsOrTails()
{
	for(;;)
	{
		int c;
		int choice;
		drawGrid(RESET);
		sysOut(4, 0);
		while ((c = getchar()) != '\n')
			choice = c;
		putchar('\n');

		if (choice == 'h')
			return 0;
		else if (choice == 't')
			return 1;
	}
	return -1;
}

/*
 * Returns a 50 / 50 result, a virtual coin toss if you will humour me that,
 * about as random as a dice in space ...
 */
int coinToss(int dieSides)
{
	int coin;
	srand(time(NULL));
	coin = rand()%dieSides;
	return coin;
}

/*
 * The Computer or player two starts the game.
 */
void player2WinsToss(int player)
{
	drawGrid(EMPTY);
	sysOut(5, player);
	sleep(2);
	computerMove(player);
	updateGame(player);
}

/*
 * Player one starts the game, this is simply the text output.
 */
void playet1WinsTheToss(int player)
{
	drawGrid(EMPTY);
	sysOut(5, player);
	sleep(2);
}

/*
 * Request move from player, x can be either 1, 2 or 3 and y a, b or c.
 */
int yourMove(int player)
{
	// If none can move, get out of here.
	if (checkStaleMate())
		return 5;

	// Ok play.
	int status;
	status = updateGame(player);
	playerStatus[player-1][0] = status;
	sysOut(7, player);

	int x = 0;
	int y = 0;
	int i = 0;
	int c;

	int a;
	int b;

	// Do not accept '\n' until two char are entered.
	while ((c = getchar()) != '\n') {
		if (i == 0 && c != '\n')
			x = c;
		if (i == 1 && c != '\n')
			y = c;
		i++;
	}

	// Inverse the coordinates if reasonable values.
	if ((x > 48 && x < 52) && (y > 96 && y < 100))
	{
		a = x;
		b = y;
		x = b;
		y = a;
	}
	// Repeat until reasonable values are entered ...
	else if ((x < 97 || x > 99) || (y < 49 || y > 51))
	{
		yourMove(player);
		return 0;
	}

	// Write to the moves array.
	if (!traslateCharForMove(x, y, player)) {
		yourMove(player);
		return 0;
	}

	keepCount(AUGMENT);
	
	status = updateGame(player);
	playerStatus[player-1][0] = status;
	return status;
}

/*
 * The twisted logic of my first ever AI, call me 'Frankenstein' if you will
 * but I shall have the last laugh yet. [Evil laughter ensues, scene fades to
 * black]
 */ 
int computerMove(int player)
{
	// If none can move, get out of here.
	if (checkStaleMate())
		return 5;

	// Ok play.
	int status;
	int coin;
	status = updateGame(player);
	playerStatus[player-1][0] = status;
	sysOut(7, player);
	sleep(2);

	switch (level)
	{
		case 1: randomMove(player);
			break;
		case 2: coin = coinToss(3);
			if (coin) {
				randomMove(player);
			} else {
				bestPossibleMove(player);
			}
			break;
		case 3: coin = coinToss(2);
			if (coin) {
				randomMove(player);
			} else {
				bestPossibleMove(player);
			}
			break;
		case 4: coin = coinToss(3);
			if (coin) {
				bestPossibleMove(player);
			} else {
				randomMove(player);
			}
			break;
		case 5: bestPossibleMove(player);
			break;
		default:
			break;
	}

	keepCount(AUGMENT);
	status = updateGame(player);
	playerStatus[player-1][0] = status;
	return status;
}

/*
 * A totally random move using no logic other than, how many spaces remain
 * after n moves, the square is chosen on a 1..n random basis where n
 * is the number of empty squares remaining.
 */
int randomMove(int player)
{
	/*
	 * Generate randomly the choice of the next move.
	 */
	int movesLeft;
	int choice;
	srand(time(NULL));

	// Get the quantity of spaces left remaining.
	movesLeft = MATRIX - keepCount(VALUE);

	if (movesLeft > 1) {
		choice = rand()%(movesLeft-1);
		choice++;
	}
	else 
	{
		choice = 1;
	}

	/*
	 * Count empty squares until move selection is reached.
	 */
	int count = 1;

	for (int i = 0; i < M_SQRT; i++) {
		for (int j = 0; j < M_SQRT; j++)
		{
			if ( (moves[i][j] != 1) && (moves[i][j] != 2) )
			{
				if (count == choice) {
					moves[i][j] = player;
					return 0;
				}
				count++;
			}
		}
	}
	return 0;
}

int bestPossibleMove(int player)
{
	int playerM;
	int opponent;
	int value;

	/*
	 * Resolve the player id's so as to find the corresponding index for the
	 * player status array.
	 */
	opponent = (player % 2); 
	playerM = player-1;

	/*
	 * If the center square is empty and there is no winning move, move
	 * there.
	 */
	if (moves[1][1] == 0 && playerStatus[playerM][0] != 3) {
		moves[1][1] = player;
		return 0;
	}

	/*
	 * If there is a winning move, take it.
	 *
	 * 2*M_SQRT+2 is the number of columns and rows plus the two diagonals.
	 * The loop starts at one due to the position being used to store the
	 * status.
	 */
	if (playerStatus[playerM][0] == 3) {
		for (int i = 1; i <= 2*M_SQRT+2; i++) {
			value = playerStatus[playerM][i];
			if (value == 3 || value == 5 || value == 6 ) {
				translateStatus(value, i, player);
				return 4;
			}
		}
	}

	// If opponent has a winning move, block them.
	if (playerStatus[opponent][0] == 3) {
		for (int i = 1; i <= 2*M_SQRT+2; i++) {
			value = playerStatus[opponent][i];
			if (value == 3 || value == 5 || value == 6 ) {
				translateStatus(value, i, player);
				return 0;
			}
		}
	}

	clearNextMoves();

	// Evaluate next best moves.
	if (playerStatus[playerM][0] == 2) {
		for (int i = 1; i <= 2*M_SQRT+2; i++) {
			value = playerStatus[playerM][i];
			if (value == 1 || value == 2 || value == 4 ) {
				translateStatus(value, i, player);
				translateStatus(value, i, opponent+1);
			}
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (nextMoves[playerM][i][j] && nextMoves[opponent][i][j]) {
					moves[i][j] = player;
					// TODO make this a random choice if there are
					// more than one choices.
					return 0;
				}
			}
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (nextMoves[playerM][i][j]) {
					moves[i][j] = player;
					// TODO make this a random choice
					return 0;
				}
			}
		}
	}

	randomMove(player);
	return 0;
}

void clearNextMoves()
{
	for (int i = 0; i < 18; i++)
		*(**nextMoves+i) = 0;
}

/*
 * Check to see if anyone can move.
 */
int checkStaleMate()
{
	if (MATRIX - keepCount(VALUE) == 0) {
		return 1;
	}
	else 
		return 0;
}

/*
 * Erases the arrays which store each players game status; Called between 
 * games.
 */
void clearStatusArrays(int whichArraySet)
{
	for (int i = 0; i < 2*M_SQRT+3; i++) {
		*(*playerStatus+i) = 0;
	}
}

/*
 * Fills the status arrays which are then in turn used to calculate the AI's
 * next move, first scanning the status of each horizontal row, then the
 * vertical column and finally the two diagonals. 
 */
int calculateStatus(int player)
{
	int playerMod;
	int state;
	int marker;
	int i;
	int j;
	int x;
	state = marker = x = 0;

	// Player indices rectified for array index.
	playerMod = player - 1;

	// Horizontal status
	for (i = 0; i < M_SQRT; i++) {
		for (j = 0; j < M_SQRT; j++)
		{
			if (moves[i][j] == player)
			{
				x = readLineStatus(j, x);
			}
			else if ((moves[i][j] == 1) || (moves[i][j] == 2))
			{
				x = 8;
				break;
			}
		}
		playerStatus[playerMod][i+1] = x;
		marker = getStatusValue(x);
		if (marker > state) {
			state = marker;
		}
		x = 0;
	}

	// Vertical status
	for (j = 0; j < M_SQRT; j++) {
		for (i = 0; i < M_SQRT; i++)
		{
			if (moves[i][j] == player)
			{
				x = readLineStatus(i, x);
			}
			else if ((moves[i][j] == 1) || (moves[i][j] == 2))
			{
				x = 8;
				break;
			}
		}
		playerStatus[playerMod][j+5] = x;
		marker = getStatusValue(x);
		if (marker > state) {
			state = marker;
		}
		x = 0;
	}

	// Diagonal one
	for (i = 0; i < 3; i++)
	{
		if (moves[i][i] == player)
		{
			x = readLineStatus(i, x);
		}
		else if ((moves[i][i] == 1) || (moves[i][i] == 2))
		{
			x = 8;
			break;
		}
	}
	playerStatus[playerMod][4] = x;
	marker = getStatusValue(x);
	if (marker > state) {
		state = marker;
	}

	x = 0;
	j = 0;

	// Diagonal two
	for (i = 2; i >= 0; i--)
	{
		if (moves[i][j] == player)
		{
			x = readLineStatus(i, x);
		}
		else if ((moves[i][j] == 1) || (moves[i][j] == 2))
		{
			x = 8;
			break;
		}
		j++;
	}
	playerStatus[playerMod][8] = x;
	marker = getStatusValue(x);
	if (marker > state) {
		state = marker;
	}
	x = 0;
	return state;
}

/*
 * This function is used by the previous one to count this places filled by each
 * opponents moves. A value is ascribed to x dependant upon the 'geometry' of
 * the move, that is to say it position in a binary row, the value returned is
 * defined by this position, in the same manor that a value is ascribed to a
 * bit in binary notation.
 */
int readLineStatus(int j, int x)
{
	/*
	 * Use 3 bit binary pattern to read row state.
	 * 000 -> 0
	 * 001 -> 1
	 * 010 -> 2
	 *  ...
	 */
	switch(j)
	{
		case 0:
			x = x + 4;
			return x;
		case 1:
			x = x + 2;
			return x;
		case 2:
			x = x + 1;
			return x;
		default:
			return x;
	}
	return -1;
}

/*
 * The final count of each rows logic is given here, this value allows the
 * computer to understand the state of each row. This value is used in the
 * computation of the computers next move, when the difficulty is set to the
 * higher or the intermediate value.
 */
int getStatusValue(int x)
{
	// Act upon states 0 through 7
	switch(x)
	{
		case 0:
			// Anyone's
			return 1;
		case 1:
			// Good move
			return 2;
		case 2:
			// Good move
			return 2;
		case 3:
			// Move to win
			return 3;
		case 4:
			// Good move
			return 2;
		case 5:
			// Move to win
			return 3;
		case 6:
			// Move to win
			return 3;
		case 7:
			// Player wins!
			return 4;
		case 8:
			// Opponent present.
			return 0;
		default:
			return 0;
	}
}

int translateStatus(int state, int line, int player)
{

	/*
	 * Act upon states 0 through 7
	 * 0 -> 000
	 * 1 -> 001
	 * 2 -> 010
	 * 3 -> 011
	 * 4 -> 100
	 * 5 -> 101
	 * 6 -> 110
	 * 7 -> 111
	 */

	switch(state)
	{
		case 0:
			// Anyone's
			// 000
			break;
		case 1:
			// Good move
			// 001
			if 	(line == 1) nextMoves[player-1][0][0] = player; 
			else if (line == 2) nextMoves[player-1][1][0] = player; 
			else if (line == 3) nextMoves[player-1][2][0] = player; 
			/* */
			else if (line == 4) nextMoves[player-1][0][0] = player; 
			/* */
			else if (line == 5) nextMoves[player-1][0][0] = player; 
			else if (line == 6) nextMoves[player-1][0][1] = player; 
			else if (line == 7) nextMoves[player-1][0][2] = player; 
			/* */
			else if (line == 8) nextMoves[player-1][0][2] = player; 

			if 	(line == 1) nextMoves[player-1][0][1] = player; 
			else if (line == 2) nextMoves[player-1][1][1] = player; 
			else if (line == 3) nextMoves[player-1][2][1] = player; 
			/* */
			else if (line == 4) nextMoves[player-1][1][1] = player; 
			/* */
			else if (line == 5) nextMoves[player-1][1][0] = player; 
			else if (line == 6) nextMoves[player-1][1][1] = player; 
			else if (line == 7) nextMoves[player-1][1][2] = player; 
			/* */
			else if (line == 8) nextMoves[player-1][1][1] = player; 

			break;
		case 2:
			// Good move
			// 010
			if 	(line == 1) nextMoves[player-1][0][0] = player; 
			else if (line == 2) nextMoves[player-1][1][0] = player; 
			else if (line == 3) nextMoves[player-1][2][0] = player; 
			/* */
			else if (line == 4) nextMoves[player-1][0][0] = player; 
			/* */
			else if (line == 5) nextMoves[player-1][0][0] = player; 
			else if (line == 6) nextMoves[player-1][0][1] = player; 
			else if (line == 7) nextMoves[player-1][0][2] = player; 
			/* */
			else if (line == 8) nextMoves[player-1][0][2] = player; 

			if 	(line == 1) nextMoves[player-1][0][2] = player; 
			else if (line == 2) nextMoves[player-1][1][2] = player; 
			else if (line == 3) nextMoves[player-1][2][2] = player; 
			/* */
			else if (line == 4) nextMoves[player-1][2][2] = player; 
			/* */
			else if (line == 5) nextMoves[player-1][2][0] = player; 
			else if (line == 6) nextMoves[player-1][2][1] = player; 
			else if (line == 7) nextMoves[player-1][2][2] = player; 
			/* */
			else if (line == 8) nextMoves[player-1][2][0] = player; 

			break;
		case 3:
			// Move to win
			// 011
			if 	(line == 1) moves[0][0] = player; 
			else if (line == 2) moves[1][0] = player; 
			else if (line == 3) moves[2][0] = player; 
			/* */
			else if (line == 4) moves[0][0] = player; 
			/* */
			else if (line == 5) moves[0][0] = player; 
			else if (line == 6) moves[0][1] = player; 
			else if (line == 7) moves[0][2] = player; 
			/* */
			else if (line == 8) moves[0][2] = player; 

			break;
		case 4:
			// Good move
			// 100
			if 	(line == 1) nextMoves[player-1][0][1] = player; 
			else if (line == 2) nextMoves[player-1][1][1] = player; 
			else if (line == 3) nextMoves[player-1][2][1] = player; 
			/* */
			else if (line == 4) nextMoves[player-1][1][1] = player; 
			/* */
			else if (line == 5) nextMoves[player-1][1][0] = player; 
			else if (line == 6) nextMoves[player-1][1][1] = player; 
			else if (line == 7) nextMoves[player-1][1][2] = player; 
			/* */
			else if (line == 8) nextMoves[player-1][1][1] = player; 

			if 	(line == 1) nextMoves[player-1][0][2] = player; 
			else if (line == 2) nextMoves[player-1][1][2] = player; 
			else if (line == 3) nextMoves[player-1][2][2] = player; 
			/* */
			else if (line == 4) nextMoves[player-1][2][2] = player; 
			/* */
			else if (line == 5) nextMoves[player-1][2][0] = player; 
			else if (line == 6) nextMoves[player-1][2][1] = player; 
			else if (line == 7) nextMoves[player-1][2][2] = player; 
			/* */
			else if (line == 8) nextMoves[player-1][2][0] = player; 

			break;
		case 5:
			// Move to win
			// 101
			if 	(line == 1) moves[0][1] = player; 
			else if (line == 2) moves[1][1] = player; 
			else if (line == 3) moves[2][1] = player; 
			/* */
			else if (line == 4) moves[1][1] = player; 
			/* */
			else if (line == 5) moves[1][0] = player; 
			else if (line == 6) moves[1][1] = player; 
			else if (line == 7) moves[1][2] = player; 
			/* */
			else if (line == 8) moves[1][1] = player; 
			break;
		case 6:
			// Move to win
			// 110
			if 	(line == 1) moves[0][2] = player; 
			else if (line == 2) moves[1][2] = player; 
			else if (line == 3) moves[2][2] = player; 
			/* */
			else if (line == 4) moves[2][2] = player; 
			/* */
			else if (line == 5) moves[2][0] = player; 
			else if (line == 6) moves[2][1] = player; 
			else if (line == 7) moves[2][2] = player; 
			/* */
			else if (line == 8) moves[2][0] = player; 
			break;
		case 7:
			// Player wins!
			// 111
			break;
		case 8:
			// Opponent present.
			break;
		default:
			break;

	}
	return 0;
}

