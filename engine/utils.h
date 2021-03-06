#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include "bitmap.h"
#include <wchar.h>

//#define DEBUG_FILENAME "./"
#define DEBUG_FILENAME "../logs/debug"

#define LOGGER_0(...) logger2(__VA_ARGS__)

#if defined (DEBUG3) || defined (DEBUG2) || defined (DEBUG1) || defined (DEBUG4)
	#define LOGGER_1(...) logger2(__VA_ARGS__)
#else
	#define LOGGER_1(...) {}
#endif

#if defined (DEBUG2) || defined (DEBUG3) || defined (DEBUG4)
	#define LOGGER_2(...) logger2(__VA_ARGS__)
#else
	#define LOGGER_2(...) {}
#endif

#if defined (DEBUG3) || defined (DEBUG4)
	#define LOGGER_3(...) logger2(__VA_ARGS__)
#else
	#define LOGGER_3(...) {}
#endif

#if defined (DEBUG4)
	#define LOGGER_4(...) logger2(__VA_ARGS__)
#else
	#define LOGGER_4(...) {}
#endif

#if defined (DEBUG3) || defined (DEBUG2) || defined (DEBUG1) || defined (DEBUG4)
	#define DEB_1(x) {x;}
#else
	#define DEB_1(x) {}
#endif

#if defined (DEBUG3) || defined (DEBUG2) || defined (DEBUG4)
	#define DEB_2(x) {x;}
#else
	#define DEB_2(x) {}
#endif

#if defined (DEBUG3) || defined (DEBUG4)
	#define DEB_3(x) {x;}
#else
	#define DEB_3(x) {}
#endif

#if defined (DEBUG4)
	#define DEB_4(x) {x;}
#else
	#define DEB_4(x) {}
#endif


#define DBOARDS_LEN 10

typedef struct _debugEntry {
	BITVAR key;
	BITVAR map;
} debugEntry;

extern debugEntry DBOARDS[DBOARDS_LEN+1];

#define DPATHSmaxLen 256
#define DPATHSwidth 20
typedef MOVESTORE _dpaths[DPATHSmaxLen];
extern _dpaths DPATHS[DPATHSwidth+1];

int logger(char *p, char *s,char *a);
int logger2(char *, ...);
int open_log(char *filename);
int close_log(void);
char * tokenizer(char *str, char *delim, char **index);
int indexer(char *str, char *delim, char **index);
int indexof(char **index, char *str);

unsigned long long int readClock(void);

unsigned long long diffClock(struct timespec start, struct timespec end);
int readClock_wall(struct timespec *t);
int readClock_proc(struct timespec *t);
int generate_log_name(char *n, char *pref, char *b);
int parse_cmd_line_check_sec(int argc, char *argv[]);

int initDBoards(debugEntry *);
int validatePATHS(board *b, MOVESTORE *m);
int initDPATHS(board *b, _dpaths *);
int compareDBoards(board *b, debugEntry *h);
int compareDPaths(tree_store *tree, _dpaths *dp, int plylen);
int UTF8toWchar(unsigned char *in, wchar_t *out);
int WchartoUTF8(wchar_t *in, unsigned char *out);

#endif
