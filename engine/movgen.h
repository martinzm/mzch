#ifndef MOVGEN_H
#define MOVGEN_H

#include "bitmap.h"

typedef struct _undo {
	int8_t castle[ER_SIDE];
	int8_t side;
	MOVESTORE move;
	int8_t captured; //what was captured
	int8_t moved; // promoted to in case promotion, otherwise the same as old
	int8_t old; //what was the old piece
	int16_t rule50move;
	int8_t ep;
	int8_t mindex_validity;
	BITVAR key;
	BITVAR pawnkey;
	BITVAR old50key; //what was the old key
	BITVAR old50pos; //what was the old position
} UNDO;

#if 0
#define CHECKFLAG 0x10000U

#define PackMove(from,to,prom,spec)  ((((from)&63) + (((to)&63) << 6) + (((prom)&7) << 12))|((spec)&(CHECKFLAG)))
#define UnPackFrom(a) ((a) & 63)
#define UnPackTo(a) (((a)>>6) & 63)
#define UnPackProm(a) (((a)>>12) & 7)
#define UnPackSpec(a) (((a) & SPECFLAG))
#define UnPackCheck(a) (((a) & CHECKFLAG))
#define UnPackPPos(a) ((a) & 0x0FFF) //only from & to extracted

#else
// uint_16_t
// checkflag 1, from 6, to 6, prom 3
#define CHECKFLAG (0x8000)

#define PackMove(from,to,prom,spec)  ((MOVESTORE)((((from) & 0x3F) | (((to) & 0x3F) <<6) | (((prom) & 7) <<12))|((spec)&CHECKFLAG)))
#define UnPackFrom(a)  ((int) ((a) & 0x3F))
#define UnPackTo(a)    ((int) (((a)>>6) & 0x3F))
#define UnPackProm(a)  ((int) (((a) >>12) & 7))
#define UnPackCheck(a) ((int) ((a) & CHECKFLAG))
#define UnPackPPos(a)  ((MOVESTORE) ((a) & 0xFFF)) //only from & to extracted

#endif

BITVAR isInCheck_Eval(board *b, attack_model *a, int side);
void generateCaptures(board * b, attack_model *a, move_entry ** m, int gen_u);
void generateMoves(board * b, attack_model *a, move_entry ** m);
void generateInCheckMoves(board * b, attack_model *a, move_entry ** m);
void generateQuietCheckMoves(board * b, attack_model *a, move_entry ** m);
int alternateMovGen(board * b, MOVESTORE *filter);
UNDO MakeMove(board *b, MOVESTORE move);
UNDO MakeNullMove(board *b);
void UnMakeMove(board *b, UNDO u);
void UnMakeNullMove(board *b, UNDO u);
int MoveList_Legal(board *b, attack_model *a, int  h, move_entry *n, int count, int ply, int sort);
int sortMoveList_Init(board *b, attack_model *a, int  h,move_entry *n, int count, int ply, int sort);
int sortMoveList_QInit(board *b, attack_model *a, int  h,move_entry *n, int count, int ply, int sort);
int getNSorted(board *b, move_entry *n, int total, int start, int count);
int getQNSorted(board *b, move_entry *n, int total, int start, int count);
int is_quiet_move(board *, attack_model *a, move_entry *m);
int isQuietCheckMove(board * b, attack_model *a, move_entry *m);

void printfMove(board *b, MOVESTORE m);
void sprintfMoveSimple(MOVESTORE m, char *buf);
void sprintfMove(board *b, MOVESTORE m, char * buf);
void printBoardNice(board *b);
void printBoardEval_PSQ(board *b, attack_model *a);
void printBoardEval_MOB(board *b, attack_model *a);
int printScoreExt(attack_model *a);
void log_divider(char *s);

int copyStats(struct _statistics *source, struct _statistics *dest);

void dump_moves(board *b, move_entry * m, int count, int ply, char *cmt);
int compareBoardSilent(board *source, board *dest);
int copyBoard(board *source, board *dest);

int boardCheck(board *b, char *name);
void clearKillers();
int updateKillers(int depth, int move);

int pininit(void);
int pindump(void);

int gradeMoveInRow(board *, attack_model *, MOVESTORE, move_entry *, int);

#endif
