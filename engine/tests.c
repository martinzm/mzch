/*
 *
 * $Id: tests.c,v 1.15.2.11 2006/02/13 23:08:39 mrt Exp $
 *
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
//#include <math.h>
#include "bitmap.h"
#include "generate.h"
#include "attacks.h"
#include "movgen.h"
#include "search.h"
#include "tests.h"
#include "hash.h"
#include "defines.h"
#include "evaluate.h"
#include "utils.h"
#include "ui.h"
#include "openings.h"
#include "globals.h"
#include "search.h"
#include "pers.h"

//#include "search.h"

#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

char *perft_default_tests[]={"8/k1P5/8/1K6/8/8/8/8 w - - 0 1 perft 7 = 567584 ; id X stalemate/checkmate;",
							"r4rk1/pp2ppNp/2pp2p1/6q1/8/P2nPPK1/1P1P2PP/R1B3NR w - - 0 1 perft 1 = 1; jumping over pieces under check;",
							"8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1 perft 5 = 1004658 ; id  X discovered check;",
							"5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1 perft 5 = 1004658 ; id  X discovered check;",
							"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1 perft 4 = 1274206 ; id  X castling (including losing cr due to rook capture);",
							"r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1 perft 4 = 1274206 ; id  X castling (including losing cr due to rook capture);",
							"2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1 perft 6 = 3821001 ; id X promote out of check;",
							"3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1 perft 6 = 3821001 ; id X promote out of check;",
							"8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1 perft 6 = 824064 ; id X avoid illegal en passant capture;",
							"8/8/1k6/8/2pP4/8/5BK1/8 b - d3 0 1 perft 6 = 824064 ; id X avoid illegal en passant capture;",
							"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1 perft 6 = 1440467 ; id X en passant capture checks opponent;",
							"8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1 perft 6 = 1440467 ; id X en passant capture checks opponent;",
							"8/p7/8/1P6/K1k3p1/6P1/7P/8 w - - 0 1 perft 5 = 14062 ; id X numpty 1 l5;",
							"K1k5/8/P7/8/8/8/8/8 w - - 0 1 perft 6 = 2217 ;  id X self stalemate;",
							"8/8/8/8/8/p7/8/k1K5 b - - 0 1 perft 6 = 2217 ;  id X self stalemate;",
							"8/P1k5/K7/8/8/8/8/8 w - - 0 1 perft 6 = 92683 ; id X underpromote to check;",
							"8/8/8/8/8/k7/p1K5/8 b - - 0 1 perft 6 = 92683 ; id X underpromote to check;",
							"4k3/1P6/8/8/8/8/K7/8 w - - 0 1 perft 6 = 217342 ; id X promote to give check;",
							"8/k7/8/8/8/8/1p6/4K3 b - - 0 1 perft 6 = 217342 ; id X promote to give check;",
							"8/8/8/8/1k6/8/K1p5/8 b - - 0 1 perft 7 = 567584 ; id X stalemate/checkmate;",
							"5k2/8/8/8/8/8/8/4K2R w K - 0 1 perft 6 = 661072 ; id  X short castling gives check;",
							"4k2r/8/8/8/8/8/8/5K2 b k - 0 1 perft 6 = 661072 ; id  X short castling gives check;",
							"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1 perft 6 = 803711 ; id  X long castling gives check;",
							"r3k3/8/8/8/8/8/8/3K4 b q - 0 1 perft 6 = 803711 ; id  X long castling gives check;",
							"r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1 perft 4 = 1720476 ; id  X castling prevented;",
							"r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1 perft 4 = 1720476 ; id  X castling prevented;",
							"8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1 perft 4 = 23527 ; id  X double check;",
							"8/5k2/8/5N2/5Q2/2K5/8/8 w - - 0 1 perft 4 = 23527 ; id  X double check;",
							NULL };

char *perft_default_tests2[]={
		"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1 perft 4 = 1274206 ; id  X castling (including losing cr due to rook capture);",
							NULL };

char *remis_default_tests[]={"3k4/4R1R1/8/8/8/K5pp/6r1/7q w - - 0 1",
							 "qq3rk1/ppp1p2p/3p2p1/8/8/3Q4/2Q3PK/8 w - - 0 1",
							 "5k2/8/5P2/5K2/8/8/8/8 w - - 2 3",
							 "4k3/7p/8/8/8/8/6R1/R3K3 w - - 98 55",
							NULL };

char *key_default_tests[]={"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 key = 463b96181691fc9c;",
		"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1 key = 823c9b50fd114196;",
		"rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2 key = 0756b94461c50fb0;",
		"rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2 key = 662fafb965db29d4;",
		"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3 key = 22a48b5a8e47ff78;",
		"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3 key = 652a607ca3f242c1;",
		"rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4 key = 00fdd303c946bdd9;",
		"rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3 0 3 key = 3c8123ea7b067637;",
		"rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq - 0 4 key = 5c3f9b829b279560;", NULL
};


char *timed_default_tests[]={ "8/4PK2/3k4/8/8/8/8/8 w - - 2 13 bm e8=Q; \"xxx\";",
		"8/4k3/8/8/4PK2/8/8/8 w - - 0 1 bm e4; \"test E1x\";",
								NULL
};
attack_model aa[50];

board TESTBOARD;

int triggerBoard(){
	printf("trigger!\n");
return 0;
}

int compareBoard(board *source, board *dest){
int i;
char a1[100];
char a2[100];
char a3[100];
int r;

	if(dest->ep!=source->ep) { printf("EP!\n"); triggerBoard(); }
	if(dest->gamestage!=source->gamestage) { printf("GAMESTAGE!\n"); triggerBoard(); }
	if(dest->key!=source->key) { printf("KEY!\n"); triggerBoard(); }
	if(dest->move!=source->move) { printf("MOVE!\n"); triggerBoard(); }
	if(dest->norm!=source->norm) { printf("NORM!\n"); triggerBoard(); }
	if(dest->r45L!=source->r45L) { printf("r45L!\n"); triggerBoard(); }
	if(dest->r45R!=source->r45R) { printf("r45R!\n"); triggerBoard(); }
	if(dest->r90R!=source->r90R) { printf("r90R!\n"); triggerBoard(); }
	if(dest->rule50move!=source->rule50move) { printf("rule50move!\n"); triggerBoard(); }
	if(dest->side!=source->side) { printf("side!\n"); triggerBoard(); }

	for(i=WHITE;i<ER_SIDE;i++) if(dest->castle[i]!=source->castle[i]) { printf("CASTLE %d!\n",i); triggerBoard(); }
	for(i=0;i<2;i++) if(dest->colormaps[i]!=source->colormaps[i]) { printf("COLORMAPS %d!\n",i); triggerBoard(); }
	for(i=0;i<2;i++) if(dest->king[i]!=source->king[i]) { printf("KING %d!\n",i); triggerBoard(); }
	for(i=0;i<6;i++) if(dest->maps[i]!=source->maps[i]) { printf("MAPS %d!\n",i); triggerBoard(); }
//	for(i=0;i<2;i++) if(dest->mcount[i]!=source->mcount[i]) { printf("MCOUNT %d!\n",i); triggerBoard(); }
	for(i=0;i<64;i++) if(dest->pieces[i]!=source->pieces[i]) { printf("PIECES %d!\n",i); triggerBoard(); }
	for(i=0;i<MAXPLY;i++) if(dest->positions[i]!=source->positions[i]) { printf("POSITIONS %d!\n",i); triggerBoard(); }
	for(i=0;i<MAXPLY;i++) if(dest->posnorm[i]!=source->posnorm[i]) { printf("POSNORM %d!\n",i); triggerBoard(); }
	for(i=0;i<ER_PIECE;i++) if(dest->material[WHITE][i]!=source->material[WHITE][i]) { printf("MATERIAL WHITE %d!\n",i); triggerBoard(); }
	for(i=0;i<ER_PIECE;i++) if(dest->material[BLACK][i]!=source->material[BLACK][i]) { printf("MATERIAL BLACK %d!\n",i); triggerBoard(); }
	if(dest->mindex!=source->mindex) { 
		printf("MIndex!\n"); 
		r=source->mindex-dest->mindex;
		outbinary((BITVAR)source->mindex, a1);
		outbinary((BITVAR)dest->mindex, a2);
		outbinary((BITVAR)r, a3);
		printf("s: %s\nd: %s\nr: %s\n", a1, a2, a3);
		triggerBoard(); 
	}
return 0;
}

/*
 * b contains setup board
 * simple loop
 *
 * - generateMoves
 * 	- store board status
 * 	- make move
 * 	- unmake move
 * 	- check board with stored
 */

int moveGenTest1(board *z){
board *b, work;
UNDO u;
move_entry move[300], *m, *n;
int tc, cc, hashmove;
attack_model att;

	b=&work;
	b->stats=allocate_stats(1);
	b->hs=allocateHashStore(HASHSIZE, 2048);

	copyBoard(z,b);

	m=move;
	att.phase=eval_phase(b, b->pers);
	eval(b, &att, b->pers);
	if(isInCheck(b, b->side)!=0) {
		generateInCheckMoves(b, &att, &m);
	} else {
		generateCaptures(b, &att, &m, 1);
		generateMoves(b, &att, &m);
	}

	n=move;
	hashmove=DRAW_M;
	tc=sortMoveList_Init(b, &att, hashmove, move, (int)(m-n), 1, (int)(m-n) );

	cc = 0;
	while (cc<tc) {
		u=MakeMove(b, move[cc].move);
		if(computeMATIdx(b)!=b->mindex) {
			printf("matindex problem!\n");
			triggerBoard();
		}
		UnMakeMove(b, u);
		compareBoard(z, b);
		cc++;
	}
	freeHashStore(b->hs);
	deallocate_stats(b->stats);
return 0;
}

/*
 * tahy jsou odd�leny mezerou
 * moznosti (whitespace)*(Alnum)+(whitespace)*
			(whitespace)*"(cokoliv krome ")+"(whitespace)*
 */

int getEPDmoves(char *bu, char (*m)[20])
{
	int i, c;
	size_t ap;
	size_t x;
	char *b2;
	(*m)[0]='\0';
	if(bu==NULL) return 0;
	x=strlen(bu);
	while(x>0) {
		while(isspace(*bu)&&(x>0)) {
			bu++;
			x--;
		}
		if(x>0) {
			if(*bu=='\"') {
				b2=strstr(bu, "\"");
				bu++;
				if(b2!=NULL) ap=(size_t)(b2-bu-1); else ap=(size_t)x;
				ap--;
				if(ap>0) {
					strncpy(*m, bu, ap);
					m++;
					i++;
					x-=ap;
					bu+=ap;
				}
			} else {
				c=0;
				while(!isspace(*bu)&&(x>0)) {
					(*m)[c++]=*bu;
					bu++;
					x--;
				}
				(*m)[c++]='\0';
//				printf("%s\n", *m);
				m++;
				i++;
			}
		}
	}
	(*m)[0]='\0';
	return i;
}

int getEPD_str(char *b, char *w,  char *f)
{
char *z1, *z2, *z3, *z4, *zz;
int i1;
	i1=0;
	zz=b;
	while(i1==0) {
		z1=strstr(zz, w); if(z1==NULL) return 0;
		z2=strstr(zz, ";"); if(z2==NULL) z2=z1+strlen(z1);
		z3=strstr(zz, "\""); if(z3==NULL) z3=z1+strlen(z1);
		if((z2<z1) && (z2<z3)) {
			zz=z2+1;
			continue;
		}
		if(z3<z1) {
			z4=strstr(z3+1, "\"");
			if(z4==NULL) return 0;
			zz=z4+1;
			if(zz==NULL) return 0;
			continue;
		}
		i1=1;
	}

	zz=z1+1;
	z1+=strlen(w);

	z2=strstr(zz, ";"); if(z2==NULL) z2=z1+strlen(z1);
	z3=strstr(zz, "\""); if(z3==NULL) z3=z1+strlen(z1);
	if(z3<z2) {
		z4=strstr(z3+1, "\"");
		if(z4!=NULL) {
			zz=z4+1;
			z1=z3+1;
			z2=z4;
		} else {
			z2=z1+strlen(z1);
		}
	}
	while(z2>=z1) {
		z2--;
		if(!isspace(*z2)) break;
	}
	zz=z2+1;
	while(z2>=z1) {
		if(!isspace(*z1)) break;
		z1++;
	}
	zz=z2+1;
	//		zz++;

	strncpy(f, z1, (size_t)(zz-z1));
	f[zz-z1]='\0';
return 1;
}

int get_token(char *st,int first, char *del, int *b, int *e) {
int  f, i, ret;
	size_t dl,l;
	ret=0;
	dl=strlen(del);
	l=strlen(st);
	f=first;
// skip mezery
	while((f<l)&&(isspace(st[f]))) {
		f++;
	}
	*e=*b=f;

	while((f<l)&&(ret<=0)) {
		for(i=0; i<dl; i++) {
			if(st[f]==del[i]) {
				*e=f;
				ret=(*e)-(*b);
				break;
			}
		}
		f++;
	}
	if((f==l)) {
		*e=f;
		ret=(*e)-(*b);
	}
	return ret;
}

/*
 * FEN: position activecolor castlingAvail ENSquare halfmoveclock fullmovenumber
 * EPD: position activecolor castlingAvail ENSquare { operations }
 *
 * operations   opcode {operand | "xx sdjsk";}
 * operandy
 * avoid move	am{ tah}*
 * best move	bm{ tah}*
 * direct move count dm { pocet }
 * id	id string
 * predicted move	pm string
 * perft	perft depth = pocet nodu 
 * material key 	key = HexString
 * Musim udelat rozliseni FEN a EPD !!!
 * dm - direct moves - pocet tahu do matu
 * pv - principal variation
 * cX (c0..c9) - comment. In STS C0 contains solutions with score for it, for example  c0 "f5=10, Be5+=2, Bf2=3, Bg4=2";
 */

int parseEPD(char * buffer, char FEN[100], char (*am)[20], char (*bm)[20], char (*pv)[20], char (*cm)[20], int *matec, char **name)
{
char * an, *endp;
char b[256], token[256], comment[256];
int count;
int f;
int s,e,i;
size_t l;

// get FEN
//			printf("buffer: %s\n", buffer);

			count=3;
//			st=0;
			if(!isalnum(buffer[0])) return 0;
			e=0;
			while(count>0) {
				s=e;
				l=get_token(buffer,s," ",&s, &e);
				if(l==0) break;
				count--;
			}
			if(count !=0) {
				printf(" FEN error %d!\n", count);
				printf("%s\n", buffer+e);
				return 0;
			}
			count=1;
			while(count>0) {
				s=e;
				l=get_token(buffer,s," ;",&s, &e);
				if(l==0) break;
				count--;
			}
			if(count !=0) {
				printf(" FEN error %d!\n", count);
				printf("%s\n", buffer+e);
				return 0;
			}
// auto/detect epd / fen 
// pokud jsou nasledujici dve pole pouze cisla pak je budeme povazovat za halfmoves a moves
			f=s=e;
			i=0;
			l=get_token(buffer,s," ;",&s, &e);
			if(l>0) {
				strncpy(token, buffer+s, l);
// mozna zakoncit string
				token[l]='\0';
				strtol(token, &endp, 10);
				if(*endp=='\0') {
					s=e;
					l=get_token(buffer,s," ;",&s, &e);
					if(l>0) {
						strncpy(token, buffer+s, l);
						token[l]='\0';
						strtol(token, &endp, 10);
						if(*endp=='\0') {
							i=1;
							f=e;
						}
					}
				}
			}

			strncpy(FEN,buffer, (size_t)f);
			FEN[f]='\0';
			f++;
			if(i==0) {
// hack			
				strcat(FEN," 0 1");
				f--;
			}
			
//hledam id
// an zacatek bufferu
// an2 zacatek hledaneho retezce
// an3 konec pole
// an4 rozdeleni na radek
			an=buffer+f;

			*name=(char *)malloc(256);
			(*name)[0]='\0';
			if(getEPD_str(an, "id ", b)) {
//				*name=(char *) malloc(strlen(b)+256);
				strcpy(*name, b);
			}
//			else {
//				*name=(char *)malloc(256);
//				(*name)[0]='\0';
//			}

			if(am!=NULL) {
				am[0][0]='\0';
				if(getEPD_str(an, "am ", b)) {
					getEPDmoves(b, am);
				}
			}

			if(bm!=NULL) {
				bm[0][0]='\0';
				if(getEPD_str(an, "bm ", b)) {
					getEPDmoves(b, bm);
				}
			}

			if(matec!=NULL) {
				*matec=-1;
				if(getEPD_str(an, "dm ", b)) {
					*matec= atoi(b);
				}
			}

			if(pv!=NULL) {
				pv[0][0]='\0';
				if(getEPD_str(an, "pv ", b)) {
					getEPDmoves(b, pv);
				}
			}

			if(cm!=NULL) {
				cm[0][0]='\0';
				if(getEPD_str(an, "c0 ", b)) {
					getEPDmoves(b, cm);
				}
			}
return 1;
}

int getKeyFEN(char FEN[100], BITVAR *key){
int ret;

	char * pst;
	ret=0;
	pst=strstr(FEN,"key");
	if(pst!=NULL){
		sscanf(pst, "key = %llx", (long long unsigned *) key);
		ret=1;
	}
	return ret;
}

int getPerft(char FEN[100], int *depth, unsigned long long *moves){
int ret;

	char * pst;
	ret=0;

	pst=strstr(FEN,"perft");
	if(pst!=NULL){
		sscanf(pst, "%*s %d = %llu", depth, moves);
		ret=1;
	}
	return ret;
}

int matchLine(int *pv, tree_store *t){
int f;
	if(pv==NULL) return 1;
	if(pv[0]==0) return 1;
	for(f=1;f<=pv[0];f++){
		if(pv[f]!=t->tree[0][f-1].move) return 0;
	}
	return 1;
}

int evaluateAnswer(board *b, int ans, int adm ,MOVESTORE *aans, MOVESTORE *bans, int *pv, int dm, tree_store *t){
	int as, ad, ap, src, des, p, res, prom_need, ba;

	as=UnPackFrom(ans);
	ad=UnPackTo(ans);
	ap=UnPackProm(ans);
	//	asp=UnPackSpec(ans);
	prom_need=0;
	if((b->side==WHITE) && (ad>=A8) && (b->pieces[as]==PAWN)) prom_need=1;
	else if((b->side==BLACK) && (ad>=H1) && (b->pieces[as]==PAWN)) prom_need=1;

	ba=res=0;
// the move must be in bans - best moves, if bans is populated
	if(bans!=NULL) {
		while(*bans!=NA_MOVE) {
			src=UnPackFrom(*bans);
			des=UnPackTo(*bans);
			p=UnPackProm(*bans);
			ba++;
			if((src==as)&&(des==ad)) {
				if((prom_need!=0)) {
					if (ap==p) res=1;
				} else res=1;
			}
			bans++;
		}
	}
	if(ba==0) res=1;
// the move must NOT be in aans - avoid moves, if aans is populated
	if(aans!=NULL) {
		while(*aans!=NA_MOVE) {
			src=UnPackFrom(*aans);
			des=UnPackTo(*aans);
			p=UnPackProm(*aans);
			if((src==as)&&(des==ad)) {
				if((prom_need!=0)) {
					if (ap==p) res=0;
				} else res=0;
			}
			aans++;
		}
	}
// if DM available, the solution should be that far
	if(dm>0) {
//get full moves from adm
		if(adm!=dm) res=0;
	}

// if PV available, then the PV of the result should be the same
	if(pv!=NULL) if(!matchLine(pv,t)) res=0;
	return res;
}

int evaluateStsAnswer(board *b, int ans, MOVESTORE *bans, MOVESTORE *cans, int *val){
int as, ad, ap, src, des, p, res, prom_need, ba;
int i;

	res=0;
	as=UnPackFrom(ans);
	ad=UnPackTo(ans);
	ap=UnPackProm(ans);
	prom_need=0;
	if((b->side==WHITE) && (ad>=A8) && (b->pieces[as]==PAWN)) prom_need=1;
	else if((b->side==BLACK) && (ad>=H1) && (b->pieces[as]==PAWN)) prom_need=1;

	ba=i=-1;
	if(*cans==NA_MOVE) {
		while(*bans!=NA_MOVE) {
			src=UnPackFrom(*bans);
			des=UnPackTo(*bans);
			p=UnPackProm(*bans);
			ba++;
			if((src==as)&&(des==ad)) {
				if((prom_need!=0)) {
					if (ap==p) i=ba;
				} else i=ba;
			} 
			bans++;
		}
		if(i!=-1) res=10;
	} else {
		while(*cans!=NA_MOVE) {
			src=UnPackFrom(*cans);
			des=UnPackTo(*cans);
			p=UnPackProm(*cans);
			ba++;
			if((src==as)&&(des==ad)) {
				if((prom_need!=0)) {
					if (ap==p) i=ba;
				} else i=ba;
			} 
			cans++;
		}
		if(i!=-1) res=val[i];
	}
	return res;
}

//move
//  [Piece][Zdroj][X]DestP[=?|e.p.][+]
// 0-0 0-0-0

MOVESTORE parseOneMove(board *b, char *m)
{
int r,c,p, pp, sr, sf, sp, des, ep_t, p_pole, src, prom_need, cap;
BITVAR aa, xx, bb, x2;
MOVESTORE mm[2];
int zl, sl, l, ll, tl;

MOVESTORE res=NA_MOVE;

		cap=0;
		sr=sf=-1;
		p=sp=ER_PIECE;
		prom_need=0;

		if(strstr(m, "O-O-O")!=NULL) {
			sp=KING;
			p=KING;
			aa=b->maps[KING] & b->colormaps[b->side];
//			prom_need=1;
			sf=4;
			c=2;
			if(b->side==WHITE) {
				sr=0;
				r=0;
			} else {
				sr=7;
				r=7;
			}
			des=r*8+c;
			goto POKR;
		}
		if(strstr(m, "O-O")!=NULL) {
			sp=KING;
			p=KING;
			aa=b->maps[KING] & b->colormaps[b->side];
//			prom_need=1;
			sf=4;
			c=6;
			if(b->side==WHITE) {
				sr=0;
				r=0;
			} else {
				sr=7;
				r=7;
				//break
			}
			des=r*8+c;
			goto POKR;
		}
		//sync on destination, ll je delka, l ma ukazovat na zacatek dest pole, tl na pridavne info, zl je figura a pocatecni pole
		ll=l=(int)strlen(m);
		if(l<2) return NA_MOVE;
		for(;l>0;l--) {
			if(isdigit(m[l])) break;
		}
		if(l==0) return NA_MOVE;
		l--;
		c=toupper(m[l])-'A';
		r=m[l+1]-'1';

		if(b->side==WHITE) {
			if(r==7) prom_need=1;
		} else if(r==0) prom_need=1;

		tl=l+2;
		if(tl>=ll) goto ZTAH;
		if(m[tl]=='=') {
			// promotion
			tl++;
			pp=toupper(m[tl]);
			switch(pp) {
			case 'Q' : p=QUEEN;
			break;
			case 'R' : p=ROOK;
			break;
			case 'B' : p=BISHOP;
			break;
			case 'N' : p=KNIGHT;
			break;
			default:
					return NA_MOVE;
				break;
			}
			tl++;
		}
		if(tl>=ll) goto ZTAH;

		if(strstr(m+tl, "e.p.")!=NULL) {
			tl+=4;
		}
		if(tl>=ll) goto ZTAH;

		if(m[tl]=='+') {
			tl++;
		}
		if(m[tl]=='#') {
			tl++;
		}
ZTAH:
        sl=0;
		zl=l-1;
		sp=PAWN;
		if(zl<0) goto ETAH;
		if(m[zl]=='x') {
			cap=1;
			zl--;
		}
		if(zl<0) goto ETAH;
		pp=m[sl];
		if(isupper(pp)) {
			switch(pp) {
			case 'Q' : sp=QUEEN;
     				   sl++;
     				   break;
			case 'R' : sp=ROOK;
			   	   	   sl++;
			   	   	   break;
			case 'B' : sp=BISHOP;
				       sl++;
				       break;
			case 'N' : sp=KNIGHT;
          			   sl++;
          			   break;
			case 'K' : sp=KING;
					   sl++;
					   break;
			case 'P' : sp=PAWN;
					   sl++;
			break;
			default:
				sp=PAWN;
				break;
			}
		}
		if(zl>=sl) {
			if(isdigit(m[zl])) {
				sr=m[zl]-'1';
				zl--;
			}
		}
		if(zl>=sl) {
			if(isalpha(m[zl])) {
				sf=tolower(m[zl])-'a';
				zl--;
			}
		}
ETAH:

/*
 * mame limitovanou informaci, zdali jde o tah, brani, jakou figurou a mozna odkud
 * je na case najit pozici figurky
 */
// kontrola
		des=r*8+c;
// ep test
		xx=0;
		if(sp==PAWN) {
			if(cap!=0)	{
				if(b->side==WHITE) ep_t=b->ep+8;
				else ep_t=b->ep-8;
				if((b->ep!=-1) && (des==ep_t)) {
					xx=(attack.ep_mask[b->ep]) & (b->maps[PAWN]) & (b->colormaps[b->side]);
				//						prom_need=1;
					p=PAWN;
				}
			} else {
				if(b->side==WHITE) p_pole=des-8;
				else p_pole=des+8;
				x2 = normmark[p_pole];
				xx = xx | x2;
				if((b->side==WHITE && r==3) && ((x2 & b->norm)==0)) xx = xx | normmark[p_pole-8];
				else if((b->side==BLACK && r==4)&& ((x2 & b->norm)==0)) xx = xx | normmark[p_pole+8];
			}
		}
		//ostatni test
		//		printBoardNice(b);
		// krome pescu ostatni figury utoci stejne jako se pohybuji, odstranime pesce neni li to brani
		aa=AttackedTo(b, des);
		if(cap!=0) {
			aa|=xx;
		} else {
			aa&=(~(b->maps[PAWN] & b->colormaps[b->side]));
			aa|=xx;
		}
		bb=b->maps[sp];
		aa&=bb;
		bb=b->colormaps[b->side];
		aa&=bb;
		if(sr!=-1) aa=aa& attack.rank[sr*8];
		if(sf!=-1) aa=aa& attack.file[sf];
POKR:

		if(BitCount(aa)!=1) {
			LOGGER_3("EPDmove parse problem, bitcount %d!\n", BitCount(aa));
		} else {
//			if(prom_need==1 && p==PAWN) p=QUEEN;
			src=LastOne(aa);
			res = PackMove(src, des,  p, 0);
			mm[0]=1;
			mm[1]=res;
			if(validatePATHS(b, &(mm[0]))!=1) res=NA_MOVE;
			else res=mm[1];
		}
return res;
}

int parseEDPMoves(board *b, MOVESTORE *ans,  char (*bm)[20])
{
	char b2[256];
	while((*bm)[0]!='\0') {
		*ans=parseOneMove(b, *bm);
		if(*ans!=NA_MOVE) {
			DEB_3(sprintfMove(b, *ans, b2));
			LOGGER_3("Move A/B: %s\n",b2);
			ans++;
			}
		bm++;
	}
	*ans=NA_MOVE;
return 1;
}

int parseCommentMoves(board *b, MOVESTORE *ans, int *val, char (*bm)[20])
{
char b2[256], m[256], v[256];
size_t i;
char *p, *q;
	while((*bm)[0]!='\0') {
		p=strstr(*bm,"=");
		if(p!=NULL) {
			i=(size_t)(p-(*bm));
			strncpy(m,*bm, i);
			m[i]='\0';
			p++;
			strcpy(v,p);
			q=v+strlen(v)-1;
			while((q>=v)&&(isdigit(*q)==0)) {
				q--;
			}
			q++;
			*q='\0';
			*ans=parseOneMove(b, m);

			*val=atoi(v);
			if(*ans!=NA_MOVE) {
				DEB_3(sprintfMove(b, *ans, b2));
				LOGGER_3("Move C: %s, %d\n",b2, *val);
				ans++;
				val++;
			}
		}
		bm++;
	}
	*ans=NA_MOVE;
return 1;
}

int parsePVMoves(board *b, int *ans, char (*bm)[20])
{
UNDO u[256]	;
attack_model att;
MOVESTORE mm[2];
int f,i,r, *z;
	char b2[256];
//	printBoardNice(b);

	z=ans;
	ans++;
	f=1;
	mm[1]=0;
	while((*bm)[0]!='\0') {
		mm[0]=parseOneMove(b, *bm);
		if(mm[0]!=NA_MOVE) {
			DEB_1(sprintfMove(b, mm[0], b2));
			LOGGER_1("Move PV: %s\n",b2);
			i=alternateMovGen(b, mm);
			if(i!=1) {
				LOGGER_2("INFO3: move problem!\n");
				break;
			}
			eval(b, &att, b->pers);
			u[f]=MakeMove(b, mm[0]);
//			printBoardNice(b);
			f++;
			*ans=mm[0];
			ans++;
		}
		bm++;
	}
	*ans=NA_MOVE;
	f--;
	*z=f;

	for(;f>0;f--) {
	 UnMakeMove(b, u[f]);
	}

//	printBoardNice(b);
	r=1;
return r;
}


void movegenTest(char *filename)
{
	char buffer[512], fen[100];
	char am[10][20];
	char bm[10][20];
	char cm[10][20];
	char pm[256][20];
	int dm;
	FILE * handle;
	int i,x;
	board b;
	char * name;
	char * xx;
	struct _ui_opt uci_options;

	b.uci_options=&uci_options;
	b.stats=allocate_stats(1);
	b.hs=allocateHashStore(HASHSIZE, 2048);

			if((handle=fopen(filename, "r"))==NULL) {
				printf("File %s is missing\n",filename);
				return;
			}
			xx=fgets(buffer, 511, handle);
			i=0;

			b.pers=(personality *) init_personality("pers.xml");

			while(!feof(handle)) {
				if(parseEPD(buffer, fen, am, bm, pm, cm, &dm, &name)==1) {
					setup_FEN_board(&b, fen);
					printBoardNice(&b);
					printf("----- MoveGenTest, name:%s -----\n",name);
					DEB_4(boardCheck(&b));
					moveGenTest1(&b);
					free(name);
				}
				i++;
				xx=fgets(buffer, 511, handle);
			}
			free(b.pers);
			fclose(handle);
			freeHashStore(b.hs);
			deallocate_stats(b.stats);
}

/*
 * b contains setup board
 * simple loop
 *
 * - generateMoves
 * 	- store board status
 * 	- make move
 * 	- unmake move
 * 	- check board with stored
 */

unsigned long long int perftLoop(board *b, int d, int side){
UNDO u;
move_entry move[300], *m, *n;
int opside, incheck;
unsigned int tc, cc;
unsigned long long nodes, tnodes;
attack_model *a, ATT;


	if (d==0) return 1;

	nodes=0;
	opside = (side == WHITE) ? BLACK : WHITE;
//	a=&(aa[d]);
	a=&ATT;

//	a->phase=eval_phase(b);
//	eval(b, a, b->pers);
	a->phase=eval_phase(b, b->pers);
	eval_king_checks_all(b, a);
	simple_pre_movegen(b, a, b->side);
	simple_pre_movegen(b, a, opside);

	if(isInCheck_Eval(b, a, opside)!=0) {
		log_divider("OPSIDE in check!");
		printBoardNice(b);
		printboard(b);
		printf("Opside in check!\n");
		return 0;
	}
	if(isInCheck_Eval(b, a, side)!=0) {
		incheck=1;
	}	else incheck=0;

	m=move;
	n=move;
	if(incheck==1) {
		generateInCheckMoves(b, a, &m);
	} else {
		generateCaptures(b, a, &m, 1);
		generateMoves(b, a, &m);
	}

//	hashmove=DRAW_M;
//	tc=sortMoveList_Init(b, a, hashmove, move, m-n, d, m-n );
	tc=(unsigned int)(m-n);
//	printBoardNice(b);
//	dump_moves(b, move, m-n );
	cc = 0;
	if(d==1) return tc;

	while (cc<tc) {
//		readClock_wall(&start);
//		sprintfMove(b, move[cc].move, buf);
//		sprintfMoveSimple(move[cc].move, buf);
		u=MakeMove(b, move[cc].move);
//		writeEPD_FEN(b, fen, 0,"");
		tnodes=perftLoop(b, d-1, opside);
		nodes+=tnodes;
		UnMakeMove(b, u);
		cc++;
	}
return nodes;
}


unsigned long long int perftLoop_divide(board *b, int d, int side){
UNDO u;
move_entry move[300], *m, *n;
int tc, cc, opside, incheck;
unsigned long long nodes, tnodes;
attack_model *a, ATT;
struct timespec start, end;
unsigned long long int totaltime;
char buf[20], fen[100];

	if (d==0) return 1;

	nodes=0;
	opside = (side == WHITE) ? BLACK : WHITE;
//	a=&(aa[d]);
	a=&ATT;

//	a->phase=eval_phase(b);
//	eval(b, a, b->pers);
	a->phase=eval_phase(b, b->pers);
	eval_king_checks_all(b, a);
	simple_pre_movegen(b, a, b->side);
	simple_pre_movegen(b, a, opside);

	if(isInCheck_Eval(b, a, opside)!=0) {
		log_divider("OPSIDE in check!");
		printBoardNice(b);
		printboard(b);
		printf("Opside in check!\n");
		return 0;
	}
	if(isInCheck_Eval(b, a, side)!=0) {
		incheck=1;
	}	else incheck=0;

	m=move;
	n=move;
	if(incheck==1) {
		generateInCheckMoves(b, a, &m);
	} else {
		generateCaptures(b, a, &m, 1);
		generateMoves(b, a, &m);
	}

//	hashmove=DRAW_M;
//	tc=sortMoveList_Init(b, a, hashmove, move, m-n, d, m-n );
	tc=(int)(m-n);
//	printBoardNice(b);
//	dump_moves(b, move, m-n );
	cc = 0;
//	if(d==1) return tc;

	while (cc<tc) {
		readClock_wall(&start);
//		sprintfMove(b, move[cc].move, buf);
		sprintfMoveSimple(move[cc].move, buf);
		u=MakeMove(b, move[cc].move);
		writeEPD_FEN(b, fen, 0,"");
		tnodes=perftLoop(b, d-1, opside);
		nodes+=tnodes;
		UnMakeMove(b, u);
		readClock_wall(&end);
		totaltime=diffClock(start, end)+1;
		printf("%s\t\t%lld\t\t(%lld:%lld.%lld\t%lld tis/sec,\t\t%s)\n", buf, tnodes, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, tnodes*1000/totaltime, fen );
		LOGGER_1("%s\t\t%lld\t\t(%lld:%lld.%lld\t%lld tis/sec,\t\t%s)\n", buf, tnodes, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, tnodes*1000/totaltime, fen );
		cc++;
	}
return nodes;
}

unsigned long long int perftLoop_divide_N(board *b, int d, int side){
UNDO u;
move_entry move[300], *m, *n;
int opside, incheck;
MOVESTORE hashmove;
unsigned int tc, cc;
unsigned long long nodes, tnodes;
attack_model *a, ATT;
struct timespec start, end;
unsigned long long int totaltime;
char buf[20], fen[100];

	if (d==0) return 1;

	nodes=0;
	opside = (side == WHITE) ? BLACK : WHITE;
//	a=&(aa[d]);
	a=&ATT;

	a->phase=eval_phase(b, b->pers);
//	eval(b, a, b->pers);

	eval_king_checks_all(b, a);
	if(isInCheck_Eval(b, a, opside)!=0) {
		log_divider("OPSIDE in check!");
		printBoardNice(b);
		printboard(b);
		printf("Opside in check!\n");
		return 0;
	}
	if(isInCheck_Eval(b, a, side)!=0) {
		incheck=1;
	}	else incheck=0;

	simple_pre_movegen(b, a, b->side);
	simple_pre_movegen(b, a, opside);
//	eval(b, a, b->pers);

	
	m=move;
	n=move;
	if(incheck==1) {
		generateInCheckMoves(b, a, &m);
	} else {
		generateCaptures(b, a, &m, 1);
		generateMoves(b, a, &m);
	}

	hashmove=DRAW_M;
	tc=MoveList_Legal(b, a, hashmove, move, (int)(m-n), d, (int)(m-n) );
//	printBoardNice(b);
//	dump_moves(b, move, m-n );
	cc = 0;
	if(d==1) return tc;

	while (cc<tc) {
		readClock_wall(&start);
//		sprintfMove(b, move[cc].move, buf);
		sprintfMoveSimple(move[cc].move, buf);
		u=MakeMove(b, move[cc].move);
		writeEPD_FEN(b, fen, 0,"");
		tnodes=perftLoop(b, d-1, opside);
		nodes+=tnodes;
		UnMakeMove(b, u);
		readClock_wall(&end);
		totaltime=diffClock(start, end);
		printf("%s\t\t%lld\t\t(%lld:%lld.%lld\t%lld tis/sec,\t\t%s)\n", buf, tnodes, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, tnodes*1000/totaltime, fen );
		LOGGER_1("%s\t\t%lld\t\t(%lld:%lld.%lld\t%lld tis/sec,\t\t%s)\n", buf, tnodes, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, tnodes*1000/totaltime, fen );
		cc++;
	}
return nodes;
}

// callback funkce
#define CBACK int (*cback)(char *fen, void *data)

void perft_driver(int min, int max, int sw, CBACK, void *cdata)
{
char buffer[512], fen[100];
int i, depth;
board b;
unsigned long long int nodes, counted;
char * name;
struct timespec start, end, st, et;
unsigned long long int totaltime, nds;

unsigned long long int (*loop)(board *b, int d, int side);

struct _ui_opt uci_options;

	b.uci_options=&uci_options;
	b.stats=allocate_stats(1);
	b.hs=allocateHashStore(HASHSIZE, 2048);


// normal mode
		switch(sw) {
			case 1: loop=&perftLoop_divide;
					break;
			case 2: loop=&perftLoop_divide_N;
					break;
			default:
					loop=&perftLoop;
		}
		b.pers=(personality *) init_personality("pers.xml");		
		nds=0;
		i=1;
		readClock_wall(&st);
		while(cback(buffer,cdata)) {
			if(parseEPD(buffer, fen, NULL, NULL, NULL, NULL, NULL, &name)>0) {
				if(getPerft(buffer,&depth,&nodes)==1) {
					setup_FEN_board(&b, fen);

					LOGGER_1("----- Evaluate:%d Begin, Depth:%d, Nodes Exp:%llu; %s -----\n",i, depth, nodes, name);
					printf("----- Evaluate:%d Begin, Depth:%d, Nodes Exp:%llu; %s -----\n",i, depth, nodes, name);
					readClock_wall(&start);
					if((min<=depth)&&(depth<=max)) {
						counted=loop(&b, depth, b.side);
						readClock_wall(&end);
						totaltime=diffClock(start, end)+1;
						printf("----- Evaluate:%d -END-, Depth:%d, Nodes Cnt:%llu, Time: %lld:%lld.%lld; %lld tis/sec,  %s -----\n",i, depth, counted,totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, (counted*1000/totaltime), name);
						LOGGER_1("----- Evaluate:%d -END-, Depth:%d, Nodes Cnt:%llu, Time: %lld:%lld.%lld; %lld tis/sec,  %s -----\n",i, depth, counted,totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, (counted*1000/totaltime), name);
						nds+=counted;
						if(nodes!=counted){
							printf("Not Match!\n");
							LOGGER_1("NOT MATCH!\n");
							printBoardNice(&b);
						}
					} else {
						printf("----- Evaluate:%d -END-, SKIPPED -----\n",i);
						LOGGER_1("----- Evaluate:%d -END-, SKIPPED -----\n",i);
					}
					free(name);
				}
			}
			i++;
		}
		readClock_wall(&et);
		totaltime=diffClock(st, et)+1;
		printf("Nodes: %llu, Time: %lldm:%llds.%lld; %lld tis/sec\n",nds, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, (nds*1000/totaltime));
		LOGGER_1("Nodes: %llu, Time: %lldm:%llds.%lld; %lld tis/sec\n",nds, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000, (nds*1000/totaltime));
		deallocate_stats(b.stats);
		freeHashStore(b.hs);
		free(b.pers);
}

int perft2_def_cback(char *fen, void *data){
int *i;
	i = (int *)data ;
	if(perft_default_tests[*i]!=NULL) {
		strcpy(fen, perft_default_tests[*i]);
		(*i)++;
		return 1;
	}
	return 0;
}

void perft2_def(int min, int max, int sw){
int i=0;
	perft_driver(min, max, sw, perft2_def_cback, &i);
}

typedef struct __perft2_cb_data {
	FILE * handle;
	int i;
} perft2_cb_data;

int perft2_cback(char *fen, void *data){
char buffer[512];
int x;
char *xx;
perft2_cb_data *i;
	i = (perft2_cb_data *)data ;
	if(!feof(i->handle)) {
		xx=fgets(buffer, 511, i->handle);
		strcpy(fen, buffer);
		return 1;
	}
	return 0;
}

typedef struct {
	FILE * handle;
	int i;
	int n;
} sts_cb_data;

int sts_cback(char *fen, void *data){
char buffer[512];
int x;
char *xx;
sts_cb_data *i;
	i = (sts_cb_data *)data ;
	
	while(!feof(i->handle)) {
		xx=fgets(buffer, 511, i->handle);
		strcpy(fen, buffer);
		i->n++;
		if((i->n<=100)) {
			return 1;
		}
	}
	return 0;
}

void perft2(char * filename, int min, int max, int sw){
perft2_cb_data cb;
	if((cb.handle=fopen(filename, "r"))==NULL) {
		printf("File %s is missing\n",filename);
		return;
	}
	perft_driver(min, max, sw, perft2_cback, &cb);
	fclose(cb.handle);
}

int timed2_def_cback(char *fen, void *data){
int *i;
	i = (int *)data ;
	if(timed_default_tests[*i]!=NULL) {
		strcpy(fen, timed_default_tests[*i]);
		(*i)++;
		return 1;
	}
	return 0;
}

int timed2_remis_cback(char *fen, void *data){
int *i;
	i = (int *)data ;
	if(remis_default_tests[*i]!=NULL) {
		strcpy(fen, remis_default_tests[*i]);
		(*i)++;
		return 1;
	}
	return 0;
}

int timed_driver(int t, int d, int max,personality *pers_init, int sts_mode, struct _results *results, CBACK, void *cdata)
{
	char buffer[512], fen[100], b2[1024], b3[1024], b4[512];
	char bx[512];
	char am[10][20];
	char bm[10][20];
	char cc[10][20], (*cm)[20];
	int v[10];
	char pm[256][20];
	char (*x)[20];
	MOVESTORE bans[20], aans[20], cans[20];
	int dm, adm;
	int pv[256];
	int i, time, depth, f, n;
	board b;
	int val, error, passed, res_val;
	unsigned long long starttime, endtime, ttt;
	struct _statistics s;
	struct _ui_opt uci_options;
	struct _statistics *stat;

	char * name;
	tree_store * moves;
	// normal mode
	cm=NULL;
	if(sts_mode!=0) cm=cc;
	// cm = cc;
	passed=error=res_val=0;
	moves = (tree_store *) malloc(sizeof(tree_store));
	b.stats=allocate_stats(1);
	b.pers=pers_init;
	b.hs=allocateHashStore(HASHSIZE, 2048);
	b.uci_options=&uci_options;

	stat = allocate_stats(1);
	moves->tree_board.stats=stat;

// personality should be provided by caller
	i=0;
	clearSearchCnt(&s);
	while(cback(bx, cdata)&&(i<max)) {
		if(parseEPD(bx, fen, am, bm, pm, cm, &dm, &name)>0) {

			time=t;
			depth=d;
			setup_FEN_board(&b, fen);
			DEB_3(printBoardNice(&b);)
			parseEDPMoves(&b,bans, bm);
			parseEDPMoves(&b,aans, am);
			parsePVMoves(&b, pv, pm);
			if(sts_mode!=0) parseCommentMoves(&b, cans, v, cm);

			//setup limits
			b.uci_options->engine_verbose=1;
			b.uci_options->binc=0;
			b.uci_options->btime=0;
			b.uci_options->depth=depth;
			b.uci_options->infinite=0;
			b.uci_options->mate=0;
			b.uci_options->movestogo=1;
			b.uci_options->movetime=0;
			b.uci_options->ponder=0;
			b.uci_options->winc=0;
			b.uci_options->wtime=0;
			b.uci_options->search_moves[0]=0;

			b.uci_options->nodes=0;
			b.uci_options->movetime=time-100;

			b.run.time_move=b.uci_options->movetime;
			b.run.time_crit=b.uci_options->movetime;

			engine_stop=0;
			clear_killer_moves();
			initHash(b.hs);
//			invalidateHash();
			clearSearchCnt(b.stats);

			starttime=readClock();
			b.run.time_start=starttime;

			val=IterativeSearch(&b, 0-iINFINITY, iINFINITY, 0, b.uci_options->depth, b.side, 0, moves);

			endtime=readClock();
			ttt=endtime-starttime;
			results[i].bestscore=val;
			results[i].time=ttt;
			results[i].passed=1;
//			(printSearchStat(b.stats));
			(LOGGER_1("TIMESTAMP: Start: %llu, Stop: %llu, Diff: %lld milisecs\n", b.run.time_start, endtime, (endtime-b.run.time_start)));
			CopySearchCnt(&(results[i].stats), b.stats);
			AddSearchCnt(&s, b.stats);
			printPV(moves, 999);
			printPV(moves, 999);
			sprintfMove(&b, b.bestmove, buffer);

			if(isMATE(b.bestscore))  {
				int ply=GetMATEDist(b.bestscore);
				if(ply==0) adm=1;
				else {
					adm= (b.side==WHITE ? (ply+1)/2 : (ply/2)+1);
				}
			} else adm=-1;
			// ignore exact PV
			val=0;
			if(sts_mode!=0) {
				val=evaluateStsAnswer(&b, b.bestmove, bans, cans, v);
				results[i].passed=val;
			} else {
				val=evaluateAnswer(&b, b.bestmove, adm , aans, bans, NULL, adm, moves);
			}
//			val=evaluateAnswer(&b, b.bestmove, adm , aans, bans, pv, dm, moves);
			if(val<=0) {
				results[i].passed=0;
				sprintf(b2, "Error: %s %d, proper:",buffer, val);
				error++;
				if((*bm)[0]!=0) {
					sprintf(b4,"BM ");
					x=bm;
					while((*x)[0]!=0) {
						strcat(b4, (*x));
						strcat(b4," ");
						x++;
					}
					strcat(b2, b4);
				}
				if((*am)[0]!=0) {
					sprintf(b4,"AM ");
					x=am;
					while((*x)[0]!=0) {
						strcat(b4, (*x));
						strcat(b4," ");
						x++;
					}
					strcat(b2, b4);
				}
				if(dm>=0) {
					sprintf(b4, "DM %i", dm);
					strcat(b2, b4);
				}
			}
			else {
				sprintf(b2, "Passed, Move: %s, toMate: %i", buffer, adm);
				passed++;
				res_val+=val;
			}
			sprintf(b3, "Position %d, name:%s, %s, Time: %dh, %dm, %ds,, %lld\n",i,name, b2, (int) ttt/3600000, (int) (ttt%3600000)/60000, (int) (ttt%60000)/1000, ttt);
//			logger2(b3);
			tell_to_engine(b3);
			free(name);
			i++;
		}
//		i++;
		//				break;
	}

	CopySearchCnt(&(results[i].stats), &s);
	freeHashStore(b.hs);
	deallocate_stats(stat);
	deallocate_stats(b.stats);
	free(moves);
	if(sts_mode!=0) sprintf(b3, "Positions Total %d, Passed %d with total Value %d, Error %d\n",passed+error, passed, res_val, error);
	else sprintf(b3, "Positions Total %d, Passed %d, Error %d\n",passed+error, passed, error);
	tell_to_engine(b3);
	return i;
}

int timed_driver_eval(int t, int d, int max,personality *pers_init, int sts_mode, struct _results *results, CBACK, void *cdata)
{
	char buffer[512], fen[100], b3[1024], b4[512];
	char bx[512];
	char cc[10][20];
	MOVESTORE cans[20];
	int i, time, depth, f, n;
	board b;
	int val, error, passed, res_val;
	struct _statistics *stat;

	attack_model a;
	struct _ui_opt uci_options;
	struct _statistics s;
	int ev, ph, ev2;

	char * name;
	b.stats=allocate_stats(1);
	b.pers=pers_init;
	b.hs=allocateHashStore(HASHSIZE, 2048);
	b.uci_options=&uci_options;

	stat = allocate_stats(1);

	i=0;

// personality should be provided by caller
	i=0;
	while(cback(bx, cdata)&&(i<max)) {
		if(parseEPD(bx, fen, NULL, NULL, NULL, NULL, NULL, &name)>0) {

			time=t;
			depth=d;

			setup_FEN_board(&b, fen);
			DEB_3(printBoardNice(&b);)

			ph= eval_phase(&b, pers_init);
			ev=eval(&b, &a, pers_init);

			if((i%1000)==0) printf("Records %d\n",i);
			b.side = (b.side==WHITE) ? BLACK : WHITE;
			ev2=eval(&b, &a, pers_init);

			results[i].bestscore=ev;
			results[i].passed=ev-ev2;
			if(ev!=ev2) {
				printf("Nesoulad %d\n", i);
			}
			i++;
		}
	}

	freeHashStore(b.hs);
	deallocate_stats(stat);
	deallocate_stats(b.stats);
//	tell_to_engine(b3);
	return i;
}

void timed2_def(int time, int depth, int max){
int i=0;
personality *pi;
struct _results *r;
	r = (struct _results*)malloc(sizeof(struct _results) * (max+1));
	pi=(personality *) init_personality("pers.xml");
	timed_driver(time, depth, max, pi, 0, r, timed2_def_cback, &i);
	printSearchStat(&(r[max].stats));
	free(r);
	free(pi);
}

void timed2_remis(int time, int depth, int max){
int i=0;
personality *pi;
struct _results *r;
	r = malloc(sizeof(struct _results) * (max+1));
	pi=(personality *) init_personality("pers.xml");
	timed_driver(time, depth, max, pi, 0, r, timed2_remis_cback, &i);
	printSearchStat(&(r[max].stats));
	free(r);
	free(pi);
}

void timed2Test(char *filename, int max_time, int max_depth, int max_positions){
perft2_cb_data cb;
personality *pi;
int p1,f,i1,i;
unsigned long long t1;
char b[1024];
struct _results *r1;

	r1 = malloc(sizeof(struct _results) * (max_positions+1));
	pi=(personality *) init_personality("pers.xml");

	if((cb.handle=fopen(filename, "r"))==NULL) {
		printf("File %s is missing\n",filename);
		goto cleanup;
	}
	i1=timed_driver(max_time, max_depth, max_positions, pi, 0, r1, perft2_cback, &cb);
	fclose(cb.handle);

// prepocitani vysledku
	t1=0;
	p1=0;
	for(f=0;f<i1;f++){
		t1+=r1[f].time;
		if(r1[f].passed>0) p1++;
	}

//reporting
	logger2("Details  \n====================\n");
	logger2("Run#1 Results %d/%d, , Time: %dh, %dm, %ds,, %lld\n",p1,i1, (int) t1/3600000, (int) (t1%3600000)/60000, (int) (t1%60000)/1000, t1);
	logger2("Stats\n");
	i= i1;
	for(f=0;f<i;f++){
	logger2("====================\n");
		t1=r1[f].time;
		logger2("RUN #1, Position %d, , Time: %dh, %dm, %ds,, %lld\n",f, (int) t1/3600000, (int) (t1%3600000)/60000, (int) (t1%60000)/1000, t1);
		logger2("========\n");
		printSearchStat(&(r1[f].stats));
	}
	logger2("====================\n");

cleanup:
	free(r1);
	free(pi);
}

void timed2Test_x(char *filename, int max_time, int max_depth, int max_positions){
perft2_cb_data cb;
personality *pi;
int p1,f,i1;
char b[1024];
struct _results *r1;

	r1 = malloc(sizeof(struct _results) * (max_positions+1));
	pi=(personality *) init_personality("pers.xml");

	if((cb.handle=fopen(filename, "r"))==NULL) {
		printf("File %s is missing\n",filename);
		goto cleanup;
	}
	i1=timed_driver_eval(max_time, max_depth, max_positions, pi, 0, r1, perft2_cback, &cb);
	fclose(cb.handle);

// prepocitani vysledku
	p1=0;
	for(f=0;f<i1;f++){
		if(r1[f].passed==0) p1++;
	}

//reporting
	logger2("Details  \n====================\n");
	logger2("Run#1 Results passed %d/%d\n",p1,i1);

cleanup:
	free(r1);
	free(pi);
}

void timed2STS(int max_time, int max_depth, int max_positions){
sts_cb_data cb;
personality *pi;
int p1[20],f,i1[20], v1[20],vt1[20], n, q;
int times[]= { 10000, 60000, 420000 };
unsigned long long t1[20];
char b[1024], filename[512];
struct _results *r1[13];
struct _results *rh;

char *sts_tests[]= { "../tests/sts1.epd","../tests/sts2.epd", "../tests/sts3.epd","../tests/sts4.epd","../tests/sts5.epd","../tests/sts6.epd","../tests/sts7.epd","../tests/sts8.epd",
		"../tests/sts9.epd","../tests/sts10.epd","../tests/sts11.epd","../tests/sts12.epd","../tests/sts13.epd", "../tests/sts14.epd" };
//int tests_setup[]= { 10,100, 1,100, 6,00, 7,00, 12,00, 8,00, 11,00, 3,00, 4,00, 0,00, 2,00, 9,00, 5,00 ,-1};
//int tests_setup[]= { 10,100, 1,100, 6,100, 7,100, 12,100, 8,100, 11,100, 3,100, 4,100, 0,100, 2,100, 9,100, 5,100 ,-1};
int tests_setup[]= { 10,10, 1,10, 6,10, 7,10, 12,10, 8,10, 11,10, 3,10, 4,10, 0,10, 2,10, 9,10, 5,10 ,-1};
int index, mx;


	pi=(personality *) init_personality("pers.xml");
	max_positions = (max_positions > 100) ? 100 : max_positions;
	rh = malloc(sizeof(struct _results) * max_positions * 14);

	for(q=0;q<2;q++) {

		max_time=times[q];

		index=0;
		while(tests_setup[index]!=-1) {
			n=tests_setup[index++];
			mx = (tests_setup[index] > max_positions) ? max_positions : tests_setup[index];
			index++;
			if(mx<=0) continue;
			strcpy(filename, sts_tests[n]);

			r1[n] = rh+n*max_positions;

			if((cb.handle=fopen(filename, "r"))==NULL) {
				printf("File %s is missing\n",filename);
				goto cleanup;
			}
			cb.n=0;
			i1[n]=timed_driver(max_time, max_depth, mx, pi, 1, r1[n], sts_cback, &cb);
			fclose(cb.handle);

			// prepocitani vysledku
			t1[n]=0;
			p1[n]=0;
			v1[n]=0;
			vt1[n]=0;
			for(f=0;f<i1[n];f++){
				t1[n]+=r1[n][f].time;
				if(r1[n][f].passed>0) (p1[n])++;
				v1[n]+=r1[n][f].passed;
				vt1[n]+=10;
			}
		}

//reporting
		logger2("Details  \n====================\n");
		index=0;
		while(tests_setup[index]!=-1) {
			f=tests_setup[index++];
			if(tests_setup[index++]<=0) continue;
			logger2("Run#1 Results for STS:%d %d/%d, value %d/%d (%d%), Time: %dh, %dm, %ds,, %lld\n",f, p1[f],i1[f], v1[f],vt1[f], v1[f]*100/vt1[f], (int) t1[f]/3600000, (int) (t1[f]%3600000)/60000, (int) (t1[f]%60000)/1000, t1[f]);
		}

	}
cleanup:
	free(rh);
	free(pi);
}

void timed2Test_comp(char *filename, int max_time, int max_depth, int max_positions){
perft2_cb_data cb;
personality *pi;
int p1,p2,f,i1,i2,i;
unsigned long long t1,t2;
char b[1024];
struct _results *r1, *r2;

//	max_positions=10;
	r1 = malloc(sizeof(struct _results) * (max_positions+1));
	r2 = malloc(sizeof(struct _results) * (max_positions+1));
	pi=(personality *) init_personality("pers.xml");

// round one
	if((cb.handle=fopen(filename, "r"))==NULL) {
		printf("File %s is missing\n",filename);
		goto cleanup;
	}
	i1=timed_driver(max_time, max_depth, max_positions, pi, 0, r1, perft2_cback, &cb);
	fclose(cb.handle);
	pi=(personality *) init_personality("pers2.xml");

// round two
	if((cb.handle=fopen(filename, "r"))==NULL) {
		printf("File %s is missing\n",filename);
		goto cleanup;
	}
	i2=timed_driver(max_time, max_depth, max_positions, pi, 0, r2, perft2_cback, &cb);
	fclose(cb.handle);

// prepocitani vysledku
	t1=0;
	p1=0;
	for(f=0;f<i1;f++){
		t1+=r1[f].time;
		if(r1[f].passed>0) p1++;
	}

	t2=0;
	p2=0;
	for(f=0;f<i2;f++){
		t2+=r2[f].time;
		if(r2[f].passed>0) p2++;
	}

//reporting
	logger2("Details  \n====================\n");
	logger2("Run#1 Results %d/%d, , Time: %dh, %dm, %ds,, %lld\n",p1,i1, (int) t1/3600000, (int) (t1%3600000)/60000, (int) (t1%60000)/1000, t1);
	logger2("Run#2 Results %d/%d, , Time: %dh, %dm, %ds,, %lld\n",p2,i2, (int) t2/3600000, (int) (t2%3600000)/60000, (int) (t2%60000)/1000, t2);
	if(i1!=i2) {
		logger2("Different number of tests %d:%d!\n", i1, i2);
	} else {
		for(f=0;f<i2;f++) {
			if(r1[f].passed!=r2[f].passed) {
				logger2("Test %d results %d:%d, time %dh, %dm, %ds, %dh, %dm, %ds\n", f,r1[f].passed, r2[f].passed,(int) r1[f].time/3600000, (int) (r1[f].time%3600000)/60000, (int) (r1[f].time%60000)/1000, (int) r2[f].time/3600000, (int) (r2[f].time%3600000)/60000, (int) (r2[f].time%60000)/1000);
			}
		}
	}
	logger2("Stats\n");
	i= i1>i2 ? i2:i1;
	for(f=0;f<i;f++){
	logger2("====================\n");
		t1=r1[f].time;
		t2=r2[f].time;
		logger2("RUN #1, Position %d, , Time: %dh, %dm, %ds,, %lld\n",f, (int) t1/3600000, (int) (t1%3600000)/60000, (int) (t1%60000)/1000, t1);
		logger2("RUN #2, Position %d, , Time: %dh, %dm, %ds,, %lld\n",f, (int) t2/3600000, (int) (t2%3600000)/60000, (int) (t2%60000)/1000, t2);
		logger2("========\n");
		printSearchStat(&(r1[f].stats));
		logger2("====\n");
		printSearchStat(&(r2[f].stats));
	}
	logger2("====================\n");

cleanup:
	free(r1);
	free(r2);
	free(pi);
}

void see_test()
{
int result, move;
	board b;
	char *fen[]= {
			"1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - -",
			"1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - -"
	};
	struct _ui_opt uci_options;

	b.uci_options=&uci_options;

	b.stats=allocate_stats(1);
	b.hs=allocateHashStore(HASHSIZE, 2048);
	b.pers=(personality *) init_personality("pers.xml");

	setup_FEN_board(&b, fen[0]);
	printBoardNice(&b);
	move = PackMove(E1, E5,  ER_PIECE, 0);
	result=SEE(&b, move);

	setup_FEN_board(&b, fen[1]);
	printBoardNice(&b);
	move = PackMove(D3, E5,  ER_PIECE, 0);
	result=SEE(&b, move);
	freeHashStore(b.hs);
	deallocate_stats(b.stats);
	return;
}



void keyTest_def(void){
	char fen[100];
	char am[10][20];
	char bm[10][20];
	char cm[10][20];
	char pm[256][20];
	int dm;
	int i;
	board b;
	BITVAR key, k2;
	char * name;
	struct _ui_opt uci_options;

	b.uci_options=&uci_options;

	i=0;
	b.stats=allocate_stats(1);
	b.hs=allocateHashStore(HASHSIZE, 2048);
	while(key_default_tests[i]!=NULL) {
		if(parseEPD(key_default_tests[i], fen, am, bm, pm, cm, &dm, &name)>0) {
			if(getKeyFEN(key_default_tests[i],&key)==1) {
				setup_FEN_board(&b, fen);
				//						DEBUG_BOARD_CHECK(&b);
				DEB_4(boardCheck(&b));
				computeKey(&b, &k2);
				printf("----- Evaluate: %d -END-, %llx -----\n",i, (long long) key);
				free(name);
				if(key!=k2){
					printf("Not Match!\n");
					printBoardNice(&b);
				}
			}
		}
		i++;
	}
	freeHashStore(b.hs);
	deallocate_stats(b.stats);
}

// see_0 tests
// 1k2r3/1p1bP3/2p2p1Q/Ppb5/5p1P/5N1P/5PB/4q1K w - - 1 3; tah q xx->E1


void see0_test()
{
int result, move;
	board b;
	struct _ui_opt uci_options;

	b.uci_options=&uci_options;

	char *fen[]= {
			"1k2r3/1p1bP3/2p2p1Q/Ppb5/5p1P/5N1P/5PB/4q1K w - - 1 3",
	};

	b.stats=allocate_stats(1);
	b.pers=(personality *) init_personality("pers.xml");
	b.hs=allocateHashStore(HASHSIZE, 2048);

	setup_FEN_board(&b, fen[0]);
	printBoardNice(&b);
	move = PackMove(E8, E1,  ER_PIECE, 0);
	result=SEE_0(&b, move);
	deallocate_stats(b.stats);
	freeHashStore(b.hs);

	return;
}

void eeval_test()
{
int result, move;
int ev;
	board b;
	attack_model *a, ATT;
	struct _ui_opt uci_options;

	b.uci_options=&uci_options;

	char *fen[]= {
			"4k2r/1p1bb1pp/1q1ppn2/1p2B3/4P3/1N6/PPPQBPP1/R4RK b kq - 1 13",
	};

	LOGGER_0("EEVAL test\n");
	b.stats=allocate_stats(1);
	b.pers=(personality *) init_personality("pers.xml");
	b.hs=allocateHashStore(HASHSIZE, 2048);

	setup_FEN_board(&b, fen[0]);
//	printBoardNice(&b);
//	ev=eval(&b, &ATT, b.pers);
	result=eval_king_checks_all(&b, &ATT);
	deallocate_stats(b.stats);
	freeHashStore(b.hs);
	LOGGER_0("EEVAL test finished\n");

	return;
}


void fill_test()
{
char fen[]={"8/pk5P/1p4P1/2p2P2/3pP3/3Pp3/2P2p2/4K3 w - - 0 1"};
int i;
board b;
BITVAR res;
attack_model *a, ATT;
struct _ui_opt uci_options;
int ev;
	b.uci_options=&uci_options;
	b.stats=allocate_stats(1);
	b.pers=(personality *) init_personality("pers.xml");

	setup_FEN_board(&b, fen);
	printBoardNice(&b);
	res=FillNorth(RANK1, b.maps[PAWN]&b.colormaps[WHITE], RANK1);
	printmask(res, "RES1");
	res=FillNorth(RANK1, ~(b.maps[PAWN]&b.colormaps[WHITE]), RANK1);
	printmask(res, "RES2");
	res=FillNorth(RANK1, (b.maps[PAWN]&b.colormaps[WHITE]), 0);
	printmask(res, "RES3");
	res=FillNorth(RANK1, ~(b.maps[PAWN]&b.colormaps[WHITE]), 0);
	printmask(res, "RES4");
	ATT.phase= eval_phase(&b, b.pers);
	ev=eval(&b, &ATT, b.pers);
	deallocate_stats(b.stats);
}


typedef struct {
	personality *p;
	int stage;
} tuner_variables_pass;


int free_matrix(matrix_type *m, int count)
{
	int i;
	if(m!=NULL) {
		for(i=0;i<count;i++) {
			if(m[i].init_data!=NULL) free(m[i].init_data);
		}
		if(m!=NULL) free(m);
	}
	return 0;
}

int to_matrix(matrix_type **m, personality *p)
{
int i, max, gs, sd, pi, sq;
int len=2048;
matrix_type *mat;
tuner_variables_pass *v;

	max=len;
	mat=malloc(sizeof(matrix_type)*len);
	*m=mat;
	i=0;
#if 1
// eval_BIAS
		mat[i].init_f=NULL;
		mat[i].restore_f=NULL;
		mat[i].init_data=NULL;
		mat[i].upd=0;
		mat[i].u[0]=&p->eval_BIAS;
		mat[i].mid=0;
		mat[i].ran=10000;
		mat[i].max=mat[i].ran/2+mat[i].mid;
		mat[i].min=mat[i].mid-mat[i].ran/2;
		i++;
#endif
#if 0
	// pawn isolated
		for(gs=0;gs<=1;gs++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
			mat[i].upd=0;
			mat[i].u[0]=&p->isolated_penalty[gs];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
		}

	// pawn protected
		for(gs=0;gs<=1;gs++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
				mat[i].upd=0;
				mat[i].u[0]=&p->pawn_protect[gs];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
		}

	// pawn backward
		for(gs=0;gs<=1;gs++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
				mat[i].upd=0;
				mat[i].u[0]=&p->backward_penalty[gs];
				mat[i].mid=0;
				mat[i].ran=5000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
		}

	// pawn backward fixable
		for(gs=0;gs<=1;gs++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
				mat[i].upd=0;
				mat[i].u[0]=&p->backward_fix_penalty[gs];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
		}

	// pawn doubled
		for(gs=0;gs<=1;gs++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
				mat[i].upd=0;
				mat[i].u[0]=&p->doubled_penalty[gs];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
		}
#endif
#if 0
	// pawn on ah
		for(gs=0;gs<=1;gs++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
				mat[i].upd=0;
				mat[i].u[0]=&p->pawn_ah_penalty[gs];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
		}
#endif
#if 1
		// passer bonus
			for(gs=0;gs<=1;gs++) {
				for(sq=0;sq<=5;sq++) {
					mat[i].init_f=NULL;
					mat[i].restore_f=NULL;
					mat[i].init_data=NULL;
					mat[i].upd=1;
					mat[i].u[0]=&p->passer_bonus[gs][WHITE][sq];
					mat[i].u[1]=&p->passer_bonus[gs][BLACK][sq];
					mat[i].mid=0;
					mat[i].ran=10000;
					mat[i].max=mat[i].ran/2+mat[i].mid;
					mat[i].min=mat[i].mid-mat[i].ran/2;
					i++;
				}
			}
#endif
#if 0
		// pawn blocked penalty
			for(gs=0;gs<=1;gs++) {
				for(sq=0;sq<=4;sq++) {
					mat[i].init_f=NULL;
					mat[i].restore_f=NULL;
					mat[i].init_data=NULL;
					mat[i].upd=1;
					mat[i].u[0]=&p->pawn_blocked_penalty[gs][WHITE][sq];
					mat[i].u[1]=&p->pawn_blocked_penalty[gs][BLACK][sq];
					mat[i].mid=0;
					mat[i].ran=10000;
					mat[i].max=mat[i].ran/2+mat[i].mid;
					mat[i].min=mat[i].mid-mat[i].ran/2;
					i++;
				}
			}
		// pawn stopped penalty
				for(gs=0;gs<=1;gs++) {
					for(sq=0;sq<=4;sq++) {
						mat[i].init_f=NULL;
						mat[i].restore_f=NULL;
						mat[i].init_data=NULL;
						mat[i].upd=1;
						mat[i].u[0]=&p->pawn_stopped_penalty[gs][WHITE][sq];
						mat[i].u[1]=&p->pawn_stopped_penalty[gs][BLACK][sq];
						mat[i].mid=0;
						mat[i].ran=10000;
						mat[i].max=mat[i].ran/2+mat[i].mid;
						mat[i].min=mat[i].mid-mat[i].ran/2;
						i++;
					}
				}
#endif

#if 0
// king safety
	for(gs=0;gs<=1;gs++) {
		for(sq=0;sq<=6;sq++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
			mat[i].upd=1;
			mat[i].u[0]=&p->king_s_pdef[gs][WHITE][sq];
			mat[i].u[1]=&p->king_s_pdef[gs][BLACK][sq];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
		}
	}
	for(gs=0;gs<=1;gs++) {
		for(sq=0;sq<=6;sq++) {
			mat[i].init_f=NULL;
			mat[i].restore_f=NULL;
			mat[i].init_data=NULL;
			mat[i].upd=1;
			mat[i].u[0]=&p->king_s_patt[gs][WHITE][sq];
			mat[i].u[1]=&p->king_s_patt[gs][BLACK][sq];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
		}
	}
#endif
#if 0
//piece to square - single
	for(gs=0;gs<=0;gs++) {
		for(pi=0;pi<=0;pi++) {
			for(sq=0;sq<=63;sq++){
				mat[i].init_f=NULL;
				mat[i].restore_f=NULL;
				mat[i].init_data=NULL;
				mat[i].upd=1;
				mat[i].u[0]=&p->piecetosquare[gs][WHITE][pi][sq];
				mat[i].u[1]=&p->piecetosquare[gs][BLACK][pi][Square_Swap[sq]];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
			}
		}
	}
#endif

#if 0
	for(gs=0;gs<=1;gs++) {
		for(pi=0;pi<=5;pi++) {
			for(sq=0;sq<=63;sq++){
				mat[i].init_f=NULL;
				mat[i].restore_f=NULL;
				mat[i].init_data=NULL;
				mat[i].upd=1;
				mat[i].u[0]=&p->piecetosquare[gs][WHITE][pi][sq];
				mat[i].u[1]=&p->piecetosquare[gs][BLACK][pi][Square_Swap[sq]];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
			}
		}
	}
#endif

#if 0
// rook on 7th
	for(gs=0;gs<=1;gs++) {
		mat[i].init_f=NULL;
		mat[i].restore_f=NULL;
		mat[i].init_data=NULL;
			mat[i].upd=0;
			mat[i].u[0]=&p->rook_on_seventh[gs];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
	}

// rook on open
	for(gs=0;gs<=1;gs++) {
		mat[i].init_f=NULL;
		mat[i].restore_f=NULL;
		mat[i].init_data=NULL;
			mat[i].upd=0;
			mat[i].u[0]=&p->rook_on_open[gs];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
	}

// rook on semiopen
	for(gs=0;gs<=1;gs++) {
		mat[i].init_f=NULL;
		mat[i].restore_f=NULL;
		mat[i].init_data=NULL;
			mat[i].upd=0;
			mat[i].u[0]=&p->rook_on_semiopen[gs];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
	}
#endif

#if 0
// mobility
	int mob_lengths[]= { 0, 9, 14, 15, 28, 9, -1  };
	for(gs=0;gs<=1;gs++) {
		for(pi=1;pi<=5;pi++) {
			for(sq=0;sq<mob_lengths[pi];sq++){
				mat[i].init_f=NULL;
				mat[i].restore_f=NULL;
				mat[i].init_data=NULL;
				mat[i].upd=1;
				mat[i].u[0]=&p->mob_val[gs][WHITE][pi][sq];
				mat[i].u[1]=&p->mob_val[gs][BLACK][pi][sq];
				mat[i].mid=0;
				mat[i].ran=10000;
				mat[i].max=mat[i].ran/2+mat[i].mid;
				mat[i].min=mat[i].mid-mat[i].ran/2;
				i++;
			}
		}
	}

#endif

// for these we need callback function
#if 0
	for(gs=0;gs<=1;gs++) {
		mat[i].init_f=variables_reinit_material;
		mat[i].restore_f=variables_restore_material;
		v=malloc(sizeof(tuner_variables_pass));
		v->p=p;
		v->stage=gs;
		mat[i].init_data=v;

			mat[i].upd=0;
			mat[i].u[0]=&p->rook_to_pawn[gs];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
	}

	for(gs=0;gs<=1;gs++) {
		mat[i].init_f=variables_reinit_material;
		mat[i].restore_f=variables_restore_material;
		v=malloc(sizeof(tuner_variables_pass));
		v->p=p;
		v->stage=gs;
		mat[i].init_data=v;

			mat[i].upd=0;
			mat[i].u[0]=&p->bishopboth[gs];
			mat[i].mid=0;
			mat[i].ran=10000;
			mat[i].max=mat[i].ran/2+mat[i].mid;
			mat[i].min=mat[i].mid-mat[i].ran/2;
			i++;
	}
#endif
#if 0
	gs=0;
	for(sq=1;sq<=2;sq++) {
		mat[i].init_f=variables_reinit_material;
		mat[i].restore_f=variables_restore_material;
		v=malloc(sizeof(tuner_variables_pass));
		v->p=p;
		v->stage=gs;
		mat[i].init_data=v;
		mat[i].upd=0;
		mat[i].u[0]=&p->Values[gs][sq];
		mat[i].mid=0;
		mat[i].ran=30000;
		mat[i].max=mat[i].ran/2+mat[i].mid;
		mat[i].min=mat[i].mid-mat[i].ran/2;
		i++;
	}
	gs=1;
	for(sq=1;sq<=2;sq++) {
		mat[i].init_f=variables_reinit_material;
		mat[i].restore_f=variables_restore_material;
		v=malloc(sizeof(tuner_variables_pass));
		v->p=p;
		v->stage=gs;
		mat[i].init_data=v;
		mat[i].upd=0;
		mat[i].u[0]=&p->Values[gs][sq];
		mat[i].mid=0;
		mat[i].ran=30000;
		mat[i].max=mat[i].ran/2+mat[i].mid;
		mat[i].min=mat[i].mid-mat[i].ran/2;
		i++;
	}
#endif
return i;
}


//int meval_table_gen(meval_t *t, personality *p, int stage);

int variables_reinit_material(void *data){
tuner_variables_pass *v;

	v=(tuner_variables_pass*)data;
	if(v->stage==0) {
		meval_table_gen(v->p->mat , v->p, 0);
	} else {
		meval_table_gen(v->p->mate_e,v->p, 1);
	}
	mat_info(v->p->mat_info);
	mat_faze(v->p->mat_faze);
	return 0;
}

int variables_restore_material(void *data){
tuner_variables_pass *v;

	v=(tuner_variables_pass*)data;
	if(v->stage==0) {
		meval_table_gen(v->p->mat , v->p, 0);
	} else {
		meval_table_gen(v->p->mate_e,v->p, 1);
	}
	mat_info(v->p->mat_info);
	mat_faze(v->p->mat_faze);
return 0;
}

double norm_val(double val, double range, double mid)
{
	return (val-mid)*2/range;
}

double unnorm_val(double norm, double range, double mid)
{
	return (norm*range/2+mid);
}

double calc_dir_penalty( matrix_type *m, tuner_global *tun, int pcount)
{
int i;
double pen=0.0;
		for(i=0;i<pcount;i++) {
			// compute loss
			pen+=tun->reg_la*(pow(norm_val(*(m[i].u[0]), m[i].ran,m[i].mid),2));
		}
//return pen/tun->len;
return pen;
}

double calc_dir_penalty_jac(double *var, matrix_type *m, tuner_global *tun, int pcount)
{
int i;
double pen=0.0;
		for(i=0;i<pcount;i++) {
			// compute loss
			pen+=tun->reg_la*(pow(norm_val(var[i], m[i].ran,m[i].mid),2));
		}
return pen;
}

int compute_eval_dir(board *b, uint8_t *ph, personality *p, long offset)
{
attack_model a;
struct _ui_opt uci_options;
struct _statistics s;
int ev;
	b[offset].stats=&s;
	b[offset].uci_options=&uci_options;
	a.phase = ph[offset];
// eval - white pov
	ev=eval(&b[offset], &a, p);
return ev;
}

/*
 * J is matrix of partial derivations for particular position at posn, in contains score with parameters in m
 * m is matrix of initial parameters
 * val is matrix of current/tested parameters
 * pcount is numbers of parameters
 * 
 *  evaluaci prevest na f(x)=f(x0)+df/dx(x0)*(x-x0)
 *			x0 je v ival[i];
 *			x je nval[i];
 *			f(x0) je v J[pos][parameters+1]
 *			bias je v J[pos][parameters]
 *			x je v val
 */
#define LK1 (-1.0)
#define LK2 (8500.0)

double compute_loss(board *b, int8_t *rs, uint8_t *ph, personality *p, int count, int *indir, long offset)
{
double res, r1, r2, ry, cost, h0, ev;
attack_model a;
struct _ui_opt uci_options;
struct _statistics s;
int i, q;
	res=0;
	for(i=0;i<count;i++) {
		q=indir[i+offset];
		b[q].stats=&s;
		b[q].uci_options=&uci_options;
		a.phase = ph[q];

// eval - white pov
		ev=(double)eval(&b[q], &a, p);
		ry=rs[q];
// results - white pov
		h0=(2.0/(1+exp(LK1*ev/LK2)));
		cost = (-log(h0/2.0)*ry/2.0-log(1-h0/2.0)*(1-ry/2.0));
		res+=cost;
	}
return res;
}

double compute_loss_dir(board *b, int8_t *rs, uint8_t *ph, personality *p, int count, long offset)
{
double res, r1, r2, ry, cost, h0, ev;
attack_model a;
struct _ui_opt uci_options;
struct _statistics s;
int evi,i, q;
	res=0;
	for(i=0;i<count;i++) {
		q=i+offset;
		b[q].stats=&s;
		b[q].uci_options=&uci_options;
		a.phase = ph[q];

// eval - white pov
		evi=eval(&b[q], &a, p);
		ev=(double) evi;
		ry=rs[q];
// results - white pov
// sig=rrr-h0
// r=0-2, h0=0-2
		h0=(2.0/(1+exp(LK1*ev/LK2)));
		assert((h0>0)&&(h0<2));
		assert((ry>=0)&&(ry<=2));
// alternate cost; cost = (-log10l(h0/2)*ry/2-log10l(1-h0/2)*(1-ry/2))
		cost = (-log(h0/2.0)*ry/2.0-log(1-h0/2.0)*(1-ry/2.0));
		res+=cost;
	}
return res;
}

double compute_loss_jac(int8_t *rs, int count, int *indir, long offset,double *JJ, double *ival, double *nval, int pcount)
{
double res, r1, r2, ry, cost, h0, nv, iv, ev;
double *J;
int ii, i, q;
	res=0;
	for(ii=0;ii<count;ii++) {
		q=indir[ii+offset];

// eval - white pov
// evaluaci prevest na f(x)=f(x0)+df/dx(x0)*(x-x0)
/*
 *			x0 je v ival[i];
 *			x je v nval[i];
 *			f(x0) je v JJ[pos][parameters+1]
 *			bias je v JJ[pos][parameters]
 *			
 */
		J=JJ+q*(pcount+3);
		ev=(double) J[pcount+1];
		for(i=0;i<pcount;i++) {
			ev+=(double)(nval[i]-ival[i])*J[i];
		}

//		ev=comp_jac_pos(JJ, q, ival, nval, pcount);
		ry=rs[q];
// results - white pov
//		h0=(2/(1+pow(PWR,(LK1*ev/LK2))));
//		cost = (-log10(h0/2)*ry/2-log10(1-h0/2)*(1-ry/2));
		h0=(2.0/(1+exp(LK1*ev/LK2)));
		cost = (-log(h0/2.0)*ry/2.0-log(1-h0/2.0)*(1-ry/2.0));
		res+=cost;
	}
return res;
}

double compute_loss_jac_diff(int8_t *rs, int count, int *indir, long offset,double *JJ, double *ival, double *nval, int i, double new_val, int pcount)
{
double res, r1, r2, ry, cost, h0, nv, iv, ev;
double *J, old;
int ii, q;
	res=0;
	for(ii=0;ii<count;ii++) {
		q=indir[ii+offset];

// eval - white pov
// evaluaci prevest na f(x)=f(x0)+df/dx(x0)*(x-x0)
// vice parametru
/*
 * x a x0 jsou ve skutecnosti vektory, kde kazda dimenze odpovida jednomu ladenemu parametru evaluacni funkce
 */
/*
 *			x0 je v ival[i];
 *			x je v nval[i];
 *			f(x0) je v JJ[pos][parameters+1]
 *			bias je v JJ[pos][parameters]
 *			f(x) je v JJ[pos][parameters+2]
 *			funkce spocita zmenu skore pri zmene jedne z hodnot v nval
 */
		J=JJ+q*(pcount+3);
//		old=nval[i];
		ev=(double) J[pcount+2];
		ev-=(double)(nval[i]-ival[i])*J[i];
		ev+=(double)(new_val-ival[i])*J[i];

		ry=rs[q];
// results - white pov
//		h0=(2/(1+pow(PWR,(LK1*ev/LK2))));
//		cost = (-log10(h0/2)*ry/2-log10(1-h0/2)*(1-ry/2));
		h0=(2.0/(1+exp(LK1*ev/LK2)));
		cost = (-log(h0/2.0)*ry/2.0-log(1-h0/2.0)*(1-ry/2.0));
		res+=cost;
	}
return res;
}

double compute_loss_jac_dir(int8_t *rs, int count, long offset,double *JJ, double *ival, double *nval, int pcount)
{
double res, r1, r2, ry, cost, h0, ev;
double *J;
int ii, i, q;
	res=0;
	for(ii=0;ii<count;ii++) {
		q=ii+offset;

// eval - white pov
// evaluaci prevest na f(x)=f(x0)+df/dx(x0)*(x-x0)
/*
 *			x0 je v ival[i];
 *			x je v nval[i];
 *			f(x0) je v JJ[pos][parameters+1]
 *			bias je v JJ[pos][parameters]
 *			
 */

		J=JJ+q*(pcount+3);
		ev=(double)J[pcount+1];
		for(i=0;i<pcount;i++) {
			ev+=(double)(nval[i]-ival[i])*J[i];
		}
//		ev=comp_jac_pos(JJ, q, ival, nval, pcount);
		ry=rs[q];
// results - white pov
//		h0=(2/(1+pow(PWR,(LK1*ev/LK2))));
//		cost = (-log10(h0/2)*ry/2-log10(1-h0/2)*(1-ry/2));
		h0=(2.0/(1+exp(LK1*ev/LK2)));
		cost = (-log(h0/2.0)*ry/2.0-log(1-h0/2.0)*(1-ry/2.0));
		res+=cost;
	}
return res;
}


// tuner_run - runtime variables needed for tuner,incl real representation of values/parameters
// matrix_type - matrix of pointers to int values/parameters for tuning

int init_tuner(tuner_run *state,matrix_type *m, int pcount){
int i;
	for(i=0;i<pcount;i++) state[i].or1=0.00000001;
	for(i=0;i<pcount;i++) state[i].or2=0.00000001;
//	for(i=0;i<pcount;i++) state[i].update=*(m[i].u[0]);
	for(i=0;i<pcount;i++) state[i].grad=0;
	for(i=0;i<pcount;i++) state[i].real=*(m[i].u[0]);
	return 0;
}

int init_tuner_jac(tuner_run *state,double *var, int pcount){
int i;
	for(i=0;i<pcount;i++) state[i].or1=0.00000001;
	for(i=0;i<pcount;i++) state[i].or2=0.00000001;
//	for(i=0;i<pcount;i++) state[i].update=*(m[i].u[0]);
	for(i=0;i<pcount;i++) state[i].grad=0;
	for(i=0;i<pcount;i++) state[i].real=var[i];
	return 0;
}

int allocate_tuner(tuner_run **tr, int pcount){
//tuner_run *t;
	*tr=malloc(sizeof(tuner_run)*pcount);
	return 0;
}

int backup_matrix_values(matrix_type *m, int *backup, int pcount){
int i;
	for(i=0;i<pcount;i++) {
		backup[i]=*(m[i].u[0]);
	}
	return 0;
}

int restore_matrix_values(int *backup, matrix_type *m, int pcount){
	int i, ii, on;
		for(i=0;i<pcount;i++) {
			for(ii=0;ii<=m[i].upd;ii++) {
			*(m[i].u[ii])=backup[i];
		}
	}
	return 0;
}

int copy_vars_jac(int source, int dest, double *ivar, double *nvar, int pcount){
int f;
double *s, *d;
//	printf("***copy vars, dest %d, source %d***\n", dest, source);
	s=ivar+source*pcount;
	d=ivar+dest*pcount;
	for(f=0;f<pcount;f++) { 
//		printf("Ivar: %f=%f\n", *d,*s ); 
		*d=*s; d++; s++; 
	}
	s=nvar+source*pcount;
	d=nvar+dest*pcount;
	for(f=0;f<pcount;f++) { 
//		printf("Nvar: %f=%f\n", *d,*s );
		*d=*s; d++; s++; 
	}

return 0;
}

int jac_to_matrix(int source, matrix_type *m, double *var, int pcount ){
double *s;
int i, ii, on;
	s=var+source*pcount;
	for(i=0;i<pcount;i++) {
		on=round(s[i]);
		for(ii=0;ii<=m[i].upd;ii++) {
			*(m[i].u[ii])=on;
		}
	}
return 0;
}

int matrix_to_jac(int dest, matrix_type *m, double *var, int pcount ){
double *d;
int i;
	d=var+dest*pcount;
	for(i=0;i<pcount;i++) {
		d[i]=*(m[i].u[0]);
	}
return 0;
}

/*
 * count  - pocet B
 * pcount - pocet parametru
 */

void p_tuner(board *b, int8_t *rs, uint8_t *ph, personality *p, int count, matrix_type *m, tuner_global *tun, tuner_run *state, int pcount, char * outp, int* indir, long offset, int iter)
{
	int step, diff, ioon;
	double fx, fxh, fxh2, fxh3, fxt, x,y,z,zt, fxdiff, oon, g_reg, y_hat, x_hat;
	//!!!!
	int m_back[2048];
	int i, n, sq, ii;
	int o,q,g, on;
	int gen;

	n=0;

	for(gen=0;gen<1; gen++) {
		// loop over parameters
		for(i=0;i<pcount;i++) {
//			step=tun->diff_step;
			step=m[i].ran/4;
			// get parameter value
			o=*(m[i].u[0]);
			on=o+step;
			// iterate over the same parameters and update them with change;
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=on;
			}
			if(m[i].init_f!=NULL) m[i].init_f(m[i].init_data);
			// compute loss
			fxh=compute_loss(b, rs, ph, p, count, indir, offset);
			on=o-step;
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=on;
			}
			if(m[i].init_f!=NULL) m[i].init_f(m[i].init_data);
			fxh2=compute_loss(b, rs, ph, p, count, indir, offset);
			// compute gradient
			fxdiff=fxh-fxh2;
			state[i].grad=(fxdiff/count)/(2*step);
			//restore original values
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=o;
			}
			if(m[i].init_f!=NULL) m[i].init_f(m[i].init_data);
		}
		// gradient descent
		for(i=0;i<pcount;i++) {
			g_reg=2*tun->reg_la*(norm_val(*(m[i].u[0]), m[i].ran,m[i].mid))*count;
			state[i].grad+=g_reg;
			if(tun->method==2) {
				/*
				 * rmsprop
				 */
				// accumulate gradients
				state[i].or2=(state[i].or2*tun->la2)+(pow(state[i].grad,2))*(1-tun->la2); //!!!
				// compute update
				y=sqrt(state[i].or2+tun->small_c); //!!!
				// update
				z=unnorm_val(0-tun->rms_step*state[i].grad/y, m[i].ran,m[i].mid);
//				printf("GRD dif: %.10f, upd: %.10f, y:%.10f, z:%.10f,\n", state[i].grad, (0-tun->rms_step*state[i].grad/y),y,z);
			}
			else if(tun->method==1){
				/*
				 * AdaDelta
				 */
				// accumulate gradients
				state[i].or2=(state[i].or2*tun->la2)+(pow(state[i].grad,2))*(1-tun->la2);
				x=sqrt(state[i].or1);
				y=sqrt(state[i].or2+tun->small_c);
				// adadelta update
				z=0-state[i].grad*x/y;
				// accumulate updates / deltas
				state[i].or1=(state[i].or1*tun->la1)+(pow(z,2))*(1-tun->la1);
				// store update / delta / rescale to parameter range
				z=unnorm_val(tun->adadelta_step*z,m[i].ran,m[i].mid);
			} else {
				/*
				 * Adam mod.
				 */
				// accumulate gradients
				state[i].or2=(state[i].or2*tun->la2)+(pow(state[i].grad,2))*(1-tun->la2);
				// accumulate grads
				state[i].or1=(state[i].or1*tun->la1)+(pow(state[i].grad,1))*(1-tun->la1);
				// compute update
//				iter=1;
				y_hat=state[i].or2/(1.0-pow(tun->la2, iter));
				x_hat=state[i].or1/(1.0-pow(tun->la1, iter));
				x=x_hat;
				y=sqrt(y_hat)+tun->small_c;
				// update
				z=0-x/y;
				// store update / delta / rescale to parameter range
//				fflush(stdout);
				z=unnorm_val(z*tun->adam_step,m[i].ran,m[i].mid);
			}
			state[i].update=z;
			// store new computed parameter value
			state[i].real+=z;
			// check limits
			oon=Max(m[i].min, Min(m[i].max, state[i].real));
			;
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=(int)oon;
			}
		}
		n++;
	}
}

int recompute_jac(double *J, int count, double *ivar, double *nvar, int pcount)
{
	int diff_step, pos;
	int fxh, fxh2;
	double fxdiff, *JJ;
	//!!!!
	int i,ii;
	int o,q,g, on;

	for(pos=0;pos<count;pos++) {
		JJ=J+pos*(pcount+3);
		fxh=JJ[pcount+1];
		for(i=0;i<pcount;i++) {
			fxh+=(nvar[i]-ivar[i])*JJ[i];
		}
		JJ[pcount+2]=fxh;
	}
return 0;
}

/*
 * count  - pocet B
 * pcount - pocet parametru
 */

void p_tuner_jac(int8_t *rs, int count, matrix_type *m, tuner_global *tun, tuner_run *state, double *ivar, double *nvar, int pcount, char * outp, int* indir, long offset, int iter)
{
	int step, diff, ioon;
	double fx, fxh, fxh2, fxh3, fxt, x,y,z, fxdiff, oon, g_reg, y_hat, x_hat, o, on;
	//!!!!
//	int m_back[2048];
	int i, n, sq, ii;
	int q,g;
	int gen;

	for(gen=0;gen<1; gen++) {
		// loop over parameters
		for(i=0;i<pcount;i++) {
// compute gradient for cost function
			step=m[i].ran/4;
			// get parameter value
			o=nvar[i];
			on=o+step;
			// iterate over the same parameters and update them with change;
			nvar[i]=on;
			// compute loss
			fxh=compute_loss_jac(rs, count, indir, offset, tun->jac, ivar, nvar, pcount);
//			fxh=compute_loss_jac_diff(rs, count, indir, offset, tun->jac, ivar, nvar, i, on, pcount);
			on=o-step;
			nvar[i]=on;
			fxh2=compute_loss_jac(rs, count, indir, offset, tun->jac, ivar, nvar, pcount);
//			fxh2=compute_loss_jac_diff(rs, count, indir, offset, tun->jac, ivar, nvar, i, on, pcount);
			// compute gradient for cost functions
			fxdiff=fxh-fxh2;
//			if(fxdiff!=0) printf("%d %Lf\n",i, nvar[i]);
			state[i].grad=(fxdiff/count)/(2*step);
			//restore original values
//			nvar[i]=o;
		}
		// gradient descent
		for(i=0;i<pcount;i++) {
			g_reg=2*tun->reg_la*(norm_val(nvar[i], m[i].ran,m[i].mid))*count;
			state[i].grad+=g_reg;
			if(tun->method==2) {
				/*
				 * rmsprop
				 */
				// accumulate gradients
				state[i].or2=(state[i].or2*tun->la2)+(pow(state[i].grad,2))*(1-tun->la2); //!!!
				// compute update
				y=sqrt(state[i].or2+tun->small_c); //!!!
				// update
				z=unnorm_val(0-tun->rms_step*state[i].grad/y, m[i].ran, m[i].mid);
//				printf("GRD dif: %.10f, upd: %.10f, y:%.10f, z:%.10f,\n", state[i].grad, (0-tun->rms_step*state[i].grad/y),y,z);
			}
			else if(tun->method==1){
				/*
				 * AdaDelta
				 */
				// accumulate gradients
				state[i].or2=(state[i].or2*tun->la2)+(pow(state[i].grad,2))*(1-tun->la2);
				x=sqrt(state[i].or1+tun->small_c);
				y=sqrt(state[i].or2+tun->small_c);
				// adadelta update
				z=0-state[i].grad*x/y;
				// accumulate updates / deltas
				state[i].or1=(state[i].or1*tun->la1)+(pow(z,2))*(1-tun->la1);
				// store update / delta / rescale to parameter range
				z=unnorm_val(z*tun->adadelta_step, m[i].ran, m[i].mid);
			} else {
				/*
				 * Adam mod.
				 */
				// accumulate gradients
				state[i].or2=(state[i].or2*tun->la2)+(pow(state[i].grad,2))*(1-tun->la2);
				// accumulate grads
				state[i].or1=(state[i].or1*tun->la1)+(pow(state[i].grad,1))*(1-tun->la1);
				// compute update
//				iter=1;
				y_hat=state[i].or2/(1.0-pow(tun->la2, iter));
				x_hat=state[i].or1/(1.0-pow(tun->la1, iter));
				x=x_hat;
				y=sqrt(y_hat)+tun->small_c;
				// update
				z=0-x/y;
				// store update / delta / rescale to parameter range
				z=unnorm_val(z*tun->adam_step, m[i].ran, m[i].mid);
			}
			state[i].update=z;
			// store new computed parameter value
			state[i].real+=z;
			// check limits
			oon=Max(m[i].min, Min(m[i].max, state[i].real));
			nvar[i]=oon;
//			printf("GRD dif: %.10f, x: %.10f, y:%.10f, z:%.10f, oon:%.10f\n", state[i].grad, x,y,z,oon);
		}
		// recompute jacobian
		recompute_jac(tun->jac, count, ivar, nvar, pcount);
	}
}

double * allocate_jac(int records, int params){
double *J;

	printf ("double %i, long double %i, request %Lf\n", sizeof(double), sizeof(long double),(long double) (sizeof(double)*(params+3)*records));
	J=(double*)malloc(sizeof(double)*(params+3)*records);
	return J;
}

int free_jac(double *J){
	if(J!=NULL) {
	    free(J);
	}
	return 0;
}

int populate_jac(double *J,board *b, int8_t *rs, uint8_t *ph, personality *p, int start, int stop, matrix_type *m, int pcount)
{
	int diff_step, pos;
	int fxh, fxh2;
	double fxdiff, *JJ;
	//!!!!
	int i,ii;
	int o,q,g, on;

	for(pos=start;pos<=stop;pos++) {
		// loop over parameters
		JJ=J+pos*(pcount+3);
		for(i=0;i<pcount;i++) {
			// get parameter value
			diff_step=m[i].ran/4;
			o=*(m[i].u[0]);
			on=o+diff_step;
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=on;
			}
			if(m[i].init_f!=NULL) m[i].init_f(m[i].init_data);
			// compute eval
			fxh=compute_eval_dir(b, ph, p, pos);
			on=o-diff_step;
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=on;
			}
			if(m[i].init_f!=NULL) m[i].init_f(m[i].init_data);
			fxh2=compute_eval_dir(b, ph, p, pos);
			// compute gradient/partial derivative
			fxdiff=fxh-fxh2;
			JJ[i]=(fxdiff)/(2*diff_step);
			//restore original values
			for(ii=0;ii<=m[i].upd;ii++) {
				*(m[i].u[ii])=o;
			}
			if(m[i].init_f!=NULL) m[i].init_f(m[i].init_data);
		}
		fxh=compute_eval_dir(b, ph, p, pos);
// recompute score with gradients

		fxh2=0;
		for(i=0;i<pcount;i++) {
			fxh2+=*(m[i].u[0])*JJ[i];
//			for(ii=0;ii<=m[i].upd;ii++) {
//				fxh2+=*(m[i].u[ii])*JJ[i];
//			}
		}
		JJ[i++]=fxh-fxh2;
		JJ[i++]=fxh;
		JJ[i++]=fxh;
	}
return 0;
}

typedef struct {
	double *J;
	board *b;
	int8_t *rs; 
	uint8_t *ph; 
	personality *p; 
	int start;
	int stop;
	matrix_type *m; 
	int pcount;
} _tt_data;

void *jac_engine_thread(void *arg){
_tt_data *d;
	d=(_tt_data *)arg;
	populate_jac(d->J,d->b, d->rs, d->ph, d->p, d->start, d->stop, d->m, d->pcount);
return arg;
}

// do populate jac multithreaded
int populate_jac_pl(double *J,board *b, int8_t *rs, uint8_t *ph, personality *p, int start, int stop, matrix_type *m, int pcount, int threads){
void * status;
pthread_t th[10];
_tt_data data[10];
pthread_attr_t attr[10];
int f,pos,s,r;

	if(threads>10) threads=10;
	pos=(stop-start+1);
	if(pos<threads) threads=1;
	s=pos/threads;
	r=pos%threads;
	for(f=0;f<threads;f++) {
		data[f].J=J;
		data[f].b=b;
		data[f].rs=rs;
		data[f].ph=ph;
		data[f].p=init_personality(NULL);
		copyPers(p, data[f].p);
		data[f].start=(start+f*s);
		data[f].stop=(start+f*s+s-1);
		to_matrix(&(data[f].m), data[f].p);
		data[f].pcount=pcount;
	}
	data[f-1].stop+=r;

	for(f=0;f<threads;f++) {
		pthread_attr_init(attr+f);
		pthread_attr_setdetachstate(attr+f, PTHREAD_CREATE_JOINABLE);
		pthread_create(th+f,attr+f, jac_engine_thread, (void *) (data+f));
		pthread_attr_destroy(attr+f);
	}

	sleep(1);
	for(f=0;f<threads;f++) {
		pthread_join(th[f], &status);
	}
	for(f=0;f<threads;f++) {
		free_matrix(data[f].m, data[f].pcount);
		free(data[f].p);
	}

return 0;
}





int texel_load_files(tuner_global *tuner){
//	char *sts_tests[]= { "texel/0.5-0.5.txt" };
//	int tests_setup[]= { 1, -1 };
// results from white pov
	char *sts_tests[]= { "../texel/1-0.txt", "../texel/0.5-0.5.txt", "../texel/0-1.txt" };

//	char *sts_tests[]= { "../texel/1-0.epd", "../texel/0.5-0.5.epd", "../texel/0-1.epd" };
	int8_t tests_setup[]= { 2, 1, 0, -1 };
	FILE * handle;
	char filename[256];
	char buffer[512];
	char fen[512];
	char *name;

	int i,l,x;
	char *xx;
	l=i=0;
	tuner->len=0;

	while((tests_setup[l]!=-1)&&(tuner->len<tuner->max_records)) {
		strcpy(filename, sts_tests[l]);
		if((handle=fopen(filename, "r"))==NULL) {
			printf("File %s is missing\n",filename);
			return -1;
		}
		while(!feof(handle)&&(tuner->len<tuner->max_records)) {
			xx=fgets(buffer, 511, handle);
			if(parseEPD(buffer, fen, NULL, NULL, NULL, NULL, NULL, &name)>0) {
				if(i%(tuner->nth+tuner->records_offset)==0) {
					setup_FEN_board(&tuner->boards[tuner->len], fen);
					tuner->phase[tuner->len]= eval_phase(&tuner->boards[tuner->len], tuner->pi);
					tuner->results[tuner->len]=tests_setup[l];
//					LOGGER_0("Load %d, %"PRIi8 ", %"PRIi8 "\n",tuner->phase[tuner->len],tuner->results[tuner->len],tests_setup[l]);
					tuner->len++;
				}
				free(name);
				i++;
			}
		}
		fclose(handle);
		l++;
	}
	printf("Imported %d from total %d of records\n", tuner->len, i);
	return 1;
}

int texel_test_init(tuner_global *tuner)
{
	tuner->matrix_var_backup=NULL;
	tuner->pi=NULL;
	tuner->m=NULL;
	tuner->jac=NULL;
	tuner->nvar=NULL;

/*
 *  boards - array of boards to be analyzed
 *  results - array of corresponding results
 *  phase - array of corresponing phase
 */

	tuner->boards=malloc(sizeof(board)*tuner->max_records);
	tuner->results=malloc(sizeof(int8_t)*tuner->max_records);
	tuner->phase=malloc(sizeof(uint8_t)*tuner->max_records);
	if((tuner->boards==NULL)||(tuner->results==NULL)) abort();

	tuner->pi=(personality *) init_personality(NULL);
	tuner->pcount=to_matrix(&(tuner->m), tuner->pi);

	tuner->matrix_var_backup=malloc(sizeof(int)*tuner->pcount*17);
	tuner->nvar=malloc(sizeof(double)*tuner->pcount*17*2);
	tuner->ivar=tuner->nvar+tuner->pcount*17;

return 0;
}

int texel_test_fin(tuner_global *tuner)
{
	if(tuner->nvar!=NULL) free(tuner->nvar);
	if(tuner->matrix_var_backup!=NULL) free(tuner->matrix_var_backup);
	free_matrix(tuner->m, tuner->pcount);
	if(tuner->pi!=NULL) free(tuner->pi);
	if(tuner->phase!=NULL) free(tuner->phase);
	if(tuner->results!=NULL) free(tuner->results);
	if(tuner->boards!=NULL) free(tuner->boards);
return 0;
}

void texel_test_loop_jac(tuner_global *tuner, char * base_name)
{
	int n,i,l;
	tuner_run *state;

	unsigned long long int totaltime;
	struct timespec start, end;

	int gen, perc, ccc;
	int *rnd, *rids, rrid, r1,r2;
	char nname[256];
	double fxh, fxh2=0, fxb;

// tuner->m maps personality in tuner->pi into variables used by tuner
	allocate_tuner(&state, tuner->pcount);
	
	rids=rnd=NULL;
// randomization init
	rnd=malloc(sizeof(int)*tuner->len);
	rids=malloc(sizeof(int)*tuner->len);

	for(i=0;i<tuner->len;i++){
		rnd[i]=i;
		rids[i]=i;
	}

	srand(time(NULL));

// looping over testing ...
	recompute_jac(tuner->jac, tuner->len, tuner->ivar, tuner->nvar, tuner->pcount);
	fxb=fxh=compute_loss_jac(tuner->results, tuner->len, rnd, 0, tuner->jac, tuner->ivar, tuner->nvar, tuner->pcount)/tuner->len;
	for(gen=1;gen<=tuner->generations;gen++) {

		for(i=0;i<tuner->len;i++){
			rrid=rand() %tuner->len;
			r1=rnd[i];
			r2=rnd[rrid];
			rnd[i]=r2;
			rnd[rrid]=r1;
			rids[r2]=i;
			rids[r1]=rrid;
		}

		{
			readClock_wall(&start);
			init_tuner_jac(state, tuner->nvar, tuner->pcount);
			sprintf(nname,"%s_%d_%d.xml",base_name, tuner->batch_len,gen);
			// compute loss prior tuning
			printf("GEN %d, blen %d, Initial loss of whole data =%f\n", gen, tuner->batch_len, fxh);
			LOGGER_0("GEN %d, blen %d, Initial loss of whole data =%f\n", gen, tuner->batch_len, fxh);

			// tuning part
			// in minibatches
			ccc=1;
			i=0;
			perc=10;
			while(tuner->len>i) {
				l= ((tuner->len-i)>tuner->batch_len) ? tuner->batch_len : tuner->len-i;
				p_tuner_jac(tuner->results, l, tuner->m, tuner, state, tuner->ivar, tuner->nvar, tuner->pcount, nname, rnd,i, ccc);
				ccc++;
				if((i*100/tuner->len) > perc) {
					printf("*");
					fflush(stdout);
					perc+=10;
				}
				i+=l;
			}
			fxh2=compute_loss_jac(tuner->results, tuner->len, rnd, 0, tuner->jac, tuner->ivar, tuner->nvar, tuner->pcount)/tuner->len;
			readClock_wall(&end);
			totaltime=diffClock(start, end);
			printf("\nGEN %d, blen %d,Time: %lldm:%llds.%lld\n", gen, tuner->batch_len, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000);
			printf("GEN %d, blen %d, Final loss of whole data =%f\n", gen, tuner->batch_len, fxh2);
			LOGGER_0("GEN %d, blen %d, Final loss of whole data =%f\n", gen, tuner->batch_len, fxh2);
			if(fxh2<fxb) {
				copy_vars_jac(0,15,tuner->ivar, tuner->nvar, tuner->pcount);
				jac_to_matrix(15, tuner->m, tuner->nvar, tuner->pcount);
				write_personality(tuner->pi, nname);
				fxb=fxh2;
			}
			fxh=fxh2;
		}
	}
	if(rnd!=NULL) free(rnd);
	if(rids!=NULL) free(rids);
	if(state!=NULL) free(state);
}

void texel_test_loop(tuner_global *tuner, char * base_name)
{
	int n,i,l;
	tuner_run *state;

	unsigned long long int totaltime;
	struct timespec start, end;

	int gen, perc, ccc;
	int *rnd, *rids, rrid, r1,r2;
	char nname[256];
	double fxh, fxh2=0, fxb;

// tuner->m maps personality in tuner->pi into variables used by tuner
	allocate_tuner(&state, tuner->pcount);
	
	rids=rnd=NULL;
// randomization init
	rnd=malloc(sizeof(int)*tuner->len);
	rids=malloc(sizeof(int)*tuner->len);

	for(i=0;i<tuner->len;i++){
		rnd[i]=i;
		rids[i]=i;
	}

	srand(time(NULL));

// looping over testing ...

	fxb=fxh=compute_loss(tuner->boards, tuner->results, tuner->phase, tuner->pi, tuner->len, rnd, 0)/tuner->len;
	for(gen=1;gen<=tuner->generations;gen++) {

		for(i=0;i<tuner->len;i++){
			rrid=rand() %tuner->len;
			r1=rnd[i];
			r2=rnd[rrid];
			rnd[i]=r2;
			rnd[rrid]=r1;
			rids[r2]=i;
			rids[r1]=rrid;
		}

		{
			readClock_wall(&start);
			init_tuner(state, tuner->m, tuner->pcount);
			sprintf(nname,"%s_%d_%d.xml",base_name, tuner->batch_len,gen);
			// compute loss prior tuning
			printf("GEN %d, blen %d, Initial loss of whole data =%f\n", gen, tuner->batch_len, fxh);
			LOGGER_0("GEN %d, blen %d, Initial loss of whole data =%f\n", gen, tuner->batch_len, fxh);

			// tuning part
			// in minibatches
			ccc=1;
			i=0;
			perc=10;
			while(tuner->len>i) {
				l= ((tuner->len-i)>tuner->batch_len) ? tuner->batch_len : tuner->len-i;
				p_tuner(tuner->boards, tuner->results, tuner->phase, tuner->pi, l, tuner->m, tuner, state, tuner->pcount, nname, rnd,i, ccc);
				ccc++;
				if((i*100/tuner->len) > perc) {
					printf("*");
					fflush(stdout);
					perc+=10;
				}
				i+=l;
			}
			fxh2=compute_loss(tuner->boards, tuner->results, tuner->phase, tuner->pi, tuner->len, rnd,0)/tuner->len;
			readClock_wall(&end);
			totaltime=diffClock(start, end);
			printf("\nGEN %d, blen %d,Time: %lldm:%llds.%lld\n", gen, tuner->batch_len, totaltime/60000000,(totaltime%60000000)/1000000,(totaltime%1000000)/1000);
			printf("GEN %d, blen %d, Final loss of whole data =%f\n", gen, tuner->batch_len, fxh2);
			LOGGER_0("GEN %d, blen %d, Final loss of whole data =%f\n", gen, tuner->batch_len, fxh2);
			if(fxh2<fxb) {
				backup_matrix_values(tuner->m, tuner->matrix_var_backup, tuner->pcount);
				write_personality(tuner->pi, nname);
				fxb=fxh2;
			}
			fxh=fxh2;
		}
	}
	if(rnd!=NULL) free(rnd);
	if(rids!=NULL) free(rids);
	if(state!=NULL) free(state);
}
void texel_test()
{
int i, *iv;
tuner_global tuner;
double fxb1, fxb2, fxb3, fxbj;

// initialize tuner
	tuner.max_records=10000000;
	texel_test_init(&tuner);

	tuner.generations=100;
	tuner.batch_len=1024;
	tuner.records_offset=2;
	tuner.nth=1000;
//	tuner.reg_la=1E-10;
	tuner.reg_la=0;
	tuner.small_c=1E-8;

// load position files and personality to seed tuning params 
	texel_load_files(&tuner);
	load_personality("../texel/pers.xml", tuner.pi);

// backup values from values to backup slot 16, normal tuning
	iv=tuner.matrix_var_backup+tuner.pcount*16;
	backup_matrix_values(tuner.m, iv, tuner.pcount);

// tranfer values from slot 16 to JAC based tuner - slot 0
	matrix_to_jac(16, tuner.m, tuner.ivar, tuner.pcount );
	matrix_to_jac(16, tuner.m, tuner.nvar, tuner.pcount );

// allocate jacobian and compute partial derivatives for each position loaded
	tuner.jac=NULL;
	tuner.jac=allocate_jac(tuner.len, tuner.pcount);
	if(tuner.jac!=NULL) {
		LOGGER_0("JACOBIAN population, positions %d, parameters %d\n", tuner.len, tuner.pcount);
		printf("JACOBIAN population, positions %d, parameters %d\n", tuner.len, tuner.pcount);
		populate_jac_pl(tuner.jac,tuner.boards, tuner.results, tuner.phase, tuner.pi, 0, tuner.len-1, tuner.m, tuner.pcount, 4);
//		populate_jac(tuner.jac,tuner.boards, tuner.results, tuner.phase, tuner.pi, 0, tuner.len-1, tuner.m, tuner.pcount);
		LOGGER_0("JACOBIAN populated\n");
		printf("JACOBIAN populated\n");
	}

// compute loss based on JACOBIAN with values from jac tuner slot 0
//	tuner.penalty=calc_dir_penalty_jac(tuner.nvar, tuner.m, &tuner, tuner.pcount);
	fxbj=(compute_loss_jac_dir(tuner.results, tuner.len, 0,tuner.jac, tuner.ivar, tuner.nvar, tuner.pcount))/tuner.len;
	LOGGER_0("INIT JAC loss %f\n", fxbj);
	printf("INIT JAC loss %f\n", fxbj);


// rmsprop
	LOGGER_0("RMSprop\n");
	tuner.method=2;
	tuner.la1=0.8;
	tuner.la2=0.9;
	tuner.rms_step=0.5;
	printf("RMSprop %d %f %f %f\n", tuner.batch_len, tuner.la1, tuner.la2, tuner.rms_step);
	texel_test_loop(&tuner, "../texel/pers_test_rms_");
// store best result into slot 1
	backup_matrix_values(tuner.m, tuner.matrix_var_backup+tuner.pcount*1, tuner.pcount);
/*
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*16, tuner.m, tuner.pcount);
// adadelta
	LOGGER_0("ADADelta\n");
	tuner.method=1;
	tuner.la1=0.8;
	tuner.la2=0.9;
	tuner.adadelta_step=5;
	printf("ADADelta %d %f %f %f\n", tuner.batch_len, tuner.la1, tuner.la2, tuner.adadelta_step);
	texel_test_loop(&tuner, "../texel/pers_test_adelta_");
// store best result into slot 2
	backup_matrix_values(tuner.m, tuner.matrix_var_backup+tuner.pcount*2, tuner.pcount);
*/
/*
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*16, tuner.m, tuner.pcount);
// adam
	LOGGER_0("ADAM\n");
	tuner.method=0;
	tuner.la1=0.9;
	tuner.la2=0.999;
	tuner.adam_step=3;
	printf("ADAM %d %f %f %f\n", tuner.batch_len, tuner.la1, tuner.la2, tuner.adam_step);
	texel_test_loop(&tuner, "../texel/pers_test_adam_");
// store best result into slot 3	
	backup_matrix_values(tuner.m, tuner.matrix_var_backup+tuner.pcount*3, tuner.pcount);
*/

// jac: copy values from slot 16 to slot 0. Slot 0 is temporary, after tuner run slot 15 contains best values
	copy_vars_jac(16,0,tuner.ivar, tuner.nvar, tuner.pcount);
	copy_vars_jac(16,15,tuner.ivar, tuner.nvar, tuner.pcount);

// adadelta JAC based
	LOGGER_0("ADADelta JAC\n");
	tuner.method=1;
	tuner.la1=0.9;
	tuner.la2=0.9;
	tuner.adadelta_step=1000;
	printf("ADADelta JAC %d %f %f %f\n", tuner.batch_len, tuner.la1, tuner.la2, tuner.adadelta_step);
//	texel_test_loop_jac(&tuner, "../texel/ptest_adeltaJ_");
// store best result into jac slot 2
	copy_vars_jac(15,2,tuner.ivar, tuner.nvar, tuner.pcount);

// rmsprop from JAC, restore initial values from slot 16
	copy_vars_jac(16,0,tuner.ivar, tuner.nvar, tuner.pcount);
	copy_vars_jac(16,15,tuner.ivar, tuner.nvar, tuner.pcount);

	LOGGER_0("RMSprop JAC\n");
	tuner.method=2;
	tuner.la1=0.8;
	tuner.la2=0.9;
	tuner.rms_step=0.5;
	printf("RMSprop JAC %d %f %f %f\n", tuner.batch_len, tuner.la1, tuner.la2, tuner.rms_step);
	texel_test_loop_jac(&tuner, "../texel/ptest_rmsJ_");

// store best result into slot 1
	copy_vars_jac(15,1,tuner.ivar, tuner.nvar, tuner.pcount);

	copy_vars_jac(16,0,tuner.ivar, tuner.nvar, tuner.pcount);
	copy_vars_jac(16,15,tuner.ivar, tuner.nvar, tuner.pcount);
// adam JAC
	LOGGER_0("ADAM JAC N\n");
	tuner.method=0;
	tuner.la1=0.8;
	tuner.la2=0.9;
	tuner.adam_step=1.0;
	printf("ADAM JAC %d %f %f %f\n", tuner.batch_len, tuner.la1, tuner.la2, tuner.adam_step);
//	texel_test_loop_jac(&tuner, "../texel/ptest_adamJ_");
// store best result into slot 3
	copy_vars_jac(15,3,tuner.ivar, tuner.nvar, tuner.pcount);

/*
 * Verifications?
 */

// verification run
	tuner.max_records=10000000;
	tuner.records_offset=0;
	tuner.nth=100;
//	tuner.diff_step=1000;
	tuner.reg_la=0;
	tuner.small_c=1E-8;

	texel_load_files(&tuner);

// init faze, loss based on evaluation
	LOGGER_0("INIT verification\n");
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*16, tuner.m, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	tuner.penalty=0;
	fxb1=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0));
//	LOGGER_0("INIT verification, FXB1 %f\n", fxb1);
//	printf("INIT verification, FXB1 %f\n", fxb1);
	fxb1=(fxb1+tuner.penalty)/tuner.len;
	LOGGER_0("INIT verification, loss %f %f %d\n", fxb1, tuner.penalty, tuner.len);
	printf("INIT verification, loss %f %f %d\n", fxb1, tuner.penalty, tuner.len);

/*
// rmsprop best values verification, normal loss calculation
	LOGGER_0("RMSprop verification\n");
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*1, tuner.m, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	tuner.penalty=0;
	fxb1=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0)+tuner.penalty)/tuner.len;
	LOGGER_0("RMSprop verification, loss %f\n", fxb1);
	printf("RMSprop verification, loss %f\n", fxb1);

// adadelta verification , normal loss calculation
	LOGGER_0("ADADelta verification\n");
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*2, tuner.m, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	tuner.penalty=0;
	fxb2=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0)+tuner.penalty)/tuner.len;
	LOGGER_0("ADADelta verification, loss %f\n", fxb2);
	printf("ADADelta verification, loss %f\n", fxb2);
*/
/*
// adam, normal loss calculation
	LOGGER_0("ADAM verification\n");
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*3, tuner.m, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	tuner.penalty=0;
	fxb3=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0))/tuner.len;
	LOGGER_0("ADAM verification, loss %f\n", fxb3);
	printf("ADAM verification, loss %f\n", fxb3);
*/

// compute jacobian for verification positions
	free_jac(tuner.jac);
	restore_matrix_values(tuner.matrix_var_backup+tuner.pcount*16, tuner.m, tuner.pcount);
	tuner.jac=NULL;
	tuner.jac=allocate_jac(tuner.len, tuner.pcount);
	if(tuner.jac!=NULL) {
		LOGGER_0("JACOBIAN population, positions %d, parameters %d\n", tuner.len, tuner.pcount);
		printf("JACOBIAN population, positions %d, parameters %d\n", tuner.len, tuner.pcount);
//		populate_jac(tuner.jac,tuner.boards, tuner.results, tuner.phase, tuner.pi, 0, tuner.len-1, tuner.m, tuner.pcount);
		populate_jac_pl(tuner.jac,tuner.boards, tuner.results, tuner.phase, tuner.pi, 0, tuner.len-1, tuner.m, tuner.pcount, 4);
		LOGGER_0("JACOBIAN populated\n");
		printf("JACOBIAN populated\n");
	}

// compute INIT loss JAC based, values from jac tuner
	copy_vars_jac(16,0,tuner.ivar, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty_jac(tuner.nvar, tuner.m, &tuner, tuner.pcount);
	fxbj=(compute_loss_jac_dir(tuner.results, tuner.len, 0,tuner.jac, tuner.ivar, tuner.nvar, tuner.pcount))/tuner.len;
	LOGGER_0("INIT JAC JAC, loss %f\n", fxbj);
	printf("INIT JAC JAC, loss %f %f %d\n", fxbj, tuner.penalty, tuner.len);

// rmsprop, loss normal, values from jac tuner
	LOGGER_0("RMSprop JAC verification\n");
// jac slot 1 to evaluate parameters
	jac_to_matrix(1, tuner.m, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	fxb1=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0))/tuner.len;
	LOGGER_0("RMSprop JAC verification, loss %f\n", fxb1);
	printf("RMSprop JAC verification, loss %f\n", fxb1);

// rmsprop, loss jac, values from jac tuner, from slot 1 to slot 0
	copy_vars_jac(1,0,tuner.ivar, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty_jac(tuner.nvar, tuner.m, &tuner, tuner.pcount);
	fxbj=(compute_loss_jac_dir(tuner.results, tuner.len, 0,tuner.jac, tuner.ivar, tuner.nvar, tuner.pcount))/tuner.len;
	LOGGER_0("RMSprop JAC JAC loss %f\n", fxbj);
	printf("RMSprop JAC JAC, loss %f\n", fxbj);

// adadelta, loss normal, values from jac tuner
	LOGGER_0("ADADelta JAC verification\n");
	jac_to_matrix(2, tuner.m, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	fxb1=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0))/tuner.len;
	LOGGER_0("ADADelta JAC verification, loss %f\n", fxb1);
	printf("ADADelta JAC verification, loss %f\n", fxb1);

// adadelta, loss jac, values from jac tuner, from slot 2 to slot 0
	copy_vars_jac(2,0,tuner.ivar, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty_jac(tuner.nvar, tuner.m, &tuner, tuner.pcount);
	fxb1=(compute_loss_jac_dir(tuner.results, tuner.len, 0,tuner.jac, tuner.ivar, tuner.nvar, tuner.pcount))/tuner.len;
	LOGGER_0("ADADelta JAC JAC verification, loss %f\n", fxb1);
	printf("ADADelta JAC JAC verification, loss %f\n", fxb1);

// adam, normal loss, JAC values
	LOGGER_0("ADAM JAC verification\n");
	jac_to_matrix(3, tuner.m, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty(tuner.m, &tuner, tuner.pcount);
	fxb1=(compute_loss_dir(tuner.boards, tuner.results, tuner.phase, tuner.pi, tuner.len, 0))/tuner.len;
	LOGGER_0("ADAM JAC verification, loss %f\n", fxb1);
	printf("ADAM JAC verification, loss %f\n", fxb1);

// adam, loss jac, values from jac tuner, from slot 3 to slot 0
	copy_vars_jac(3,0,tuner.ivar, tuner.nvar, tuner.pcount);
//	tuner.penalty=calc_dir_penalty_jac(tuner.nvar, tuner.m, &tuner, tuner.pcount);
	fxb1=(compute_loss_jac_dir(tuner.results, tuner.len, 0,tuner.jac, tuner.ivar, tuner.nvar, tuner.pcount))/tuner.len;
	LOGGER_0("ADAM JAC JAC verification, loss %f\n", fxb1);
	printf("ADAM JAC JAC verification, loss %f\n", fxb1);

	free_jac(tuner.jac);
	texel_test_fin(&tuner);
}
