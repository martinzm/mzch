/*
 *
 * $Id: tests.h,v 1.1.6.3 2006/02/09 20:30:06 mrt Exp $
 *
 */

#ifndef TESTS_H
#define TESTS_H
#include "bitmap.h"

struct _results {
	struct _statistics stats;
	unsigned long long int time;
	int passed;
	int bestscore;
};

//void perft(char * filename, int mix, int max, int sw);
void perft2(char *, int, int, int);
//void perft_def();
void perft2_def(int, int, int);
void testEPD(char * filename);
void timed2Test(char *, int, int, int);
void timed2Test_comp(char *, int , int , int );
void timed2STS(int, int, int, char *, char *);
void movegenTest(char *filename);
void keyTest_def(void);
void timed2_def(int time, int depth, int max);
void timed2_remis(int time, int depth, int max);
void see_test();
void epd_parse(char * filename, char * f2);
void epd_driver(char * filename);

void texel_test();
void timed2Test_x(char *, int, int, int);

int computeMATIdx(board *b);
void fill_test();
void eeval_test();
void pawnEvalTest(char*, int);
#endif
