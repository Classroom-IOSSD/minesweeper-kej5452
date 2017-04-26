#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conio.h"
#include <stdbool.h>
#define MAX 10

	/// background color
#define BRED  "\x1B[41m"
#define BGRN  "\x1B[42m"
#define BYEL  "\x1B[43m"
#define BBLU  "\x1B[44m"
#define BMAG  "\x1B[45m"
#define BCYN  "\x1B[46m"
#define BWHT  "\x1B[47m"

	/// text color
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define BIT(x) (0x01 << (x))
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) (((x) & (y)) == (y))

const unsigned char FLAG_MASK = BIT(4);
const unsigned char UNCOVERED_MASK = BIT(5);
const unsigned char MINE_MASK = BIT(6);

const unsigned char NUM_MASK_3 = BIT(3);
const unsigned char NUM_MASK_2 = BIT(2);
const unsigned char NUM_MASK_1 = BIT(1);
const unsigned char NUM_MASK_0 = BIT(0);

// global variables
// game table
unsigned int table_array[MAX][MAX];
// location of cursor
int x = 0, y = 0;
// flag: input mode = 0, flag mode = 1, check mode = 2
int game_mode = 0;


inline bool has_mine(unsigned int cell) {
	return BITMASK_CHECK(cell, MINE_MASK);
}
inline bool is_uncovered(unsigned int cell) {
	return BITMASK_CHECK(cell, UNCOVERED_MASK);
}
inline bool is_flagged(unsigned int cell) {
	return BITMASK_CHECK(cell, FLAG_MASK);
}
inline void put_mine(unsigned int cell) {
	BITMASK_SET(cell, MINE_MASK);
}
inline void uncover(unsigned int cell) {
	BITMASK_SET(cell, UNCOVERED_MASK);
}
inline void put_flag(unsigned int cell) {
	BITMASK_SET(cell, FLAG_MASK);
}


inline unsigned int num_mines(unsigned int cell) {
	unsigned int num_mine = 0;
	if (BITMASK_CHECK(cell, NUM_MASK_3)) {
		num_mine = num_mine + 8;
	}
	if (BITMASK_CHECK(cell, NUM_MASK_2)) {
		num_mine = num_mine + 4;
	}
	if (BITMASK_CHECK(cell, NUM_MASK_1)) {
		num_mine = num_mine + 2;
	}
	if (BITMASK_CHECK(cell, NUM_MASK_0)) {
		num_mine = num_mine + 1;
	}
	return num_mine;
}
/**
* This is a recursive function which uncovers blank cells while they are adjacent
*/
int uncover_blank_cell(int row, int col) {
	int rows[8], columns[8], i;
	unsigned int value;
	if (!BITMASK_CHECK(table_array[row][col],0x0))
		return 0; // error

	uncover(table_array[row][col]); // uncover current cell
	// Get position of adjacent cells of current cell
	rows[0] = row - 1;
	columns[0] = col + 1;
	rows[1] = row;
	columns[1] = col + 1;
	rows[2] = row + 1;
	columns[2] = col + 1;
	rows[3] = row - 1;
	columns[3] = col;
	rows[4] = row + 1;
	columns[4] = col;
	rows[5] = row - 1;
	columns[5] = col - 1;
	rows[6] = row;
	columns[6] = col - 1;
	rows[7] = row + 1;
	columns[7] = col - 1;

	for (i = 0; i < 8; i++) {
		value = table_array[rows[i]][columns[i]];
		bool isValid = (rows[i] >= 0 && rows[i] < MAX) && (columns[i] >= 0 && columns[i] < MAX);
		if (isValid) {	
			// to prevent negative index and out of bounds
			if (num_mines(value) > 0 && num_mines(value) <= 8)
				uncover(table_array[rows[i]][columns[i]]);				// it is a cell with 1-8 number so we need to uncover
			else if (num_mines(value) == 0)
				uncover_blank_cell(rows[i], columns[i]);
		}

	}

	return 1; // success!
}

void print_table() {
	// clear screen
	system("clear");

	int i, j;
	unsigned int value;

	for (i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			if (x == j && y == i) {
				if (game_mode == 1) {
					printf("|%sF%s", BMAG, KNRM);
					continue;
				}
				else if (game_mode == 2) {
					printf("|%sC%s", BMAG, KNRM);
					continue;
				}
			}

			value = table_array[i][j];

			if (!is_uncovered(value))
				printf("|X");
			else if (is_uncovered(value) && num_mines(value) == 0) // clean area
				printf("|%s%d%s", KCYN, num_mines(value), KNRM);
			else if (is_uncovered(value) && num_mines(value) == 1) // the number of near mine is 1
				printf("|%s%d%s", KYEL, num_mines(value), KNRM);
			else if (is_uncovered(value) && num_mines(value) > 1 && num_mines(value) <= 8) // the number of near mine is greater than 1
				printf("|%s%d%s", KRED, num_mines(value), KNRM);
			else if (is_flagged(value) && ((num_mines(value)>= 0 && num_mines(value) <= 8) || has_mine(value)))
				printf("|%sF%s", KGRN, KNRM);
			else
				printf("ERROR"); // test purposes

		}
		printf("|\n");
	}

	printf("cell values: 'X' unknown, '%s0%s' no mines close, '1-8' number of near mines, '%sF%s' flag in cell\n", KCYN, KNRM, KGRN, KNRM);
	if (game_mode == 0) {
		printf("f (put/remove Flag in cell), c (Check cell), n (New game), q (Exit game): ");
	}
	else if (game_mode == 1) {
		printf("Enter (select to put/remove Flag in cell), q (Exit selection): ");
	}
	else if (game_mode == 2) {
		printf("Enter (select to check cell), q (Exit selection): ");
	}
}

void move_direction(int direction, int* x, int* y) {
	if (direction == '8') {
		// up
		--(*y);
		*y = (MAX + (*y)) % MAX;
	}
	else if (direction == '2') {
		// down
		(*y)++;
		*y = (*y) % MAX;
	}
	else if (direction == '4') {
		(*x)--;
		*x = (MAX + (*x)) % MAX;
	}
	else if (direction == '6') {
		(*x)++;
		*x = (*x) % MAX;
	}
}

int main(int argc, char *argv[]) {

	char ch;
	int number_Mines; // the number of the remaining mines
	int i, j, row, col, rows[8], columns[8];
	unsigned int value;

	while (1) {
		// the number of mines
		number_Mines = 10;
		if (argc == 2) {
			number_Mines = atoi(argv[1]);
		}
		srand(time(NULL));						// random seed
												// setting cursor
		x = 0;
		y = 0;
		// set all cells to 0
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
				table_array[i][j] = 0;

		for (i = 0; i < number_Mines; i++) {
			/* initialize random seed: */

			row = rand() % 10;					// it generates a integer in the range 0 to 9
			col = rand() % 10;

			// put mines
			if (has_mine(table_array[row][col])) {
				put_mine(table_array[row][col]);

				// Get position of adjacent cells of current cell
				rows[0] = row - 1;
				columns[0] = col + 1;
				rows[1] = row;
				columns[1] = col + 1;
				rows[2] = row + 1;
				columns[2] = col + 1;
				rows[3] = row - 1;
				columns[3] = col;
				rows[4] = row + 1;
				columns[4] = col;
				rows[5] = row - 1;
				columns[5] = col - 1;
				rows[6] = row;
				columns[6] = col - 1;
				rows[7] = row + 1;
				columns[7] = col - 1;

				for (j = 0; j < 8; j++) {
					value = table_array[rows[j]][columns[j]];
					bool isValid = (rows[i] >= 0 && rows[i] < MAX) && (columns[i] >= 0 && columns[i] < MAX);
					if (isValid) {	// to prevent negative index and out of bounds
						if (!has_mine(table_array[row][col]))																// to prevent remove mines
							table_array[rows[j]][columns[j]] += 1;									// sums 1 to each adjacent cell
					}
				}
			}
			else {							// to make sure that there are the properly number of mines in table
				i--;
				continue;
			}
		}

		while (number_Mines != 0) {			// when nMines becomes 0 you will win the game
			print_table();

			ch = getch();
			// cursor direction
			char direction;
			switch (ch) {
			// flag mode
			case 'f':
			case 'F':
				game_mode = 1;
				do {
					print_table();
					direction = getch();
					move_direction(direction, &x, &y);
					if (direction == 'c' || direction == 'C') {
						//goto check_mode;
						game_mode = 2;
						do {
							print_table();
							direction = getch();

							// arrow direction
							move_direction(direction, &x, &y);
							if (direction == 'f' || direction == 'F') {
								break;
							}

							else if (direction == '\n') {
								value = table_array[y][x];
								if (value == 0) {
									// blank case
									uncover_blank_cell(y, x);
								}
								else if (has_mine(value)) {		// mine case
									game_mode = 0;
									print_table();
									printf("\nGAME OVER\n");

									if (number_Mines == 0)
										printf("you won!!!!\n");

									else
										printf("BOOM! you LOOSE!\n");

									do {
										printf("Are you sure to exit? (y or n)? ");
										ch = getch();
										putchar('\n');
										if (ch == 'y' || ch == 'Y') {
											break;
										}
										else if (ch == 'n' || ch == 'N') {
											continue;
										}
										printf("Please answer y or n\n");
									} while (1);
									printf("See you next time!\n");

									fflush(stdin);

									return 0;
								}
								else if (num_mines(value) > 0 && num_mines(value) <= 8) {	// number case (the next cell is a mine)
									uncover(table_array[y][x]);
								}
								//	break;
							}
						} while (direction != 'q' && direction != 'Q');

						if (direction == 'f' || direction == 'F') {
							game_mode = 1;
							continue;
						}

						game_mode = 0;

						break;
					}
					else if (direction == '\n') {
						value = table_array[y][x];

						if (has_mine(value)) {				// mine case
							put_flag(table_array[y][x]);
							number_Mines -= 1;				// mine found
						}
						else if (num_mines(value) >= 0 && num_mines(value) <= 8) {	// number of mines case (the next cell is a mine)
							put_flag(table_array[y][x]);
						}
						else if (num_mines(value) >= 0 && num_mines(value) <= 8 && is_flagged(value)) {
							put_flag(table_array[y][x]);
						}

						if (number_Mines == 0)
							break;
					}
				} while (direction != 'q' && direction != 'Q');
				game_mode = 0;
				break;

				// check cell
			case 'c':
			case 'C':
				game_mode = 2;
				do {
					print_table();
					direction = getch();

					// arrow direction
					move_direction(direction, &x, &y);
					if (direction == 'f' || direction == 'F') {
						game_mode = 1;
						do {
							print_table();
							direction = getch();
							// arrow direction
							move_direction(direction, &x, &y);
							if (direction == 'c' || direction == 'C') {
								break;
							}
							else if (direction == '\n') {
								value = table_array[y][x];

								if (has_mine(value)) {				// mine case
									put_flag(table_array[y][x]);
									number_Mines -= 1;				// mine found
								}
								else if (num_mines(value) >= 0 && num_mines(value) <= 8) {	// number of mines case (the next cell is a mine)
									put_flag(table_array[y][x]);
								}
								else if (num_mines(value) >= 0 && num_mines(value) <= 8 && is_flagged(value)) {
									put_flag(table_array[y][x]);
								}

								if (number_Mines == 0)
									break;
							}
						} while (direction != 'q' && direction != 'Q');
						if (direction == 'c' || direction == 'C') {
							game_mode = 2;
							continue;
						}
						game_mode = 0;
						break;
					}

					else if (direction == '\n') {
						value = table_array[y][x];
						if (value == 0) {
							// blank case
							uncover_blank_cell(y, x);
						}
						else if (has_mine(value)) {		// mine case
							game_mode = 0;
							print_table();
							printf("\nGAME OVER\n");

							if (number_Mines == 0)
								printf("you won!!!!\n");

							else
								printf("BOOM! you LOOSE!\n");

							do {
								printf("Are you sure to exit? (y or n)? ");
								ch = getch();
								putchar('\n');
								if (ch == 'y' || ch == 'Y') {
									break;
								}
								else if (ch == 'n' || ch == 'N') {
									continue;
								}
								printf("Please answer y or n\n");
							} while (1);
							printf("See you next time!\n");

							fflush(stdin);

							return 0;
						}
						else if (num_mines(value) >= 0 && num_mines(value) <= 8) {	// number case (the next cell is a mine)
							uncover(table_array[y][x]);
						}
						//	break;
					}
				} while (direction != 'q' && direction != 'Q');
				game_mode = 0;

				break;

				// jump to a new game
			case 'n':
			case 'N':
				continue;

				// exit
			case 'q':
			case 'Q':
				game_mode = 0;
				print_table();
				printf("\nGAME OVER\n");

				if (number_Mines == 0)
					printf("you won!!!!\n");

				else
					printf("BOOM! you LOOSE!\n");

				do {
					printf("Are you sure to exit? (y or n)? ");
					ch = getch();
					putchar('\n');
					if (ch == 'y' || ch == 'Y') {
						break;
					}
					else if (ch == 'n' || ch == 'N') {
						continue;
					}
					printf("Please answer y or n\n");
				} while (1);
				printf("See you next time!\n");

				fflush(stdin);

				return 0;

			default:
				break;
			}
		}
	}
	return 0;
}
