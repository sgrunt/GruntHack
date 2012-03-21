/*	SCCS Id: @(#)mcastu.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

extern const int monstr[];
extern void demonpet();

#ifndef COMBINED_SPELLS
/* monster mage spells */
#define MGC_PSI_BOLT	 0
#define MGC_CURE_SELF	 1
#define MGC_HASTE_SELF	 2
#define MGC_STUN_YOU	 3
#define MGC_DISAPPEAR	 4
#define MGC_WEAKEN_YOU	 5
#define MGC_DESTRY_ARMR	 6
#define MGC_CURSE_ITEMS	 7
#define MGC_AGGRAVATION	 8
#define MGC_SUMMON_MONS	 9
#define MGC_CLONE_WIZ	10
#define MGC_DEATH_TOUCH	11
#endif

/* monster cleric spells */
#define CLC_OPEN_WOUNDS	 0
#define CLC_CURE_SELF	 1
#define CLC_CONFUSE_YOU	 2
#define CLC_PARALYZE	 3
#define CLC_BLIND_YOU	 4
#define CLC_INSECTS	 5
#define CLC_CURSE_ITEMS	 6
#define CLC_LIGHTNING	 7
#define CLC_FIRE_PILLAR	 8
#define CLC_GEYSER	 9

extern void you_aggravate(struct monst *);

STATIC_DCL void FDECL(cursetxt,(struct monst *,BOOLEAN_P));
STATIC_DCL int FDECL(choose_magic_spell, (struct monst *, int));
STATIC_DCL int FDECL(choose_clerical_spell, (int));
STATIC_DCL void FDECL(cast_wizard_spell,(struct monst *, int,int));
STATIC_DCL void FDECL(cast_cleric_spell,(struct monst *, int,int));
STATIC_DCL boolean FDECL(is_undirected_spell,(unsigned int,int));
STATIC_DCL boolean FDECL(spell_would_be_useless,(struct monst *,unsigned int,int));
STATIC_DCL boolean FDECL(uspell_would_be_useless,(struct monst *,unsigned int,int));
STATIC_DCL void FDECL(ucast_wizard_spell,(struct monst *,struct monst *,int,int));
STATIC_DCL void FDECL(ucast_cleric_spell,(struct monst *,struct monst *,int,int));

#ifdef OVL0

#ifdef COMBINED_SPELLS
boolean
is_spellcaster(ptr)
register struct permonst *ptr;
{
	int i = 0;
	for (; i < NATTK; i++) {
		if(ptr->mattk[i].aatyp == AT_MAGC &&
		   ptr->mattk[i].adtyp == AD_SPEL) {
		   	return TRUE;
		}
	}

	return FALSE;
}

boolean
can_cast_spells(mtmp)
register struct monst *mtmp;
{
	return mtmp->mspec_used <= maxspelltimeout(mtmp);
}

int maxspelltimeout(mtmp)
register struct monst *mtmp;
{
	if (mtmp->m_lev >= 38)
		return 255;
	return 8*mtmp->m_lev*((38-mtmp->m_lev)*2/3);
}

int spelltimeout(mtmp, level)
register struct monst *mtmp;
register int level;
{
	if (mtmp->m_lev >= 38)
		return 0;
	return 5*level*((38-mtmp->m_lev)*2/3);
}
#endif

extern const char * const flash_types[];	/* from zap.c */

/* feedback when frustrated monster couldn't cast a spell */
STATIC_OVL
void
cursetxt(mtmp, undirected)
struct monst *mtmp;
boolean undirected;
{
	if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
	    const char *point_msg;  /* spellcasting monsters are impolite */

	    if (undirected)
		point_msg = "all around, then curses";
	    else if ((Invis && !sees_invis(mtmp) &&
			(mtmp->mux != u.ux || mtmp->muy != u.uy)) ||
		    (youmonst.m_ap_type == M_AP_OBJECT &&
			youmonst.mappearance == STRANGE_OBJECT) ||
		    u.uundetected)
		point_msg = "and curses in your general direction";
	    else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		point_msg = "and curses at your displaced image";
	    else
		point_msg = "at you, then curses";

	    pline("%s points %s.", Monnam(mtmp), point_msg);
	} else if ((!(moves % 4) || !rn2(4))) {
	    if (flags.soundok) Norep("You hear a mumbled curse.");
	}
}

#endif /* OVL0 */
#ifdef OVLB

/* convert a level based random selection into a specific mage spell;
   inappropriate choices will be screened out by spell_would_be_useless() */
STATIC_OVL int
choose_magic_spell(mtmp, spellval)
struct monst *mtmp;
int spellval;
{
#ifdef COMBINED_SPELLS
    int level = 1;

    if (mtmp == &youmonst) {
	/* special cases */
	if (Sick && abs(spellval) >= 5)
	    return SPE_CURE_SICKNESS;

	if (Blind && abs(spellval) >= 3)
    	    return SPE_CURE_BLINDNESS;

	if (mtmp->mhp * 4 < mtmp->mhpmax)
	{
    	    if (abs(spellval >= 5))
		return SPE_EXTRA_HEALING;
	    else
		return SPE_HEALING;
	}

	if (u.uspellprot == 0)
    	    return SPE_PROTECTION;

	if (!Levitation && !Flying && abs(spellval) >= 7)
	    return SPE_LEVITATION;
    } else {
	/* special cases */
	if (mtmp->msick && abs(spellval) >= 5)
	    return SPE_CURE_SICKNESS;

	if (mtmp->mblinded && abs(spellval) >= 3)
    	    return SPE_CURE_BLINDNESS;

	if (mtmp->mhp * 4 < mtmp->mhpmax)
	{
    	    if (abs(spellval >= 5))
		return SPE_EXTRA_HEALING;
	    else
		return SPE_HEALING;
	}

	if (!mtmp->mprotection)
    	    return SPE_PROTECTION;

	if ((Levitation || Flying) && !levitating(mtmp) && !is_flyer(mtmp->data) &&
    	    abs(spellval) >= 7)
	  return SPE_LEVITATION;
    }
    
    /* spellval < 0 indicates friendly spellcasting; */
    /* we want that to be better with luck */
    if (spellval < 0) {
        int efflevel = (-spellval + 1) / 2;
    	level += efflevel - rnl(efflevel);
    }
    else level += rnl((spellval + 1) / 2);
    if (level > 7) level = 7;
    switch(level) {
    	case 7:
	    switch (rn2(3)) {
	    	case 0: return SPE_FINGER_OF_DEATH;
		case 1: return SPE_CANCELLATION;
	        /* double trouble - use a spellbook that's not a spellbook. */
		case 2: return SPE_BOOK_OF_THE_DEAD;
	    }
	    break;
	case 6:
	    switch (rn2(5)) {
	    	case 0: return SPE_CREATE_FAMILIAR;
		case 1: return SPE_TURN_UNDEAD;
		case 2: return SPE_TELEPORT_AWAY;
		case 3: return SPE_POLYMORPH;
		case 4: return SPE_ACID_BLAST;
	    }
	    break;
	case 5:
	    switch (rn2(4)) {
	    	case 0: return SPE_MAGIC_MAPPING;
		case 1: return SPE_DIG;
		case 2: return SPE_TOUCH_OF_DEATH;
		case 3: return SPE_POISON_BLAST;
	    }
	    break;
	case 4:
	    switch (rn2(8)) {
	    	case 0: return SPE_CONE_OF_COLD;
		case 1: return SPE_FIREBALL;
		case 2: return SPE_LIGHTNING;
		case 3: return SPE_DETECT_TREASURE;
		case 4: return SPE_INVISIBILITY;
		case 5: return SPE_LEVITATION;
		case 6: return SPE_RESTORE_ABILITY;
		case 7: return SPE_CAUSE_AGGRAVATION;
	    }
	    break;
	case 3:
	    switch (rn2(9)) {
	    	case 0: return SPE_REMOVE_CURSE;
		case 1: return SPE_CLAIRVOYANCE;
		case 2: return SPE_DETECT_UNSEEN;
		case 3: return SPE_IDENTIFY;
		case 4: return SPE_CAUSE_FEAR;
		case 5: return SPE_CHARM_MONSTER;
		case 6: return SPE_HASTE_SELF;
		case 7: return SPE_CURE_SICKNESS;
		case 8: return SPE_EXTRA_HEALING;
	    }
	    break;
	case 2:
	    switch (rn2(8)) {
	    	case 0: return SPE_DRAIN_LIFE;
		case 1: return SPE_MAGIC_MISSILE;
		case 2: return SPE_CREATE_MONSTER;
		case 3: return SPE_DETECT_FOOD;
		case 4: return SPE_CONFUSE_MONSTER;
		case 5: return SPE_SLOW_MONSTER;
		case 6: return SPE_CURE_BLINDNESS;
		case 7: return SPE_WIZARD_LOCK;
	    }
	    break;
	case 1:
	default:
	    switch (rn2(11)) {
	    	case 0: return SPE_FORCE_BOLT;
		case 1: return SPE_PSI_BOLT;
		case 2: return SPE_PROTECTION;
		case 3: return SPE_DETECT_MONSTERS;
		case 4: return SPE_LIGHT;
		case 5: return SPE_SLEEP;
		case 6: return SPE_JUMPING;
		case 7: return SPE_HEALING;
		case 8: return SPE_KNOCK;
		case 9: return SPE_FLAME_SPHERE;
		case 10: return SPE_FREEZE_SPHERE;
	    }
	    break;
    }
    return SPE_PSI_BOLT;
#else
    switch (spellval) {
    case 22:
    case 21:
    case 20:
	return MGC_DEATH_TOUCH;
    case 19:
    case 18:
	return MGC_CLONE_WIZ;
    case 17:
    case 16:
    case 15:
	return MGC_SUMMON_MONS;
    case 14:
    case 13:
	return MGC_AGGRAVATION;
    case 12:
    case 11:
    case 10:
	return MGC_CURSE_ITEMS;
    case 9:
    case 8:
	return MGC_DESTRY_ARMR;
    case 7:
    case 6:
	return MGC_WEAKEN_YOU;
    case 5:
    case 4:
	return MGC_DISAPPEAR;
    case 3:
	return MGC_STUN_YOU;
    case 2:
	return MGC_HASTE_SELF;
    case 1:
	return MGC_CURE_SELF;
    case 0:
    default:
	return MGC_PSI_BOLT;
    }
#endif
}

/* convert a level based random selection into a specific cleric spell */
STATIC_OVL int
choose_clerical_spell(spellnum)
int spellnum;
{
    switch (spellnum) {
    case 13:
	return CLC_GEYSER;
    case 12:
	return CLC_FIRE_PILLAR;
    case 11:
	return CLC_LIGHTNING;
    case 10:
    case 9:
	return CLC_CURSE_ITEMS;
    case 8:
	return CLC_INSECTS;
    case 7:
    case 6:
	return CLC_BLIND_YOU;
    case 5:
    case 4:
	return CLC_PARALYZE;
    case 3:
    case 2:
	return CLC_CONFUSE_YOU;
    case 1:
	return CLC_CURE_SELF;
    case 0:
    default:
	return CLC_OPEN_WOUNDS;
    }
}

/* return values:
 * 1: successful spell
 * 0: unsuccessful spell
 */
int
castmu(mtmp, mattk, thinks_it_foundyou, foundyou)
	register struct monst *mtmp;
	register struct attack *mattk;
	boolean thinks_it_foundyou;
	boolean foundyou;
{
	int	dmg, ml = mtmp->m_lev;
	int ret;
	int spellnum = 0;

	/* Three cases:
	 * -- monster is attacking you.  Search for a useful spell.
	 * -- monster thinks it's attacking you.  Search for a useful spell,
	 *    without checking for undirected.  If the spell found is directed,
	 *    it fails with cursetxt() and loss of mspec_used.
	 * -- monster isn't trying to attack.  Select a spell once.  Don't keep
	 *    searching; if that spell is not useful (or if it's directed),
	 *    return and do something else. 
	 * Since most spells are directed, this means that a monster that isn't
	 * attacking casts spells only a small portion of the time that an
	 * attacking monster does.
	 */
	if ((mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) && ml) {
	    int cnt = 40;

	    do {
		if (mattk->adtyp == AD_SPEL)
#ifdef COMBINED_SPELLS
		    spellnum = choose_magic_spell(mtmp, ml);
#else
		    spellnum = choose_magic_spell(mtmp, rn2(ml));
#endif
		else
		    spellnum = choose_clerical_spell(rn2(ml));
		/* not trying to attack?  don't allow directed spells */
		if (!thinks_it_foundyou) {
		    if (!is_undirected_spell(mattk->adtyp, spellnum) ||
			spell_would_be_useless(mtmp, mattk->adtyp, spellnum)) {
			if (foundyou)
			    impossible("spellcasting monster found you and doesn't know it?");
			return 0;
		    }
		    break;
		}
	    } while(--cnt > 0 &&
		    spell_would_be_useless(mtmp, mattk->adtyp, spellnum));
	    if (cnt == 0) return 0;
	}

	/* monster unable to cast spells? */
	if(mtmp->mcan ||
#ifdef COMBINED_SPELLS
		((mattk->adtyp == AD_SPEL && !can_cast_spells(mtmp)) ||
		(mattk->adtyp != AD_SPEL && mtmp->mspec_used))
#else
		mtmp->mspec_used
#endif
	   || !ml) {
	    cursetxt(mtmp, is_undirected_spell(mattk->adtyp, spellnum));
	    return(0);
	}

	if (mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) {
	    register struct obj *obj;
#ifdef COMBINED_SPELLS
            if (mattk->adtyp == AD_SPEL)
	    {
	    	mtmp->mspec_used +=
			spelltimeout(mtmp, objects[spellnum].oc_level); 
	    }
	    else
#endif
	    mtmp->mspec_used = 10 - mtmp->m_lev;
	    if (mtmp->mspec_used < 1) mtmp->mspec_used = 1;
	    for (obj = mtmp->minvent; obj; obj = obj->nobj)
	        if (obj->oartifact &&
		    obj->oartifact == ART_EYE_OF_THE_AETHIOPICA)
		{
		    mtmp->mspec_used = 0;
		    break;
		}
	}

	/* monster can cast spells, but is casting a directed spell at the
	   wrong place?  If so, give a message, and return.  Do this *after*
	   penalizing mspec_used. */
	if (!foundyou && thinks_it_foundyou &&
		!is_undirected_spell(mattk->adtyp, spellnum)
#ifdef COMBINED_SPELLS
	        && (mattk->adtyp != AD_SPEL && mattk->adtyp != AD_CLRC)
#endif
		
		) {
	    pline("%s casts a spell at %s!",
		canseemon(mtmp) ? Monnam(mtmp) : "Something",
		levl[mtmp->mux][mtmp->muy].typ == WATER
		    ? "empty water" : "thin air");
	    return(0);
	}

	nomul(0);
#ifdef COMBINED_SPELLS
	if (mtmp->mconf) {
#else
	if(rn2(ml*10) < (mtmp->mconf ? 100 : 20)) {	/* fumbled attack */
#endif
	    if (canseemon(mtmp) && flags.soundok)
		pline_The("air crackles around %s.", mon_nam(mtmp));
	    return(0);
	}
	if (canspotmon(mtmp) || !is_undirected_spell(mattk->adtyp, spellnum)) {
	    pline("%s casts a spell%s!",
		  canspotmon(mtmp) ? Monnam(mtmp) : "Something",
		  is_undirected_spell(mattk->adtyp, spellnum) ? "" :
		  (Invisible && !sees_invis(mtmp) && 
		   (mtmp->mux != u.ux || mtmp->muy != u.uy)) ?
		  " at a spot near you" :
		  (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy)) ?
		  " at your displaced image" :
		  " at you");
	}

/*
 *	As these are spells, the damage is related to the level
 *	of the monster casting the spell.
 */
#ifndef COMBINED_SPELLS
	if (!foundyou) {
	    dmg = 0;
	    if (mattk->adtyp != AD_SPEL && mattk->adtyp != AD_CLRC) {
		impossible(
	      "%s casting non-hand-to-hand version of hand-to-hand spell %d?",
			   Monnam(mtmp), mattk->adtyp);
		return(0);
	    }
	} else
#endif
	if (mattk->damd)
	    dmg = d((int)((ml/2) + mattk->damn), (int)mattk->damd);
	else dmg = d((int)((ml/2) + 1), 6);
	if (Half_spell_damage) dmg = (dmg+1) / 2;

#ifdef COMBINED_SPELLS
	/* this short-circuit behaviour prevents attacks from */
	/* after spells from being used unless there's no */
	/* magical energy to cast the spell. */
	ret = (mattk->adtyp == AD_SPEL &&
		is_racial(mtmp->data)) ? 3 : 1;
#else
	ret = 1;
#endif

	switch (mattk->adtyp) {

	    case AD_FIRE:
		pline("You're enveloped in flames.");
		if(Fire_resistance) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		burn_away_slime();
		break;
	    case AD_COLD:
		pline("You're covered in frost.");
		if(Cold_resistance) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_MAGM:
		You("are hit by a shower of missiles!");
		if(Antimagic) {
			shieldeff(u.ux, u.uy);
			pline_The("missiles bounce off!");
			dmg = 0;
		} else dmg = d((int)mtmp->m_lev/2 + 1,6);
		break;
	    case AD_SPEL:	/* wizard spell */
	    case AD_CLRC:       /* clerical spell */
	    {
		if (mattk->adtyp == AD_SPEL)
		    cast_wizard_spell(mtmp, dmg, spellnum);
		else
		    cast_cleric_spell(mtmp, dmg, spellnum);
		dmg = 0; /* done by the spell casting functions */
		break;
	    }
	}
	if(dmg) mdamageu(mtmp, mattk, dmg);
	return(ret);
}

int FDECL(mbhitm, (struct monst *,struct obj *));
void FDECL(mbhit,
	(struct monst *,int,int FDECL((*),(MONST_P,OBJ_P)),
	int FDECL((*),(OBJ_P,OBJ_P)),struct obj *));

/* monster wizard and cleric spellcasting functions */
/*
   If dmg is zero, then the monster is not casting at you.
   If the monster is intentionally not casting at you, we have previously
   called spell_would_be_useless() and spellnum should always be a valid
   undirected spell.
   If you modify either of these, be sure to change is_undirected_spell()
   and spell_would_be_useless().
 */
STATIC_OVL
void
cast_wizard_spell(mtmp, dmg, spellnum)
struct monst *mtmp;
int dmg;
int spellnum;
{
    struct obj *pseudo = 0;
    int tmp1, tmp2;
    coord bypos;

    if (dmg == 0 && !is_undirected_spell(AD_SPEL, spellnum)) {
	impossible("cast directed wizard spell (%d) with dmg=0?", spellnum);
	return;
    }

    switch (spellnum) {
#ifdef COMBINED_SPELLS
    case SPE_TOUCH_OF_DEATH:
#else
    case MGC_DEATH_TOUCH:
#endif
	pline("Oh no, %s's using the touch of death!", mhe(mtmp));
	if (nonliving(youmonst.data) || is_demon(youmonst.data)) {
	    You("seem no deader than before.");
	} else if (!Antimagic && rn2(mtmp->m_lev) > 12) {
	    if (Hallucination) {
		You("have an out of body experience.");
	    } else {
	        char knbuf[BUFSZ];
		Sprintf(knbuf, "%s touch of death",
		        s_suffix(done_in_name(curmonst)));
		killer_format = KILLED_BY;
		killer = knbuf;
		done(DIED);
	    }
	} else {
	    if (Antimagic) shieldeff(u.ux, u.uy);
	    pline("Lucky for you, it didn't work!");
	}
	dmg = 0;
	break;
#ifdef COMBINED_SPELLS
    case SPE_BOOK_OF_THE_DEAD:
#else
    case MGC_CLONE_WIZ:
#endif
	if (mtmp->iswiz && flags.no_of_wizards == 1) {
	    pline("Double Trouble...");
	    clonewiz(&youmonst);
	    dmg = 0;
	} else
	    impossible("bad wizard cloning?");
	break;
#ifndef COMBINED_SPELLS
    case MGC_SUMMON_MONS:
    {
	int count;

	count = nasty(mtmp);	/* summon something nasty */
	if (mtmp->iswiz)
	    verbalize("Destroy the thief, my pet%s!", plur(count));
	else {
	    const char *mappear =
		(count == 1) ? "A monster appears" : "Monsters appear";

	    /* messages not quite right if plural monsters created but
	       only a single monster is seen */
	    if (Invisible && !sees_invis(mtmp) &&
				    (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around a spot near you!", mappear);
	    else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around your displaced image!", mappear);
	    else
		pline("%s from nowhere!", mappear);
	}
	dmg = 0;
	break;
    }
    case MGC_AGGRAVATION:
	You_feel("that monsters are aware of your presence.");
	aggravate();
	dmg = 0;
	break;
    case MGC_CURSE_ITEMS:
	You_feel("as if you need some help.");
	rndcurse();
	dmg = 0;
	break;
    case MGC_DESTRY_ARMR:
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (!destroy_arm(some_armor(&youmonst))) {
	    Your("skin itches.");
	}
	dmg = 0;
	break;
    case MGC_WEAKEN_YOU:		/* drain strength */
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily weakened.");
	} else {
	    You("suddenly feel weaker!");
	    dmg = mtmp->m_lev - 6;
	    if (Half_spell_damage) dmg = (dmg + 1) / 2;
	    losestr(rnd(dmg));
	    if (u.uhp < 1)
		done_in_by(mtmp, attacktype_fordmg(mtmp->data, AT_MAGC, AD_SPEL));
	}
	dmg = 0;
	break;
#endif
#ifdef COMBINED_SPELLS
    case SPE_INVISIBILITY:
#else
    case MGC_DISAPPEAR:		/* makes self invisible */
#endif
	if (!mtmp->minvis && !mtmp->invis_blkd) {
	    if (canseemon(mtmp))
		pline("%s suddenly %s!", Monnam(mtmp),
		      !See_invisible ? "disappears" : "becomes transparent");
	    mon_set_minvis(mtmp);
	    dmg = 0;
	} else
	    impossible("no reason for monster to cast disappear spell?");
	break;
#ifndef COMBINED_SPELLS
    case MGC_STUN_YOU:
	if (Antimagic || Free_action) {
	    shieldeff(u.ux, u.uy);
	    if (!Stunned)
		You_feel("momentarily disoriented.");
	    make_stunned(1L, FALSE);
	} else {
	    You(Stunned ? "struggle to keep your balance." : "reel...");
	    dmg = d(ACURR(A_DEX) < 12 ? 6 : 4, 4);
	    if (Half_spell_damage) dmg = (dmg + 1) / 2;
	    make_stunned(HStun + dmg, FALSE);
	}
	dmg = 0;
	break;
#endif
#ifdef COMBINED_SPELLS
    case SPE_HASTE_SELF:
#else
    case MGC_HASTE_SELF:
#endif
	mon_adjust_speed(mtmp, 1, (struct obj *)0);
	dmg = 0;
	break;
#ifdef COMBINED_SPELLS
    case SPE_EXTRA_HEALING:
	if (mtmp->mhp < mtmp->mhpmax) {
	    if (canseemon(mtmp))
		pline("%s looks much better.", Monnam(mtmp));
	    if ((mtmp->mhp += d(6,8)) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;
	    dmg = 0;
	}
	break;
    case SPE_HEALING:
	if (mtmp->mhp < mtmp->mhpmax) {
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    if ((mtmp->mhp += d(6,4)) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;
	    dmg = 0;
	}
	break;
#else
    case MGC_CURE_SELF:
	if (mtmp->mhp < mtmp->mhpmax) {
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    /* note: player healing does 6d4; this used to do 1d8 */
	    if ((mtmp->mhp += d(3,6)) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;
	    dmg = 0;
	}
	break;
#endif
#ifdef COMBINED_SPELLS
    case SPE_PSI_BOLT:
#else
    case MGC_PSI_BOLT:
#endif
	/* prior to 3.4.0 Antimagic was setting the damage to 1--this
	   made the spell virtually harmless to players with magic res. */
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}
	if (dmg <= 5)
	    You("get a slight %sache.", body_part(HEAD));
	else if (dmg <= 10)
	    Your("brain is on fire!");
	else if (dmg <= 20)
	    Your("%s suddenly aches painfully!", body_part(HEAD));
	else
	    Your("%s suddenly aches very painfully!", body_part(HEAD));
	break;
#ifdef COMBINED_SPELLS
    case SPE_FORCE_BOLT:
    case SPE_DRAIN_LIFE:
    case SPE_SLOW_MONSTER:
    case SPE_TELEPORT_AWAY:
    case SPE_KNOCK:
    case SPE_WIZARD_LOCK:
    case SPE_POLYMORPH:
    case SPE_CANCELLATION:
	pseudo = mksobj(spellnum, FALSE, FALSE);
	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;
	tmp1 = tbx; tmp2 = tby;
	tbx = mtmp->mux - mtmp->mx;
	tby = mtmp->muy - mtmp->my;
	mbhit(mtmp,rn1(8,6),mbhitm,bhito,pseudo);
	obfree(pseudo, (struct obj *)0);
	tbx = tmp1; tby = tmp2;
	dmg = 0;
    	break;
    case SPE_FIREBALL:
    case SPE_CONE_OF_COLD:
    case SPE_POISON_BLAST:
    case SPE_ACID_BLAST:
    	if (mtmp->iswiz || is_prince(mtmp->data) || is_lord(mtmp->data) ||
		mtmp->data->msound == MS_NEMESIS)
	{
		int n = rnd(8) + 1;
		coord pos;
		pos.x = mtmp->mux;
		pos.y = mtmp->muy;
		You("are blasted by %s!",
			spellnum == SPE_FIREBALL ? "a fireball" :
			spellnum == SPE_CONE_OF_COLD ? "a cone of cold" :
			spellnum == SPE_POISON_BLAST ? "poison gas" :
			spellnum == SPE_ACID_BLAST ? "acid" :
			"a strange effect");
		while(n--) {
		    explode(pos.x, pos.y,
			    spellnum - SPE_MAGIC_MISSILE + 10,
			    mtmp->m_lev/2 + 1, 0,
				(spellnum == SPE_CONE_OF_COLD) ?
					EXPL_FROSTY :
				(spellnum == SPE_POISON_BLAST) ?
					EXPL_NOXIOUS :
				(spellnum == SPE_ACID_BLAST) ?
					EXPL_WET :
				EXPL_FIERY);
		    if (spellnum != SPE_CONE_OF_COLD)
		        scatter(pos.x, pos.y,
				mtmp->m_lev/2 + 1,
				VIS_EFFECTS|MAY_HIT|
				MAY_DESTROY|MAY_FRACTURE, NULL);
		}
		pos.x = mtmp->mux+rnd(3)-2; pos.y = mtmp->muy+rnd(3)-2;
		if (!isok(pos.x,pos.y) || !cansee(pos.x,pos.y) ||
		    IS_STWALL(levl[pos.x][pos.y].typ)) {
		    /* Spell is reflected back to center */
		    pos.x = mtmp->mux; pos.y = mtmp->muy;
	        }
		dmg = 0;
		break;
	}
    case SPE_SLEEP:
    case SPE_MAGIC_MISSILE:
    case SPE_FINGER_OF_DEATH:
    case SPE_LIGHTNING:
	buzz(-(spellnum - SPE_MAGIC_MISSILE + 10),
	     mtmp->m_lev / 2 + 1,
	     mtmp->mx, mtmp->my,
	     sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
	dmg = 0;
    	break;
    case SPE_CAUSE_FEAR:
    {
	int count = rnl(73);
	
	dmg = 0;

        if (mtmp->mux != u.ux || mtmp->muy != u.uy)
	    break;

	if (Antimagic) {
		count = 3;
	}
	frighten_player(count);
        break;
    }
    case SPE_CONFUSE_MONSTER:
	dmg = 0;

        if (mtmp->mux != u.ux || mtmp->muy != u.uy)
	    break;
	if (Antimagic) {
	    if (Confusion)
	        You("feel even dizzier.");
	    else You("feel slightly dizzy.");
	    make_confused(HConfusion + 1 + rnl(3), FALSE);
	} else {
	    if(Confusion)
		You("are getting even more confused.");
	    else You("are getting confused.");
	    make_confused(HConfusion + 1 + rnl(100), FALSE);
	}
        break;
    case SPE_CREATE_MONSTER:
    {
    	int count = 1 + (!rn2(73)) ? rnd(4) : 0;
	int seencount = 0;
	boolean confused = mtmp->mconf;
	register struct monst *mpet;
	bypos.x = mtmp->mx;
	bypos.y = mtmp->my;
	dmg = 0;
	for (; count > 0; count--)
	{
		if (enexto(&bypos, mtmp->mx, mtmp->my, (struct permonst *)0))
		{
        		mpet = makemon(
				confused ? &mons[PM_ACID_BLOB]
				         : (struct permonst *)0,
				bypos.x, bypos.y, NO_MM_FLAGS);
			if (mpet && canspotmon(mpet))
				seencount++;
		}
	}
	if (seencount)
	    	pline("%s from nowhere!", 
			seencount == 1 ? "A monster appears"
			               : "Monsters appear");
    	break;
    }
    case SPE_FLAME_SPHERE:
    case SPE_FREEZE_SPHERE:
    case SPE_SHOCK_SPHERE:
    case SPE_CREATE_FAMILIAR:
    {
    	int count = 1, seencount = 0, heardcount = 0;
    	register struct monst *mpet = 0;
	register struct permonst *montype =
		spellnum == SPE_FLAME_SPHERE  ? &mons[PM_FLAMING_SPHERE] :
		spellnum == SPE_FREEZE_SPHERE ? &mons[PM_FREEZING_SPHERE] :
		spellnum == SPE_SHOCK_SPHERE  ? &mons[PM_SHOCKING_SPHERE] :
		(!rn2(3) ? (rn2(2) ? &mons[PM_LITTLE_DOG] : &mons[PM_KITTEN])
			    : rndmonst() );
	if (spellnum != SPE_CREATE_FAMILIAR)
	{
		if (mtmp->iswiz || is_prince(mtmp->data) ||
		    mtmp->data->msound == MS_NEMESIS)
			count += rn1(3,2);
		else if (is_lord(mtmp->data))
			count += rn1(2,1);
	}
	bypos.x = mtmp->mx;
	bypos.y = mtmp->my;
	
	dmg = 0;
	
	for (; count > 0; count--)
	{
		if (enexto(&bypos, mtmp->mx, mtmp->my, montype))
			mpet = makemon(montype, bypos.x, bypos.y,
				(mtmp->mtame) ? MM_EDOG :  NO_MM_FLAGS);
		if (mpet)
		{
	                mpet->msleeping = 0;
	                if (mtmp->mtame)
			    initedog(mpet);
			else if (mtmp->mpeaceful)
			        mpet->mpeaceful = 1;
			else mpet->mpeaceful = mpet->mtame = 0;

			if (spellnum != SPE_CREATE_FAMILIAR)
			{
				mpet->m_lev = mtmp->m_lev;
	    		    	mpet->mhpmax = mpet->mhp =
					d((int)mpet->m_lev, 8);
			}
	
			set_malign(mpet);
			if (canspotmon(mpet)) {
				seencount++;
				/*pline("%s appears from nowhere!", Amonnam(mpet));*/
			} else {
				heardcount++;
			}
		}
	}
	if (seencount > 0)
	{
		pline("%s%s appear%s from nowhere!", 
			seencount > 1 ? "Some " : "",
			seencount > 1 ? makeplural(m_monnam(mpet))
			              : Amonnam(mpet),
			seencount == 1 ? "s" : "");
	}
	else if (heardcount > 0 && flags.soundok && spellnum != SPE_CREATE_FAMILIAR)
		You_hear("crackling.");
    	break;
    }
    case SPE_LEVITATION:
    	dmg = 0;

    	if (!levitating(mtmp) && canseemon(mtmp))
	        pline("%s begins to float in the air!", Monnam(mtmp));
	
	mtmp->mlevitating += (mtmp->iswiz ||
	                      mtmp->data->msound == MS_NEMESIS ||
			      is_prince(mtmp->data) ||
			      is_lord(mtmp->data))
		? rn1(50,250) : rn1(140, 10);
	break;
    case SPE_PROTECTION:
    {
    	int natac = find_mac(mtmp) + mtmp->mprotection;
	int loglev = 0, l = mtmp->m_lev;
	int gain = 0;

    	dmg = 0;
	
	for (; l > 0; l /= 2)
		loglev++;

	gain = loglev - mtmp->mprotection / (4 - min(3,(10 - natac)/10));

    	if (canseemon(mtmp) && gain)
	{
		if (mtmp->mprotection)
		{
			pline_The("%s haze around %s becomes more dense.",
				hcolor(NH_GOLDEN), mon_nam(mtmp));
		}
		else
		{
			mtmp->mprottime =
				(mtmp->iswiz || is_prince(mtmp->data)
				|| mtmp->data->msound == MS_NEMESIS)
				? 20 : 10;
			pline_The("air around %s begins to shimmer with a %s haze.",
				mon_nam(mtmp), hcolor(NH_GOLDEN));
		}
	}
	mtmp->mprotection += gain;
    	break;
    }
    case SPE_CAUSE_AGGRAVATION:
    	dmg = 0;
	aggravate();
    	break;
    case SPE_CURE_BLINDNESS:
    	mtmp->mblinded = 0;
	if (canseemon(mtmp)) pline("%s can see again.", Monnam(mtmp));
	dmg = 0;
	break;
    case SPE_CURE_SICKNESS:
    	mtmp->msick = 0;
	if (canseemon(mtmp)) pline("%s looks relieved.", Monnam(mtmp));
	dmg = 0;
	break;
#endif
    default:
	impossible("mcastu: invalid magic spell (%d)", spellnum);
	dmg = 0;
	break;
    }

    if (dmg) mdamageu(mtmp, NULL, dmg);
}

STATIC_OVL
void
cast_cleric_spell(mtmp, dmg, spellnum)
struct monst *mtmp;
int dmg;
int spellnum;
{
    if (dmg == 0 && !is_undirected_spell(AD_CLRC, spellnum)) {
	impossible("cast directed cleric spell (%d) with dmg=0?", spellnum);
	return;
    }

    switch (spellnum) {
    case CLC_GEYSER:
	/* this is physical damage, not magical damage */
	pline("A sudden geyser slams into you from nowhere!");
	dmg = d(8, 6);
	if (Half_physical_damage) dmg = (dmg + 1) / 2;
	break;
    case CLC_FIRE_PILLAR:
	pline("A pillar of fire strikes all around you!");
	if (Fire_resistance) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	} else
	    dmg = d(8, 6);
	if (Half_spell_damage) dmg = (dmg + 1) / 2;
	burn_away_slime();
	(void) burnarmor(&youmonst);
	destroy_item(SCROLL_CLASS, AD_FIRE);
	destroy_item(POTION_CLASS, AD_FIRE);
	destroy_item(SPBOOK_CLASS, AD_FIRE);
	(void) burn_floor_paper(u.ux, u.uy, TRUE, FALSE);
	break;
    case CLC_LIGHTNING:
    {
	boolean reflects;

	pline("A bolt of lightning strikes down at you from above!");
	reflects = ureflects("It bounces off your %s%s.", "");
	if (reflects || Shock_resistance) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	    if (reflects)
		break;
	} else
	    dmg = d(8, 6);
	if (Half_spell_damage) dmg = (dmg + 1) / 2;
	destroy_item(WAND_CLASS, AD_ELEC);
	destroy_item(RING_CLASS, AD_ELEC);
	break;
    }
    case CLC_CURSE_ITEMS:
	You_feel("as if you need some help.");
	rndcurse();
	dmg = 0;
	break;
    case CLC_INSECTS:
      {
	/* Try for insects, and if there are none
	   left, go for (sticks to) snakes.  -3. */
	struct permonst *pm = mkclass(S_ANT,0);
	struct monst *mtmp2 = (struct monst *)0;
	char let = (pm ? S_ANT : S_SNAKE);
	coord bypos;
	boolean success;
	int i;
	int quan;

	quan = (mtmp->m_lev < 2) ? 1 : rnd((int)mtmp->m_lev / 2);
	if (quan < 3) quan = 3;
	success = pm ? TRUE : FALSE;
	for (i = 0; i <= quan; i++) {
	    if (!enexto(&bypos, mtmp->mux, mtmp->muy, mtmp->data))
		break;
	    if ((pm = mkclass(let,0)) != 0 &&
		    (mtmp2 = makemon(pm, bypos.x, bypos.y, NO_MM_FLAGS)) != 0) {
		success = TRUE;
		mtmp2->msleeping = mtmp2->mpeaceful = mtmp2->mtame = 0;
		set_malign(mtmp2);
	    }
	}
	/* Not quite right:
         * -- message doesn't always make sense for unseen caster (particularly
	 *    the first message)
         * -- message assumes plural monsters summoned (non-plural should be
         *    very rare, unlike in nasty())
         * -- message assumes plural monsters seen
         */
	if (!success)
	    pline("%s casts at a clump of sticks, but nothing happens.",
		Monnam(mtmp));
	else if (let == S_SNAKE)
	    pline("%s transforms a clump of sticks into snakes!",
		Monnam(mtmp));
	else if (Invisible && !sees_invis(mtmp) &&
				(mtmp->mux != u.ux || mtmp->muy != u.uy))
	    pline("%s summons insects around a spot near you!",
		Monnam(mtmp));
	else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
	    pline("%s summons insects around your displaced image!",
		Monnam(mtmp));
	else
	    pline("%s summons insects!", Monnam(mtmp));
	dmg = 0;
	break;
      }
    case CLC_BLIND_YOU:
	/* note: resists_blnd() doesn't apply here */
	if (!Blinded) {
	    int num_eyes = eyecount(youmonst.data);
	    pline("Scales cover your %s!",
		  (num_eyes == 1) ?
		  body_part(EYE) : makeplural(body_part(EYE)));
	    make_blinded(Half_spell_damage ? 100L : 200L, FALSE);
	    if (!Blind) Your(vision_clears);
	    dmg = 0;
	} else
	    impossible("no reason for monster to cast blindness spell?");
	break;
    case CLC_PARALYZE:
	if (Antimagic || Free_action) {
	    shieldeff(u.ux, u.uy);
	    if (multi >= 0)
		You("stiffen briefly.");
	    nomul2(-1, "paralyzed");
	} else {
	    if (multi >= 0)
		You("are frozen in place!");
	    dmg = 4 + (int)mtmp->m_lev;
	    if (Half_spell_damage) dmg = (dmg + 1) / 2;
	    nomul2(-dmg, "paralyzed");
	}
	dmg = 0;
	break;
    case CLC_CONFUSE_YOU:
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily dizzy.");
	} else {
	    boolean oldprop = !!Confusion;

	    dmg = (int)mtmp->m_lev;
	    if (Half_spell_damage) dmg = (dmg + 1) / 2;
	    make_confused(HConfusion + dmg, TRUE);
	    if (Hallucination)
		You_feel("%s!", oldprop ? "trippier" : "trippy");
	    else
		You_feel("%sconfused!", oldprop ? "more " : "");
	}
	dmg = 0;
	break;
    case CLC_CURE_SELF:
	if (mtmp->mhp < mtmp->mhpmax) {
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    /* note: player healing does 6d4; this used to do 1d8 */
	    if ((mtmp->mhp += d(3,6)) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;
	    dmg = 0;
	}
	break;
    case CLC_OPEN_WOUNDS:
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}
	if (dmg <= 5)
	    Your("skin itches badly for a moment.");
	else if (dmg <= 10)
	    pline("Wounds appear on your body!");
	else if (dmg <= 20)
	    pline("Severe wounds appear on your body!");
	else
	    Your("body is covered with painful wounds!");
	break;
    default:
	impossible("mcastu: invalid clerical spell (%d)", spellnum);
	dmg = 0;
	break;
    }

    if (dmg) mdamageu(mtmp, NULL, dmg);
}

STATIC_DCL
boolean
is_undirected_spell(adtyp, spellnum)
unsigned int adtyp;
int spellnum;
{
    if (adtyp == AD_SPEL) {
	switch (spellnum) {
#ifdef COMBINED_SPELLS
	case SPE_LIGHT:
	case SPE_DETECT_MONSTERS:
	case SPE_HEALING:
	case SPE_CREATE_MONSTER:
	case SPE_DETECT_FOOD:
	case SPE_CLAIRVOYANCE:
	case SPE_CURE_SICKNESS:
	case SPE_HASTE_SELF:
	case SPE_DETECT_UNSEEN:
	case SPE_LEVITATION:
	case SPE_EXTRA_HEALING:
	case SPE_RESTORE_ABILITY:
	case SPE_INVISIBILITY:
	case SPE_DETECT_TREASURE:
	case SPE_REMOVE_CURSE:
	case SPE_MAGIC_MAPPING:
	case SPE_IDENTIFY:
	case SPE_CREATE_FAMILIAR:
	case SPE_PROTECTION:
	case SPE_FLAME_SPHERE:
	case SPE_FREEZE_SPHERE:
	case SPE_SHOCK_SPHERE:
#else
	case MGC_CLONE_WIZ:
	case MGC_SUMMON_MONS:
	case MGC_AGGRAVATION:
	case MGC_DISAPPEAR:
	case MGC_HASTE_SELF:
	case MGC_CURE_SELF:
#endif
	    return TRUE;
	default:
	    break;
	}
    } else if (adtyp == AD_CLRC) {
	switch (spellnum) {
	case CLC_INSECTS:
	case CLC_CURE_SELF:
	    return TRUE;
	default:
	    break;
	}
    }
    return FALSE;
}

/* Some spells are useless under some circumstances. */
STATIC_DCL
boolean
spell_would_be_useless(mtmp, adtyp, spellnum)
struct monst *mtmp;
unsigned int adtyp;
int spellnum;
{
    /* Some spells don't require the player to really be there and can be cast
     * by the monster when you're invisible, yet still shouldn't be cast when
     * the monster doesn't even think you're there.
     * This check isn't quite right because it always uses your real position.
     * We really want something like "if the monster could see mux, muy".
     */
    boolean mcouldseeu = couldsee(mtmp->mx, mtmp->my);

    if (adtyp == AD_SPEL) {
#ifdef COMBINED_SPELLS
	if ((spellnum == SPE_PSI_BOLT ||
	     spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_CONFUSE_MONSTER ||
	     spellnum == SPE_CAUSE_FEAR) &&
             um_dist(mtmp->mx,mtmp->my, 1))
	     return TRUE;
	/* haste self when already fast */
	if (mtmp->permspeed == MFAST && spellnum == SPE_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((mtmp->minvis || mtmp->invis_blkd) && spellnum == SPE_INVISIBILITY)
	    return TRUE;
	/* peaceful monster won't cast invisibility if you can't see invisible,
	   same as when monsters drink potions of invisibility.  This doesn't
	   really make a lot of sense, but lets the player avoid hitting
	   peaceful monsters by mistake */
	if (mtmp->mpeaceful && !See_invisible && spellnum == SPE_INVISIBILITY)
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax &&
	    (spellnum == SPE_HEALING || spellnum == SPE_EXTRA_HEALING))
	    return TRUE;
	/* don't summon monsters if it doesn't think you're around */
	if (!mcouldseeu && (spellnum == SPE_CREATE_MONSTER ||
	                    spellnum == SPE_CREATE_FAMILIAR ||
		(!mtmp->iswiz && spellnum == SPE_BOOK_OF_THE_DEAD)))
	    return TRUE;
	if ((!mtmp->iswiz || flags.no_of_wizards > 1)
					&& spellnum == SPE_BOOK_OF_THE_DEAD)
	    return TRUE;
	
	if (spellnum == SPE_REMOVE_CURSE)
	{
            register struct obj *otmp;
	    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
	    {
		    if (otmp->cursed && 
		        (otmp->otyp == LOADSTONE ||
			 otmp->owornmask))
		{
	      	    return FALSE;
	        } 
	    }
	    return TRUE;
	}
	
	if ((spellnum == SPE_FIREBALL ||
	     spellnum == SPE_CONE_OF_COLD ||
	     spellnum == SPE_POISON_BLAST ||
	     spellnum == SPE_ACID_BLAST) &&
	     EReflecting &&
	     !mtmp->iswiz && !is_prince(mtmp->data) && !is_lord(mtmp->data) &&
	     mtmp->data->msound != MS_NEMESIS)
	    return TRUE;

	if ((spellnum == SPE_SLEEP ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_LIGHTNING) && EReflecting)
	    return TRUE;

	if ((spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_FINGER_OF_DEATH) &&
	     (nonliving(youmonst.data) || is_demon(youmonst.data)))
	     return TRUE;

	if ((spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_SLEEP ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_POLYMORPH ||
	     spellnum == SPE_FORCE_BOLT) && Antimagic)
	    return TRUE;

	if ((spellnum == SPE_DRAIN_LIFE) && (Antimagic || Drain_resistance))
	    return TRUE;

	if ((spellnum == SPE_TELEPORT_AWAY) && Teleport_control)
	    return TRUE;

	if ((((spellnum == SPE_FIREBALL || spellnum == SPE_FLAME_SPHERE)
		&& Fire_resistance) ||
	    ((spellnum == SPE_CONE_OF_COLD || spellnum == SPE_FREEZE_SPHERE)
	    	&& Cold_resistance) ||
	    ((spellnum == SPE_LIGHTNING || spellnum == SPE_SHOCK_SPHERE)
	    	&& Shock_resistance) ||
	    (spellnum == SPE_POISON_BLAST && Poison_resistance) ||
	    (spellnum == SPE_ACID_BLAST && Acid_resistance)))
	    return TRUE;

	if (spellnum == SPE_TURN_UNDEAD)
	    return TRUE;

	if (spellnum == SPE_DETECT_MONSTERS)
	    return TRUE;

	if (spellnum == SPE_LIGHT)
	    return TRUE;

	if (spellnum == SPE_DETECT_FOOD)
	    return TRUE;

	if (spellnum == SPE_CLAIRVOYANCE)
	    return TRUE;

	if (spellnum == SPE_DETECT_UNSEEN
	    /* && (!Invisible || !sees_invis(mtmp))*/)
	    return TRUE;

	if (spellnum == SPE_IDENTIFY)
	    return TRUE;

	if (spellnum == SPE_DETECT_TREASURE)
	    return TRUE;

	if (spellnum == SPE_MAGIC_MAPPING)
	    return TRUE;

	if (spellnum == SPE_CHARM_MONSTER)
	    return TRUE;

	if (spellnum == SPE_JUMPING)
	    return TRUE;
	
	if (spellnum == SPE_LEVITATION && (is_flyer(mtmp->data) || levitating(mtmp)))
	    return TRUE;

	if (spellnum == SPE_DIG)
	    return TRUE;

	if (spellnum == SPE_CURE_BLINDNESS && !mtmp->mblinded)
	    return TRUE;

	if (spellnum == SPE_CURE_SICKNESS && !mtmp->msick)
	    return TRUE;

	if (spellnum == SPE_STONE_TO_FLESH)
	    return TRUE;

	if (spellnum == SPE_RESTORE_ABILITY)
	    return TRUE;

	if (spellnum == SPE_KNOCK)
	    return TRUE;

	if (spellnum == SPE_WIZARD_LOCK)
	    return TRUE;

	if (spellnum == SPE_CREATE_MONSTER && mtmp->mtame)
	    return TRUE;

#ifndef TAME_SUMMONING
	/* not going to include spheres in this; they're harmless enough */
	if (spellnum == SPE_CREATE_FAMILIAR && mtmp->mtame)
	    return TRUE;
#endif
#else
	/* aggravate monsters, etc. won't be cast by peaceful monsters */
	if (mtmp->mpeaceful && (spellnum == MGC_AGGRAVATION ||
		spellnum == MGC_SUMMON_MONS || spellnum == MGC_CLONE_WIZ))
	    return TRUE;
	/* haste self when already fast */
	if (mtmp->permspeed == MFAST && spellnum == MGC_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((mtmp->minvis || mtmp->invis_blkd) && spellnum == MGC_DISAPPEAR)
	    return TRUE;
	/* peaceful monster won't cast invisibility if you can't see invisible,
	   same as when monsters drink potions of invisibility.  This doesn't
	   really make a lot of sense, but lets the player avoid hitting
	   peaceful monsters by mistake */
	if (mtmp->mpeaceful && !See_invisible && spellnum == MGC_DISAPPEAR)
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && spellnum == MGC_CURE_SELF)
	    return TRUE;
	/* don't summon monsters if it doesn't think you're around */
	if (!mcouldseeu && (spellnum == MGC_SUMMON_MONS ||
		(!mtmp->iswiz && spellnum == MGC_CLONE_WIZ)))
	    return TRUE;
	if ((!mtmp->iswiz || flags.no_of_wizards > 1)
						&& spellnum == MGC_CLONE_WIZ)
	    return TRUE;
#endif
    } else if (adtyp == AD_CLRC) {
	/* summon insects/sticks to snakes won't be cast by peaceful monsters */
	if (mtmp->mpeaceful && spellnum == CLC_INSECTS)
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && spellnum == CLC_CURE_SELF)
	    return TRUE;
	/* don't summon insects if it doesn't think you're around */
	if (!mcouldseeu && spellnum == CLC_INSECTS)
	    return TRUE;
	/* blindness spell on blinded player */
	if (Blinded && spellnum == CLC_BLIND_YOU)
	    return TRUE;
    }
    return FALSE;
}

STATIC_DCL
boolean
mspell_would_be_useless(mtmp, mdef, adtyp, spellnum)
struct monst *mtmp;
struct monst *mdef;
unsigned int adtyp;
int spellnum;
{
    if (adtyp == AD_SPEL) {
#ifdef COMBINED_SPELLS
	if ((spellnum == SPE_PSI_BOLT ||
	     spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_CONFUSE_MONSTER ||
	     spellnum == SPE_CAUSE_FEAR) &&
             dist2(mtmp->mx,mtmp->my,mdef->mx,mdef->my) > 2)
	     return TRUE;
	/* haste self when already fast */
	if (mtmp->permspeed == MFAST && spellnum == SPE_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((mtmp->minvis || mtmp->invis_blkd) && spellnum == SPE_INVISIBILITY)
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && (spellnum == SPE_HEALING
				       || spellnum == SPE_EXTRA_HEALING))
	    return TRUE;
	/* don't summon monsters if it doesn't think you're around */
	if ((!mtmp->iswiz || flags.no_of_wizards > 1)
			&& spellnum == SPE_BOOK_OF_THE_DEAD)
	    return TRUE;
	if (spellnum == SPE_CREATE_MONSTER)
	    return TRUE;
#ifndef TAME_SUMMONING
	if (spellnum == SPE_CREATE_FAMILIAR)
	    return TRUE;
#endif
	
	if (spellnum == SPE_REMOVE_CURSE)
	{
            register struct obj *otmp;
	    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
	    {
		    if (otmp->cursed && 
		        (otmp->otyp == LOADSTONE ||
			 otmp->owornmask))
		{
	      	    return FALSE;
	        } 
	    }
	    return TRUE;
	}
	
	if ((spellnum == SPE_FIREBALL ||
	     spellnum == SPE_CONE_OF_COLD ||
	     spellnum == SPE_POISON_BLAST ||
	     spellnum == SPE_ACID_BLAST) &&
	     mon_reflects(mdef, (char *)0) &&
	     !mtmp->iswiz && !is_prince(mtmp->data) && !is_lord(mtmp->data) &&
	     mtmp->data->msound != MS_NEMESIS)
	    return TRUE;
	
	if ((spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_FINGER_OF_DEATH) &&
	     (nonliving(mdef->data) || is_demon(mdef->data)))
	     return TRUE;

	if ((spellnum == SPE_SLEEP ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_LIGHTNING) && mon_reflects(mdef, (char *)0))
	    return TRUE;

	if ((spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_SLEEP ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_POLYMORPH ||
	     spellnum == SPE_FORCE_BOLT) && resists_magm(mdef))
	    return TRUE;

	if ((spellnum == SPE_DRAIN_LIFE) &&
	    (resists_magm(mdef) || resists_drli(mdef)))
	    return TRUE;

	if ((spellnum == SPE_TELEPORT_AWAY) && control_teleport(mdef->data))
	    return TRUE;

	if ((((spellnum == SPE_FIREBALL || spellnum == SPE_FLAME_SPHERE)
		&& resists_fire(mdef)) ||
	    ((spellnum == SPE_CONE_OF_COLD || spellnum == SPE_FREEZE_SPHERE)
	    	&& resists_cold(mdef)) ||
	    ((spellnum == SPE_LIGHTNING || spellnum == SPE_SHOCK_SPHERE)
	    	&& resists_elec(mdef)) ||
	    (spellnum == SPE_POISON_BLAST && resists_poison(mdef)) ||
	    (spellnum == SPE_ACID_BLAST && resists_acid(mdef))))
	    return TRUE;

	if (spellnum == SPE_TURN_UNDEAD)
	    return TRUE;

	if (spellnum == SPE_DETECT_MONSTERS)
	    return TRUE;

	if (spellnum == SPE_LIGHT)
	    return TRUE;

	if (spellnum == SPE_DETECT_FOOD)
	    return TRUE;

	if (spellnum == SPE_CLAIRVOYANCE)
	    return TRUE;

	if (spellnum == SPE_DETECT_UNSEEN)
	    return TRUE;

	if (spellnum == SPE_IDENTIFY)
	    return TRUE;

	if (spellnum == SPE_DETECT_TREASURE)
	    return TRUE;

	if (spellnum == SPE_MAGIC_MAPPING)
	    return TRUE;

	if (spellnum == SPE_CHARM_MONSTER)
	    return TRUE;

	if (spellnum == SPE_JUMPING)
	    return TRUE;
	
	if (spellnum == SPE_LEVITATION && (is_flyer(mtmp->data) || levitating(mtmp)))
	    return TRUE;
	
	if (spellnum == SPE_DIG)
	    return TRUE;

	if (spellnum == SPE_CURE_BLINDNESS && !mtmp->mblinded)
	    return TRUE;

	if (spellnum == SPE_CURE_SICKNESS && !mtmp->msick)
	    return TRUE;

	if (spellnum == SPE_STONE_TO_FLESH)
	    return TRUE;

	if (spellnum == SPE_RESTORE_ABILITY)
	    return TRUE;

	if (spellnum == SPE_KNOCK)
	    return TRUE;

	if (spellnum == SPE_WIZARD_LOCK)
	    return TRUE;
#else
	/* haste self when already fast */
	if (mtmp->permspeed == MFAST && spellnum == MGC_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((mtmp->minvis || mtmp->invis_blkd) && spellnum == MGC_DISAPPEAR)
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && spellnum == MGC_CURE_SELF)
	    return TRUE;
	/* don't summon monsters if it doesn't think you're around */
	if ((!mtmp->iswiz || flags.no_of_wizards > 1)
						&& spellnum == MGC_CLONE_WIZ)
	    return TRUE;
#ifndef TAME_SUMMONING
        if (spellnum == MGC_SUMMON_MONS)
	    return TRUE;
#endif
#endif
    } else if (adtyp == AD_CLRC) {
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && spellnum == CLC_CURE_SELF)
	    return TRUE;
	/* blindness spell on blinded player */
	if ((!haseyes(mdef->data) || mdef->mblinded) && spellnum == CLC_BLIND_YOU)
	    return TRUE;
    }
    return FALSE;
}

STATIC_DCL
boolean
uspell_would_be_useless(mdef, adtyp, spellnum)
register struct monst *mdef;
unsigned int adtyp;
int spellnum;
{
    if (adtyp == AD_SPEL) {
#ifdef COMBINED_SPELLS
	/* aggravate monsters, etc. won't be cast by peaceful monsters */
	if (spellnum == SPE_BOOK_OF_THE_DEAD)
	    return TRUE;
	/* haste self when already fast */
	if (Fast && spellnum == SPE_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((HInvis & INTRINSIC) && spellnum == SPE_INVISIBILITY)
	    return TRUE;
	/* healing when already healed */
	if (u.mh == u.mhmax && (spellnum == SPE_HEALING || spellnum == SPE_EXTRA_HEALING))
	    return TRUE;
#ifndef TAME_SUMMONING
        if (spellnum == SPE_CREATE_FAMILIAR)
	    return TRUE;
#endif
	if (spellnum == SPE_LEVITATION && (Flying || Levitation))
	    return TRUE;
	
	if (spellnum == SPE_DIG)
	    return TRUE;
	
	if (spellnum == SPE_REMOVE_CURSE)
	{
            register struct obj *otmp;
	    for (otmp = invent; otmp; otmp = otmp->nobj)
	    {
		    if (otmp->cursed && 
		        (otmp->otyp == LOADSTONE ||
			 otmp->owornmask))
		{
	      	    return FALSE;
	        } 
	    }
	    return TRUE;
	}
	
	if ((spellnum == SPE_FIREBALL ||
	     spellnum == SPE_CONE_OF_COLD ||
	     spellnum == SPE_POISON_BLAST ||
	     spellnum == SPE_ACID_BLAST ||
	     spellnum == SPE_SLEEP ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_LIGHTNING ||
	     spellnum == SPE_FORCE_BOLT ||
	     spellnum == SPE_PSI_BOLT ||
	     spellnum == SPE_POLYMORPH ||
	     spellnum == SPE_DRAIN_LIFE ||
	     spellnum == SPE_TELEPORT_AWAY) && (mdef == (struct monst *)0))
	     return TRUE;
	
	if ((spellnum == SPE_FIREBALL ||
	     spellnum == SPE_CONE_OF_COLD ||
	     spellnum == SPE_POISON_BLAST ||
	     spellnum == SPE_ACID_BLAST) &&
	     (mdef)) { 
	     if (mon_reflects(mdef, (char *)0) &&
	     !youmonst.iswiz &&
	     !is_prince(youmonst.data) && !is_lord(youmonst.data) &&
	     youmonst.data->msound != MS_NEMESIS)
	    return TRUE;
	}

	if ((spellnum == SPE_SLEEP ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_LIGHTNING) &&
	     (mdef)) {
	     if (mon_reflects(mdef, (char *)0))
		    return TRUE;
	}
	
	if ((spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_FINGER_OF_DEATH) &&
	     (mdef)) {
	     if (nonliving(mdef->data) || is_demon(mdef->data))
		     return TRUE;
	}

	if ((spellnum == SPE_TOUCH_OF_DEATH ||
	     spellnum == SPE_SLEEP ||
	     spellnum == SPE_FINGER_OF_DEATH ||
	     spellnum == SPE_MAGIC_MISSILE ||
	     spellnum == SPE_POLYMORPH ||
	     spellnum == SPE_FORCE_BOLT) && mdef) {
	     if (!mdef || resists_magm(mdef))
		    return TRUE;
	}

	if ((spellnum == SPE_DRAIN_LIFE) &&
	    (mdef)) {
	    if (resists_magm(mdef) || resists_drli(mdef))
		    return TRUE;
	}

	if ((spellnum == SPE_TELEPORT_AWAY) && mdef) {
		if (control_teleport(mdef->data))
		    return TRUE;
	}

	if ((((spellnum == SPE_FIREBALL || spellnum == SPE_FLAME_SPHERE)
		&& (!mdef || resists_fire(mdef))) ||
	    ((spellnum == SPE_CONE_OF_COLD || spellnum == SPE_FREEZE_SPHERE)
	    	&& (!mdef || resists_cold(mdef))) ||
	    ((spellnum == SPE_LIGHTNING || spellnum == SPE_SHOCK_SPHERE)
	    	&& (!mdef || resists_elec(mdef))) ||
	    (spellnum == SPE_POISON_BLAST
	        && (!mdef || resists_poison(mdef))) ||
	    (spellnum == SPE_ACID_BLAST 
	        && (!mdef || resists_acid(mdef)))))
	    return TRUE;

	if (spellnum == SPE_TURN_UNDEAD)
	    return TRUE;

	if (spellnum == SPE_DETECT_MONSTERS)
	    return TRUE;

	if (spellnum == SPE_LIGHT)
	    return TRUE;

	if (spellnum == SPE_DETECT_FOOD)
	    return TRUE;

	if (spellnum == SPE_CLAIRVOYANCE)
	    return TRUE;

	if (spellnum == SPE_DETECT_UNSEEN/* && See_invisible*/)
	    return TRUE;

	if (spellnum == SPE_IDENTIFY)
	    return TRUE;

	if (spellnum == SPE_DETECT_TREASURE)
	    return TRUE;

	if (spellnum == SPE_MAGIC_MAPPING)
	    return TRUE;

	if (spellnum == SPE_CHARM_MONSTER)
	    return TRUE;

	if (spellnum == SPE_JUMPING)
	    return TRUE;
	
	if (spellnum == SPE_LEVITATION && (Flying || Levitation))
	    return TRUE;
	
	if (spellnum == SPE_DIG)
	    return TRUE;

	if (spellnum == SPE_CURE_BLINDNESS && !Blind)
	    return TRUE;

	if (spellnum == SPE_CURE_SICKNESS && !Sick)
	    return TRUE;

	if (spellnum == SPE_STONE_TO_FLESH)
	    return TRUE;

	if (spellnum == SPE_RESTORE_ABILITY)
	    return TRUE;

	if (spellnum == SPE_KNOCK)
	    return TRUE;

	if (spellnum == SPE_WIZARD_LOCK)
	    return TRUE;

	if (spellnum == SPE_CREATE_MONSTER)
	    return TRUE;
#else
	/* aggravate monsters, etc. won't be cast by peaceful monsters */
	if (spellnum == MGC_CLONE_WIZ)
	    return TRUE;
	/* haste self when already fast */
	if (Fast && spellnum == MGC_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((HInvis & INTRINSIC) && spellnum == MGC_DISAPPEAR)
	    return TRUE;
	/* healing when already healed */
	if (u.mh == u.mhmax && spellnum == MGC_CURE_SELF)
	    return TRUE;
#ifndef TAME_SUMMONING
        if (spellnum == MGC_SUMMON_MONS)
	    return TRUE;
#endif
#endif
    } else if (adtyp == AD_CLRC) {
	/* healing when already healed */
	if (u.mh == u.mhmax && spellnum == SPE_HEALING)
	    return TRUE;
    }
    return FALSE;
}
#endif /* OVLB */
#ifdef OVL0

/* convert 1..10 to 0..9; add 10 for second group (spell casting) */
#define ad_to_typ(k) (10 + (int)k - 1)

int
buzzmu(mtmp, mattk)		/* monster uses spell (ranged) */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	/* don't print constant stream of curse messages for 'normal'
	   spellcasting monsters at range */
	if (mattk->adtyp > AD_SPC2)
	{
#ifdef COMBINED_SPELLS
	    if (mattk->adtyp == AD_SPEL) {
	    	if (!mtmp->mpeaceful && !mtmp->mtame && lined_up(mtmp))
		    return castmu(mtmp,mattk,TRUE,(mtmp->mux==u.ux && mtmp->muy==u.uy));
	    }
#endif
	    return(0);
	}

	if (mtmp->mcan) {
	    cursetxt(mtmp, FALSE);
	    return(0);
	}
	if(lined_up(mtmp) && rn2(3)) {
	    nomul(0);
	    if(mattk->adtyp && (mattk->adtyp < 11)) { /* no cf unsigned >0 */
		if(canseemon(mtmp))
		    pline("%s zaps you with a %s!", Monnam(mtmp),
			  flash_types[ad_to_typ(mattk->adtyp)]);
		buzz(-ad_to_typ(mattk->adtyp), (int)mattk->damn,
		     mtmp->mx, mtmp->my, sgn(tbx), sgn(tby));
	    } else impossible("Monster spell %d cast", mattk->adtyp-1);
	}
	return(1);
}

/* return values:
 * 2: target died
 * 1: successful spell
 * 0: unsuccessful spell
 */
int
castmm(mtmp, mdef, mattk)
	register struct monst *mtmp;
	register struct monst *mdef;
	register struct attack *mattk;
{
	int	dmg, ml = mtmp->m_lev;
	int ret;
	int spellnum = 0;

	if ((mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) && ml) {
	    int cnt = 40;

	    do {
		if (mattk->adtyp == AD_SPEL)
#ifdef COMBINED_SPELLS
		    spellnum = choose_magic_spell(mtmp, mtmp->mtame ? -ml : ml);
#else
		    spellnum = choose_magic_spell(mtmp, rn2(ml));
#endif
		else
		    spellnum = choose_clerical_spell(rn2(ml));
		/* not trying to attack?  don't allow directed spells */
	    } while(--cnt > 0 &&
		    mspell_would_be_useless(mtmp, mdef,
		                            mattk->adtyp, spellnum));
	    if (cnt == 0) return 0;

	}

	/* monster unable to cast spells? */
	if(mtmp->mcan ||
#ifdef COMBINED_SPELLS
		((mattk->adtyp == AD_SPEL && !can_cast_spells(mtmp)) ||
		 (mattk->adtyp != AD_SPEL && mtmp->mspec_used))
#else
		mtmp->mspec_used
#endif
		|| !ml) {
	    if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my))
	    {
                char buf[BUFSZ];
		Sprintf(buf, Monnam(mtmp));

		if (is_undirected_spell(mattk->adtyp, spellnum))
	            pline("%s points all around, then curses.", buf);
		else
	            pline("%s points at %s, then curses.",
		          buf, mon_nam(mdef));

	    } else if ((!(moves % 4) || !rn2(4))) {
	        if (flags.soundok) Norep("You hear a mumbled curse.");
	    }
	    return(0);
	}

	if (mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) {
	    register struct obj *obj;
#ifdef COMBINED_SPELLS
            if (mattk->adtyp == AD_SPEL)
	    	mtmp->mspec_used +=
			spelltimeout(mtmp, objects[spellnum].oc_level);
	    else
#endif
	    mtmp->mspec_used = 10 - mtmp->m_lev;
	    if (mtmp->mspec_used < 1) mtmp->mspec_used = 1;
	    for (obj = mtmp->minvent; obj; obj = obj->nobj)
	        if (obj->oartifact &&
		    obj->oartifact == ART_EYE_OF_THE_AETHIOPICA)
		{
		    mtmp->mspec_used = 0;
		    break;
		}
	}

#ifdef COMBINED_SPELLS
	if (mtmp->mconf) {
#else
	if(rn2(ml*10) < (mtmp->mconf ? 100 : 20)) {	/* fumbled attack */
#endif
	    if (canseemon(mtmp) && flags.soundok)
		pline_The("air crackles around %s.", mon_nam(mtmp));
	    return(0);
	}
	if (canspotmon(mtmp) || canspotmon(mdef)) {
            char buf[BUFSZ];
	    Sprintf(buf, " at ");
	    Strcat(buf, mon_nam(mdef));
	    pline("%s casts a spell%s!",
		  canspotmon(mtmp) ? Monnam(mtmp) : "Something",
		  is_undirected_spell(mattk->adtyp, spellnum) ? "" : buf);
	}

	if (mattk->damd)
	    dmg = d((int)((ml/2) + mattk->damn), (int)mattk->damd);
	else dmg = d((int)((ml/2) + 1), 6);

#ifdef COMBINED_SPELLS
	/* this short-circuit behaviour prevents attacks from */
	/* after spells from being used unless there's no */
	/* magical energy to cast the spell. */
	ret = (mattk->adtyp == AD_SPEL &&
		is_racial(mtmp->data)) ? 3 : 1;
#else
	ret = 1;
#endif

	switch (mattk->adtyp) {

	    case AD_FIRE:
	        if (canspotmon(mdef))
		    pline("%s is enveloped in flames.", Monnam(mdef));
		if(resists_fire(mdef)) {
			shieldeff(mdef->mx, mdef->my);
	                if (canspotmon(mdef))
			    pline("But %s resists the effects.",
			        mhe(mdef));
			dmg = 0;
		}
		break;
	    case AD_COLD:
	        if (canspotmon(mdef))
		    pline("%s is covered in frost.", Monnam(mdef));
		if(resists_fire(mdef)) {
			shieldeff(mdef->mx, mdef->my);
	                if (canspotmon(mdef))
			    pline("But %s resists the effects.",
			        mhe(mdef));
			dmg = 0;
		}
		break;
	    case AD_MAGM:
	        if (canspotmon(mdef))
		    pline("%s is hit by a shower of missiles!", Monnam(mdef));
		if(resists_magm(mdef)) {
			shieldeff(mdef->mx, mdef->my);
	                if (canspotmon(mdef))
			    pline_The("missiles bounce off!");
			dmg = 0;
		} else dmg = d((int)mtmp->m_lev/2 + 1,6);
		break;
	    case AD_SPEL:	/* wizard spell */
	    case AD_CLRC:       /* clerical spell */
	    {
	        /*aggravation is a special case;*/
		/*it's undirected but should still target the*/
		/*victim so as to aggravate you*/
	        if (is_undirected_spell(mattk->adtyp, spellnum)
#ifndef COMBINED_SPELLS
		&& (mattk->adtyp != AD_SPEL
		    || (spellnum != MGC_AGGRAVATION &&
		      spellnum != MGC_SUMMON_MONS))
#endif
		      )
		{
		    if (mattk->adtyp == AD_SPEL)
		        cast_wizard_spell(mtmp, dmg, spellnum);
		    else
		        cast_cleric_spell(mtmp, dmg, spellnum);
		}
		else if (mattk->adtyp == AD_SPEL)
		    ucast_wizard_spell(mtmp, mdef, dmg, spellnum);
		else
		    ucast_cleric_spell(mtmp, mdef, dmg, spellnum);
		dmg = 0; /* done by the spell casting functions */
		break;
	    }
	}
	if (dmg > 0 && mdef->mhp > 0)
	{
	    mdef->mhp -= dmg;
	    if (mdef->mhp < 1) monkilled(mdef, "", mattk->adtyp);
	}
	if (mdef && mdef->mhp < 1) return 2;
	return(ret);
}

/* return values:
 * 2: target died
 * 1: successful spell
 * 0: unsuccessful spell
 */
int
castum(mtmp, mattk)
        register struct monst *mtmp; 
	register struct attack *mattk;
{
	int dmg, ml = mons[u.umonnum].mlevel;
	int ret;
	int spellnum = 0;
	boolean directed = FALSE;

	/* Three cases:
	 * -- monster is attacking you.  Search for a useful spell.
	 * -- monster thinks it's attacking you.  Search for a useful spell,
	 *    without checking for undirected.  If the spell found is directed,
	 *    it fails with cursetxt() and loss of mspec_used.
	 * -- monster isn't trying to attack.  Select a spell once.  Don't keep
	 *    searching; if that spell is not useful (or if it's directed),
	 *    return and do something else. 
	 * Since most spells are directed, this means that a monster that isn't
	 * attacking casts spells only a small portion of the time that an
	 * attacking monster does.
	 */
	if ((mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) && ml) {
	    int cnt = 40;

	    do {
		if (mattk->adtyp == AD_SPEL)
#ifdef COMBINED_SPELLS
		    spellnum = choose_magic_spell(&youmonst, -ml);
#else
		    spellnum = choose_magic_spell(&youmonst, rn2(ml));
#endif
		else
		    spellnum = choose_clerical_spell(rn2(ml));
		/* not trying to attack?  don't allow directed spells */
		if (!mtmp || mtmp->mhp < 1) {
		    if (is_undirected_spell(mattk->adtyp, spellnum) && 
			!uspell_would_be_useless(mtmp, mattk->adtyp, spellnum)) {
		        break;
		    }
		}
	    } while(--cnt > 0 &&
	            ((!mtmp && !is_undirected_spell(mattk->adtyp, spellnum))
		    || uspell_would_be_useless(mtmp, mattk->adtyp, spellnum)));
	    if (cnt == 0) {
	        You("have no spells to cast right now!");
		return 0;
	    }
	}

#ifndef COMBINED_SPELLS
	if (spellnum == MGC_AGGRAVATION && !mtmp)
	{
	    /* choose a random monster on the level */
	    int j = 0, k = 0;
	    for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	        if (!mtmp->mtame && !mtmp->mpeaceful) j++;
	    if (j > 0)
	    {
	        k = rn2(j); 
	        for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	            if (!mtmp->mtame && !mtmp->mpeaceful)
		        if (--k < 0) break;
	    }
	}
#endif

	directed = mtmp && !is_undirected_spell(mattk->adtyp, spellnum);

	/* unable to cast spells? */
#ifdef COMBINED_SPELLS
	if(u.uen < 5*objects[spellnum].oc_level) {
#else
	if(u.uen < ml) {
#endif
	    if (directed)
	        You("point at %s, then curse.", mon_nam(mtmp));
	    else
	        You("point all around, then curse.");
	    return(0);
	}

#ifdef COMBINED_SPELLS
	if (mattk->adtyp == AD_SPEL) {
	    u.uen -= 5*objects[spellnum].oc_level;
	} else if (mattk->adtyp == AD_CLRC) {
	    u.uen -= ml;
	}
#else
	if (mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) {
	    u.uen -= ml;
	}
#endif

#ifdef COMBINED_SPELLS
	if (Confusion) {
#else
	if(rn2(ml*10) < (Confusion ? 100 : 20)) {	/* fumbled attack */
#endif
	    pline_The("air crackles around you.");
	    return(0);
	}

        You("cast a spell%s%s!",
	      directed ? " at " : "",
	      directed ? mon_nam(mtmp) : "");

/*
 *	As these are spells, the damage is related to the level
 *	of the monster casting the spell.
 */
	if (mattk->damd)
	    dmg = d((int)((ml/2) + mattk->damn), (int)mattk->damd);
	else dmg = d((int)((ml/2) + 1), 6);

#ifdef COMBINED_SPELLS
	/* this short-circuit behaviour prevents attacks from */
	/* after spells from being used unless there's no */
	/* magical energy to cast the spell. */
	ret = (mattk->adtyp == AD_SPEL &&
		is_racial(youmonst.data)) ? 3 : 1;
#else
	ret = 1;
#endif

	switch (mattk->adtyp) {
	    case AD_FIRE:
		pline("%s is enveloped in flames.", Monnam(mtmp));
		if(resists_fire(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
			pline("But %s resists the effects.",
			    mhe(mtmp));
			dmg = 0;
		}
		break;
	    case AD_COLD:
		pline("%s is covered in frost.", Monnam(mtmp));
		if(resists_fire(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
			pline("But %s resists the effects.",
			    mhe(mtmp));
			dmg = 0;
		}
		break;
	    case AD_MAGM:
		pline("%s is hit by a shower of missiles!", Monnam(mtmp));
		if(resists_magm(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
			pline_The("missiles bounce off!");
			dmg = 0;
		} else dmg = d((int)ml/2 + 1,6);
		break;
	    case AD_SPEL:	/* wizard spell */
	    case AD_CLRC:       /* clerical spell */
	    {
		if (mattk->adtyp == AD_SPEL)
		    ucast_wizard_spell(&youmonst, mtmp, dmg, spellnum);
		else
		    ucast_cleric_spell(&youmonst, mtmp, dmg, spellnum);
		dmg = 0; /* done by the spell casting functions */
		break;
	    }
	}

	if (mtmp && dmg > 0 && mtmp->mhp > 0)
	{
	    mtmp->mhp -= dmg;
	    if (mtmp->mhp < 1) killed(mtmp, mattk->adtyp);
	}
	if (mtmp && mtmp->mhp < 1) return 2;

	return(ret);
}

extern NEARDATA const int nasties[];
extern void NDECL(cast_protection);

/* monster wizard and cleric spellcasting functions */
/*
   If dmg is zero, then the monster is not casting at you.
   If the monster is intentionally not casting at you, we have previously
   called spell_would_be_useless() and spellnum should always be a valid
   undirected spell.
   If you modify either of these, be sure to change is_undirected_spell()
   and spell_would_be_useless().
 */
STATIC_OVL
void
ucast_wizard_spell(mattk, mtmp, dmg, spellnum)
struct monst *mattk;
struct monst *mtmp;
int dmg;
int spellnum;
{
    boolean resisted = FALSE;
    boolean yours = (mattk == &youmonst);
    struct obj *pseudo = 0;
    int tmp1, tmp2;
    coord bypos;

    if (dmg == 0 && !is_undirected_spell(AD_SPEL, spellnum)) {
	impossible("cast directed wizard spell (%d) with dmg=0?", spellnum);
	return;
    }

    if (mtmp && mtmp->mhp < 1)
    {
        impossible("monster already dead?");
	return;
    }

    switch (spellnum) {
#ifdef COMBINED_SPELLS
    case SPE_TOUCH_OF_DEATH:
#else
    case MGC_DEATH_TOUCH:
#endif
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("touch of death with no mtmp");
	    return;
	}
	if (yours)
	    pline("You're using the touch of death!");
	else if (canseemon(mattk))
	{
	    char buf[BUFSZ];
	    Sprintf(buf, "%s%s", mtmp->mtame ? "Oh no, " : "",
	                         mhe(mattk));
	    if (!mtmp->mtame)
	        *buf = highc(*buf);

	    pline("%s's using the touch of death!", buf);
	}

	if (nonliving(mtmp->data) || is_demon(mtmp->data)) {
	    if (yours || canseemon(mtmp))
	        pline("%s seems no deader than before.", Monnam(mtmp));
	} else if (!(resisted = resist(mtmp, 0, 0, FALSE)) ||
	           rn2(mons[u.umonnum].mlevel) > 12) {
            mtmp->mhp = -1;
	    if (yours) killed(mtmp, AD_SPEL);
	    else monkilled(mtmp, "", AD_SPEL);
	    return;
	} else {
	    if (resisted) shieldeff(mtmp->mx, mtmp->my);
	    if (yours || canseemon(mtmp))
	    {
	        if (mtmp->mtame)
		    pline("Lucky for %s, it didn't work!", mon_nam(mtmp));
		else
	            pline("That didn't work...");
            }
	}
	dmg = 0;
	break;
#ifdef COMBINED_SPELLS
    case SPE_BOOK_OF_THE_DEAD:
#else
    case MGC_CLONE_WIZ:
#endif
	if (mattk->iswiz && flags.no_of_wizards == 1) {
	    if (canspotmon(mattk))
	    	pline("Double Trouble...");
	    clonewiz(mtmp);
	    dmg = 0;
	} else
	    impossible("bad wizard cloning?");
	break;
#ifndef COMBINED_SPELLS
    case MGC_SUMMON_MONS:
    {
	int count = 0;
        register struct monst *mpet;

        if (!rn2(10) && Inhell) {
	    if (yours) demonpet();
	    else msummon(mattk);
	} else {
	    register int i, j;
            int makeindex, tmp = (u.ulevel > 3) ? u.ulevel / 3 : 1;

	    if (mtmp)
	        bypos.x = mtmp->mx, bypos.y = mtmp->my;
	    else if (yours)
	        bypos.x = u.ux, bypos.y = u.uy;
            else
	        bypos.x = mattk->mx, bypos.y = mattk->my;

	    for (i = rnd(tmp); i > 0; --i)
	        for(j=0; j<20; j++) {

	            do {
	                makeindex = pick_nasty();
	            } while (attacktype(&mons[makeindex], AT_MAGC) &&
	                     monstr[makeindex] >= monstr[u.umonnum]);
                    if (yours &&
		        !enexto(&bypos, u.ux, u.uy, &mons[makeindex]))
		        continue;
                    if (!yours &&
		        !enexto(&bypos, mattk->mx, mattk->my, &mons[makeindex]))
		        continue;
		    if ((mpet = makemon(&mons[makeindex], 
                          bypos.x, bypos.y, 
			  (yours || mattk->mtame) ? MM_EDOG :
			                            NO_MM_FLAGS)) != 0) {
                        mpet->msleeping = 0;
                        if (yours || mattk->mtame)
			    initedog(mpet);
			else if (mattk->mpeaceful)
			    mpet->mpeaceful = 1;
			else mpet->mpeaceful = mpet->mtame = 0;

                        set_malign(mpet);
                    } else /* GENOD? */
                        mpet = makemon((struct permonst *)0,
                                            bypos.x, bypos.y, NO_MM_FLAGS);
                    if(mpet && (u.ualign.type == 0 ||
		        mpet->data->maligntyp == 0 || 
                        sgn(mpet->data->maligntyp) == sgn(u.ualign.type)) ) {
                        count++;
                        break;
                    }
                }

	    const char *mappear =
		    (count == 1) ? "A monster appears" : "Monsters appear";

	    if (yours || canseemon(mtmp))
	        pline("%s from nowhere!", mappear);
	}

	dmg = 0;
	break;
    }
    case MGC_AGGRAVATION:
	if (!mtmp || mtmp->mhp < 1) {
	    You_feel("lonely.");
	    return;
	}
	/*You_feel("that monsters are aware of %s presence.",
	 *    s_suffix(mon_nam(mtmp))); */
	you_aggravate(mtmp);
	dmg = 0;
	break;
    case MGC_CURSE_ITEMS:
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("curse spell with no mtmp");
	    return;
	}
	if (yours || canseemon(mtmp))
	    You_feel("as though %s needs some help.", mon_nam(mtmp));
	mrndcurse(mtmp);
	dmg = 0;
	break;
    case MGC_DESTRY_ARMR:
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("destroy spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    if (yours || canseemon(mtmp))
	        pline("A field of force surrounds %s!",
	               mon_nam(mtmp));
	} else {
            register struct obj *otmp = some_armor(mtmp), *otmp2;

#define oresist_disintegration(obj) \
		(objects[obj->otyp].oc_oprop == DISINT_RES || \
		 obj_resists(obj, 0, 90) || is_quest_artifact(obj))

	    if (otmp &&
	        !oresist_disintegration(otmp))
	    {
	        pline("%s %s %s!",
		      s_suffix(Monnam(mtmp)),
		      xname(otmp),
		      is_cloak(otmp)  ? "crumbles and turns to dust" :
		      is_shirt(otmp)  ? "crumbles into tiny threads" :
		      is_helmet(otmp) ? "turns to dust and is blown away" :
		      is_gloves(otmp) ? "vanish" :
		      is_boots(otmp)  ? "disintegrate" :
		      is_shield(otmp) ? "crumbles away" :
		                        "turns to dust"
		      );
		obj_extract_self(otmp);
		obfree(otmp, (struct obj *)0);
 	    }
	    else if (yours || canseemon(mtmp))
	        pline("%s looks itchy.", Monnam(mtmp)); 
	}
	dmg = 0;
	break;
    case MGC_WEAKEN_YOU:		/* drain strength */
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("weaken spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    pline("%s looks momentarily weakened.", Monnam(mtmp));
	} else {
	    if (mtmp->mhp < 1)
	    {
	        impossible("trying to drain monster that's already dead");
		return;
	    }
	    if (yours || canseemon(mtmp))
	        pline("%s suddenly seems weaker!", Monnam(mtmp));
            /* monsters don't have strength, so drain max hp instead */
	    mtmp->mhpmax -= dmg;
	    if ((mtmp->mhp -= dmg) <= 0) {
	        if (yours) killed(mtmp, AD_SPEL);
		else monkilled(mtmp, "", AD_SPEL);
            }
	}
	dmg = 0;
	break;
#endif
#ifdef COMBINED_SPELLS
    case SPE_INVISIBILITY:
#else
    case MGC_DISAPPEAR:		/* makes self invisible */
#endif
        if (!yours) {
	    impossible("ucast disappear but not yours?");
	    return;
	}
	if (!(HInvis & INTRINSIC)) {
	    HInvis |= FROMOUTSIDE;
	    if (!Blind && !BInvis) self_invis_message();
	    dmg = 0;
	} else
	    impossible("no reason for player to cast disappear spell?");
	break;
#ifndef COMBINED_SPELLS
    case MGC_STUN_YOU:
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("stun spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    if (yours || canseemon(mtmp))
	        pline("%s seems momentarily disoriented.", Monnam(mtmp));
	} else {
	    
	    if (yours || canseemon(mtmp)) {
	        if (mtmp->mstun)
	            pline("%s struggles to keep %s balance.",
	 	          Monnam(mtmp), mhis(mtmp));
                else
	            pline("%s reels...", Monnam(mtmp));
	    }
	    mtmp->mstun = 1;
	}
	dmg = 0;
	break;
#endif
#ifdef COMBINED_SPELLS
    case SPE_HASTE_SELF:
#else
    case MGC_HASTE_SELF:
#endif
        if (!yours) {
	    impossible("ucast haste but not yours?");
	    return;
	}
        if (!(HFast & INTRINSIC))
	    You("are suddenly moving faster.");
	HFast |= INTRINSIC;
	dmg = 0;
	break;
#ifdef COMBINED_SPELLS
    case SPE_EXTRA_HEALING:
        if (!yours) impossible("ucast healing but not yours?");
	else if (u.mh < u.mhmax) {
	    You("feel better.");
	    if ((u.mh += d(6,8)) > u.mhmax)
		u.mh = u.mhmax;
	    flags.botl = 1;
	    dmg = 0;
	}
	break;
    case SPE_HEALING:
        if (!yours) impossible("ucast healing but not yours?");
	else if (u.mh < u.mhmax) {
	    You("feel better.");
	    if ((u.mh += d(6,4)) > u.mhmax)
		u.mh = u.mhmax;
	    flags.botl = 1;
	    dmg = 0;
	}
	break;
#else
    case MGC_CURE_SELF:
        if (!yours) impossible("ucast healing but not yours?");
	else if (u.mh < u.mhmax) {
	    You("feel better.");
	    if ((u.mh += d(3,6)) > u.mhmax)
		u.mh = u.mhmax;
	    flags.botl = 1;
	    dmg = 0;
	}
	break;
#endif
#ifdef COMBINED_SPELLS
    case SPE_PSI_BOLT:
#else
    case MGC_PSI_BOLT:
#endif
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("psibolt spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    dmg = (dmg + 1) / 2;
	}
	if (canseemon(mtmp))
	    pline("%s winces%s", Monnam(mtmp), (dmg <= 5) ? "." : "!");
	break;
#ifdef COMBINED_SPELLS
    case SPE_FORCE_BOLT:
    case SPE_DRAIN_LIFE:
    case SPE_SLOW_MONSTER:
    case SPE_TELEPORT_AWAY:
    case SPE_KNOCK:
    case SPE_WIZARD_LOCK:
    case SPE_POLYMORPH:
    case SPE_CANCELLATION:
	pseudo = mksobj(spellnum, FALSE, FALSE);
	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;
	if (yours) {
		u.dx = mtmp->mx - u.ux;
		u.dy = mtmp->my - u.uy;
		(void) weffects(pseudo);
	} else {
		tmp1 = tbx; tmp2 = tby;
		tbx = mtmp->mx - mattk->mx;
		tby = mtmp->my - mattk->my;
		mbhit(mattk,rn1(8,6),mbhitm,bhito,pseudo);
	}
	obfree(pseudo, (struct obj *)0);
	if (!yours) {
		tbx = tmp1; tby = tmp2;
	}
	dmg = 0;
    	break;
    case SPE_FIREBALL:
    case SPE_CONE_OF_COLD:
    case SPE_POISON_BLAST:
    case SPE_ACID_BLAST:
    	if (mattk->iswiz || is_prince(mattk->data) || is_lord(mattk->data) ||
		mattk->data->msound == MS_NEMESIS)
	{
		int n = rnd(8) + 1;
		coord pos;
		pos.x = mattk->mx;
		pos.y = mattk->my;
		while(n--) {
		    explode(pos.x, pos.y,
			    spellnum - SPE_MAGIC_MISSILE + 10,
			    yours ? u.ulevel/2 + 1 : 
			    mattk->m_lev/2 + 1,
			    0,
				(spellnum == SPE_CONE_OF_COLD) ?
					EXPL_FROSTY :
				(spellnum == SPE_POISON_BLAST) ?
					EXPL_NOXIOUS :
				(spellnum == SPE_ACID_BLAST) ?
					EXPL_WET :
				EXPL_FIERY);
		    if (spellnum != SPE_CONE_OF_COLD)
		        scatter(pos.x, pos.y,
				yours ? u.ulevel/2 + 1 : 
				mattk->m_lev/2 + 1,
				VIS_EFFECTS|MAY_HIT|
				MAY_DESTROY|MAY_FRACTURE, NULL);
		}
		pos.x = mtmp->mx+rnd(3)-2; pos.y = mtmp->my+rnd(3)-2;
		if (!isok(pos.x,pos.y) || !cansee(pos.x,pos.y) ||
		    IS_STWALL(levl[pos.x][pos.y].typ)) {
		    /* Spell is reflected back to center */
		    pos.x = mtmp->mx; pos.y = mtmp->my;
	        }
		dmg = 0;
		break;
	}
    case SPE_SLEEP:
    case SPE_MAGIC_MISSILE:
    case SPE_FINGER_OF_DEATH:
    case SPE_LIGHTNING:
    {
    	int eff = (spellnum - SPE_MAGIC_MISSILE + 10) * ((yours) ? 1 : -1);
	buzz(eff,
	     mattk->m_lev / 2 + 1,
	     yours ? u.ux : mattk->mx,
	     yours ? u.uy : mattk->my,
	     yours ? sgn(mtmp->mx-u.ux) : sgn(mtmp->mx-mattk->mx),
	     yours ? sgn(mtmp->my-u.uy) : sgn(mtmp->my-mattk->my));
	dmg = 0;
    	break;
    }
    case SPE_CAUSE_FEAR:
	dmg = 0;
    	monflee(mtmp, rnd(6), FALSE, TRUE);
        break;
    case SPE_CONFUSE_MONSTER:
	dmg = 0;
	if (canseemon(mtmp)) pline("%s looks confused.", Monnam(mtmp));
    	mtmp->mconf = 1;
        break;
    case SPE_CREATE_MONSTER:
    {
    	int count = 1 + (!rn2(73)) ? rnd(4) : 0;
	boolean confused = (yours) ? (Confusion > 0) : mattk->mconf;
	const char *mappear =
		(count == 1) ? "A monster appears" : "Monsters appear";
	bypos.x = mattk->mx;
	bypos.y = mattk->my;
	dmg = 0;
	for (; count > 0; count--)
	{
		if (enexto(&bypos, mattk->mx, mattk->my, (struct permonst *)0))
        		(void) makemon(
				confused ? &mons[PM_ACID_BLOB]
				         : (struct permonst *)0,
				bypos.x, bypos.y, NO_MM_FLAGS);
	}
	pline("%s from nowhere!", mappear);
    	break;
    }
    case SPE_CAUSE_AGGRAVATION:
    	dmg = 0;
	you_aggravate(mtmp);
	break;
    case SPE_FLAME_SPHERE:
    case SPE_FREEZE_SPHERE:
    case SPE_SHOCK_SPHERE:
    case SPE_CREATE_FAMILIAR:
    {
    	register struct monst *mpet = 0;
	register struct permonst *montype =
		spellnum == SPE_FLAME_SPHERE  ? &mons[PM_FLAMING_SPHERE] :
		spellnum == SPE_FREEZE_SPHERE ? &mons[PM_FREEZING_SPHERE] :
		spellnum == SPE_SHOCK_SPHERE  ? &mons[PM_SHOCKING_SPHERE] :
		(!rn2(3) ? (rn2(2) ? &mons[PM_LITTLE_DOG] : &mons[PM_KITTEN])
			    : rndmonst() );
	bypos.x = mattk->mx;
	bypos.y = mattk->my;
	
	dmg = 0;
	
	if (enexto(&bypos, mattk->mx, mattk->my, montype))
		mpet = makemon(montype, bypos.x, bypos.y,
			(yours || mattk->mtame) ? MM_EDOG :  NO_MM_FLAGS);
	if (mpet)
	{
                mpet->msleeping = 0;
                if (yours || mattk->mtame)
		    initedog(mpet);
		else if (mattk->mpeaceful)
		        mpet->mpeaceful = 1;
		else mpet->mpeaceful = mpet->mtame = 0;

		set_malign(mpet);
		if (canseemon(mpet))
			pline("%s appears from nowhere!", Amonnam(mpet));
	}
		break;
	case SPE_CURE_BLINDNESS:
	{
		if (yours)
			healup(0, 0, FALSE, TRUE);
		dmg = 0;
		break;
	}
	case SPE_CURE_SICKNESS:
		if (yours) {
			if (Sick) You("are no longer ill.");
			if (Slimed) {
			    pline_The("slime disappears!");
			    Slimed = 0;
			 /* flags.botl = 1; -- healup() handles this */
			}
			healup(0, 0, TRUE, FALSE);
		}
		dmg = 0;
		break;
	case SPE_PROTECTION:
		if (yours)
			cast_protection();
		dmg = 0;
		break;
	case SPE_LEVITATION:
		if (yours) {
			struct obj *pseudo = mksobj(spellnum, FALSE, FALSE);
			pseudo->cursed = 0;
			pseudo->blessed =
				(youmonst.iswiz ||
				 is_lord(youmonst.data) ||
				 is_prince(youmonst.data) ||
				 youmonst.data->msound == MS_NEMESIS);
			pseudo->quan = 20L;
			(void) peffects(pseudo);
			obfree(pseudo, (struct obj *)0);
		}
		dmg = 0;
    		break;
    }
#endif
    default:
	impossible("ucastm: invalid magic spell (%d)", spellnum);
	dmg = 0;
	break;
    }

    if (dmg > 0 && mtmp && mtmp->mhp > 0)
    {
        mtmp->mhp -= dmg;
        if (mtmp->mhp < 1) {
	    if (yours) killed(mtmp, AD_SPEL);
	    else monkilled(mtmp, "", AD_SPEL);
	}
    }
}

STATIC_OVL
void
ucast_cleric_spell(mattk, mtmp, dmg, spellnum)
struct monst *mattk;
struct monst *mtmp;
int dmg;
int spellnum;
{
    boolean yours = (mattk == &youmonst);

    if (dmg == 0 && !is_undirected_spell(AD_CLRC, spellnum)) {
	impossible("cast directed cleric spell (%d) with dmg=0?", spellnum);
	return;
    }

    if (mtmp && mtmp->mhp < 1)
    {
        impossible("monster already dead?");
	return;
    }

    switch (spellnum) {
    case CLC_GEYSER:
	/* this is physical damage, not magical damage */
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("geyser spell with no mtmp");
	    return;
	}
	if (yours || canseemon(mtmp))
	    pline("A sudden geyser slams into %s from nowhere!", mon_nam(mtmp));
	dmg = d(8, 6);
	break;
    case CLC_FIRE_PILLAR:
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("firepillar spell with no mtmp");
	    return;
	}
	if (yours || canseemon(mtmp))
	    pline("A pillar of fire strikes all around %s!", mon_nam(mtmp));
	if (resists_fire(mtmp)) {
	    shieldeff(mtmp->mx, mtmp->my);
	    dmg = 0;
	} else
	    dmg = d(8, 6);
	(void) burnarmor(mtmp);
	destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
	destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
	destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
	(void) burn_floor_paper(mtmp->mx, mtmp->my, TRUE, FALSE);
	break;
    case CLC_LIGHTNING:
    {
	boolean reflects;
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("lightning spell with no mtmp");
	    return;
	}

	if (yours || canseemon(mtmp))
	    pline("A bolt of lightning strikes down at %s from above!",
	          mon_nam(mtmp));
	reflects = mon_reflects(mtmp, "It bounces off %s %s.");
	if (reflects || resists_elec(mtmp)) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	    if (reflects)
		break;
	} else
	    dmg = d(8, 6);
	destroy_mitem(mtmp, WAND_CLASS, AD_ELEC);
	destroy_mitem(mtmp, RING_CLASS, AD_ELEC);
	break;
    }
    case CLC_CURSE_ITEMS:
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("curse spell with no mtmp");
	    return;
	}
	if (yours || canseemon(mtmp))
	    You_feel("as though %s needs some help.", mon_nam(mtmp));
	mrndcurse(mtmp);
	dmg = 0;
	break;
    case CLC_INSECTS:
      {
	/* Try for insects, and if there are none
	   left, go for (sticks to) snakes.  -3. */
	struct permonst *pm = mkclass(S_ANT,0);
	struct monst *mtmp2 = (struct monst *)0;
	char let = (pm ? S_ANT : S_SNAKE);
	boolean success;
	int i;
	coord bypos;
	int quan;
        
	if (!mtmp || mtmp->mhp < 1) {
	    impossible("insect spell with no mtmp");
	    return;
	}

	quan = (mons[u.umonnum].mlevel < 2) ? 1 : 
	       rnd(mons[u.umonnum].mlevel / 2);
	if (quan < 3) quan = 3;
	success = pm ? TRUE : FALSE;
	for (i = 0; i <= quan; i++) {
	    if (!enexto(&bypos, mtmp->mx, mtmp->my, mtmp->data))
		break;
	    if ((pm = mkclass(let,0)) != 0 &&
		    (mtmp2 = makemon(pm, bypos.x, bypos.y, NO_MM_FLAGS)) != 0) {
		success = TRUE;
		mtmp2->msleeping = 0;
		if (yours || mattk->mtame)
		    (void) tamedog(mtmp2, (struct obj *)0);
		else if (mattk->mpeaceful)
		    mattk->mpeaceful = 1;
		else mattk->mpeaceful = 0;

		set_malign(mtmp2);
	    }
	}

        if (yours)
	{
	    if (!success)
	        You("cast at a clump of sticks, but nothing happens.");
	    else if (let == S_SNAKE)
	        You("transforms a clump of sticks into snakes!");
	    else
	        You("summon insects!");
        } else if (canseemon(mtmp)) {
	    if (!success)
	        pline("%s casts at a clump of sticks, but nothing happens.",
		      Monnam(mattk));
	    else if (let == S_SNAKE)
	        pline("%s transforms a clump of sticks into snakes!",
		      Monnam(mattk));
	    else
	        pline("%s summons insects!", Monnam(mattk));
	}
	dmg = 0;
	break;
      }
    case CLC_BLIND_YOU:
        if (!mtmp || mtmp->mhp < 1) {
	    impossible("blindness spell with no mtmp");
	    return;
	}
	/* note: resists_blnd() doesn't apply here */
	if (!mtmp->mblinded &&
	    haseyes(mtmp->data)) {
	    if (!resists_blnd(mtmp)) {
	        int num_eyes = eyecount(mtmp->data);
	        pline("Scales cover %s %s!",
	          s_suffix(mon_nam(mtmp)),
		  (num_eyes == 1) ? "eye" : "eyes");

		  mtmp->mblinded = 127;
	    }
	    dmg = 0;

	} else
	    impossible("no reason for monster to cast blindness spell?");
	break;
    case CLC_PARALYZE:
        if (!mtmp || mtmp->mhp < 1) {
	    impossible("paralysis spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    if (yours || canseemon(mtmp))
	        pline("%s stiffens briefly.", Monnam(mtmp));
	} else {
	    if (yours || canseemon(mtmp))
	        pline("%s is frozen in place!", Monnam(mtmp));
	    dmg = 4 + mons[u.umonnum].mlevel;
	    mtmp->mcanmove = 0;
	    mtmp->mfrozen = dmg;
	}
	dmg = 0;
	break;
    case CLC_CONFUSE_YOU:
        if (!mtmp || mtmp->mhp < 1) {
	    impossible("confusion spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    if (yours || canseemon(mtmp))
	        pline("%s seems momentarily dizzy.", Monnam(mtmp));
	} else {
	    if (yours || canseemon(mtmp))
	        pline("%s seems %sconfused!", Monnam(mtmp),
	              mtmp->mconf ? "more " : "");
	    mtmp->mconf = 1;
	}
	dmg = 0;
	break;
    case CLC_CURE_SELF:
	if (u.mh < u.mhmax) {
	    You("feel better.");
	    /* note: player healing does 6d4; this used to do 1d8 */
	    if ((u.mh += d(3,6)) > u.mhmax)
		u.mh = u.mhmax;
	    flags.botl = 1;
	    dmg = 0;
	}
	break;
    case CLC_OPEN_WOUNDS:
        if (!mtmp || mtmp->mhp < 1) {
	    impossible("wound spell with no mtmp");
	    return;
	}
	if (resist(mtmp, 0, 0, FALSE)) { 
	    shieldeff(mtmp->mx, mtmp->my);
	    dmg = (dmg + 1) / 2;
	}
	/* not canseemon; if you can't see it you don't know it was wounded */
	if (yours)
	{
	    if (dmg <= 5)
	        pline("%s looks itchy!", Monnam(mtmp));
	    else if (dmg <= 10)
	        pline("Wounds appear on %s!", mon_nam(mtmp));
	    else if (dmg <= 20)
	        pline("Severe wounds appear on %s!", mon_nam(mtmp));
	    else
	        pline("%s is covered in wounds!", Monnam(mtmp));
	}
	break;
    default:
	impossible("ucastm: invalid clerical spell (%d)", spellnum);
	dmg = 0;
	break;
    }

    if (dmg > 0 && mtmp->mhp > 0)
    {
        mtmp->mhp -= dmg;
        if (mtmp->mhp < 1) {
	    if (yours) killed(mtmp, AD_CLRC);
	    else monkilled(mtmp, "", AD_CLRC);
	}
    }
}

#endif /* OVL0 */

/*mcastu.c*/
