 
#include "hash.h"
#include "generate.h"
#include "search.h"
#include "utils.h"
#include "globals.h"
#include "evaluate.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "randoms.h"
//#include "randoms2.h"

kmoves kmove_store[MAXPLY * KMOVES_WIDTH];

#define HASHBITS

BITVAR getRandom2(int *i)
{
	BITVAR ret;
	int l;
	size_t r;
	(*i)++;
	int rd = open("/dev/urandom", O_RDONLY);
	l=0;
	ret=0;
	while (l < 8)
	{
		ssize_t res = read(rd, &r, sizeof (uint8_t));
		if (res < 0)
		{
			// error, unable to read /dev/random
		}
		else {
			l += 1;
			ret<<=8;
			ret+=(uint8_t)r;
		}
	}
	close(rd);

//	ret=((unsigned long long) rand() << 33)^((unsigned long long) rand() <<16)^((unsigned long long) rand());
	return ret;

}

BITVAR getRandom(int *i)
{
	(*i)++;
	return RANDOMS[*i];
}

// sq, sd, pie = 768
// ep = 8
// sideKey = 1
// castle = 4

void initRandom()
{
int *y;
int sq,sd,pc,f, i;
	y=&i;
	i=-1;
		for(sq=0;sq<ER_SQUARE;sq++)
			for(sd=0;sd<ER_SIDE;sd++) {
				for(pc=0;pc<ER_PIECE;pc++){
					randomTable[sd][sq][pc]=getRandom(y);
//					printf("Hash rand: %d:%d:%d=%llx\n", sq,sd,pc,randomTable[sq][sd][pc]);
				}
			}
		sideKey=getRandom(y);
		for(sq=A1;sq<=H1;sq++) {
			epKey[sq]=getRandom(y);
			epKey[sq+8]=epKey[sq];
			epKey[sq+16]=epKey[sq];
			epKey[sq+24]=epKey[sq];
			epKey[sq+32]=epKey[sq];
			epKey[sq+40]=epKey[sq];
			epKey[sq+48]=epKey[sq];
			epKey[sq+56]=epKey[sq];
		}

		for(sd=0;sd<ER_SIDE;sd++) {
			castleKey[sd][NOCASTLE]=0;
			castleKey[sd][QUEENSIDE]=getRandom(y);
			castleKey[sd][KINGSIDE]=getRandom(y);
			castleKey[sd][BOTHSIDES]=castleKey[sd][QUEENSIDE]^castleKey[sd][KINGSIDE];
		}
}

BITVAR getKey(board *b)
{
BITVAR x;
BITVAR key;
int from;
                
	key=0;
	x = b->colormaps[WHITE];
	while (x) {
		from = LastOne(x);
		x=ClrNorm(from,x);
		key^=randomTable[WHITE][from][b->pieces[from]];
	}
	x = b->colormaps[BLACK];
	while (x) {
		from = LastOne(x);
		x=ClrNorm(from,x);
		key^=randomTable[BLACK][from][b->pieces[from]&PIECEMASK];
	}
	key^=castleKey[WHITE][b->castle[WHITE]];
	key^=castleKey[BLACK][b->castle[BLACK]];
	if(b->side==BLACK) key^=sideKey;
	if(b->ep!=-1) key^=epKey[b->ep]; 
	return key;
}

void setupRandom(board *b)
{
	b->key=getKey(b);
	b->positions[b->move-b->move_start]=b->key;
	b->posnorm[b->move-b->move_start]=b->norm;
}

/*
	if mated, then score propagated is -MATESCORE+current_DEPTH for in mate position
	we should store score into hash table modified by distance from current depth to depth the mate position occurred
 */

void storeHash(hashStore * hs, hashEntry * hash, int side, int ply, int depth, struct _statistics *s){
int i,c,q;
BITVAR f, hi;

//	return;
	s->hashStores++;
	
	f=hash->key%(BITVAR)hs->hashlen;
	hi=hash->key/(BITVAR)hs->hashlen;

	switch(isMATE(hash->value)) {
		case -1:
			hash->value-=ply;
			break;
		case  1:
			hash->value+=ply;
			break;
		default:
			break;
	}

	for(i=0;i<HASHPOS;i++) {
		if((hi==hs->hash[f].e[i].key)) {
// mame nas zaznam
			s->hashStoreHits++;
			s->hashStoreInPlace++;
			c=i;
			if((hs->hash[f].e[i].map!=hash->map)) s->hashStoreColl++;
			goto replace;
		}
	}
	q=9999999;
	for(i=0;i<HASHPOS;i++) {
		if((hs->hash[f].e[i].age!=hs->hashValidId)) {
			if(hs->hash[f].e[i].depth<q) {
				q=hs->hash[f].e[i].depth;
				c=i;
			}
		}
	}
	if(i<HASHPOS) goto replace;

	c=0;
	q=9999999;
	for(i=0;i<HASHPOS;i++) {
		if(hs->hash[f].e[i].depth<q) {
			q=hs->hash[f].e[i].depth;
			c=i;
		}
	}

replace:
	hs->hash[f].e[c].depth=hash->depth;
	hs->hash[f].e[c].value=hash->value;
	hs->hash[f].e[c].key=hi;
	hs->hash[f].e[c].bestmove=hash->bestmove;
	hs->hash[f].e[c].scoretype=hash->scoretype;
	hs->hash[f].e[c].age=(uint8_t)hs->hashValidId;
	hs->hash[f].e[c].map=hash->map;
}

void storeExactPV(hashStore * hs, BITVAR key, BITVAR map, tree_store * orig, int level){
int i,c,q,n,m;
BITVAR f, hi;
char b2[256], buff[2048];

	if(level>MAXPLY) {
		LOGGER_0("Error Depth: %d\n", level);
		abort();
	}

	f=key%(BITVAR)hs->hashPVlen;
	hi=key/(BITVAR)hs->hashPVlen;

//	LOGGER_0("ExPV: STORE key: 0x%08llX, f: 0x%08llX, hi: 0x%08llX, map: 0x%08llX, age: %d\n", key, f, hi, map, hs->hashValidId );

	for(i=0;i<16;i++) {
		if((hi==hs->pv[f].e[i].key)) {
// mame nas zaznam
			c=i;
			goto replace;
		}
	}
	for(i=0;i<16;i++) {
		if((hs->pv[f].e[i].age!=hs->hashValidId)) {
			c=i;
			goto replace;
		}
	}
//	if(i<16) goto replace;
	c=0;
replace:
//	LOGGER_0("ExPV: REPLACING key: c: %d, f: 0x%08llX, hi: 0x%08llX, map: 0x%08llX, age: %d\n",c, f,hs->pv[f].e[c].key, hs->pv[f].e[c].map, hs->pv[f].e[c].age );
	hs->pv[f].e[c].key=hi;
	hs->pv[f].e[c].age=(uint8_t)hs->hashValidId;
	hs->pv[f].e[c].map=map;
	for(n=level,m=0;n<=MAXPLY;n++,m++) {
		hs->pv[f].e[c].pv[m]=orig->tree[level][n];
	}
#if 0
	sprintf(buff,"!!! ");
	for(m=0;m<=20;m++) {
				sprintfMoveSimple(hs->pv[f].e[c].pv[m], b2);
				strcat(buff, b2);
				strcat(buff," ");
	}
	printf("%s\n",buff);
#endif
}

int restoreExactPV(hashStore * hs, BITVAR key, BITVAR map, int level, tree_store * rest){
int i,q,n,m,c;
BITVAR f, hi;

	if(level>MAXPLY) {
		LOGGER_0("Error Depth: %d\n", level);
		abort();
	}

	f=key%(BITVAR)hs->hashPVlen;
	hi=key/(BITVAR)hs->hashPVlen;

//	LOGGER_1("ExPV: RESTORING key: 0x%08llX, f: 0x%08llX, hi: 0x%08llX, map: 0x%08llX\n", key, f, hi, map );

	for(i=0;i<16;i++) {
		if((hi==hs->pv[f].e[i].key)&&(hs->pv[f].e[i].age==hs->hashValidId)&&(hs->pv[f].e[i].map==map)) {
// mame nas zaznam
			for(n=level+1,m=0;n<=MAXPLY;n++,m++) {
				rest->tree[level][n]=hs->pv[f].e[i].pv[m];
			}
			return 1;
		}
	}
	LOGGER_2("ExPV: NO restore!\n");
	for(c=0;c<16;c++) {
//		LOGGER_0("ExPV: DUMP key: c: %d, f: 0x%08llX, hi: 0x%08llX, map: 0x%08llX, age: %d\n",c, f,hs->pv[f].e[c].key, hs->pv[f].e[c].map, hs->pv[f].e[c].age );
		if(hi==hs->pv[f].e[c].key) {
//			LOGGER_0("ExPV: Key match\n");
			if(hs->pv[f].e[c].age!=hs->hashValidId) LOGGER_0("ExPV: wrong AGE\n");
			if(hs->pv[f].e[c].map!=map) LOGGER_0("ExPV: wrong MAP\n");
		}
	}
	return 0;
}

void storePVHash(hashStore *hs, hashEntry * hash, int ply, struct _statistics *s){
int i,c,q;
BITVAR f, hi;

//	return;
	s->hashStores++;

	f=hash->key%(BITVAR)hs->hashlen;
	hi=hash->key/(BITVAR)hs->hashlen;

	switch(isMATE(hash->value)) {
		case -1:
			hash->value-=ply;
			break;
		case  1:
			hash->value+=ply;
			break;
		default:
			break;
	}

	hash->scoretype=NO_NULL;
	hash->depth=0;
	for(i=0;i<HASHPOS;i++) {
		if((hi==hs->hash[f].e[i].key)) {
// mame nas zaznam
			c=i;
			if((hs->hash[f].e[i].map==hash->map)) {
				hash->scoretype=hs->hash[f].e[i].scoretype;
				hash->depth=hs->hash[f].e[i].depth;
			}
			goto replace;
		}
	}
	q=9999999;
	for(i=0;i<HASHPOS;i++) {
		if((hs->hash[f].e[i].age!=hs->hashValidId)) {
			if(hs->hash[f].e[i].depth<q) {
				q=hs->hash[f].e[i].depth;
				c=i;
			}
		}
	}
	if(i<HASHPOS) goto replace;
	c=0;
	q=9999999;
	for(i=0;i<HASHPOS;i++) {
		if(hs->hash[f].e[i].depth<q) {
			q=hs->hash[f].e[i].depth;
			c=i;
		}
	}

replace:
	hs->hash[f].e[c].depth=hash->depth;
	hs->hash[f].e[c].value=hash->value;
	hs->hash[f].e[c].key=hi;
	hs->hash[f].e[c].bestmove=hash->bestmove;
	hs->hash[f].e[c].scoretype=hash->scoretype;
	hs->hash[f].e[c].age=(uint8_t)hs->hashValidId;
	hs->hash[f].e[c].map=hash->map;
//	if(hash->bestmove==0) {
//		printf("error!\n");
//	}
}

int initHash(hashStore * hs){
int f,c;

	for(f=0;f<hs->hashlen;f++) {
		for(c=0; c< HASHPOS; c++) {
			hs->hash[f].e[c].depth=0;
			hs->hash[f].e[c].value=0;
			hs->hash[f].e[c].key=0;
			hs->hash[f].e[c].bestmove=0;
			hs->hash[f].e[c].scoretype=0;
			hs->hash[f].e[c].age=0;
			hs->hash[f].e[c].map=0;
		}
	}
	for(f=0;f<hs->hashPVlen;f++) {
		for(c=0; c< 16; c++) {
			hs->pv[f].e[c].key=0;
			hs->pv[f].e[c].age=0;
			hs->pv[f].e[c].map=0;
		}
	}
	hs->hashValidId=1;
	return 0;
}

int retrieveHash(hashStore *hs, hashEntry *hash, int side, int ply, int depth, int use_previous, struct _statistics *s)
{
int xx,i;
BITVAR f,hi;
		s->hashAttempts++;
		xx=0;

		f=hash->key%(BITVAR)hs->hashlen;
		hi=hash->key/(BITVAR)hs->hashlen;
		for(i=0; i< HASHPOS; i++) {
			if((hs->hash[f].e[i].key==hi)) {
				if((hs->hash[f].e[i].map!=hash->map)) xx=1;
				if((use_previous==0)&&(hs->hash[f].e[i].age!=hs->hashValidId)) continue;
				break;
			}
		}

		if(xx==1) {
			s->hashColls++;
			s->hashMiss++;
			return 0;
		}
		if(i==HASHPOS) {
			s->hashMiss++;
			return 0;
		}
		hash->depth=hs->hash[f].e[i].depth;
		hash->value=hs->hash[f].e[i].value;
		hash->bestmove=hs->hash[f].e[i].bestmove;
		hash->scoretype=hs->hash[f].e[i].scoretype;
		hash->age=hs->hash[f].e[i].age;
// update age aby bylo jasne, ze je to pouzito i ve stavajicim hledani
		if(depth<hash->depth) hs->hash[f].e[i].age=(uint8_t)hs->hashValidId;
		s->hashHits++;

		switch(isMATE(hash->value)) {
			case -1:
				hash->value+=ply;
				break;
			case  1:
				hash->value-=ply;
				break;
			default:
				break;
		}
		return 1;
}

int invalidateHash(hashStore *hs){
	hs->hashValidId++;
	if(hs->hashValidId>63) hs->hashValidId=0;
return 0;
}

int clear_killer_moves(){
int i,f;
kmoves *g;
	killer_moves=kmove_store;
	for(f=0;f<MAXPLY;f++) {
		g=&(killer_moves[f*KMOVES_WIDTH]);
		for(i=0;i<KMOVES_WIDTH;i++) {
			g->move=0;
			g->value=0;
			g++;
		}
	}
return 0;
}

// just 2 killers
int update_killer_move(int ply, MOVESTORE move, struct _statistics *s) {
kmoves *a, *b;
	a=&(killer_moves[ply*KMOVES_WIDTH]);
	if(a->move==move) return 1;
	b=a+1;
	*b=*a;
	a->move=move;
return 0;
}

int check_killer_move(int ply, MOVESTORE move, struct _statistics *s) {
kmoves *a, *b;
int i;
	a=&(killer_moves[ply*KMOVES_WIDTH]);
	for(i=0;i<KMOVES_WIDTH;i++) {
		if(a->move==move) return i+1;
		a++;
	}
	if(ply>2) {
// two plies shallower
		a=&(killer_moves[(ply-2)*KMOVES_WIDTH]);
		for(i=0;i<KMOVES_WIDTH;i++) {
			if(a->move==move) return i+1+KMOVES_WIDTH;
			a++;
		}
	}
return 0;
}

hashStore * allocateHashStore(int hashLen, int hashPVLen) {
hashStore * hs;

size_t hl, hp;

	hl=hashLen;
	hp=hashPVLen;
	hs = (hashStore *) malloc(sizeof(hashStore)*2 + sizeof(hashEntry_e)*hl + sizeof(hashEntryPV_e)*hp);
	hs->hashlen=hashLen;
	hs->hash = (hashEntry_e*) (hs+1);
	hs->hashPVlen=hashPVLen;
	hs->pv= (hashEntryPV_e*) (hs->hash+hashLen);
	initHash(hs);

return hs;
}

hashPawnStore * allocateHashPawnStore(int hashLen) {
hashPawnStore * hs;

size_t hl, hp;

	hl=hashLen;
	hs = (hashPawnStore *) malloc(sizeof(hashPawnStore)*2 + sizeof(hashPawnEntry_e)*hl);
	hs->hashlen=hashLen;
	hs->hash = (hashPawnEntry_e*) (hs+1);
	initPawnHash(hs);

return hs;
}

int freeHashStore(hashStore *hs)
{
	free(hs);
	return 1;
}

int freeHashPawnStore(hashPawnStore *hs)
{
	free(hs);
	return 1;
}

int generateRandomFile(char *n)
{
FILE *h;
unsigned long long i1, i2, i3, i4, i5, i6, i7, i8;
int q, y;

	h=fopen(n, "w");
	q=0;
	fprintf(h, "BITVAR RANDOMS[]= {\n");
	for(y=0;y<99;y++) {
			i1=getRandom(&q);
			i2=getRandom(&q);
			i3=getRandom(&q);
			i4=getRandom(&q);
			i5=getRandom(&q);
			i6=getRandom(&q);
			i7=getRandom(&q);
			i8=getRandom(&q);
			fprintf(h,"\t\t\t\t0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX,\n", i1, i2, i3, i4, i5, i6, i7, i8);
	}
	i1=getRandom(&q);
	i2=getRandom(&q);
	i3=getRandom(&q);
	i4=getRandom(&q);
	i5=getRandom(&q);
	i6=getRandom(&q);
	i7=getRandom(&q);
	i8=getRandom(&q);
	fprintf(h,"\t\t\t\t0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX, 0x%08llX\n", i1, i2, i3, i4, i5, i6, i7, i8);
	fprintf(h, "\t\t};");
	fclose(h);

	return 0;
}

void storePawnHash(hashPawnStore * hs, hashPawnEntry * hash, struct _statistics *s){
int i,c,q;
BITVAR f, hi;

	s->hashPawnStores++;	
	f=hash->key%(BITVAR)hs->hashlen;
	hi=hash->key/(BITVAR)hs->hashlen;

	for(i=0;i<HASHPAWNPOS;i++) {
		if((hi==hs->hash[f].e[i].key)) {
// mame nas zaznam
			s->hashPawnStoreHits++;
			s->hashPawnStoreInPlace++;
			c=i;
			if((hs->hash[f].e[i].map!=hash->map)) s->hashPawnStoreColl++;
			goto replace;
		}
	}
	for(i=0;i<HASHPAWNPOS;i++) {
		if((hs->hash[f].e[i].age!=hs->hashValidId)) {
				c=i;
		}
	}
	if(i<HASHPAWNPOS) goto replace;
	c=0;
replace:
	hs->hash[f].e[c].value=hash->value;
	hs->hash[f].e[c].age=(uint8_t)hs->hashValidId;
	hs->hash[f].e[c].map=hash->map;
	hs->hash[f].e[c].key=hi;
}

int invalidatePawnHash(hashPawnStore *hs){
	hs->hashValidId++;
	if(hs->hashValidId>63) hs->hashValidId=0;
return 0;
}

int initPawnHash(hashPawnStore * hs){
int f,c;

	for(f=0;f<hs->hashlen;f++) {
		for(c=0; c< HASHPAWNPOS; c++) {
			hs->hash[f].e[c].key=0;
			hs->hash[f].e[c].age=0;
			hs->hash[f].e[c].map=0;
		}
	}
	hs->hashValidId=1;
	return 0;
}

int retrievePawnHash(hashPawnStore *hs, hashPawnEntry *hash, struct _statistics *s)
{
int xx,i;
BITVAR f,hi;
	s->hashPawnAttempts++;
	xx=0;
	f=hash->key%(BITVAR)hs->hashlen;
	hi=hash->key/(BITVAR)hs->hashlen;
	for(i=0; i< HASHPAWNPOS; i++) {
		if((hs->hash[f].e[i].key==hi)) {
			if((hs->hash[f].e[i].map!=hash->map)) xx=1;
				break;
			}
		}
	if(xx==1) {
		s->hashPawnColls++;
		s->hashPawnMiss++;
		return 0;
	}
	if(i==HASHPAWNPOS) {
		s->hashPawnMiss++;
		return 0;
	}
	hash->value=hs->hash[f].e[i].value;
	hash->age=hs->hash[f].e[i].age;
	s->hashPawnHits++;
	return 1;
}

void setupPawnRandom(board *b)
{
	b->pawnkey=getPawnKey(b);
}

BITVAR getPawnKey(board *b)
{
BITVAR x;
BITVAR key;
int from;
	key=0;
	x = b->colormaps[WHITE]&(b->maps[PAWN]|b->maps[KING]);
	while (x) {
		from = LastOne(x);
		x=ClrNorm(from,x);
		key^=randomTable[WHITE][from][b->pieces[from]];
	}
	x = b->colormaps[BLACK]&(b->maps[PAWN]|b->maps[KING]);
	while (x) {
		from = LastOne(x);
		x=ClrNorm(from,x);
		key^=randomTable[BLACK][from][b->pieces[from]&PIECEMASK];
	}
	return key;
}
