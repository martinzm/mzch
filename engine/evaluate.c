/*
 *
 * 
 *
 */
 

#include <stdlib.h>
#include <string.h>
#include "evaluate.h"
#include "movgen.h"
#include "attacks.h"
#include "bitmap.h"
#include "pers.h"
#include "utils.h"
#include "globals.h"
#include "assert.h"

int eval_phase2(board *b){
int i;
// 256 -- pure beginning, 0 -- total ending
	i=3*BitCount(b->maps[PAWN]); 
	i+=10*BitCount(b->maps[KNIGHT]);
	i+=12*BitCount(b->maps[BISHOP]);
	i+=15*BitCount(b->maps[ROOK]);
	i+=30*BitCount(b->maps[QUEEN]);
	//
return i;
}

uint8_t eval_phase(board *b, personality *p){
int i,i1,i2,i3,i4,i5, tot, faze, fz2, q;
int vaha[]={0,1,1,3,6};
int nc[]={16,4,4,4,2};

int bb, wb, be, we, stage;
int pw, pb, nw, nb, bwl, bwd, bbl, bbd, rw, rb, qw, qb;

// 256 -- pure beginning, 0 -- total ending
	if(b->mindex_validity==1) {
		faze=p->mat_faze[b->mindex];
	}
	else {
		tot=nc[PAWN]*vaha[PAWN]+nc[KNIGHT]*vaha[KNIGHT]+nc[BISHOP]*vaha[BISHOP]+nc[ROOK]*vaha[ROOK]+nc[QUEEN]*vaha[QUEEN];
		i1=BitCount(b->maps[PAWN])	*vaha[PAWN];
		i2=BitCount(b->maps[KNIGHT])	*vaha[KNIGHT];
		i3=BitCount(b->maps[BISHOP])	*vaha[BISHOP];
		i4=BitCount(b->maps[ROOK])	*vaha[ROOK];
		i5=BitCount(b->maps[QUEEN])	*vaha[QUEEN];
		i=i1+i2+i3+i4+i5;
		q=Min(i, tot);
		faze=q*255/tot;
	}
return (uint8_t)faze;
}

/*
 * vygenerujeme bitmapy moznych tahu pro N, B, R, Q dane strany
 */

int simple_pre_movegen(board *b, attack_model *a, int side)
{
int f, from, pp, st, en, add;
BITVAR x, q;

	if(side==BLACK) {
		st=ER_PIECE|BLACKPIECE;
		en=PAWN|BLACKPIECE;
		add=BLACKPIECE;
	} else {
		add=0;
		st=ER_PIECE;
		en=PAWN;
	}
	for(f=st;f>=en;f--) {
		a->pos_c[f]=-1;
	}
//	a->att_by_side[side]=0;
	q=0;

// rook
	x = (b->maps[ROOK]&b->colormaps[side]);
	pp=ROOK+add;
	while (x) {
		from = LastOne(x);
//		pp=b->pieces[from];
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q|=a->mvs[from] = (RookAttacks(b, from));
		ClrLO(x);
	}
// bishop
	x = (b->maps[BISHOP]&b->colormaps[side]);
	pp=BISHOP+add;
	while (x) {
		from = LastOne(x);
//		pp=b->pieces[from];
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q|=a->mvs[from] = (BishopAttacks(b, from));
		ClrLO(x);
	}
// knights
	x = (b->maps[KNIGHT]&b->colormaps[side]);
	pp=KNIGHT+add;
	while (x) {
		from = LastOne(x);
//		pp=b->pieces[from];
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q|=a->mvs[from]  = (attack.maps[KNIGHT][from]);
		ClrLO(x);
	}
// queen
	x = (b->maps[QUEEN]&b->colormaps[side]);
	pp=QUEEN+add;
	while (x) {
		from = LastOne(x);
//		pp=b->pieces[from];
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q|=a->mvs[from] = (QueenAttacks(b, from));
		ClrLO(x);
	}
// utoky pescu
	if(side==WHITE) a->pa_at[WHITE]=WhitePawnAttacks(b);
	else a->pa_at[BLACK]=BlackPawnAttacks(b);

	a->att_by_side[side]=q|a->pa_at[side];
return 0;
}

/*
 * pro mobilitu budeme pocitat pouze pole
 * - ktera nejsou napadena nepratelskymi pesci
 * - nejsou obsazena mymi figurami
 */

int make_model(board *b, attack_model *a, personality *p)
{
int from, pp, m, s, z;
BITVAR x, q, v, n;

//	printBoardNice(b);
//	boardCheck(b);
// rook
	x = (b->maps[ROOK]);
	a->specs[0][ROOK].sqr_b=a->specs[1][ROOK].sqr_b=0;
	a->specs[0][ROOK].sqr_e=a->specs[1][ROOK].sqr_e=0;
	while (x) {
		from = LastOne(x);
		pp=b->pieces[from];
		s=(pp&BLACKPIECE)!=0;
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q=a->mvs[from] = (RookAttacks(b, from));
		a->att_by_side[s]|=q;
		m=a->me[from].pos_att_tot=BitCount(q & (~b->colormaps[s]) & (~a->pa_at[s^1]));
		a->me[from].pos_mob_tot_b=p->mob_val[0][s][ROOK][m];
		a->me[from].pos_mob_tot_e=p->mob_val[1][s][ROOK][m];
		a->sq[from].sqr_b=p->piecetosquare[0][s][ROOK][from];
		a->sq[from].sqr_e=p->piecetosquare[1][s][ROOK][from];
		z=getRank(from);
		if(((s==WHITE)&&(z==6))||((s==BLACK)&&(z==1))) {
			a->specs[s][ROOK].sqr_b+=p->rook_on_seventh[0];
			a->specs[s][ROOK].sqr_e+=p->rook_on_seventh[1];
		}

		n=attack.file[from];
		v = (s==0) ? n&b->maps[PAWN]&attack.uphalf[from] : n&b->maps[PAWN]&attack.downhalf[from];
		if(v==0) {
			a->specs[s][ROOK].sqr_b+=p->rook_on_open[0];
			a->specs[s][ROOK].sqr_e+=p->rook_on_open[1];
		}

		else if((v&b->colormaps[s])==0) {
				a->specs[s][ROOK].sqr_b+=p->rook_on_semiopen[0];
				a->specs[s][ROOK].sqr_e+=p->rook_on_semiopen[1];
		}

		ClrLO(x);
	}
// bishop
	x = (b->maps[BISHOP]);
	while (x) {
		from = LastOne(x);
		pp=b->pieces[from];
		s=(pp&BLACKPIECE)>>3;
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q=a->mvs[from] = (BishopAttacks(b, from));
		a->att_by_side[s]|=q;
		m=a->me[from].pos_att_tot=BitCount(q & (~b->colormaps[s]) & (~a->pa_at[s^1]));
		a->me[from].pos_mob_tot_b=p->mob_val[0][s][BISHOP][m];
		a->me[from].pos_mob_tot_e=p->mob_val[1][s][BISHOP][m];
		a->sq[from].sqr_b=p->piecetosquare[0][s][BISHOP][from];
		a->sq[from].sqr_e=p->piecetosquare[1][s][BISHOP][from];
		ClrLO(x);
	}
// knights
	x = (b->maps[KNIGHT]);
	while (x) {
		from = LastOne(x);
		pp=b->pieces[from];
		s=(pp&BLACKPIECE)>>3;
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q=a->mvs[from]  = (attack.maps[KNIGHT][from]);
		a->att_by_side[s]|=q;
		m=a->me[from].pos_att_tot=BitCount(q & (~b->colormaps[s]) & (~a->pa_at[s^1]));
		a->me[from].pos_mob_tot_b=p->mob_val[0][s][KNIGHT][m];
		a->me[from].pos_mob_tot_e=p->mob_val[1][s][KNIGHT][m];
		a->sq[from].sqr_b=p->piecetosquare[0][s][KNIGHT][from];
		a->sq[from].sqr_e=p->piecetosquare[1][s][KNIGHT][from];
		ClrLO(x);
	}
// queen
	x = (b->maps[QUEEN]);
	while (x) {
		from = LastOne(x);
		pp=b->pieces[from];
		s=(pp&BLACKPIECE)>>3;
		a->pos_c[pp]++;
		a->pos_m[pp][a->pos_c[pp]]=from;
		q=a->mvs[from] = (QueenAttacks(b, from));
		a->att_by_side[s]|=q;
		m=a->me[from].pos_att_tot=BitCount(q & (~b->colormaps[s]) & (~a->pa_at[s^1]));
		a->me[from].pos_mob_tot_b=p->mob_val[0][s][QUEEN][m];
		a->me[from].pos_mob_tot_e=p->mob_val[1][s][QUEEN][m];
		a->sq[from].sqr_b=p->piecetosquare[0][s][QUEEN][from];
		a->sq[from].sqr_e=p->piecetosquare[1][s][QUEEN][from];
		ClrLO(x);
	}
//	boardCheck(b);
return 0;
}

int make_pawn_model(board *b, attack_model *a, personality *p) {

int from, pp, s, cc;
BITVAR x, n, ob, sb, bc, dd, from_b, w_max, b_max, b1, b2, w1, w2, fin[2], xx, x_f[2], x_ff[2], x_p[2], t, nt, ntt, z, prot, tx[2];
int pathlen;

	a->pos_c[PAWN]=-1;
	a->pos_c[PAWN|BLACKPIECE]=-1;
	x = tx[0] = b->maps[PAWN]&b->colormaps[WHITE];

// blocked - normalni pesec
// doubled - blokuje mne vlastni pesec
// passed - zadny nepratelsky pesec nemuze p sebrat a ten muze dojit az do damy
// weak - pesec ktereho neni mozno branit vlastnimi pesci
	// isolated - nema po stranach vlastni pesce
	// backward - weak jenz po ceste muze byt sebran nepratelskym pescem

	w_max=(b->maps[PAWN])|((a->pa_at[BLACK])&(~a->pa_at[WHITE]));
	b_max=(b->maps[PAWN])|((a->pa_at[WHITE])&(~a->pa_at[BLACK]));
	
	x_f[WHITE]=FillNorth(b->maps[PAWN]&b->colormaps[WHITE],~w_max, 0);
	x_f[BLACK]=FillSouth(b->maps[PAWN]&b->colormaps[BLACK],~b_max, 0);
	x_ff[WHITE]=(x_f[WHITE]|(b->maps[PAWN]&b->colormaps[WHITE]))<<8;
	x_ff[BLACK]=(x_f[BLACK]|(b->maps[PAWN]&b->colormaps[BLACK]))>>8;
	x_p[WHITE]=x_f[WHITE] & attack.rank[A8];
	x_p[BLACK]=x_f[BLACK] & attack.rank[A1];
	
// x_f path to stop
// x_ff path to stop including stop
// x_p which files reached promotion rank?

	w1=(x_f[WHITE] &(~FILEH))<<9;
	w2=(x_f[WHITE] &(~FILEA))<<7;
	b1=(x_f[BLACK] &(~FILEH))>>7;
	b2=(x_f[BLACK] &(~FILEA))>>9;

// kteri pesci mohou byt chraneni?
	fin[WHITE] = (b->maps[PAWN]&b->colormaps[WHITE]) & (w1|w2);
	fin[BLACK] = (b->maps[PAWN]&b->colormaps[BLACK]) & (b1|b2);

	for(s=WHITE;s<=BLACK;s++) {
		x = sb = b->maps[PAWN]&b->colormaps[s];
		pp=PAWN;
		if(s==BLACK) pp=PAWN|BLACKPIECE;

		while (x) {
/*
 * reasons for stop
 * promotion true in x_p
 * pawn enemy/mine x_ff 
 * attacked square
 */
			from = LastOne(x);

			a->pos_c[pp]++;
			a->pos_m[pp][a->pos_c[pp]]=from;
			a->sq[from].sqr_b=p->piecetosquare[0][s][PAWN][from];
			a->sq[from].sqr_e=p->piecetosquare[1][s][PAWN][from];
			a->me[from].pos_mob_tot_b = a->me[from].pos_mob_tot_e = a->me[from].pos_att_tot=0;
// disabling pawn scoring
#if 0
			t = x_f[s];
			n = attack.passed_p[s][from]; // forward span
			dd = attack.file[from];
			z=dd&n; // path to promotion from from
			nt=z&t; // is path to stop the same as to path to promotion?
			from_b=normmark[from];
// z contains path to promotion square
// t contains path forward to stop point
			if(((nt)==z)){
				pathlen=BitCount(nt);
// max index 0-5, path is minimum 1 square, 6 at max
				a->sq[from].sqr_b+=p->passer_bonus[0][s][pathlen-1];
				a->sq[from].sqr_e+=p->passer_bonus[1][s][pathlen-1];
				fin[s]|=(from_b);
			} else {
// get blocker - could be pawn or just attack from pawn
				ntt=x_ff[s]&z;
				//blockers
				// pawns in my way?
				if(ntt &b->maps[PAWN]) {
					// blocked...
					if(ntt & sb) {
						// doubled?
						a->sq[from].sqr_b+=p->doubled_penalty[0];
						a->sq[from].sqr_e+=p->doubled_penalty[1];
					} else {
						// blocked by opposite pawn
						// how far is blocker? 0 to 4 squares
						pathlen=BitCount(ntt)-1;
						a->sq[from].sqr_b+=p->pawn_blocked_penalty[0][s][pathlen];
						a->sq[from].sqr_e+=p->pawn_blocked_penalty[1][s][pathlen];
					}
				} else {
// stop square attacked without proper pawn protection from my side
// blocked by path forward attacked... 0 to 4 squares
					pathlen=BitCount(ntt)-1;
					a->sq[from].sqr_b+=p->pawn_stopped_penalty[0][s][pathlen];
					a->sq[from].sqr_e+=p->pawn_stopped_penalty[1][s][pathlen];
				}
// have pawn protection?
				n = attack.isolated_p[from];
				prot=(n&attack.rank[from])|attack.pawn_att[s^1][from];
				prot&=sb;
				if(prot) {
					cc=BitCount(prot);
					a->sq[from].sqr_b+=p->pawn_protect[0]*cc;
					a->sq[from].sqr_e+=p->pawn_protect[1]*cc;
				} else {
// weak
// muzu byt chranen pesci zezadu?
//isolated?
					bc=n&sb;
					if(!bc) {
						a->sq[from].sqr_b+=p->isolated_penalty[0];
						a->sq[from].sqr_e+=p->isolated_penalty[1];
//						fin[s]|=(from_b); //???
					} else {
//backward
						if(!(from_b&fin[s])) {
							a->sq[from].sqr_b+=p->backward_penalty[0];
							a->sq[from].sqr_e+=p->backward_penalty[1];
//							a->sq[from].sqr_b+=100;
//							a->sq[from].sqr_e+=200;
// can it be fixed? resp. muzu se dostat k nekomu kdo mne muze chranit?
							xx=(((x_f[s]& dd & (~FILEA))>>1) | ((x_f[s]& dd & (~FILEH))<<1));
							if(xx&sb) {
// it can,
								a->sq[from].sqr_b-=p->backward_penalty[0];
								a->sq[from].sqr_e-=p->backward_penalty[1];
								a->sq[from].sqr_b+=p->backward_fix_penalty[0];
								a->sq[from].sqr_e+=p->backward_fix_penalty[1];
							}
						}
					}
				}
			}

// fix material value
			if(from_b&(FILEA|FILEH)) {
				a->sq[from].sqr_b+=p->pawn_ah_penalty[0];
				a->sq[from].sqr_e+=p->pawn_ah_penalty[1];
			}
#endif
			ClrLO(x);
		}
	}
	return 0;
}

/*
 * Vygenerujeme vsechny co utoci na krale
 * vygenerujeme vsechny PINy - tedy ty kteri blokuji utok na krale
 * vygenerujeme vsechny RAYe utoku na krale
 */

int eval_king_checks(board *b, king_eval *ke, personality *p, int side)
{
BITVAR cr2, di2, c2, d2, c, d, c3, d3, ob;

int from, ff, o, ee;
BITVAR pp;

//		x = (b->maps[KING]) & b->colormaps[side];
//		from = LastOne(x);

		from=b->king[side];

//		s=side;
		o=side^1;

// find potential attackers - get rays, and check existence of them
		cr2=di2=0;
// vert/horiz rays
		c =ke->cr_all_ray = attack.maps[ROOK][from];
// vert/horiz attackers
		c2=c & (b->maps[ROOK]|b->maps[QUEEN])&(b->colormaps[o]);
// diag rays
		d =ke->di_all_ray = attack.maps[BISHOP][from];
// diag attackers
		d2=d & (b->maps[BISHOP]|b->maps[QUEEN])&(b->colormaps[o]);

#if 0
		printBoardNice(b);
		printmask(c,"C");
		printmask(c2,"C2");
		printmask(d,"D");
		printmask(d2,"D2");
#endif

// if it can hit king, find nearest piece, blocker?
// pins might contain false as there can be other piece between pin and distant attacker
// rook/queen
		ke->cr_pins = 0;
		ke->cr_attackers = 0;
		ke->cr_att_ray = 0;
		ke->cr_blocker_ray = 0;
		
// iterate attackers
		if(c2){
			while(c2) {
				ff = LastOne(c2);
// get line between square and attacker
				cr2=attack.rays_int[from][ff];
// check if there is piece in that line, that blocks the attack
				c3=cr2 & b->norm;

// determine status
				switch (BitCount(c3)) {
// just 1 means pin
				case 1:
					ee = LastOne(c3);
					ke->blocker_ray[ee]=(cr2|normmark[ff]);
					ke->cr_pins |=c3;
// je to jeste na neco???
					ke->cr_blocker_ray|=(cr2|normmark[ff]);
					break;
// 0 means attacked
				case 0:
					ke->cr_attackers |= normmark[ff];
					ke->cr_att_ray|=cr2;
					break;
				case 2:
// 2 means no attack no pin, with one exception
// pawn can be subject of e.p. In that case 2 pawns is just one blocker
					pp=c3&b->maps[PAWN]&attack.rank[from];
// obe figury jsou pesci?
					if((!(pp^c3)) && (b->ep!=-1)) {
					BITVAR aa;
						aa=(attack.ep_mask[b->ep])&b->colormaps[o];
						ob=(c3 & normmark[b->ep])&b->colormaps[side];
						if((aa!=0)&&(ob!=0)) {
//							ke->cr_pins |=ob;
//							ke->cr_blocker_piece |=c3;
							ke->cr_pins |=c3;
//							ee = LastOne(c3);
							ke->cr_blocker_ray|=(cr2|normmark[ff]);
//							ke->cr_blocker_ray=(rays_int[from][ee]|normmark[ff]);
						}
					}
					break;

				default:
// more than 2 means no attack no pin
					break;
				}
				ClrLO(c2);
			}
		}
// bishop/queen
		ke->di_pins = 0;
		ke->di_attackers = 0;
		ke->di_att_ray = 0;
		ke->di_blocker_ray = 0;
		if(d2){
			while(d2) {
				ff = LastOne(d2);
				di2=attack.rays_int[from][ff];
				d3=di2 & b->norm;
				switch (BitCount(d3)) {
				case 1:
					ee = LastOne(d3);
					ke->blocker_ray[ee]=(di2|normmark[ff]);
					ke->di_pins |=d3;
//???
					ke->di_blocker_ray|=(di2|normmark[ff]);
					break;
				case 0:
					ke->di_attackers |= normmark[ff];
					ke->di_att_ray|=di2;
					break;
				}
				ClrLO(d2);
			}
		}

// incorporate knights
		ke->kn_pot_att_pos=attack.maps[KNIGHT][from];
		ke->kn_attackers=ke->kn_pot_att_pos & b->maps[KNIGHT] & b->colormaps[o];
//		ke->kn_attackers=attack.maps[KNIGHT][from] & b->maps[KNIGHT] & b->colormaps[o];
//incorporate pawns
		ke->pn_pot_att_pos=attack.pawn_att[side][from];
		ke->pn_attackers=ke->pn_pot_att_pos & b->maps[PAWN] & b->colormaps[o];
		ke->attackers=ke->cr_attackers | ke->di_attackers | ke->kn_attackers | ke->pn_attackers;

#if 0
//	if((ke->di_pins!=ke->di_blocker_piece)||(ke->cr_pins!=ke->cr_blocker_piece)) {
		printBoardNice(b);
		printmask(ke->cr_attackers,"cr attackers");
		printmask(ke->di_attackers,"di attackers");
		printmask(ke->kn_attackers,"kn attackers");
		printmask(ke->pn_attackers,"pn attackers");
		printmask(ke->attackers,"attackers");
		printmask(ke->cr_all_ray,"CR all rays");
		printmask(ke->cr_att_ray,"CR att rays");
		printmask(ke->di_all_ray,"DI all rays");
		printmask(ke->di_att_ray,"DI att rays");
		printmask(ke->cr_pins,"CR pins");
		printmask(ke->di_pins,"DI pins");
		printmask(ke->kn_pot_att_pos, "kn pot att pos");
		printmask(ke->pn_pot_att_pos, "pn pot att pos");
//		printmask(ke->di_blocker_piece, "DI blocker piece");
		printmask(ke->di_blocker_ray, "DI blocker ray");
//		printmask(ke->cr_blocker_piece, "CR blocker piece");
		printmask(ke->cr_blocker_ray, "CR blocker ray");
//	}
#endif

	return 0;
}

int eval_king_checks_all(board *b, attack_model *a)
{
	eval_king_checks(b, &(a->ke[WHITE]), NULL, WHITE);
	eval_king_checks(b, &(a->ke[BLACK]), NULL, BLACK);
return 0;
}

// def - nizsi cislo radku nizsi trest
// att - nizsi cislo vyssi trest


// fixme predelat evaluaci shieldu na distance based...

int eval_w_sh_pawn2(board *b, attack_model *a, personality *p, BITVAR wdef, BITVAR watt, int file, int *eb, int *ee)
{
BITVAR f, bb, ww;
int wp=6, bp=6;
	f=attack.file[A1+file];
	bb=watt&f;
	ww=wdef&f;

//	printmask(f, "W_f");
//	printmask(bb, "W_BB");
//	printmask(ww, "W_WW");

	if(bb&b->maps[PAWN]&b->colormaps[BLACK]) bp=BitCount(bb);
	if(ww&b->maps[PAWN]&b->colormaps[WHITE]) wp=BitCount(ww);
	if(bp<wp) wp=6;
	assert((bp<=6)&&(bp>=1));
	assert((wp<=6)&&(wp>=1));
	*eb=p->king_s_pdef[0][WHITE][wp-1]+p->king_s_patt[0][WHITE][bp-1];
	*ee=p->king_s_pdef[1][WHITE][wp-1]+p->king_s_patt[1][WHITE][bp-1];
	return 0;
}

int eval_b_sh_pawn2(board *b, attack_model *a, personality *p, BITVAR bdef, BITVAR batt, int file, int *eb, int *ee)
{
BITVAR f, bb, ww;
int wp=6, bp=6;
	f=attack.file[A1+file];
	ww=batt&f;
	bb=bdef&f;
	if(ww&b->maps[PAWN]&b->colormaps[WHITE]) wp=BitCount(ww);
	if(bb&b->maps[PAWN]&b->colormaps[BLACK]) bp=BitCount(bb);
	if(wp<bp) bp=6;
	assert((bp<=6)&&(bp>=1));
	assert((wp<=6)&&(wp>=1));
	*eb=p->king_s_pdef[0][BLACK][bp-1]+p->king_s_patt[0][BLACK][wp-1];
	*ee=p->king_s_pdef[1][BLACK][bp-1]+p->king_s_patt[1][BLACK][wp-1];
	return 0;
}

int eval_king(board *b, attack_model *a, personality *p)
{
// zatim pouze pins a incheck
BITVAR x, q, mv;
int from, pp, s, m, to, ws, bs, r, r1_b, r1_e, r2_b, r2_e, rb, re;
BITVAR  w_oppos, b_oppos, w_my, b_my;

	a->specs[0][KING].sqr_b=a->specs[1][KING].sqr_b=0;
	a->specs[0][KING].sqr_e=a->specs[1][KING].sqr_e=0;

	x = (b->maps[KING]);
	while (x) {
		from = LastOne(x);
		pp=b->pieces[from];
		s=(pp&BLACKPIECE)>>3;
//		eval_king_checks(b, &(a->ke[s]), p, s);
//		eval_king_checks_all(b, a);
		q=0;

// king mobility, spocitame vsechna pole kam muj kral muze (tj. krome vlastnich figurek a poli na ktere utoci nepratelsky kral
// a poli ktera jsou napadena cizi figurou
		mv = (attack.maps[KING][from]) & (~b->colormaps[s]) & (~attack.maps[KING][b->king[s^1]]);
		while (mv) {
			to = LastOne(mv);
			if(!AttackedTo_B(b, to, s)) {
				q|=normmark[to];
			}
			ClrLO(mv);
		}
		m=a->me[from].pos_att_tot=BitCount(q);
		a->me[from].pos_mob_tot_b=p->mob_val[0][s][KING][m];
		a->me[from].pos_mob_tot_e=p->mob_val[1][s][KING][m];
		a->sq[from].sqr_b=p->piecetosquare[0][s][KING][from];
		a->sq[from].sqr_e=p->piecetosquare[1][s][KING][from];
		ClrLO(x);
	}
// evaluate shelter
// left/right just consider pawns on three outer files
// when in center
// x_oppos - nejblizsi utocici pesec
// x_my nejblizsi branici pesec

	ws=getFile(b->king[WHITE]);
	bs=getFile(b->king[BLACK]);
	
	w_oppos=FillNorth(attack.rank[b->king[WHITE]],~(b->maps[PAWN]&b->colormaps[BLACK]), 0);
	w_oppos|= ((w_oppos|attack.rank[b->king[WHITE]])<<8);
	b_oppos=FillSouth(attack.rank[b->king[BLACK]], ~(b->maps[PAWN]&b->colormaps[WHITE]), 0);
	b_oppos|= ((b_oppos|attack.rank[b->king[BLACK]])>>8);
	w_my=FillNorth(attack.rank[b->king[WHITE]],~(b->maps[PAWN]&b->colormaps[WHITE]), 0);
	w_my|= ((w_my|attack.rank[b->king[WHITE]])<<8);
	b_my=FillSouth(attack.rank[b->king[BLACK]], ~(b->maps[PAWN]&b->colormaps[BLACK]), 0);
	b_my|= ((b_my|attack.rank[b->king[BLACK]])>>8);

//	printmask(b->maps[PAWN]&b->colormaps[WHITE], "WPAWN");
//	printmask(b->maps[PAWN]&b->colormaps[BLACK], "BPAWN");
//	printmask(w_oppos, "W_O");
//	printmask(w_my, "W_M");
//	printmask(b_oppos, "B_O");
//	printmask(b_my, "B_M");
//	LOGGER_0("pos %o, %o\n", b->king[WHITE], b->king[BLACK]);
	
	r1_b=r1_e=r2_b=r2_e=0;

	if((ws-1)>=0){
		eval_w_sh_pawn2(b, a, p, w_my, w_oppos, ws-1, &rb, &re);
		r1_b+=rb;
		r1_e+=re;
	}
	eval_w_sh_pawn2(b, a, p, w_my, w_oppos, ws, &rb, &re);
	r1_b+=rb;
	r1_e+=re;
	if((ws+1<8)) {
		eval_w_sh_pawn2(b, a, p, w_my, w_oppos, ws+1, &rb, &re);
		r1_b+=rb;
		r1_e+=re;
	}
	a->specs[WHITE][KING].sqr_b=r1_b;
	a->specs[WHITE][KING].sqr_e=r1_e;

	if((bs-1)>=0) {
		eval_b_sh_pawn2(b, a, p, b_my, b_oppos, bs-1, &rb, &re);
		r2_b+=rb;
		r2_e+=re;
	}
	eval_b_sh_pawn2(b, a, p, b_my, b_oppos, bs, &rb, &re);
	r2_b+=rb;
	r2_e+=re;
	if((bs+1)<8) {
		eval_b_sh_pawn2(b, a, p, b_my, b_oppos, bs+1, &rb, &re);
		r2_b+=rb;
		r2_e+=re;
	}

	a->specs[BLACK][KING].sqr_b=r2_b;
	a->specs[BLACK][KING].sqr_e=r2_e;

return 0;
}

int isDrawBy50x(board * b) {
	return 0;
}

int is_draw(board *b, attack_model *a, personality *p)
{
int ret,i, count;

	if((b->mindex_validity==1) && (p->mat_info[b->mindex]==INSUFF)) return 1;


/*
 * The fifty-move rule - if in the previous fifty moves by each side
 * no pawn has moved and no capture has been made
 * a draw may be claimed by either player.
 * Here again, the draw is not automatic and must be claimed if the player wants the draw.
 * If the player whose turn it is to move has made only 49 such moves,
 * he may write his next move on the scoresheet and claim a draw.
 * As with the threefold repetition, the right to claim the draw is forfeited
 * if it is not used on that move, but the opportunity may occur again
 */

	ret=0;

	if((b->move-b->rule50move)>=101) {
		return 2;
	}
	
	count=0;
	i=b->move;

/*
 * threefold repetition testing
 * a position is considered identical to another if
 * the same player is on move
 * the same types of pieces of the same colors occupy the same squares
 * the same moves are available to each player; in particular, each player has the same castling and en passant capturing rights.
 */

//	i-=2;
// na i musi matchnout vzdy!
	while((count<3)&&(i>=b->rule50move)&&(i>=b->move_start)) {
		if(b->positions[i-b->move_start]==b->key) {
			DEB_3(if(b->posnorm[i-b->move_start]!=b->norm)	printf("Error: Not matching position to hash!\n"));
			count++;
			if((count==2)&&(i>b->move_ply_start)) {
				ret=2;
			}
		}
		i-=2;
	}
	if(count>=3) {
		ret=3;
	}
	return ret;
}

int mat_info(int8_t *info)
{
int f;
	for(f=0;f<419999;f++) {
			info[f]=NO_INFO;
	}
// certain values known draw
//	m=MATidx(pw,pb,nw,nb,bwl,bwd,bbl,bbd,rw,rb,qw,qb);
// pw,pb,nw,nb,bwl,bwd,bbl,bbd,rw,rb,qw,qb, TYPE

int CVL[][13]= {
// bare king
		{0,0,0,0,0,0,0,0,0,0,0,0,INSUFF}, // king
// single minor
		{0,0,1,0,0,0,0,0,0,0,0,0,INSUFF}, // knights
		{0,0,0,1,0,0,0,0,0,0,0,0,INSUFF},
		{0,0,0,0,1,0,0,0,0,0,0,0,INSUFF}, // bishops
		{0,0,0,0,0,1,0,0,0,0,0,0,INSUFF},
		{0,0,0,0,0,0,1,0,0,0,0,0,INSUFF}, // bishops
		{0,0,0,0,0,0,0,1,0,0,0,0,INSUFF},
// same color bishops
		{0,0,0,0,1,0,1,0,0,0,0,0,INSUFF}, // bishops
		{0,0,0,0,0,1,0,1,0,0,0,0,INSUFF}, // bishops
// two knights
		{0,0,2,0,0,0,0,0,0,0,0,0,INSUFF},
		{0,0,0,2,0,0,0,0,0,0,0,0,INSUFF},
		{0,0,2,2,0,0,0,0,0,0,0,0,INSUFF}, //???
// minor each
// knights
		{0,0,1,1,0,0,0,0,0,0,0,0,UNLIKELY},
		{0,0,1,0,0,0,1,0,0,0,0,0,UNLIKELY},
		{0,0,1,0,0,0,0,1,0,0,0,0,UNLIKELY},
		{0,0,0,1,1,0,0,0,0,0,0,0,UNLIKELY},
		{0,0,0,1,0,1,0,0,0,0,0,0,UNLIKELY},
//bishops
		{0,0,0,0,1,0,0,1,0,0,0,0,UNLIKELY}, // opp bishops
		{0,0,0,0,0,1,1,0,0,0,0,0,UNLIKELY}, // opp bishops

		{0,0,2,1,0,0,0,0,0,0,0,0,UNLIKELY},
		{0,0,1,2,0,0,0,0,0,0,0,0,UNLIKELY},
		{0,0,2,0,0,0,1,0,0,0,0,0,UNLIKELY},
		{0,0,2,0,0,0,0,1,0,0,0,0,UNLIKELY},
		{0,0,0,2,1,0,0,0,0,0,0,0,UNLIKELY},
		{0,0,0,2,0,1,0,0,0,0,0,0,UNLIKELY},
		{0,0,0,0,0,0,1,0,1,0,0,0,UNLIKELY}, //R-B
		{0,0,0,0,0,0,0,1,1,0,0,0,UNLIKELY}, //R-B
		{0,0,0,0,1,0,0,0,0,1,0,0,UNLIKELY}, //R-B
		{0,0,0,0,0,1,0,0,0,1,0,0,UNLIKELY}, //R-B
		{0,0,0,1,0,0,0,0,1,0,0,0,UNLIKELY}, //R-N
		{0,0,1,0,0,0,0,0,0,1,0,0,UNLIKELY}, //R-N
		{0,0,0,0,1,0,0,0,1,1,0,0,UNLIKELY}, //RB-R
		{0,0,0,0,0,1,0,0,1,1,0,0,UNLIKELY}, //RB-R
		{0,0,0,0,0,0,1,0,1,1,0,0,UNLIKELY}, //RB-R
		{0,0,0,0,0,0,0,1,1,1,0,0,UNLIKELY}, //RB-R
		{0,0,1,0,0,0,0,0,1,1,0,0,UNLIKELY}, //RN-R
		{0,0,0,1,0,0,0,0,1,1,0,0,UNLIKELY}, //RN-R
		{0,0,0,0,1,0,0,0,0,0,1,1,UNLIKELY}, //QB-Q
		{0,0,0,0,0,1,0,0,0,0,1,1,UNLIKELY}, //QB-Q
		{0,0,0,0,0,0,1,0,0,0,1,1,UNLIKELY}, //QB-Q
		{0,0,0,0,0,0,0,1,0,0,1,1,UNLIKELY}, //QB-Q
		{0,0,1,0,0,0,0,0,0,0,1,1,UNLIKELY}, //QN-Q
		{0,0,0,1,0,0,0,0,0,0,1,1,UNLIKELY}, //QN-Q
		{0,0,1,0,1,0,1,0,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,1,0,0,1,1,0,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,1,0,1,0,0,1,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,1,0,0,1,0,1,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,0,1,1,0,1,0,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,0,1,0,1,1,0,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,0,1,1,0,0,1,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,0,1,0,1,0,1,0,0,0,0,UNLIKELY}, // bn-b
		{0,0,1,1,1,0,0,0,0,0,0,0,UNLIKELY}, // bn-n
		{0,0,1,1,0,1,0,0,0,0,0,0,UNLIKELY}, // bn-n
		{0,0,1,1,0,0,1,0,0,0,0,0,UNLIKELY}, // bn-n
		{0,0,1,1,0,0,0,1,0,0,0,0,UNLIKELY}, // bn-n
		{0,0,1,1,1,0,0,0,0,0,0,0,UNLIKELY}, // bb-b
		{0,0,1,1,0,1,0,0,0,0,0,0,UNLIKELY}, // bb-b
		{0,0,1,0,1,1,0,0,0,0,0,0,UNLIKELY}, // bb-b
		{0,0,0,1,1,1,0,0,0,0,0,0,UNLIKELY}, // bb-b
		{0,0,0,2,0,0,0,0,1,0,0,0,UNLIKELY}, // 2m-R
		{0,0,0,1,0,0,1,0,1,0,0,0,UNLIKELY}, // 2m-R
		{0,0,0,1,0,0,0,1,1,0,0,0,UNLIKELY}, // 2m-R
		{0,0,0,0,0,0,1,1,1,0,0,0,UNLIKELY}, // 2m-Rw
		{0,0,2,0,0,0,0,0,0,1,0,0,UNLIKELY}, // 2m-R
		{0,0,1,0,1,0,0,0,0,1,0,0,UNLIKELY}, // 2m-R
		{0,0,1,0,0,1,0,0,0,1,0,0,UNLIKELY}, // 2m-R
		{0,0,0,0,1,1,0,0,0,1,0,0,UNLIKELY}, // 2m-Rb
    };
int i,m;
	for(i=0;i<63;i++) {
		m=MATidx(CVL[i][0],CVL[i][1], CVL[i][2], CVL[i][3], CVL[i][4], CVL[i][5], CVL[i][6],
				CVL[i][7], CVL[i][8], CVL[i][9], CVL[i][10], CVL[i][11]);
		info[m]=CVL[i][12];
	}

return 0;
}

int mat_faze(uint8_t *faze)
{
int pw, pb, nw, nb, bwl, bwd, bbl, bbd, rw, rb, qw, qb, f;
int i, tot, fz, q, m;
int vaha[]={0,1,1,3,6};
int nc[]={16,4,4,4,2};
// clear
	for(f=0;f<419999;f++) {
			faze[f]=0;
	}
	tot=nc[PAWN]*vaha[PAWN]+nc[KNIGHT]*vaha[KNIGHT]+nc[BISHOP]*vaha[BISHOP]+nc[ROOK]*vaha[ROOK]+nc[QUEEN]*vaha[QUEEN];
	for(qb=0;qb<2;qb++) {
		for(qw=0;qw<2;qw++) {
			for(rb=0;rb<3;rb++) {
				for(rw=0;rw<3;rw++) {
					for(bbd=0;bbd<2;bbd++) {
						for(bbl=0;bbl<2;bbl++) {
							for(bwd=0;bwd<2;bwd++) {
								for(bwl=0;bwl<2;bwl++) {
									for(nb=0;nb<3;nb++) {
										for(nw=0;nw<3;nw++) {
											for(pb=0;pb<9;pb++) {
												for(pw=0;pw<9;pw++) 
												{
													m=MATidx(pw,pb,nw,nb,bwl,bwd,bbl,bbd,rw,rb,qw,qb);
													i =(pb+pw)*vaha[PAWN];
													i+=(nw+nb)*vaha[KNIGHT];
													i+=(bbd+bbl+bwd+bwl)*vaha[BISHOP];
													i+=(rw+rb)*vaha[ROOK];
													i+=(qw+qb)*vaha[QUEEN];
													q=Min(i, tot);
													fz=q*255/tot;
													assert(faze[m]==0);
													faze[m]=fz;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

return 0;
}

int meval_table_gen(meval_t *t, personality *p, int stage){
int pw, pb, nw, nb, bwl, bwd, bbl, bbd, rw, rb, qw, qb, f;
int m, w, b;

/*
	milipawns
	jeden P ma hodnotu 1000
	to dava nejlepsi materialove skore kolem 41000 za normalnich okolnosti. V extremu asi 102000. 
	Rezerva na bonusy 3x.
	tj. 123000, resp. 306000. 
	jako MATESCORE dam 0x50000 -- 327680
*/

	MATIdxIncW[PAWN]=PW_MI;
	MATIdxIncB[PAWN]=PB_MI;
	MATIdxIncW[KNIGHT]=NW_MI;
	MATIdxIncB[KNIGHT]=NB_MI;
	MATIdxIncW[ROOK]=RW_MI;
	MATIdxIncB[ROOK]=RB_MI;
	MATIdxIncW[QUEEN]=QW_MI;
	MATIdxIncB[QUEEN]=QB_MI;
	MATIdxIncW[BISHOP]=BWL_MI;
	MATIdxIncB[BISHOP]=BBL_MI;
	MATIdxIncW[BISHOP+ER_PIECE]=BWD_MI;
	MATIdxIncB[BISHOP+ER_PIECE]=BBD_MI;

	MATincW2[PAWN]=PW_MI2;
	MATincB2[PAWN]=PB_MI2;
	MATincW2[KNIGHT]=NW_MI2;
	MATincB2[KNIGHT]=NB_MI2;
	MATincW2[ROOK]=RW_MI2;
	MATincB2[ROOK]=RB_MI2;
	MATincW2[QUEEN]=QW_MI2;
	MATincB2[QUEEN]=QB_MI2;
	MATincW2[BISHOP]=BWL_MI2;
	MATincB2[BISHOP]=BBL_MI2;
	MATincW2[BISHOP+ER_PIECE]=BWD_MI2;
	MATincB2[BISHOP+ER_PIECE]=BBD_MI2;

	
// clear
	for(f=0;f<419999;f++) {
			t[f].mat=0;
//			t[f].info=NO_INFO;
	}
	for(qb=0;qb<2;qb++) {
		for(qw=0;qw<2;qw++) {
			for(rb=0;rb<3;rb++) {
				for(rw=0;rw<3;rw++) {
					for(bbd=0;bbd<2;bbd++) {
						for(bbl=0;bbl<2;bbl++) {
							for(bwd=0;bwd<2;bwd++) {
								for(bwl=0;bwl<2;bwl++) {
									for(nb=0;nb<3;nb++) {
										for(nw=0;nw<3;nw++) {
											for(pb=0;pb<9;pb++) {
												for(pw=0;pw<9;pw++) {
													m=MATidx(pw,pb,nw,nb,bwl,bwd,bbl,bbd,rw,rb,qw,qb);
													w=pw*p->Values[stage][0]+nw*p->Values[stage][1]+(bwl+bwd)*p->Values[stage][2]+rw*p->Values[stage][3]+qw*p->Values[stage][4];
													b=pb*p->Values[stage][0]+nb*p->Values[stage][1]+(bbl+bbd)*p->Values[stage][2]+rb*p->Values[stage][3]+qb*p->Values[stage][4];
// tune rooks and knight based on pawns at board
//													if(pw>5) 
													{
														w+=nw*(pw-5)*p->rook_to_pawn[stage]/2;
														w+=rw*(5-pw)*p->rook_to_pawn[stage];
													}
//													if(pb>5) 
													{
														b+=nw*(pb-5)*p->rook_to_pawn[stage]/2;
														b+=rw*(5-pb)*p->rook_to_pawn[stage];
													}
// tune bishop pair
													if((bwl>=1)&&(bwd>=1)) w+=p->bishopboth[stage];
													if((bbl>=1)&&(bbd>=1)) b+=p->bishopboth[stage];
// zohlednit materialove nerovnovahy !!!

													if(t[m].mat!=0)
														printf("poplach %d %d!!!!\n", m, t[m].mat);
													t[m].mat=(w-b);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

return 0;
}

int get_material_eval(board *b, personality *p, int *mb, int *me){
int bb, wb, be, we, stage;
int pw, pb, nw, nb, bwl, bwd, bbl, bbd, rw, rb, qw, qb;
	if(b->mindex_validity==1) {
		*mb = p->mat[b->mindex].mat;
		*me= p->mate_e[b->mindex].mat;
		return 1;
	} else {
		bwd=b->material[WHITE][BISHOP+ER_PIECE];
		bbd=b->material[BLACK][BISHOP+ER_PIECE];
		bwl=b->material[WHITE][BISHOP]-bwd;
		bbl=b->material[BLACK][BISHOP]-bbd;
		pw=b->material[WHITE][PAWN];
		pb=b->material[BLACK][PAWN];
		nw=b->material[WHITE][KNIGHT];
		nb=b->material[BLACK][KNIGHT];
		rw=b->material[WHITE][ROOK];
		rb=b->material[BLACK][ROOK];
		qw=b->material[WHITE][QUEEN];
		qb=b->material[BLACK][QUEEN];
		stage=0;
		wb=pw*p->Values[stage][0]+nw*p->Values[stage][1]+(bwl+bwd)*p->Values[stage][2]+rw*p->Values[stage][3]+qw*p->Values[stage][4];
		bb=pb*p->Values[stage][0]+nb*p->Values[stage][1]+(bbl+bbd)*p->Values[stage][2]+rb*p->Values[stage][3]+qb*p->Values[stage][4];
		stage=1;
		we=pw*p->Values[stage][0]+nw*p->Values[stage][1]+(bwl+bwd)*p->Values[stage][2]+rw*p->Values[stage][3]+qw*p->Values[stage][4];
		be=pb*p->Values[stage][0]+nb*p->Values[stage][1]+(bbl+bbd)*p->Values[stage][2]+rb*p->Values[stage][3]+qb*p->Values[stage][4];
		*mb=wb-bb;
		*me=we-be;
	}
return 2;
}

int get_material_eval_f(board *b, personality *p){
int score;
int me,mb;
int phase = eval_phase(b, p);

	get_material_eval(b, p, &mb, &me);
	score=mb*phase+me*(256-phase);
	return score / 256;
}

/*
 * hodnoceni dle
 * material
 * pieceSquare
 * struktura pescu
 * speciality jednotlivych figur
 * mobilita
 * boj o stred
 * vzajemne propojeni
 * chycene figury
 * bezpecnost krale
 */
 
 // pawn attacks - what squares are attacked by pawns of relevant side
 // make model
 // make pawn model
 // eval_king
 
// WHITE POV!
int eval(board* b, attack_model* a, personality* p) {
	int f, from;
	int score, score_b, score_e;
	a->phase = eval_phase(b, p);
// setup pawn attacks

	for(f=(ER_PIECE|BLACKPIECE);f>=0;f--) {
		a->pos_c[f]=-1;
	}

	a->att_by_side[WHITE]=a->pa_at[WHITE]=WhitePawnAttacks(b);
	a->att_by_side[BLACK]=a->pa_at[BLACK]=BlackPawnAttacks(b);
// bez ep!

	make_model(b, a, p);
	make_pawn_model(b, a, p);
	eval_king(b, a, p);
	get_material_eval(b, p, &a->sc.material, &a->sc.material_e);
//	a->sc.material = p->mat[b->mindex].mat;
//	a->sc.material_e = p->mate_e[b->mindex].mat;
	// spocitat mobilitu + piece-square
	// spocitat mobilitu + piece-square
	a->sc.side[0].mobi_b = 0;
	a->sc.side[0].mobi_e = 0;
	a->sc.side[0].sqr_b = 0;
	a->sc.side[0].sqr_e = 0;
	a->sc.side[0].specs_b = 0;
	a->sc.side[0].specs_e = 0;;
	a->sc.side[1].mobi_b = 0;
	a->sc.side[1].mobi_e = 0;
	a->sc.side[1].sqr_b = 0;
	a->sc.side[1].sqr_e = 0;
	a->sc.side[1].specs_b = 0;
	a->sc.side[1].specs_e = 0;;

	for (f = a->pos_c[BISHOP]; f >= 0; f--) {
		from = a->pos_m[BISHOP][f];
		a->sc.side[0].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[0].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[0].sqr_b += a->sq[from].sqr_b;
		a->sc.side[0].sqr_e += a->sq[from].sqr_e;
	}
	for (f = a->pos_c[BISHOP | BLACKPIECE]; f >= 0; f--) {
		from = a->pos_m[BISHOP | BLACKPIECE][f];
		a->sc.side[1].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[1].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[1].sqr_b += a->sq[from].sqr_b;
		a->sc.side[1].sqr_e += a->sq[from].sqr_e;
	}

	for (f = a->pos_c[KNIGHT]; f >= 0; f--) {
		from = a->pos_m[KNIGHT][f];
		a->sc.side[0].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[0].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[0].sqr_b += a->sq[from].sqr_b;
		a->sc.side[0].sqr_e += a->sq[from].sqr_e;
	}
	for (f = a->pos_c[KNIGHT | BLACKPIECE]; f >= 0; f--) {
		from = a->pos_m[KNIGHT | BLACKPIECE][f];
		a->sc.side[1].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[1].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[1].sqr_b += a->sq[from].sqr_b;
		a->sc.side[1].sqr_e += a->sq[from].sqr_e;
	}

	for (f = a->pos_c[ROOK]; f >= 0; f--) {
		from = a->pos_m[ROOK][f];
		a->sc.side[0].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[0].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[0].sqr_b += a->sq[from].sqr_b;
		a->sc.side[0].sqr_e += a->sq[from].sqr_e;
	}
	a->sc.side[0].specs_b+=a->specs[0][ROOK].sqr_b;
	a->sc.side[0].specs_e+=a->specs[0][ROOK].sqr_e;

	for (f = a->pos_c[ROOK | BLACKPIECE]; f >= 0; f--) {
		from = a->pos_m[ROOK | BLACKPIECE][f];
		a->sc.side[1].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[1].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[1].sqr_b += a->sq[from].sqr_b;
		a->sc.side[1].sqr_e += a->sq[from].sqr_e;
	}
	a->sc.side[1].specs_b+=a->specs[1][ROOK].sqr_b;
	a->sc.side[1].specs_e+=a->specs[1][ROOK].sqr_e;

	for (f = a->pos_c[QUEEN]; f >= 0; f--) {
		from = a->pos_m[QUEEN][f];
		a->sc.side[0].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[0].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[0].sqr_b += a->sq[from].sqr_b;
		a->sc.side[0].sqr_e += a->sq[from].sqr_e;
	}
	for (f = a->pos_c[QUEEN | BLACKPIECE]; f >= 0; f--) {
		from = a->pos_m[QUEEN | BLACKPIECE][f];
		a->sc.side[1].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[1].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[1].sqr_b += a->sq[from].sqr_b;
		a->sc.side[1].sqr_e += a->sq[from].sqr_e;
	}

	for (f = a->pos_c[PAWN]; f >= 0; f--) {
		from = a->pos_m[PAWN][f];
		a->sc.side[0].sqr_b += a->sq[from].sqr_b;
		a->sc.side[0].sqr_e += a->sq[from].sqr_e;
	}
	for (f = a->pos_c[PAWN | BLACKPIECE]; f >= 0; f--) {
		from = a->pos_m[PAWN | BLACKPIECE][f];
		a->sc.side[1].sqr_b += a->sq[from].sqr_b;
		a->sc.side[1].sqr_e += a->sq[from].sqr_e;
	}

	from = b->king[WHITE];
		a->sc.side[0].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[0].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[0].sqr_b += a->sq[from].sqr_b;
		a->sc.side[0].sqr_e += a->sq[from].sqr_e;
		a->sc.side[0].specs_b +=a->specs[0][KING].sqr_b;
		a->sc.side[0].specs_e +=a->specs[0][KING].sqr_e;
	from = b->king[BLACK];
		a->sc.side[1].mobi_b += a->me[from].pos_mob_tot_b;
		a->sc.side[1].mobi_e += a->me[from].pos_mob_tot_e;
		a->sc.side[1].sqr_b += a->sq[from].sqr_b;
		a->sc.side[1].sqr_e += a->sq[from].sqr_e;
		a->sc.side[1].specs_b +=a->specs[1][KING].sqr_b;
		a->sc.side[1].specs_e +=a->specs[1][KING].sqr_e;

//all evaluations are in milipawns 
// phase is in range 0 - 256. 256 being total opening, 0 total ending
#if 0
	score_b=a->sc.material+(a->sc.side[0].mobi_b - a->sc.side[1].mobi_b)+(a->sc.side[0].sqr_b - a->sc.side[1].sqr_b)+(a->sc.side[0].specs_b-a->sc.side[1].specs_b );
	score_e=a->sc.material_e +(a->sc.side[0].mobi_e - a->sc.side[1].mobi_e)+(a->sc.side[0].sqr_e - a->sc.side[1].sqr_e)+(a->sc.side[0].specs_e-a->sc.side[1].specs_e );
	score=score_b*a->phase+score_e*(256-a->phase);
#endif
// simplified eval
	score_b=a->sc.material+(a->sc.side[0].sqr_b - a->sc.side[1].sqr_b);
	score_e=a->sc.material_e+(a->sc.side[0].sqr_e - a->sc.side[1].sqr_e);
	score=score_b*a->phase+score_e*(256-a->phase);

	
/*	
	score = a->phase * (a->sc.material) + (256 - a->phase) * (a->sc.material_e);
	score += a->phase * (a->sc.side[0].mobi_b - a->sc.side[1].mobi_b) 
			+ (256 - a->phase) * (a->sc.side[0].mobi_e - a->sc.side[1].mobi_e);
	score += a->phase * (a->sc.side[0].sqr_b - a->sc.side[1].sqr_b)
			+ (256 - a->phase) * (a->sc.side[0].sqr_e - a->sc.side[1].sqr_e);
			
*/

/*
	if((b->mindex_validity==1)&&(((b->side==WHITE)&&(score>0))||((b->side==BLACK)&&(score<0)))) {
		switch(p->mat_info[b->mindex]) {
		case NO_INFO:
			break;
		case INSUFF:
			score=0;
			break;
		case UNLIKELY:
			score/=4;
			break;
		case DIV2:
			score/=2;
			break;
		case DIV4:
			score/=4;
			break;
		case DIV8:
			score/=8;
			break;
		default:
			break;
		}
	}
*/
	a->sc.complete = score / 256;
	return a->sc.complete;
}
//
//
//
//  Pxp, PxP, PxP, RxB, BxR, QxB, QxQ
// G:1 ,  0 ,  1 ,  0 ,  5 ,  -2,  12, -2 
//   1  -1,  1 , -5,   5 , -12,
//  Pxp, BxP, RxP  ?xR
//  1,   0,   3,   2,

int SEE(board * b, int m) {
int fr, to, side,d;
int gain[32];
BITVAR ignore;
int attacker;

	ignore=FULLBITMAP;
	fr=UnPackFrom(m);
	to=UnPackTo(m);
	side=(b->pieces[fr]&BLACKPIECE)!=0;
	d=0;
	gain[d]=b->pers->Values[0][b->pieces[to]&PIECEMASK];
	attacker=fr;
	while (attacker!=-1) {
		d++;
		gain[d]=-gain[d-1]+b->pers->Values[0][b->pieces[attacker]&PIECEMASK];
		if(Max(-gain[d-1], gain[d]) < 0) break;
		side^=1;
		ignore^=normmark[attacker];
		attacker=GetLVA_to(b, to, side, ignore);
//		ignore^=normmark[attacker];
	}
	while(--d) {
		gain[d-1]= -Max(-gain[d-1], gain[d]);
	}
	return gain[0];
}

int SEE_0(board * b, int move) {
int to, side,d;
int gain[32];
BITVAR ignore;
int attacker;

	to=UnPackTo(move);
	ignore=FULLBITMAP;
	printBoardNice(b);
	side=((b->pieces[to]&BLACKPIECE)!=0)? BLACK:WHITE;
	d=0;
//	gain[d]=b->pers->Values[0][b->pieces[to]&PIECEMASK];
	gain[d]=0;
	attacker=to;
//	attacker=GetLVA_to(b, to, side, ignore);
	while (attacker!=-1) {
		d++;
		gain[d]=-gain[d-1]+b->pers->Values[0][b->pieces[attacker]&PIECEMASK];
		if(Max(-gain[d-1], gain[d]) < 0) break;
		side^=1;
		ignore^=normmark[attacker];
		attacker=GetLVA_to(b, to, side, ignore);
//		ignore^=normmark[attacker];
	}
	while(--d) {
		gain[d-1]= -Max(-gain[d-1], gain[d]);
	}
	return gain[0];
}

int copyAttModel(attack_model *source, attack_model *dest){
	memcpy(dest, source, sizeof(attack_model));
return 0;
}

// [side][piece] 
BITVAR getMatKey(unsigned char m[][2*ER_PIECE]){
BITVAR k;
	k=0;
	k^=randomTable[WHITE][A2+m[WHITE][PAWN]][PAWN];
	k^=randomTable[WHITE][A2+m[WHITE][KNIGHT]][KNIGHT];
	k^=randomTable[WHITE][A2+m[WHITE][BISHOP]-m[WHITE][ER_PIECE+BISHOP]][BISHOP];
	k^=randomTable[WHITE][A4+m[WHITE][ER_PIECE+BISHOP]][BISHOP];
	k^=randomTable[WHITE][A2+m[WHITE][ROOK]][ROOK];
	k^=randomTable[WHITE][A2+m[WHITE][QUEEN]][QUEEN];

	k^=randomTable[BLACK][A2+m[BLACK][PAWN]][PAWN];
	k^=randomTable[BLACK][A2+m[BLACK][KNIGHT]][KNIGHT];
	k^=randomTable[BLACK][A2+m[BLACK][BISHOP]-m[BLACK][ER_PIECE+BISHOP]][BISHOP];
	k^=randomTable[BLACK][A4+m[BLACK][ER_PIECE+BISHOP]][BISHOP];
	k^=randomTable[BLACK][A2+m[BLACK][ROOK]][ROOK];
	k^=randomTable[BLACK][A2+m[BLACK][QUEEN]][QUEEN];
return k;
}
 
int check_mindex_validity(board *b, int force) {

int bwl, bwd, bbl, bbd;

	if((force==1)||(b->mindex_validity==0)) {
		b->mindex_validity=0;
		bwd=b->material[WHITE][BISHOP+ER_PIECE];
		bbd=b->material[BLACK][BISHOP+ER_PIECE];
		bwl=b->material[WHITE][BISHOP]-bwd;
		bbl=b->material[BLACK][BISHOP]-bbd;

		if((b->material[WHITE][QUEEN]>1) || (b->material[BLACK][QUEEN]>1) \
			|| (b->material[WHITE][KNIGHT]>2) || (b->material[BLACK][KNIGHT]>2) \
			|| (bwd>1) || (bwl>1) || (bbl>1) || (bbd>1) \
			|| (b->material[WHITE][ROOK]>2) || (b->material[BLACK][ROOK]>2) \
			|| (b->material[WHITE][PAWN]>8) || (b->material[BLACK][PAWN]>8)) return 0; 
		
		b->mindex=MATidx(b->material[WHITE][PAWN],b->material[BLACK][PAWN],b->material[WHITE][KNIGHT], \
			b->material[BLACK][KNIGHT],bwl,bwd,bbl,bbd,b->material[WHITE][ROOK],b->material[BLACK][ROOK], \
			b->material[WHITE][QUEEN],b->material[BLACK][QUEEN]);
		b->mindex_validity=1;
	}
return 1;
}

/* attacker, victim
int LVAcap[ER_PIECE][ER_PIECE] = { 
	{ A_OR_N+P_OR,        A_OR+16*N_OR-P_OR,  A_OR+16*B_OR-P_OR,  A_OR+16*R_OR-P_OR,  A_OR+16*Q_OR-P_OR,  A_OR+16*K_OR-P_OR },
	{ A_OR2+16*P_OR-N_OR, A_OR_N+N_OR,        A_OR+16*B_OR-N_OR,  A_OR+16*R_OR-N_OR,  A_OR+16*Q_OR-N_OR,  A_OR+16*K_OR-N_OR },
	{ A_OR2+16*P_OR-B_OR, A_OR2+16*N_OR-B_OR, A_OR_N+B_OR,        A_OR+16*R_OR-B_OR,  A_OR+16*Q_OR-B_OR,  A_OR+16*K_OR-B_OR },
	{ A_OR2+16*P_OR-R_OR, A_OR2+16*N_OR-R_OR, A_OR2+16*B_OR-R_OR, A_OR_N+R_OR,        A_OR+16*Q_OR-R_OR,  A_OR+16*K_OR-R_OR },
	{ A_OR2+16*P_OR-Q_OR, A_OR2+16*N_OR-Q_OR, A_OR2+16*B_OR-Q_OR, A_OR2+16*R_OR-Q_OR, A_OR_N+Q_OR,        A_OR+16*K_OR-Q_OR },
	{ A_OR2+16*P_OR-K_OR, A_OR2+16*N_OR-K_OR, A_OR2+16*B_OR-K_OR, A_OR2+16*R_OR-K_OR, A_OR2+16*Q_OR-K_OR, A_OR_N+K_OR }
};
*/

// losing A_OR2+ 100-740
// normal A_OR_N+ 10-60
// winn	 A_OR+ 310-790

// hash
// killers


int MVVLVA_gen(int table[ER_PIECE][ER_PIECE], _values Values)
{
int v[ER_PIECE];
int vic, att;
	v[PAWN]=10;
	v[KNIGHT]=20;
	v[BISHOP]=30;
	v[ROOK]=40;
	v[QUEEN]=50;
	v[KING]=60;
	for(vic=0;vic<ER_PIECE;vic++) {
		for(att=0;att<ER_PIECE;att++) {
// all values inserted are positive!
			if(vic==att) {
				table[att][vic]=(A_OR_N+v[att]);
			} else if(vic>att) {
				table[att][vic]=(A_OR+16*v[vic]-v[att]);
			} else if(vic<att) {
				table[att][vic]=(A_OR2+16*v[vic]-v[att]);
			}
		}
	}

return 0;
}
