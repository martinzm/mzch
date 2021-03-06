/*
 *
 * $Id: search.h,v 1.16.4.6 2006/02/23 20:50:04 mrt Exp $
 *
 */
 
#ifndef SEARCH_H
#define SEARCH_H

#include "bitmap.h"
#include <limits.h>
int AlphaBeta(board *b, int alfa, int beta, int depth, int ply, int side, tree_store * tree, int nulls, attack_model *);

void clearSearchCnt(struct _statistics *s);
void AddSearchCnt(struct _statistics *, struct _statistics *);
void DecSearchCnt(struct _statistics *, struct _statistics *, struct _statistics *);
void CopySearchCnt(struct _statistics *, struct _statistics *);
void printSearchStat(struct _statistics *);
void printSearchStat2(struct _statistics *, char *);
void printPV(tree_store * tree, int depth);
void sprintfPV(tree_store * tree, int depth, char *buff);
int initDBoards();
int IterativeSearch(board *b, int alfa, int beta, const int ply, int depth, int side,int start_depth, tree_store * tree);
int Quiesce(board *b, int, int, int, int, int, tree_store *, int);

#endif
