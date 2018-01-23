#define SAVE_SCREEN
#define ENABLE_ERROR_RETURN

#define USE_TI89

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tigcclib.h>

#define true 1
#define false 0

#define BLANK 0
#define WHITE 1
#define BLACK 2
#define FULL 3

#define EXACT 1
#define UPPERBOUND 2

typedef unsigned char bool;

struct board{
	unsigned char white_x[7];
	unsigned char black_x[7];
	unsigned char win;
	unsigned char num_moves;
	unsigned long long int hash;
	unsigned long long int columns_num;
	unsigned char columns[7];
	unsigned char moves_since_win;
	unsigned char trap_white[7];
	unsigned char trap_black[7];
	unsigned char traps[7][6];
	int column_evals[7];
	bool was_trap;
	int evaluation;
};

typedef struct board board;

unsigned short int grid[14] = {
	0b1111111111111100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1000000000000100,
	0b1111111111111100
};

unsigned short int white_piece[14] = {
	0b0000000000000000,
	0b0000011110000000,
	0b0001111111100000,
	0b0011100001110000,
	0b0011000000110000,
	0b0111000000111000,
	0b0110000000011000,
	0b0110000000011000,
	0b0111000000111000,
	0b0011000000110000,
	0b0011100001110000,
	0b0001111111100000,
	0b0000011110000000,
	0b0000000000000000
};

unsigned short int black_piece[14] = {
	0b0000000000000000,
	0b0000011110000000,
	0b0001111111100000,
	0b0011111111110000,
	0b0011111111110000,
	0b0111111111111000,
	0b0111111111111000,
	0b0111111111111000,
	0b0111111111111000,
	0b0011111111110000,
	0b0011111111110000,
	0b0001111111100000,
	0b0000011110000000,
	0b0000000000000000
};

unsigned char white_arrow[6] = {
	0b000000,
	0b111111,
	0b100001,
	0b010010,
	0b001100,
	0b000000
};

unsigned char black_arrow[6] = {
	0b000000,
	0b111111,
	0b111111,
	0b011110,
	0b001100,
	0b000000
};

unsigned char square[6] = {
	0b00000000,
	0b11111111,
	0b10000001,
	0b10000001,
	0b11111111,
	0b00000000
};

bool kill_engine = false;
unsigned int show_engine;
bool engine_move;
unsigned char order[7] = {3,2,4,1,5,0,6};
unsigned long long int powers_of_six[7] = {1, 6, 36, 1296, 7776, 46656, 279936};
unsigned char killer_moves[43];
unsigned char killer_moves2[43];
unsigned char killer_moves3[43];
unsigned char best_move = 3;
bool engine_finished = false;
char depth = 0;
unsigned char engine_depth = 0;
float score = 0;
float last_score = 0;
volatile unsigned long int num_nodes;
char player_select = 0;
unsigned char computer_select = 0;
unsigned char old_computer_select = 0;
unsigned char current_player = WHITE;
char top_text[255] = "Waiting for engine...";
volatile bool quit = false;
void *kbq;
board *b;
unsigned char columns[7];

INT_HANDLER old_int_5 = NULL;

void draw_grid(){
	unsigned int x;
	unsigned int y;
	current_player = WHITE;
	memset(columns, 0, sizeof(columns));
	old_computer_select = computer_select;
	if(show_engine){
		Sprite8(computer_select*14 + 32, 10, 6, square, LCD_MEM, SPRT_XOR);
	}
	Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
	for(x = 0; x < 7; x++){
		for(y = 0; y < 6; y++){
			Sprite16(x*14 + 31, y*14 + 16, 14, grid, LCD_MEM, SPRT_OR);
		}
	}
}

board *create_board(){
	board *output;
	output = calloc(1, sizeof(board));
	return output;
}

unsigned int get_index(board *b){
	return (b->hash + b->columns_num)%1999;
}

void update_column(board *b, unsigned char x){
	unsigned char i;
	bool evens;
	bool odds;
	bool white_last;
	bool black_last;
	bool white_trap;
	bool black_trap;
	evens = true;
	odds = true;
	white_last = false;
	black_last = false;
	white_trap = false;
	black_trap = false;
	b->evaluation -= b->column_evals[x];
	b->column_evals[x] = 0;
	for(i = b->columns[x] + 1; i < 6; i++){
		if(b->traps[x][i]){
			if(!(i%2) && odds){
				if(b->traps[x][i] == WHITE){
					if(white_last && !black_trap){
						b->column_evals[x] += 150;
					} else {
						b->column_evals[x] += 50;
					}
					odds = false;
					white_last = true;
					black_last = false;
					white_trap = true;
				} else if(b->traps[x][i] == BLACK){
					if(black_last && !white_trap){
						b->column_evals[x] -= 150;
					} else {
						b->column_evals[x] -= 15;
					}
					odds = false;
					black_last = true;
					white_last = false;
					black_trap = true;
				} else {
					b->column_evals[x] += 35;
					break;
				}
			} else if(i%2 && evens){
				if(b->traps[x][i] == WHITE){
					if(white_last && !black_trap){
						b->column_evals[x] += 150;
					} else {
						b->column_evals[x] += 15;
					}
					evens = false;
					white_last = true;
					black_last = false;
					white_trap = true;
				} else if(b->traps[x][i] == BLACK){
					if(black_last && !white_trap){
						b->column_evals[x] -= 150;
					} else {
						b->column_evals[x] -= 30;
					}
					evens = false;
					black_last = true;
					white_last = false;
					black_trap = true;
				} else {
					b->column_evals[x] -= 15;
					break;
				}
			}
		} else {
			white_last = false;
			black_last = false;
		}
		if(!evens && !odds){
			break;
		}
	}
	b->evaluation += b->column_evals[x];
}

void place_piece(board *b, unsigned char x, unsigned char y, unsigned char color){
	unsigned char y_mask;
	unsigned char x_mask;
	y_mask = 1<<y;
	x_mask = 1<<x;
	if(color == WHITE){
		b->white_x[x] |= y_mask;
	} else {
		b->black_x[x] |= y_mask;
	}
	if(b->traps[x][y] != BLANK && b->traps[x][y] != FULL){
		b->traps[x][y] = BLANK;
		update_column(b, x);
	}
}

unsigned char get_color(board *b, int x, int y){
	if(x < 0 || x > 6 || y < 0 || y > 5){
		return BLANK;
	}
	if(b->white_x[x] & (1<<y)){
		return WHITE;
	} else if(b->black_x[x] & (1<<y)){
		return BLACK;
	} else {
		return BLANK;
	}
}

void trap(board *b, int x, int y, unsigned char color){
	if(x < 0 || x > 6 || y < 0 || y > 5){
		return;
	}
	if(b->traps[x][y] == BLANK){
		b->traps[x][y] = color;
	} else if(b->traps[x][y] != color){
		b->traps[x][y] = FULL;
	}
	update_column(b, x);
}

unsigned char check_win(board *b, int x, int y, unsigned char color, unsigned char first_call){
	unsigned char square;
	unsigned char counter_x;
	unsigned char counter_y;
	unsigned char counter_a;
	unsigned char counter_b;
	unsigned char trap_win;
	unsigned char trap_x1;
	unsigned char trap_x2;
	unsigned char trap_a1;
	unsigned char trap_a2;
	unsigned char trap_b1;
	unsigned char trap_b2;
	bool counting_x;
	bool counting_y;
	bool counting_a;
	bool counting_b;
	char i;
	counter_x = 1;
	counter_y = 1;
	counter_a = 1;
	counter_b = 1;
	trap_x1 = 0;
	trap_x2 = 0;
	trap_a1 = 0;
	trap_a2 = 0;
	trap_b1 = 0;
	trap_b2 = 0;
	counting_x = true;
	counting_y = true;
	counting_a = true;
	counting_b = true;
	for(i = 1; i < 4; i++){
		if(counting_x){
			square = get_color(b, x + i, y);
			if(square == color){
				counter_x += 1;
			} else {
				counting_x = false;
				if(square == BLANK && y > b->columns[x + i]){
					trap_x1 = i;
				}
			}
		}
		if(counting_y){
			square = get_color(b, x, y + i);
			if(square == color){
				counter_y += 1;
			} else {
				counting_y = false;
			}
		}
		if(counting_a){
			square = get_color(b, x + i, y - i);
			if(square == color){
				counter_a += 1;
			} else {
				counting_a = false;
				if(square == BLANK && (y - i) > b->columns[x + i]){
					trap_a1 = i;
				}
			}
		}
		if(counting_b){
			square = get_color(b, x + i, y + i);
			if(square == color){
				counter_b += 1;
			} else {
				counting_b = false;
				if(square == BLANK && (y + i) > b->columns[x + i]){
					trap_b1 = i;
				}
			}
		}
		if(!counting_x && !counting_y && !counting_a && !counting_b){
			break;
		}
	}
	counting_x = true;
	counting_y = true;
	counting_a = true;
	counting_b = true;
	for(i = 1; i < 4; i++){
		if(counting_x){
			square = get_color(b, x - i, y);
			if(square == color){
				counter_x += 1;
			} else {
				counting_x = false;
				if(square == BLANK && y > b->columns[x - i]){
					trap_x2 = i;
				}
			}
		}
		if(counting_y){
			square = get_color(b, x, y - i);
			if(square == color){
				counter_y += 1;
			} else {
				counting_y = false;
			}
		}
		if(counting_a){
			square = get_color(b, x - i, y + i);
			if(square == color){
				counter_a += 1;
			} else {
				counting_a = false;
				if(square == BLANK && (y + i) > b->columns[x - i]){
					trap_a2 = i;
				}
			}
		}
		if(counting_b){
			square = get_color(b, x - i, y - i);
			if(square == color){
				counter_b += 1;
			} else {
				counting_b = false;
				if(square == BLANK && (y - i) > b->columns[x - i]){
					trap_b2 = i;
				}
			}
		}
		if(!counting_x && !counting_y && !counting_a && !counting_b){
			break;
		}
	}
	if(trap_x1){
		if(counter_x == 1){
			if(get_color(b, x + trap_x1 + 1, y) == color && get_color(b, x + trap_x1 + 2, y) == color){
				trap(b, x + trap_x1, y, color);
			}
		} else if(counter_x == 2){
			if(get_color(b, x + trap_x1 + 1, y) == color){
				trap(b, x + trap_x1, y, color);
			}
		} else if(counter_x == 3){
			trap(b, x + trap_x1, y, color);
		}
	}
	if(trap_a1){
		if(counter_a == 1){
			if(get_color(b, x + trap_a1 + 1, y - trap_a1 - 1) == color && get_color(b, x + trap_a1 + 2, y - trap_a1 - 2) == color){
				trap(b, x + trap_a1, y - trap_a1, color);
			}
		} else if(counter_a == 2){
			if(get_color(b, x + trap_a1 + 1, y - trap_a1 - 1) == color){
				trap(b, x + trap_a1, y - trap_a1, color);
			}
		} else if(counter_a == 3){
			trap(b, x + trap_a1, y - trap_a1, color);
		}
	}
	if(trap_b1){
		if(counter_b == 1){
			if(get_color(b, x + trap_b1 + 1, y + trap_b1 + 1) == color && get_color(b, x + trap_b1 + 2, y + trap_b1 + 2) == color){
				trap(b, x + trap_b1, y + trap_b1, color);
			}
		} else if(counter_b == 2){
			if(get_color(b, x + trap_b1 + 1, y + trap_b1 + 1) == color){
				trap(b, x + trap_b1, y + trap_b1, color);
			}
		} else if(counter_b == 3){
			trap(b, x + trap_b1, y + trap_b1, color);
		}
	}
	if(trap_x2){
		if(counter_x == 1){
			if(get_color(b, x - trap_x2 - 1, y) == color && get_color(b, x - trap_x2 - 2, y) == color){
				trap(b, x - trap_x2, y, color);
			}
		} else if(counter_x == 2){
			if(get_color(b, x - trap_x2 - 1, y) == color){
				trap(b, x - trap_x2, y, color);
			}
		} else if(counter_x == 3){
			trap(b, x - trap_x2, y, color);
		}
	}
	if(trap_a2){
		if(counter_a == 1){
			if(get_color(b, x - trap_a2 - 1, y + trap_a2 + 1) == color && get_color(b, x - trap_a2 - 2, y + trap_a2 + 2) == color){
				trap(b, x - trap_a2, y + trap_a2, color);
			}
		} else if(counter_a == 2){
			if(get_color(b, x - trap_a2 - 1, y + trap_a2 + 1) == color){
				trap(b, x - trap_a2, y + trap_a2, color);
			}
		} else if(counter_a == 3){
			trap(b, x - trap_a2, y + trap_a2, color);
		}
	}
	if(trap_b2){
		if(counter_b == 1){
			if(get_color(b, x - trap_b2 - 1, y - trap_b2 - 1) == color && get_color(b, x - trap_b2 - 2, y - trap_b2 - 2) == color){
				trap(b, x - trap_b2, y - trap_b2, color);
			}
		} else if(counter_b == 2){
			if(get_color(b, x - trap_b2 - 1, y - trap_b2 - 1) == color){
				trap(b, x - trap_b2, y - trap_b2, color);
			}
		} else if(counter_b == 3){
			trap(b, x - trap_b2, y - trap_b2, color);
		}
	}
	if(counter_x >= 4 || counter_y >= 4 || counter_a >= 4 || counter_b >= 4){
		return color;
	} else {
		return BLANK;
	}
}

void display_board(board *b){
	char i;
	char j;
	for(i = 5; i >= 0; i--){
		for(j = 0; j < 7; j++){
			printf("%d", get_color(b, j, i));
		}
		printf("\n");
	}
	printf("\n");
	char ch;
	scanf("%c", &ch);
}

void make_move(board *b, unsigned char x, unsigned char color){
	unsigned char y;
	y = b->columns[x];
	place_piece(b, x, y, color);
	b->columns[x] += 1;
	if(b->win == BLANK){
		b->win = check_win(b, x, y, color, true);
	} else {
		b->moves_since_win += 1;
	}
}

void undo(board *b, unsigned char x){
	unsigned char y;
	unsigned char y_mask;
	unsigned char x_mask;
	unsigned char color;
	y = b->columns[x] - 1;
	y_mask = 1<<y;
	x_mask = 1<<x;
	if(b->white_x[x] & y_mask){
		b->white_x[x] &= ~y_mask;
	} else {
		b->black_x[x] &= ~y_mask;
	}
	b->columns[x] -= 1;
	if(b->moves_since_win == 0){
		b->win = BLANK;
	} else {
		b->moves_since_win -= 1;
	}
}

DEFINE_INT_HANDLER (time_update){
	unsigned int key;
	while(!OSdequeue(&key, kbq)){
		if(key == KEY_UP){
			show_engine = !show_engine;
			Sprite8(computer_select*14 + 32, 10, 6, square, LCD_MEM, SPRT_XOR);
			DrawStr(0, 0, "                           ", A_REVERSE);
			if(show_engine){
				sprintf(top_text, "%.2f %.2f d: %d", score, last_score, engine_depth);
				DrawStr(0, 0, top_text, A_REVERSE);
			}
		} else if(key == KEY_LEFT){
			if(current_player == WHITE){
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
			} else if(current_player == BLACK){
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
			}
			player_select -= 1;
			if(player_select < 0){
				player_select = 6;
			}
			if(current_player == WHITE){
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
			} else if(current_player == BLACK){
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
			}
		} else if(key == KEY_RIGHT){
			if(current_player == WHITE){
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
			} else if(current_player == BLACK){
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
			}
			player_select = (player_select + 1)%7;
			if(current_player == WHITE){
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
			} else if(current_player == BLACK){
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
			}
		} else if(key == KEY_DOWN){
			if(current_player == WHITE && columns[computer_select] < 6){
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
				player_select = computer_select;
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
				Sprite16(player_select*14 + 31, (5 - columns[player_select])*14 + 16, 14, white_piece, LCD_MEM, SPRT_OR);
				columns[player_select] += 1;
				depth = 0;
				kill_engine = true;
			} else if(current_player == BLACK && b->columns[computer_select] < 6){
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
				player_select = computer_select;
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
				Sprite16(player_select*14 + 31, (5 - columns[player_select])*14 + 16, 14, black_piece, LCD_MEM, SPRT_OR);
				columns[player_select] += 1;
				depth = 0;
				kill_engine = true;
			}
		} else if(key == KEY_ENTER){
			if(current_player == WHITE && columns[player_select] < 6){
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
				Sprite16(player_select*14 + 31, (5 - columns[player_select])*14 + 16, 14, white_piece, LCD_MEM, SPRT_OR);
				columns[player_select] += 1;
				depth = 0;
				kill_engine = true;
			} else if(current_player == BLACK && b->columns[player_select] < 6){
				Sprite8(player_select*14 + 33, 10, 6, black_arrow, LCD_MEM, SPRT_XOR);
				Sprite8(player_select*14 + 33, 10, 6, white_arrow, LCD_MEM, SPRT_XOR);
				Sprite16(player_select*14 + 31, (5 - columns[player_select])*14 + 16, 14, black_piece, LCD_MEM, SPRT_OR);
				columns[player_select] += 1;
				depth = 0;
				kill_engine = true;
			}
		} else if(key == KEY_ESC){
			kill_engine = true;
			quit = true;
		}
	}
	if(engine_finished){
		if(show_engine){
			sprintf(top_text, "%.2f %.2f d: %d nps: %ld", score, last_score, engine_depth, num_nodes*50);
			DrawStr(0, 0, "                           ", A_REVERSE);
			DrawStr(0, 0, top_text, A_REVERSE);
		}
		engine_finished = false;
		num_nodes = 0;
	}
	ExecuteHandler(old_int_5);
}

int negamax(board *b, unsigned char depth, int alpha, int beta, unsigned char color, unsigned char move, bool first_call){
	unsigned char i;
	unsigned char j;
	unsigned int index;
	unsigned char x;
	unsigned char prev_white;
	unsigned char prev_black;
	unsigned char traps[7][6];
	int column_evals[7];
	unsigned char inner_best = 3;
	unsigned long long int hash_before;
	unsigned long long int columns_num_before;
	int best;
	int v;
	int alpha_orig;
	int evaluation_before;
	alpha_orig = alpha;
	num_nodes += 1;
	if(kill_engine){
		return 0;
	}
	if(depth == 0){
		if(b->win == color){
			return 1000;
		} else if(b->win != color && b->win != BLANK){
			return -1000;
		}
		if(color == WHITE){
			return b->evaluation;
		} else {
			return -b->evaluation;
		}
	}
	best = -1001;
	v = -1001;
	memcpy(traps, b->traps, sizeof(traps));
	memcpy(column_evals, b->column_evals, sizeof(column_evals));
	for(i = 0; i < 10; i++){
		if(i == 0){
			x = killer_moves[move];
		} else if(i == 1){
			x = killer_moves2[move];
		} else if(i == 2){
			x = killer_moves3[move];
		} else {
			x = order[i - 3];
		}
		if((x != killer_moves[move] && x != killer_moves2[move] && x != killer_moves3[move]) || (x == killer_moves[move] && i == 0) || (x == killer_moves2[move] && i == 1) || (x == killer_moves3[move] && i == 2)){
			if(b->columns[x] < 6){
				evaluation_before = b->evaluation;
				hash_before = b->hash;
				columns_num_before = b->columns_num;
				make_move(b, x, color);
				if(b->win == color){
					v = 1000;
				} else if(b->win != color && b->win != BLANK){
					v = -1000;
				} else {
					if(color == WHITE){
						v = -negamax(b, depth - 1, -beta, -alpha, BLACK, move + 1, false);
					} else {
						v = -negamax(b, depth - 1, -beta, -alpha, WHITE, move + 1, false);
					}
				}
				undo(b, x);
				b->evaluation = evaluation_before;
				b->hash = hash_before;
				b->columns_num = columns_num_before;
				memcpy(b->traps, traps, sizeof(b->traps));
				memcpy(b->column_evals, column_evals, sizeof(b->column_evals));
				if(v > best){
					best = v;
					if(first_call){
						best_move = x;
					}
					inner_best = x;
				}
				if(v > alpha){
					alpha = v;
				}
				if(alpha >= beta){
					break;
				}
			}
		}
	}
	if(inner_best != killer_moves[move] && inner_best != killer_moves2[move]){
		killer_moves3[move] = killer_moves2[move];
		killer_moves2[move] = killer_moves[move];
		killer_moves[move] = inner_best;
	} else if(inner_best != killer_moves[move] && inner_best == killer_moves2[move]){
		killer_moves2[move] = killer_moves[move];
		killer_moves[move] = inner_best;
	}
	if(best == 1001 || best == -1001){
		return 0;
	}
	return best;
}

void _main(){
	memset(killer_moves, 3, sizeof(killer_moves));
	memset(killer_moves2, 2, sizeof(killer_moves2));
	memset(killer_moves3, 4, sizeof(killer_moves3));
	b = create_board();
	kill_engine = false;
	clrscr();
	draw_grid();
	kbq = kbd_queue();
	old_int_5 = GetIntVec(AUTO_INT_5);
	SetIntVec(AUTO_INT_5, time_update);
	float temp_score;
	unsigned int i;
	bool draw;
	depth = 0;
	DrawStr(0, 0, "                           ", A_REVERSE);
	while(1){
		num_nodes = 0;
		if(depth <= 42){
			temp_score = ((float) negamax(b, depth, -1001, 1001, current_player, 0, true))/100;
			if(quit){
				SetIntVec(AUTO_INT_5, old_int_5);
				clrscr();
				quit = false;
				free(b);
				return;
			}
			old_computer_select = computer_select;
			computer_select = best_move;
			engine_depth = depth;
			depth += 1;
			if(show_engine){
				Sprite8(old_computer_select*14 + 32, 10, 6, square, LCD_MEM, SPRT_XOR);
				Sprite8(computer_select*14 + 32, 10, 6, square, LCD_MEM, SPRT_XOR);
			}
		}
		if(!kill_engine){
			last_score = score;
			score = temp_score;
			engine_finished = true;
		} else if(!quit){
			kill_engine = false;
			make_move(b, player_select, current_player);
			if(current_player == WHITE){
				current_player = BLACK;
			} else {
				current_player = WHITE;
			}
			if(b->win != BLANK){
				DrawStr(0, 0, "                           ", A_REVERSE);
				if(b->win == WHITE){
					DrawStr(0, 0, "White Wins!", A_REVERSE);
				} else {
					DrawStr(0, 0, "Black Wins!", A_REVERSE);
				}
				ngetchx();
				SetIntVec(AUTO_INT_5, old_int_5);
				clrscr();
				free(b);
				return;
			}
			draw = true;
			for(i = 0; i < 7; i++){
				if(columns[i] != 6){
					draw = false;
					break;
				}
			}
			if(draw){
				DrawStr(0, 0, "                           ", A_REVERSE);
				DrawStr(0, 0, "Draw!", A_REVERSE);
				ngetchx();
				SetIntVec(AUTO_INT_5, old_int_5);
				clrscr();
				free(b);
				return;
			}
		}
	}
}
