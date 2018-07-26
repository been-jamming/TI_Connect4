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
#define BLACK -1
#define FULL 3

#define EXACT 1
#define UPPERBOUND 2

typedef unsigned char bool;

struct board{
	char spaces[7][6];
	char win;
	unsigned char num_moves;
	char columns[7];
	unsigned char moves_since_win;
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

char wins[70];
unsigned char win_squares[7][6][13] = {{
	{ 1, 25, 46,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 2, 25, 26, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 3, 25, 26, 27, 48,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 4, 25, 26, 27, 58,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 5, 26, 27, 59,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 6, 27, 60,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
},{
	{ 1,  7, 28, 49,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 2,  8, 28, 29, 46, 50,  0,  0,  0,  0,  0,  0,  0},
	{ 3,  9, 28, 29, 30, 47, 51, 58,  0,  0,  0,  0,  0},
	{ 4, 10, 28, 29, 30, 48, 59, 61,  0,  0,  0,  0,  0},
	{ 5, 11, 29, 30, 60, 62,  0,  0,  0,  0,  0,  0,  0},
	{ 6, 12, 30, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0}
},{
	{ 1,  7, 13, 31, 52,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 2,  8, 14, 31, 32, 49, 53, 58,  0,  0,  0,  0,  0},
	{ 3,  9, 15, 31, 32, 33, 46, 50, 54, 59, 61,  0,  0},
	{ 4, 10, 16, 31, 32, 33, 47, 51, 60, 62, 64,  0,  0},
	{ 5, 11, 17, 32, 33, 48, 63, 65,  0,  0,  0,  0,  0},
	{ 6, 12, 18, 33, 66,  0,  0,  0,  0,  0,  0,  0,  0}
},{
	{ 1,  7, 13, 19, 34, 55, 58,  0,  0,  0,  0,  0,  0},
	{ 2,  8, 14, 20, 34, 35, 52, 56, 59, 61,  0,  0,  0},
	{ 3,  9, 15, 21, 34, 35, 36, 49, 53, 57, 60, 62, 64},
	{ 4, 10, 16, 22, 34, 35, 36, 46, 50, 54, 63, 65, 67},
	{ 5, 11, 17, 23, 35, 36, 47, 51, 66, 68,  0,  0,  0},
	{ 6, 12, 18, 24, 36, 48, 69,  0,  0,  0,  0,  0,  0}
},{
	{ 7, 13, 19, 37, 61,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 8, 14, 20, 37, 38, 55, 62, 64,  0,  0,  0,  0,  0},
	{ 9, 15, 21, 37, 38, 39, 52, 56, 63, 65, 67,  0,  0},
	{10, 16, 22, 37, 38, 39, 49, 53, 57, 66, 68,  0,  0},
	{11, 17, 23, 38, 39, 50, 54, 69,  0,  0,  0,  0,  0},
	{12, 18, 24, 39, 51,  0,  0,  0,  0,  0,  0,  0,  0}
},{
	{13, 19, 40, 64,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{14, 20, 40, 41, 65, 67,  0,  0,  0,  0,  0,  0,  0},
	{15, 21, 40, 41, 42, 55, 66, 68,  0,  0,  0,  0,  0},
	{16, 22, 40, 41, 42, 52, 56, 69,  0,  0,  0,  0,  0},
	{17, 23, 41, 42, 53, 57,  0,  0,  0,  0,  0,  0,  0},
	{18, 24, 42, 54,  0,  0,  0,  0,  0,  0,  0,  0,  0}
},{
	{19, 43, 67,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{20, 43, 44, 68,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{21, 43, 44, 45, 69,  0,  0,  0,  0,  0,  0,  0,  0},
	{22, 43, 44, 45, 55,  0,  0,  0,  0,  0,  0,  0,  0},
	{23, 44, 45, 56,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{24, 45, 57,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
}};

bool kill_engine = false;
unsigned int show_engine;
bool engine_move;
unsigned char order[7] = {3,2,4,1,5,0,6};
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
char current_player = WHITE;
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

void place_piece(board *b, unsigned char x, unsigned char y, char color){
	unsigned char i;
	unsigned char win_id;
	char trap_sum;
	b->spaces[x][y] = color;
	
	if(b->win){
		return;
	}
	
	for(i = 0; i < 13; i++){
		win_id = win_squares[x][y][i];
		if(!win_id){
			break;
		}
		wins[win_id] += color;
		if(wins[win_id] == 4){
			b->win = WHITE;
			break;
		} else if(wins[win_id] == -4){
			b->win = BLACK;
			break;
		}
	}
}

void make_move(board *b, unsigned char x, unsigned char color){
	unsigned char y;
	y = b->columns[x];
	place_piece(b, x, y, color);
	b->columns[x] += 1;
	if(b->win != BLANK){
		b->moves_since_win += 1;
	}
}

void undo(board *b, unsigned char x){
	unsigned char i;
	unsigned char win_id;
	unsigned char y;
	unsigned char color;
	unsigned char trap_sum;
	b->columns[x] -= 1;
	y = b->columns[x];
	color = b->spaces[x][y];
	b->spaces[x][y] = BLANK;
	
	if(b->moves_since_win){
		b->moves_since_win -= 1;
		if(b->moves_since_win == 0){
			b->win = BLANK;
		}
		return;
	}
	
	for(i = 0; i < 13; i++){
		win_id = win_squares[x][y][i];
		if(!win_id){
			break;
		}
		wins[win_id] -= color;
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
			} else if(current_player == BLACK && columns[computer_select] < 6){
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

int negamax(board *b, unsigned char depth, int alpha, int beta, char color, unsigned char move, bool first_call){
	unsigned char i;
	unsigned char j;
	unsigned int index;
	unsigned char x;
	unsigned char prev_white;
	unsigned char prev_black;
	unsigned char inner_best = 3;
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
	clrscr();
	memset(wins, 0, sizeof(char)*70);
	wins[0] = -100;
	memset(killer_moves, 3, sizeof(killer_moves));
	memset(killer_moves2, 2, sizeof(killer_moves2));
	memset(killer_moves3, 4, sizeof(killer_moves3));
	b = create_board();
	kill_engine = false;
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
