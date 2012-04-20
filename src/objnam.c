/*	SCCS Id: @(#)objnam.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* "an [uncursed] greased [very rotten] partly eaten guardian naga hatchling (corpse)" */
#define PREFIX	80	/* (73) */
#define SCHAR_LIM 127
#define NUMOBUF 12

STATIC_DCL char *FDECL(strprepend,(char *,const char *));
#ifdef OVLB
static boolean FDECL(wishymatch, (const char *,const char *,BOOLEAN_P));
#endif
static char *NDECL(nextobuf);
static void FDECL(add_erosion_words, (struct obj *, char *));
#ifdef SORTLOOT
char * FDECL(xname2, (struct obj *, boolean));
#endif

struct Jitem {
	int item;
	const char *name;
};

/* true for gems/rocks that should have " stone" appended to their names */
#define GemStone(typ)	(typ == FLINT ||				\
			 (objects[typ].oc_material == GEMSTONE &&	\
			  (typ != DILITHIUM_CRYSTAL && typ != RUBY &&	\
			   typ != DIAMOND && typ != SAPPHIRE &&		\
			   typ != BLACK_OPAL && 	\
			   typ != EMERALD && typ != OPAL)))

#ifndef OVLB

STATIC_DCL struct Jitem Japanese_items[];

#else /* OVLB */

STATIC_OVL struct Jitem Japanese_items[] = {
	{ SHORT_SWORD, "wakizashi" },
	{ BROADSWORD, "ninja-to" },
	{ FLAIL, "nunchaku" },
	{ GLAIVE, "naginata" },
	{ LOCK_PICK, "osaku" },
	{ WOODEN_HARP, "koto" },
	{ KNIFE, "shito" },
	{ PLATE_MAIL, "tanko" },
	{ HELMET, "kabuto" },
	{ GLOVES, "yugake" },
	{ FOOD_RATION, "gunyoki" },
	{ POT_BOOZE, "sake" },
	{0, "" }
};

STATIC_OVL const char *bogusobjs[] =
{
    "pillow", "digital watch", "secret decoder ring", "mood ring",
    "seashell",

    "flesh-bound spellbook", "visaged spellbook",

    "plaid potion", "paisley potion",

    "titanium wand", "plutonium wand", "tungsten wand",
    "lead wand", "star-tipped wand",

    "Klein bottle",

    "loadboulder",

    "Pick-axe of Anhur", "magic lantern",

    "elfin chain mail", "beef jerky", "steak", "cookie", 
    "enormous rock", "Sphere of Power", "cross", /* JF Hack obsolete objects */

    "Orb of Zot", /* Dungeon Crawl */

    "head of Morgoth", /* Angband, sort of */

    "hand of Vecna", "eye of the beholder", "key of Law",
    "key of Neutrality", "key of Chaos", /* SLASH'EM */

       "silly object of hilarity",
# ifdef TOURIST
       /* Modern */
       "polo mallet",
       "string vest",
       "YAFM",                             /* rgrn */
       "applied theology textbook",        /* AFutD */
       "handbag",
       "onion ring",
       "tuxedo",
       "breath mint",
       "potion of antacid",
       "traffic cone",
       "chainsaw",
       "pair of high-heeled stilettos",    /* the *other* stiletto */

       /* Silly */
       "left-handed iron chain",
       "holy hand grenade",                /* Monty Python */
       "decoder ring",
       "amulet of huge gold chains",       /* Foo' */
       "rubber Marduk"
       "unicron horn",                     /* Transformers */
       "holy grail",                       /* Monty Python */
       "chainmail bikini",
       "first class one-way ticket to Albuquerque", /* Weird Al */
       "yellow spandex dragon scale mail", /* X-Men */

       /* Musical Instruments */
       "grand piano",
       "two slightly sampled electric eels", /* Oldfield */
       "kick drum",                        /* 303 */
       "tooled airhorn",

       /* Pop Culture */
       "flux capacitor",                   /* BTTF */
       "Walther PPK",                      /* Bond */
       "hanging chad",                     /* US Election 2000 */
       "red balloon",                      /* 80s */
       "pincers of peril",                 /* Goonies */
       "ring of schwartz",                 /* Spaceballs */
       "signed copy of Diaspora",          /* Greg Egan */
       "missing evidence in the Kelner case", /* Naked Gun */
       "helm of Des Lynam",                /* Bottom */

       /* compile with -DBRITHACK for British goodness */
#  ifdef BRITHACK
       "bum bag",
       "blessed tin of marmite",
       "tesco value potion",
       "ringtone of drawbridge opening",
       "burberry cap",
       "potion of bitter",
       "bargain plane ticket to Ibiza",
       "black pudding corpse",
#  endif
# endif
       /* Fantasy */
       "Necronomicon",                     /* Lovecraft */
       "pipe weed",                        /* LOTR */
       "knife missile",                    /* Iain M. Banks */
       "large gem",                        /* Valhalla */
       "monster manual",                   /* D&D */
       "spellbook called Octavo",          /* Discworld */
       "ring of power",                    /* LOTR */
       "lightsaber",
       "no tea",                           /* HGttG game */
       "pan-galactic gargle blaster",      /* HGttG */
       "silmaril",                         /* LOTR */
       "pentagram of protection",          /* Quake */

       /* Geekery */
       "AAA chipset",                      /* Amiga */
       "copy of Nethack for Dummies",
       "named pipe",                       /* UNIX */
       "kernel trap",
       "copy of NetHack 3.4.4",            /* recursion... */
       "cursed smooth manifold",           /* Topology */
       "vi clone",
       "maximally subsentient emacs mode",
       "bongard diagram",                  /* Intelligence test */

       /* Historical */
       "dead sea scroll",
       "cat o'nine tails",
       "pieces of eight",
       "codpiece",
       "straight-jacket",
       "bayonet",
       "iron maiden",
       "oubliette",
       "pestle and mortar"
       "plowshare",

       /* Mashups */
       "potion of score doubling",
       "scroll labelled ED AWK YACC",      /* the standard scroll */
       "scroll labelled RTFM",
       "scroll labelled KLAATU BARADA NIKTO", /* Evil Dead 3 */
       "scroll of omniscience",
       "scroll of mash keyboard",
       "scroll of RNG taming",
       "scroll of fungicide",
       "helm of telemetry",
       "blue suede boots of charisma",
       "cubic zirconium",
       "amulet of instadeath",
       "amulet of bad luck",
       "amulet of refraction",
       "potion of rebigulation",           /* Simpsons */
       "O-ring",
       "wand of washing",
       "ring named Frost Band",
       "expensive exact replica of the Amulet of Yendor",
       "giant beatle",
       "lodestone",
       "rubber chicken",                   /* c corpse */
       "tin of Player meat",
       "figurine of a god",
       "tin of whoop ass",
       "earring of adornment",
       "wisdom boots",
       "ornamental cape",
       "acid blob skeleton",

# ifdef MAIL
       "brand new, all time lowest introductory rate special offer",
# endif
       "dirty rag"
};


#endif /* OVLB */

STATIC_DCL const char *FDECL(Japanese_item_name,(int i));

#ifdef OVL1

STATIC_OVL char *
strprepend(s,pref)
register char *s;
register const char *pref;
{
	register int i = (int)strlen(pref);

	if(i > PREFIX) {
		impossible("PREFIX too short (for %d).", i);
		return(s);
	}
	s -= i;
	(void) strncpy(s, pref, i);	/* do not copy trailing 0 */
	return(s);
}

#endif /* OVL1 */
#ifdef OVLB

/* manage a pool of BUFSZ buffers, so callers don't have to */
static char *
nextobuf()
{
	static char NEARDATA bufs[NUMOBUF][BUFSZ];
	static int bufidx = 0;

	bufidx = (bufidx + 1) % NUMOBUF;
	return bufs[bufidx];
}

static boolean dump_ID_flag = FALSE;

#ifdef DYWYPISI
/* used by possessions identifier */
void dump_ID_on()
{
  dump_ID_flag = TRUE;
}
/* currently unused, but here anyway */
void dump_ID_off()
{
  dump_ID_flag = FALSE;
}
#endif

char *
obj_typename(otyp)
register int otyp;
{
	char *buf = nextobuf();
	register struct objclass *ocl = &objects[otyp];
	register const char *actualn = OBJ_NAME(*ocl);
	register const char *dn = OBJ_DESCR(*ocl);
	register const char *un = ocl->oc_uname;
	register int nn = ocl->oc_name_known;

	if (Role_if(PM_SAMURAI) && Japanese_item_name(otyp))
		actualn = Japanese_item_name(otyp);
	switch(ocl->oc_class) {
	case COIN_CLASS:
		Strcpy(buf, "coin");
		break;
	case POTION_CLASS:
		Strcpy(buf, "potion");
		break;
	case SCROLL_CLASS:
		Strcpy(buf, "scroll");
		break;
	case WAND_CLASS:
		Strcpy(buf, "wand");
		break;
	case SPBOOK_CLASS:
		Strcpy(buf, "spellbook");
		break;
	case RING_CLASS:
		Strcpy(buf, "ring");
		break;
	case AMULET_CLASS:
		if(nn)
			Strcpy(buf,actualn);
		else
			Strcpy(buf,"amulet");
		if(un)
			Sprintf(eos(buf)," called %s",un);
		if(dn)
			Sprintf(eos(buf)," (%s)",dn);
		return(buf);
	default:
		if(nn) {
			Strcpy(buf, actualn);
			if (GemStone(otyp))
				Strcat(buf, " stone");
			if(un)
				Sprintf(eos(buf), " called %s", un);
			if(dn)
				Sprintf(eos(buf), " (%s)", dn);
		} else {
			Strcpy(buf, dn ? dn : actualn);
			if(ocl->oc_class == GEM_CLASS)
				Strcat(buf, (ocl->oc_material == MINERAL) ?
						" stone" : " gem");
			if(un)
				Sprintf(eos(buf), " called %s", un);
		}
		return(buf);
	}
	/* here for ring/scroll/potion/wand */
	if(nn) {
	    if (ocl->oc_unique)
		Strcpy(buf, actualn); /* avoid spellbook of Book of the Dead */
	    else
		Sprintf(eos(buf), " of %s", actualn);
	}
	if(un)
		Sprintf(eos(buf), " called %s", un);
	if(dn)
		Sprintf(eos(buf), " (%s)", dn);
	return(buf);
}

/* less verbose result than obj_typename(); either the actual name
   or the description (but not both); user-assigned name is ignored */
char *
simple_typename(otyp)
int otyp;
{
    char *bufp, *pp, *save_uname = objects[otyp].oc_uname;

    objects[otyp].oc_uname = 0;		/* suppress any name given by user */
    bufp = obj_typename(otyp);
    objects[otyp].oc_uname = save_uname;
    if ((pp = strstri(bufp, " (")) != 0)
	*pp = '\0';		/* strip the appended description */
    return bufp;
}

boolean
obj_is_pname(obj)
register struct obj *obj;
{
    if (Hallucination) 
        return FALSE; /*lie*/

    return((boolean)
                    (obj->dknown && obj->known && obj->onamelth &&
		     /* Since there aren't any objects which are both
		        artifacts and unique, the last check is redundant. */
		     obj->oartifact && !objects[obj->otyp].oc_unique));
}

/* Give the name of an object seen at a distance.  Unlike xname/doname,
 * we don't want to set dknown if it's not set already.  The kludge used is
 * to temporarily set Blind so that xname() skips the dknown setting.  This
 * assumes that we don't want to do this too often; if this function becomes
 * frequently used, it'd probably be better to pass a parameter to xname()
 * or doname() instead.
 */
char *
distant_name(obj, func)
register struct obj *obj;
char *FDECL((*func), (OBJ_P));
{
	char *str;

	long save_Blinded = Blinded;
	Blinded = 1;
	str = (*func)(obj);
	Blinded = save_Blinded;
	return str;
}

/* convert player specified fruit name into corresponding fruit juice name
   ("slice of pizza" -> "pizza juice" rather than "slice of pizza juice") */
char *
fruitname(juice)
boolean juice;	/* whether or not to append " juice" to the name */
{
    char *buf = nextobuf();
    const char *fruit_nam = strstri(pl_fruit, " of ");

    if (fruit_nam)
	fruit_nam += 4;		/* skip past " of " */
    else
	fruit_nam = pl_fruit;	/* use it as is */

    Sprintf(buf, "%s%s", makesingular(fruit_nam), juice ? " juice" : "");
    return buf;
}

#endif /* OVLB */
#ifdef OVL1

STATIC_OVL
void
propnames(buf, props, props_known, weapon, has_of)
register char *buf;
register long props, props_known;
boolean weapon;
boolean has_of;
{
    char of[6];

    if (Hallucination) return;

    Strcpy(of, (has_of) ? "and" : "of");
    if (props & ITEM_FIRE) {
    	if ((props_known & ITEM_FIRE)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_FIRE))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, weapon ? " fire" : " fire resistance");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_FIRE))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_FROST) {
    	if ((props_known & ITEM_FROST)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_FROST))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, weapon ? " frost" : " cold resistance");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_FROST))
	    	Strcat(buf, "]");
#endif
	}
    }
    if ((props & ITEM_DRLI) && !weapon) {
    	if ((props_known & ITEM_DRLI)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DRLI))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " drain resistance");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DRLI))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_REFLECTION) {
    	if ((props_known & ITEM_REFLECTION)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_REFLECTION))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " reflection");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_REFLECTION))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_SPEED) {
    	if ((props_known & ITEM_SPEED)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_SPEED))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " speed");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_SPEED))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_POWER) {
    	if ((props_known & ITEM_POWER)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_POWER))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " power");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_POWER))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_DEXTERITY) {
    	if ((props_known & ITEM_DEXTERITY)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DEXTERITY))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " dexterity");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DEXTERITY))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_BRILLIANCE) {
    	if ((props_known & ITEM_BRILLIANCE)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_BRILLIANCE))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " brilliance");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_BRILLIANCE))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_ESP) {
    	if ((props_known & ITEM_ESP)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_BRILLIANCE))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " telepathy");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_ESP))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_DISPLACEMENT) {
    	if ((props_known & ITEM_DISPLACEMENT)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DISPLACEMENT))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " displacement");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DISPLACEMENT))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_SEARCHING) {
    	if ((props_known & ITEM_SEARCHING)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_SEARCHING))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " searching");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_SEARCHING))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_WARNING) {
    	if ((props_known & ITEM_WARNING)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_WARNING))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " warning");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_STEALTH))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_STEALTH) {
    	if ((props_known & ITEM_STEALTH)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_STEALTH))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " stealth");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_STEALTH))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_FUMBLING) {
    	if ((props_known & ITEM_FUMBLING)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_FUMBLING))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " fumbling");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_FUMBLING))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_CLAIRVOYANCE) {
    	if ((props_known & ITEM_CLAIRVOYANCE)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_CLAIRVOYANCE))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " clairvoyance");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_CLAIRVOYANCE))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_DETONATIONS) {
    	if ((props_known & ITEM_DETONATIONS)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DETONATIONS))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " detonation");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_DETONATIONS))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_HUNGER) {
    	if ((props_known & ITEM_HUNGER)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_HUNGER))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " hunger");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_HUNGER))
	    	Strcat(buf, "]");
#endif
	}
    }
    if (props & ITEM_AGGRAVATE) {
    	if ((props_known & ITEM_AGGRAVATE)
#ifdef DYWYPISI
	    || dump_ID_flag
#endif
	) {
#ifdef DYWYPISI
	    if (!(props_known & ITEM_AGGRAVATE))
	    	Strcat(buf, " [");
	    else
#endif
		Strcat(buf, " ");
            Strcat(buf, of);
            Strcat(buf, " aggravation");
	    Strcpy(of, "and");
#ifdef DYWYPISI
	    if (!(props_known & ITEM_AGGRAVATE))
	    	Strcat(buf, "]");
#endif
	}
    }
}

char *
xname(obj)
register struct obj *obj;
#ifdef SORTLOOT
{
	return xname2(obj, FALSE);
}
char *
xname2(obj, ignore_oquan)
register struct obj *obj;
boolean ignore_oquan;
#endif
{
        register boolean hallu = Hallucination;
	register char *buf;
	register int typ = (hallu) ?
	                   (rn2(NUM_OBJECTS+SIZE(bogusobjs)) >= NUM_OBJECTS
			   ? STRANGE_OBJECT : rn2(NUM_OBJECTS - 1) + 1)
			    : obj->otyp;
	register struct objclass *ocl = &objects[typ];
	register int obclass = ocl->oc_class;
	register int nn = hallu ? rn2(2) : ocl->oc_name_known;
	register const char *actualn = OBJ_NAME(*ocl);
	register const char *dn = OBJ_DESCR(*ocl);
	register const char *un = (hallu) ? 0 : ocl->oc_uname;
	register int objknown = (hallu) ? rn2(2) : obj->known; 
	
#ifdef INVISIBLE_OBJECTS
	boolean reset_invis = FALSE;
	register char *buf2;
#endif

	if (!dn)
	{
	    if (typ == STRANGE_OBJECT)
	        dn = bogusobjs[rn2(SIZE(bogusobjs))];
	    else if (actualn)
	        dn = actualn;
	}

	if (!actualn) {
	    if (typ == STRANGE_OBJECT)
	        actualn = (dn) ? dn : bogusobjs[rn2(SIZE(bogusobjs))];
	    else if (dn)
	        actualn = dn;
	}

	buf = nextobuf() + PREFIX;	/* leave room for "17 -3 " */
	
	buf[0] = '\0';

	buf2 = buf;
	
	if (Japanese_item_name(typ) &&
	    ((Role_if(PM_SAMURAI)) ^
	     (hallu && !rn2(10))))
	    actualn = Japanese_item_name(typ);

	if (!dn && !actualn)
	{
	    Strcpy(buf, "figment of your imagination");
	    return(buf);
	}

	if (obj->oinvis && !See_invisible && !obj->opresenceknown) {
	    Strcpy(buf, "invisible object");
	    /*Strcpy(buf, "something");*/
	    return(buf);
	}

	Strcpy(buf, "");

#ifdef INVISIBLE_OBJECTS
	if (!Blind) obj->iknown = TRUE;
	if (obj->oinvis && (See_invisible || obj->opresenceknown)) {
		if (obj->iknown)
		{
			Strcat(buf,"invisible ");
			buf2 = buf;
			buf += 10;
			reset_invis = TRUE;
		}
	}	
#endif

	/*
	 * clean up known when it's tied to oc_name_known, eg after AD_DRIN
	 * This is only required for unique objects since the article
	 * printed for the object is tied to the combination of the two
	 * and printing the wrong article gives away information.
	 */
	if (!hallu)
	{
	    if (!nn && ocl->oc_uses_known && ocl->oc_unique) obj->known = 0;
	    if (!Blind
#ifdef INVISIBLE_OBJECTS
		&& (!obj->oinvis || See_invisible)
#endif
	    ) obj->dknown = TRUE;
	    if (Role_if(PM_PRIEST)) obj->bknown = TRUE;
	    /* wizards sense magical auras */
	    if (Role_if(PM_WIZARD) &&
	        ((obj->oprops & ITEM_PROP_MASK) ||
		 ( objects[obj->otyp].oc_magic &&
		  !objects[obj->otyp].oc_name_known)))
		obj->oprops_known |= ITEM_MAGICAL;
	    if (obj_is_pname(obj))
	        goto nameit;
	}

	if (obj->oprops_known & ITEM_MAGICAL &&
	    (((obj->oprops && !(obj->oprops_known & ~ITEM_MAGICAL)) &&
	      (!objects[obj->otyp].oc_magic ||
	       !objects[obj->otyp].oc_name_known)) ||
	       (!obj->oprops && objects[obj->otyp].oc_magic &&
	        !objects[obj->otyp].oc_name_known)))
	    Strcat(buf, "magical ");

	switch (obclass) {
	    case AMULET_CLASS:
		if (!obj->dknown)
			Strcat(buf, "amulet");
		else {
		    if (typ == AMULET_OF_YENDOR ||
			 typ == FAKE_AMULET_OF_YENDOR)
			/* each must be identified individually */
			Strcat(buf, objknown ? actualn : dn);
		else if (nn)
			Strcat(buf, actualn);
		else if (un)
			Strcat(buf,"amulet");
		else
			Strcat(buf, dn),
			Strcat(buf," amulet");
		    propnames(buf, obj->oprops, obj->oprops_known, FALSE,
		              !!strstri(buf, " of "));
		    if (!nn && un)
		    {
		        Strcat(buf, " called ");
		        Strcat(buf, un);
		    }
		}
		break;
	    case WEAPON_CLASS:
		if (is_poisonable(obj) && obj->opoisoned)
			Strcat(buf, "poisoned ");
	    case VENOM_CLASS:
	    case TOOL_CLASS:
		if (typ == LENSES)
			Strcat(buf, "pair of ");

		if (!obj->dknown)
			Strcat(buf, dn ? dn : actualn);
		else {
		    struct obj tmpobj = zeroobj;
                    tmpobj.otyp = typ;

		    if ((obj->oprops & ITEM_DRLI) &&
		    ((obj->oprops_known & ITEM_DRLI)
#ifdef DYWYPISI
		    || dump_ID_flag
#endif
		    ))
		        Sprintf(eos(buf), "%sthirsty%s ",
				!(obj->oprops_known & ITEM_DRLI) ? "[" : "",
				!(obj->oprops_known & ITEM_DRLI) ? "]" : ""
				);

		    if ((obj->oprops & ITEM_VORPAL) &&
		    ((obj->oprops_known & ITEM_VORPAL)
#ifdef DYWYPISI
		    || dump_ID_flag
#endif
		    ))
		        Sprintf(eos(buf), "%svorpal%s ",
				!(obj->oprops_known & ITEM_VORPAL) ? "[" : "",
				!(obj->oprops_known & ITEM_VORPAL) ? "]" : ""
				);
                     
		     if (is_orcish_obj(&tmpobj) && nn)
		         Strcat(buf, "orcish "),
			 actualn += 7;
                     else if (is_elven_obj(&tmpobj) && nn)
		         Strcat(buf, "elven "),
			 actualn += 6;
                     else if (is_dwarvish_obj(&tmpobj) && nn)
		         Strcat(buf, "dwarvish "),
			 actualn += 9;

		    if (obj->otyp == LARGE_BOX && nn)
		    	Strcat(buf, "large "),
			actualn += 6; /* "large iron box" */

		    if ((obj->omaterial != objects[typ].oc_material)
		     ^ (hallu && !rn2(5)))
		    {
		        int mat = (hallu) ? rn2(MAX_MAT) : obj->omaterial;
			Strcat(buf, materialnm[mat]);
			Strcat(buf, " ");
		    }
		    
		    if (nn)
			Strcat(buf, actualn);
		else if (un) {
			Strcat(buf, dn ? dn : actualn);
		} else
			Strcat(buf, dn ? dn : actualn);

		    propnames(buf, obj->oprops, obj->oprops_known, TRUE,
		              !!strstri(buf, " of "));
		    
		    if (!nn && un)
		    {
			Strcat(buf, " called ");
			Strcat(buf, un);
		    }
	        }
		/* If we use an() here we'd have to remember never to use */
		/* it whenever calling doname() or xname(). */
		if (typ == FIGURINE)
		    Sprintf(eos(buf), " of a%s %s",
			index(vowels,*(mons[obj->corpsenm].mname)) ? "n" : "",
			mons[obj->corpsenm].mname);
		break;
	    case ARMOR_CLASS:
		/* depends on order of the dragon scales objects */
		if (typ >= GRAY_DRAGON_SCALES && typ <= YELLOW_DRAGON_SCALES) {
			Strcat(buf, "set of ");
			Strcat(buf, actualn);

			/* should this even be possible? */
		        propnames(buf, obj->oprops, obj->oprops_known, TRUE,
		                  FALSE);
			break;
		}
		
		if(obclass == ARMOR_CLASS &&
		   (objects[typ].oc_armcat == ARM_BOOTS ||
		    objects[typ].oc_armcat == ARM_GLOVES))
		    Strcat(buf,"pair of ");

		if (!hallu && obj->dknown &&
		    (obj->oprops & ITEM_OILSKIN) &&
		    ((obj->oprops_known & ITEM_OILSKIN)
#ifdef DYWYPISI
	             || dump_ID_flag
#endif
		    ))
		    Sprintf(eos(buf), "%soilskin%s ",
		    	!(obj->oprops_known & ITEM_OILSKIN) ? "[" : "",
		    	!(obj->oprops_known & ITEM_OILSKIN) ? "]" : ""
			);

		if (((obj->omaterial != objects[typ].oc_material) ||
		     (objects[typ].oc_material == LEATHER) ||
		     (obclass == ARMOR_CLASS &&
		      (objects[typ].oc_armcat == ARM_BOOTS ||
		       objects[typ].oc_armcat == ARM_GLOVES)) ||
		     (typ == DWARVISH_HELM) ||
		     (typ == ELVEN_HELM))
		  ^ (hallu && !rn2(5)))
		{
		     int mat = (hallu) ? rn2(MAX_MAT) : obj->omaterial;
		     struct obj tmpobj = zeroobj;
		     tmpobj.otyp = typ;
		     if (typ == STUDDED_ARMOR)
		         Strcat(buf, "studded "),
			 actualn = dn = "armor";
		     /*else if (typ == LOW_BOOTS || typ == HIGH_BOOTS)
		         Strcat(buf, (typ == LOW_BOOTS) ? "low " : "high "),
			 actualn = dn = "boots";*/
                     else if (is_orcish_obj(&tmpobj) && nn)
		         Strcat(buf, "orcish "),
			 actualn += 7;
                     else if (is_elven_obj(&tmpobj) && nn)
		         Strcat(buf, "elven "),
			 actualn += 6;
                     else if (is_dwarvish_obj(&tmpobj) && nn)
		         Strcat(buf, "dwarvish "),
			 actualn += 9;

		     Strcat(buf, materialnm[mat]);
		     Strcat(buf, " ");
		}

		if(typ >= ELVEN_SHIELD && typ <= ORCISH_SHIELD
				&& !obj->dknown) {
			Strcat(buf, "shield");
		        propnames(buf, obj->oprops, obj->oprops_known, FALSE,
			          FALSE);
			break;
		}
		if(typ == SHIELD_OF_REFLECTION && !obj->dknown) {
			Strcat(buf, "smooth shield");
		        propnames(buf, obj->oprops, obj->oprops_known, FALSE,
			          FALSE);
			break;
		}

		if(nn) {
		    Strcat(buf, actualn);
		    propnames(buf, obj->oprops, obj->oprops_known, FALSE,
		              !!strstri(actualn, " of "));
		}
		else if(un) {
			if(is_boots(obj))
				Strcat(buf,"boots");
			else if(is_gloves(obj))
				Strcat(buf,"gloves");
			else if(is_cloak(obj))
				Strcat(buf,"cloak");
			else if(is_helmet(obj))
				Strcat(buf,helmet_name(obj));
			else if(is_shield(obj))
				Strcat(buf,"shield");
			else
				Strcat(buf,"armor");
		        propnames(buf, obj->oprops, obj->oprops_known, FALSE,
		                  FALSE);
			Strcat(buf, " called ");
			Strcat(buf, un);
		} else {
		    Strcat(buf, dn);
		    propnames(buf, obj->oprops, obj->oprops_known, FALSE,
		              !!strstri(dn, " of "));
		}
		break;
	    case FOOD_CLASS:
		if (typ == SLIME_MOLD) {
			register struct fruit *f;

			for(f=ffruit; f; f = f->nextf) {
				if(f->fid == obj->spe ||
				   hallu) {
					Strcat(buf, f->fname);
					break;
				}
			}
			if (!f) impossible("Bad fruit #%d?", obj->spe);
			break;
		}

		Strcat(buf, actualn);
		if (typ == TIN && objknown) {
		    if(obj->spe > 0)
			Strcat(buf, " of spinach");
		    else if (obj->corpsenm == NON_PM)
		        Strcat(buf, "empty tin");
		    else if (vegetarian(&mons[obj->corpsenm]))
			Sprintf(eos(buf), " of %s", mons[obj->corpsenm].mname);
		    else
			Sprintf(eos(buf), " of %s%s meat",
			        mons[obj->corpsenm].mname,
				obj->spe == -2 ? " zombie" :
				obj->spe == -3 ? " wererat" :
				obj->spe == -4 ? " werejackal" :
				obj->spe == -5 ? " werewolf" : "");
		}
		break;
	    case COIN_CLASS:
	    case CHAIN_CLASS:
		Strcat(buf, actualn);
		break;
	    case ROCK_CLASS:
		if (typ == STATUE)
		{
		    char buf2[BUFSZ];
		    Sprintf(buf2, "%s%s%s%s of %s%s",
			(Role_if(PM_ARCHEOLOGIST) &&
			(obj->spe & STATUE_HISTORIC)) ? "historic " : "" ,
			(obj->omaterial != objects[obj->otyp].oc_material)
			    ? materialnm[obj->omaterial] : "",
			(obj->omaterial != objects[obj->otyp].oc_material)
			    ? " " : "",
			actualn,
			type_is_pname(&mons[obj->corpsenm]) ? "" :
			  (mons[obj->corpsenm].geno & G_UNIQ) ? "the " :
			    (index(vowels,*(mons[obj->corpsenm].mname)) ?
								"an " : "a "),
			mons[obj->corpsenm].mname);
		    Strcat(buf, buf2);
		}
		else
		{
 		    if ((obj->omaterial != objects[typ].oc_material)
		      ^ (hallu && !rn2(5)))
		    {
		        int mat = (hallu) ? rn2(MAX_MAT) : obj->omaterial;
		        Strcat(buf, materialnm[mat]);
		        Strcat(buf, " ");
		    }
		        Strcat(buf, actualn);
		}
		break;
	    case BALL_CLASS:
		Sprintf(buf, "%sheavy iron ball",
			(obj->owt > ocl->oc_weight) ? "very " : "");
		break;
	    case POTION_CLASS:
		if (obj->dknown && obj->odiluted)
			Strcat(buf, "diluted ");
		if(nn || un || !obj->dknown) {
			Strcat(buf, "potion");
			if(!obj->dknown) break;
			if(nn) {
			    Strcat(buf, " of ");
			    if (typ == POT_WATER &&
				obj->bknown && (obj->blessed || obj->cursed)) {
				Strcat(buf, obj->blessed ? "holy " : "unholy ");
			    }
			    Strcat(buf, actualn);
			} else {
				Strcat(buf, " called ");
				Strcat(buf, un);
			}
		} else {
			Strcat(buf, dn);
			Strcat(buf, " potion");
		}
		break;
	case SCROLL_CLASS:
		if(!obj->dknown) {
			Strcat(buf, "scroll");
			break;
		}
		if(nn) {
			Strcat(buf, "scroll of ");
			Strcat(buf, actualn);
		} else if(un) {
			Strcat(buf, "scroll called ");
			Strcat(buf, un);
		} else if (ocl->oc_magic) {
			Strcat(buf, "scroll labeled ");
			Strcat(buf, dn);
		} else {
			Strcat(buf, dn);
			Strcat(buf, " scroll");
		}
		break;
	case WAND_CLASS:
		if(!obj->dknown)
			Strcat(buf, "wand");
		else if(nn)
			Sprintf(buf, "wand of %s", actualn);
		else if(un)
			Sprintf(buf, "wand called %s", un);
		else
			Sprintf(buf, "%s wand", dn);
		break;
	case SPBOOK_CLASS:
		if (!obj->dknown) {
			Strcat(buf, "spellbook");
		} else if (nn) {
			if (typ != SPE_BOOK_OF_THE_DEAD)
			    Strcat(buf, "spellbook of ");
			Strcat(buf, actualn);
		} else if (un) {
			Sprintf(buf, "spellbook called %s", un);
		} else
			Sprintf(buf, "%s spellbook", dn);
		break;
	case RING_CLASS:
		if(!obj->dknown)
			Strcat(buf, "ring");
		else {
		    if(nn)
			Sprintf(buf, "ring of %s", actualn);
		else if(un)
			Strcat(buf, "ring"); /* handled down below */
		else
			Sprintf(buf, "%s ring", dn);
		    propnames(buf, obj->oprops, obj->oprops_known, FALSE,
		              !!strstri(buf, " of "));
		    if (!nn && un)
		    {
		        Strcat(buf, " called ");
			Strcat(buf, un);
		    }
		}
		break;
	case GEM_CLASS:
	    {
		const char *rock =
			    (ocl->oc_material == MINERAL) ? "stone" :
			    (obj->otyp == ROCK &&
			     obj->omaterial == objects[ROCK].oc_material)
			         ? "rock" :
			    (obj->otyp == ROCK) ? "chunk" : "gem";
		if (!obj->dknown) {
		    Strcat(buf, rock);
		} else if (obj->otyp == ROCK &&
		           obj->omaterial != objects[ROCK].oc_material) {
		    int mat = (hallu) ? rn2(MAX_MAT) : obj->omaterial;

		    if (obj->corpsenm != 0)
		    {
		        long age = peek_at_iced_corpse_age(obj);

		        long rotted = (monstermoves - age)/(20L);
		        if (obj->cursed) rotted += 2L;
		        else if (obj->blessed) rotted -= 2L;

			if (obj->oerodeproof) {
				Strcat(buf, "rotproof ");
			} else

		        Strcat(buf, (rotted == 0L ||
		                 obj->corpsenm == PM_ACID_BLOB ||
				 obj->corpsenm == PM_LICHEN ||
				 obj->corpsenm == PM_LIZARD) ? "" :
		                (rotted == 1L) ? 
					((!Blind
#ifdef INVISIBLE_OBJECTS
					&& (!obj->oinvis || See_invisible)
#endif
					) ? "off-color " : "smelly") :
		                (rotted <= 3L) ? "slightly rotted " :
				(rotted <= 4L) ? "rotted " :
		                (rotted <= 5L) ? "very rotted " :
		                                 "thoroughly rotted ");
		    }

	            Strcat(buf, "chunk of ");
		    if (obj->corpsenm != 0)
		    {
		        Strcat(buf, mons[obj->corpsenm].mname);
		        Strcat(buf, (obj->spe == -2) ? " zombie" :
		                    (obj->spe == -3) ? " wererat" :
			            (obj->spe == -4) ? " werejackal" :
				    (obj->spe == -5) ? " werewolf" : "");
			if (!vegan(&mons[obj->corpsenm]) &&
			    !vegetarian(&mons[obj->corpsenm]))
			    Strcat(buf, " "),
		            Strcat(buf, materialnm[mat]);
		    } else
		        Strcat(buf, materialnm[mat]);
		} else if (!nn) {
		    if (un) Sprintf(buf,"%s called %s", rock, un);
		    else Sprintf(buf, "%s %s", dn, rock);
		} else {
		    Strcat(buf, actualn);
		    if (GemStone(typ)) Strcat(buf, " stone");
		}
		break;
	    }
	default:
	        if (typ == STRANGE_OBJECT)
		    Sprintf(buf, "%s", dn);
		else
		    Sprintf(buf,"glorkum %d %d %d", obj->oclass, typ, obj->spe);
	}
#ifdef SORTLOOT
	if (!ignore_oquan)
#endif
	if (obj->quan != 1L) Strcpy(buf, makeplural(buf));

	if (obj->onamelth && (obj->dknown
#ifdef INVISIBLE_OBJECTS
	|| obj->opresenceknown
#endif
	) && !hallu) {
		Strcat(buf, " named ");
nameit:
		Strcat(buf, ONAME(obj));
	}

	if (!strncmpi(buf, "the ", 4)) buf += 4;
#ifdef INVISIBLE_OBJECTS
	if (reset_invis) {
		char buf3[BUFSZ];
		Strcpy(buf3, buf);
		buf2[10] = '\0';
		Strcat(buf2, buf3);
		buf = buf2;
	}
#endif
	return(buf);
}

/* xname() output augmented for multishot missile feedback */
char *
mshot_xname(obj)
struct obj *obj;
{
    char tmpbuf[BUFSZ];
    char *onm = xname(obj);

    if (m_shot.n > 1 && m_shot.o == obj->otyp) {
	/* copy xname's result so that we can reuse its return buffer */
	Strcpy(tmpbuf, onm);
	/* "the Nth arrow"; value will eventually be passed to an() or
	   The(), both of which correctly handle this "the " prefix */
	Sprintf(onm, "the %d%s %s", m_shot.i, ordin(m_shot.i), tmpbuf);
    }

    return onm;
}

#endif /* OVL1 */
#ifdef OVL0

/* used for naming "the unique_item" instead of "a unique_item" */
boolean
the_unique_obj(obj)
register struct obj *obj;
{
    if (Hallucination)
        return (boolean)(!rn2(4) ^ (objects[obj->otyp].oc_unique &&
			  (obj->known || obj->otyp == AMULET_OF_YENDOR)));
    if (!obj->dknown)
	return FALSE;
    else if (obj->otyp == FAKE_AMULET_OF_YENDOR && !obj->known)
	return TRUE;		/* lie */
    else
	return (boolean)(objects[obj->otyp].oc_unique &&
			 (obj->known || obj->otyp == AMULET_OF_YENDOR));
}

static void
add_erosion_words(obj,prefix)
struct obj *obj;
char *prefix;
{
	boolean iscrys = (obj->otyp == CRYSKNIFE);


	if (!is_damageable(obj) && !iscrys) return;

	/* The only cases where any of these bits do double duty are for
	 * rotted food and diluted potions, which are all not is_damageable().
	 */
	if (obj->oeroded && !iscrys) {
		switch (obj->oeroded) {
			case 2:	Strcat(prefix, "very "); break;
			case 3:	Strcat(prefix, "thoroughly "); break;
		}			
		Strcat(prefix, is_rustprone(obj) ? "rusty " : "burnt ");
	}
	if (obj->oeroded2 && !iscrys) {
		switch (obj->oeroded2) {
			case 2:	Strcat(prefix, "very "); break;
			case 3:	Strcat(prefix, "thoroughly "); break;
		}			
		Strcat(prefix, is_corrodeable(obj) ? "corroded " :
			"rotted ");
	}
	if (obj->oerodeproof && (dump_ID_flag || obj->rknown))
		Sprintf(eos(prefix), "%s%s%s ",
			obj->rknown? "" : "[",
			iscrys ? "fixed" :
			is_rustprone(obj) ? "rustproof" :
			is_corrodeable(obj) ? "corrodeproof" :	/* "stainless"? */
			is_flammable(obj) ? "fireproof" : "",
			obj->rknown? "" : "]");
}

char *
doname(obj)
register struct obj *obj;
{
        boolean hallu = Hallucination;
	boolean ispoisoned = FALSE;
#ifdef INVISIBLE_OBJECTS
	boolean isinvisible = FALSE;
#endif
	char prefix[PREFIX];
	char tmpbuf[PREFIX+1];
	/* when we have to add something at the start of prefix instead of the
	 * end (Strcat is used on the end)
	 */
	register char *bp = xname(obj), *tmp;
	
	/* display ID in addition to appearance */
	boolean do_ID = dump_ID_flag && !objects[obj->otyp].oc_name_known;
	boolean do_known = dump_ID_flag && !obj->known;
	boolean do_dknown = dump_ID_flag && !obj->dknown;
	boolean do_bknown = dump_ID_flag && !obj->bknown;
	/*boolean do_rknown = dump_ID_flag && !obj->rknown;*/
	
	if(!dump_ID_flag)
		; /* early exit */
	else if(exist_artifact(obj->otyp, (tmp = ONAME(obj)))) {
		if(do_dknown || do_known) {
			Sprintf(eos(bp), " [%s]", tmp);
		}
		else
		; /* if already known as an artifact, don't bother showing the base type */
	}
	else if(obj->otyp == EGG && obj->corpsenm >= LOW_PM &&
			!(obj->known || mvitals[obj->corpsenm].mvflags & MV_KNOWS_EGG))
		Sprintf(bp, "[%s] egg%s", mons[obj->corpsenm].mname, obj->quan>1? "s" : "");
	
	else if(do_ID || do_dknown) {
		char *cp = nextobuf();
		
		if(Role_if(PM_SAMURAI) && (tmp = (char*)Japanese_item_name(obj->otyp)))
			Strcpy(cp, tmp);
		
		else if(obj->otyp == POT_WATER && (obj->blessed || obj->cursed))
			Sprintf(cp, "%sholy water", obj->blessed? "" : "un");
		else {
			Strcpy(cp, OBJ_NAME(objects[obj->otyp]));
			if(obj->oclass == GEM_CLASS) {
				if(GemStone(obj->otyp) && obj->otyp != FLINT) Strcat(cp, " stone");
				if(obj->quan > 1) cp = makeplural(cp);
			}
		}
		
		/* ideous post-processing: try to merge the ID and appearance naturally
		   the cases are significant, to avoid matching fruit names.
		   general rules, barring bugs:
		     thing of foo [thing of actual] -> thing of foo [of actual]
		       (no such objects)
		     foo thing [thing of actual] -> foo thing [of actual]
		       eg. square amulet [of strangulation]
		     thing of foo [actual thing] -> thing of foo [of actual]
		       eg. scroll labeled DUAM XNAHT [of amnesia]
		     foo thing [actual thing] -> foo thing [actual]
		       eg. mud boots [speed boots]
		     thing [thing of actual] -> thing [of actual]
		       eg. bag [of holding]
		     thing [actual thing] -> [actual] thing
		       eg. [wax] candle
		*/
		switch(obj->oclass) {
			case COIN_CLASS:
				*cp = '\0';
				break;
			case AMULET_CLASS:
				if(obj->otyp == AMULET_VERSUS_POISON) cp += sizeof("amulet"); /* versus poison */
				else if(obj->otyp == FAKE_AMULET_OF_YENDOR) *strstr(cp, " of the Amulet of Yendor") = '\0'; /* cheap plastic imitation */
				else if(obj->otyp == AMULET_OF_YENDOR) *cp = '\0'; /* is its own description */
				else cp += sizeof("amulet");
				break;
			case WEAPON_CLASS:
				if((tmp = strstr(cp, " dagger"))) *tmp = '\0';
				else if((tmp = strstr(cp, " bow"))) *tmp = '\0';
				else if((tmp = strstr(cp, " arrow"))) *tmp = '\0';
				else if((tmp = strstr(cp, " short sword"))) *tmp = '\0';
				else if((tmp = strstr(cp, " broadsword"))) *tmp = '\0';
				else if((tmp = strstr(cp, " spear"))) *tmp = '\0';
				break;
			case ARMOR_CLASS:
				if(obj->otyp == DWARVISH_CLOAK) Strcpy(cp, "dwarvish");
				/* only remove "cloak" if unIDed is already "opera cloak" */
				else if(strstr(bp, "cloak")) {
					if((tmp = strstr(cp, " cloak"))) *tmp = '\0'; /* elven */
					else if(strstr(cp, "cloak of ")) cp += sizeof("cloak"); /* other */
				}
				else if((tmp = strstr(cp, " gloves"))) *tmp = '\0'; /* other */
				else if((tmp = strstr(cp, " boots"))) *tmp = '\0';
				/* else if((tmp = strstr(cp, " boots"))) {
					*tmp = '\0';
					memmove(cp + 3, cp, strlen(cp) + 1);
					strncpy(cp, "of ", 3);
				} foo boots [actual boots] -> foo boots [of actual] */
				else if((tmp = strstr(cp, " shoes"))) *tmp = '\0'; /* iron */
				else if(strstr(cp, "helm of ")) cp += sizeof("helm");
				else if(strstr(cp, "shield of ")) cp += sizeof("shield"); /* of reflection */
				else if((tmp = strstr(cp, " shield"))) *tmp = '\0';
				else if((tmp = strstr(cp, " ring mail"))) *tmp = '\0'; /* orcish */
				else if((tmp = strstr(cp, " chain mail"))) *tmp = '\0'; /* orcish */
				break;
			case TOOL_CLASS:
				/* thing [actual thing] -> [actual] thing */
				if((tmp = strstr(cp, " candle")) ||
				   (tmp = strstr(cp, " horn")) ||
				   (tmp = strstr(cp, " lamp")) ||
				   (tmp = strstr(cp, " flute")) ||
				   (tmp = strstr(cp, " harp")) ||
				   (tmp = strstr(cp, " whistle"))) {
					*tmp = '\0';
					memmove(cp + 1, cp, strlen(cp) + 1);
					*cp = '[';
					Strcat(cp, "] ");
					bp = strprepend(bp, cp);
					*cp = '\0';
				}
				else if(strstr(cp, "horn of ")) cp += sizeof("horn"); /* of plenty */
				else if(obj->otyp == LEATHER_DRUM) Strcpy(cp, "leather");
				else if(obj->otyp == DRUM_OF_EARTHQUAKE) Strcpy(cp, "of earthquake");
				else if((tmp = strstr(cp, "bag of "))) cp += sizeof("bag");
				break;
			case GEM_CLASS:
				if(strstr(cp, "worthless piece")) Strcpy(cp, "worthless glass");
				break;
			case VENOM_CLASS:
				/* technically, this doesn't follow the rules... if anyone cares. */
				if((tmp = strstr(cp, " venom"))) *tmp = '\0';
				break;
		}
	    /* end post-processing */
		
		if(strlen(cp)) {
			if(obj->oclass == POTION_CLASS || obj->oclass == SCROLL_CLASS
			|| (obj->oclass == SPBOOK_CLASS && obj->otyp != SPE_BOOK_OF_THE_DEAD)
			|| obj->oclass == WAND_CLASS || obj->oclass == RING_CLASS)
				Sprintf(eos(bp), " [of %s]", cp);
			else
				Sprintf(eos(bp), " [%s]", cp);
		}
	}
	else if (obj->otyp == TIN && do_known) {
		if (obj->spe > 0)
			Strcat(bp, " [of spinach]");
		else if (obj->corpsenm == NON_PM)
			Strcat(bp, " [empty]");
		else if (vegetarian(&mons[obj->corpsenm]))
			Sprintf(eos(bp), " [of %s]", mons[obj->corpsenm].mname);
		else
			Sprintf(eos(bp), " [of %s meat]", mons[obj->corpsenm].mname);
	}
	else if(obj->otyp == POT_WATER &&
			(obj->blessed || obj->cursed) && do_bknown) {
		Sprintf(bp, "potion of [%sholy] water", obj->cursed? "un" : "");
	}

	/* When using xname, we want "poisoned arrow", and when using
	 * doname, we want "poisoned +0 arrow".  This kludge is about the only
	 * way to do it, at least until someone overhauls xname() and doname(),
	 * combining both into one function taking a parameter.
	 */
	/* must check opoisoned--someone can have a weirdly-named fruit */
	if (!strncmp(bp, "poisoned ", 9) && obj->opoisoned) {
		bp += 9;
		ispoisoned = TRUE;
#ifdef INVISIBLE_OBJECTS
	} else if (!strncmp(bp, "invisible ", 10) && obj->oinvis) {
		bp += 10;
		isinvisible = TRUE;
	}
#endif

	if(obj->quan != 1L)
		Sprintf(prefix, "%ld ", obj->quan);
	else if (obj_is_pname(obj) || the_unique_obj(obj)) {
		if (!strncmpi(bp, "the ", 4))
		    bp += 4;
		Strcpy(prefix, "the ");
	} else
		Strcpy(prefix, "a ");

	if (obj->oinvis && !See_invisible && !obj->opresenceknown) {
		Strcat(prefix, "invisible ");
		goto end_descr;
	}

	if (!hallu && (obj->bknown || do_bknown) &&
	    obj->oclass != COIN_CLASS &&
	    (obj->otyp != POT_WATER || !objects[POT_WATER].oc_name_known
		|| (!obj->cursed && !obj->blessed))) {
	    /* allow 'blessed clear potion' if we don't know it's holy water;
	     * always allow "uncursed potion of water"
	     */

	    if ((obj->bknown && (obj->blessed || obj->cursed)
                 /* fall-through for uncursed */)
	    	|| (do_bknown
	    		&& !(obj->otyp == POT_WATER &&
                             (obj->blessed || obj->cursed))
	    		/* "[blessed] clear potion [of holy water]"
                            is redundant */ ))
	        Sprintf(eos(prefix), "%s%s%s ", do_bknown ? "[" : "",
	            obj->blessed ? "blessed" : obj->cursed ? "cursed" :
                                                             "uncursed",
	            do_bknown ? "]" : "");
	    else if (do_bknown && obj->otyp == POT_WATER)
	        ; /* (un)holy water, skipped as above */
	    else if (iflags.show_buc ||
	             ((!obj->known || !objects[obj->otyp].oc_charged ||
		      (obj->oclass == ARMOR_CLASS ||
		       obj->oclass == RING_CLASS))
		/* For most items with charges or +/-, if you know how many
		 * charges are left or what the +/- is, then you must have
		 * totally identified the item, so "uncursed" is unneccesary,
		 * because an identified object not described as "blessed" or
		 * "cursed" must be uncursed.
		 *
		 * If the charges or +/- is not known, "uncursed" must be
		 * printed to avoid ambiguity between an item whose curse
		 * status is unknown, and an item known to be uncursed.
		 */
#ifdef MAIL
			&& obj->otyp != SCR_MAIL
#endif
			&& obj->otyp != FAKE_AMULET_OF_YENDOR
			&& obj->otyp != AMULET_OF_YENDOR
			&& (!Role_if(PM_PRIEST))))
		Strcat(prefix, "uncursed ");
	} else if (hallu && !rn2(5)) {
	    Strcat(prefix, !rn2(8) ? "cursed " : !rn2(8) ? "blessed "
	                                                : "uncursed ");
	}

#ifdef INVISIBLE_OBJECTS
	if (isinvisible) {
		Strcat(prefix, "invisible ");
	}
#endif

	if ((hallu && !rn2(5)) ||
	    (!hallu && obj->greased)) Strcat(prefix, "greased ");
	
	if (hallu)
	{
	    if (!rn2(10))
	    {
	        if (!rn2(4))
	            Strcat(prefix, rn2(2) ? "thoroughly " : "very ");
	        switch(rn2(4))
	        {
	            case 0:
		    default: Strcat(prefix, "rusty ");    break;
		    case 1:  Strcat(prefix, "corroded "); break;
		    case 2:  Strcat(prefix, "burnt ");    break;
		    case 3:  Strcat(prefix, "rotted ");   break;
	        }
	    }
	    if (!rn2(5))
	    {
	        if (!rn2(5)) Strcat(prefix, "poisoned ");
	        Strcat(prefix, sitoa(rn1(15, -7)));
		Strcat(prefix, " ");
	    } else if (!rn2(5)) {
	        Sprintf(eos(bp), " (%d:%d)", rn2(8), rn2(15));
	    }
	}


	switch(obj->oclass) {
	case AMULET_CLASS:
		if (!hallu) add_erosion_words(obj, prefix);
		if(obj->owornmask & W_AMUL)
			Strcat(bp, " (being worn)");
		break;
	case WEAPON_CLASS:
		if(ispoisoned && !hallu)
			Strcat(prefix, "poisoned ");
plus:
		if (!hallu) add_erosion_words(obj, prefix);
		if((obj->known && !hallu) || do_known) {
			Sprintf(eos(prefix), "%s%s%s ",
				do_known ? "[" : "",
				sitoa(obj->spe),
				do_known ? "]" : "");
		}
		break;
	case ARMOR_CLASS:
		if(obj->owornmask & W_ARMOR)
			Strcat(bp, (obj == uskin) ? " (embedded in your skin)" :
				" (being worn)");
		goto plus;
	case TOOL_CLASS:
		/* weptools already get this done when we go to the +n code */
		if (!is_weptool(obj) && !hallu)
		    add_erosion_words(obj, prefix);
		if(obj->owornmask & (W_TOOL /* blindfold */
#ifdef STEED
				| W_SADDLE
#endif
				)) {
			Strcat(bp, " (being worn)");
			break;
		}
		if (obj->otyp == LEASH && obj->leashmon != 0) {
			Strcat(bp, " (in use)");
			break;
		}
		if (is_weptool(obj))
			goto plus;
		if (obj->otyp == CANDELABRUM_OF_INVOCATION
		    && !hallu) {
			if (!obj->spe)
			    Strcpy(tmpbuf, "no");
			else
			    Sprintf(tmpbuf, "%d", obj->spe);
			Sprintf(eos(bp), " (%s candle%s%s)",
				tmpbuf, plur(obj->spe),
				!obj->lamplit ? " attached" : ", lit");
			break;
		} else if (obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
			obj->otyp == BRASS_LANTERN || Is_candle(obj)) {
			if (!hallu && Is_candle(obj) &&
			    obj->age < 20L * (long)objects[obj->otyp].oc_cost)
				Strcat(prefix, "partly used ");
			if(obj->lamplit)
				Strcat(bp, " (lit)");
			break;
		}
		if(objects[obj->otyp].oc_charged)
		    goto charges;
		break;
	case SPBOOK_CLASS:
#define MAX_SPELL_STUDY 3 /* spell.c */
		if (dump_ID_flag && obj->spestudied > MAX_SPELL_STUDY / 2)
			Strcat(prefix, "[faint] ");
		break;
	case WAND_CLASS:
		if (!hallu) add_erosion_words(obj, prefix);
charges:
		if((obj->known && !hallu) || do_known)
		    Sprintf(eos(bp), " %s(%d:%d)%s",
			do_known ? "[" : "",
			(int)obj->recharged, obj->spe,
			do_known ? "]" : ""
			);
		break;
	case POTION_CLASS:
		if (obj->otyp == POT_OIL && obj->lamplit)
		    Strcat(bp, " (lit)");
		break;
	case RING_CLASS:
		if (!hallu) add_erosion_words(obj, prefix);
ring:
		if(obj->owornmask & W_RINGR) Strcat(bp, " (on right ");
		if(obj->owornmask & W_RINGL) Strcat(bp, " (on left ");
		if(obj->owornmask & W_RING) {
		    Strcat(bp, body_part(HAND));
		    Strcat(bp, ")");
		}
		if(((obj->known && !hallu) || do_known) &&
		   objects[obj->otyp].oc_charged && !hallu) {
			Sprintf(eos(prefix), "%s%s%s ",
				do_known ? "[" : "",
				sitoa(obj->spe),
				do_known ? "]" : ""
				);
		}
		break;
	case FOOD_CLASS:
	        if (hallu) break; /* no extra information */
		if (obj->oeaten)
		    Strcat(prefix, "partly eaten ");
		if (obj->otyp == CORPSE) {
		        long age = peek_at_iced_corpse_age(obj);

		        long rotted = (monstermoves - age)/(20L);
		        if (obj->cursed) rotted += 2L;
		        else if (obj->blessed) rotted -= 2L;
		    if (mons[obj->corpsenm].geno & G_UNIQ) {

			Sprintf(prefix, "%s%s ",
				(type_is_pname(&mons[obj->corpsenm]) ?
					"" : "the "),
				s_suffix(mons[obj->corpsenm].mname));
			if (obj->oeaten) Strcat(prefix, "partly eaten ");

			if (obj->oerodeproof) {
				Strcat(prefix, "rotproof ");
			} else
		        Strcat(prefix, (rotted == 0L ||
		                 obj->corpsenm == PM_ACID_BLOB ||
				 obj->corpsenm == PM_LICHEN ||
				 obj->corpsenm == PM_LIZARD) ? "" :
		                (rotted == 1L) ?
					((!Blind
#ifdef INVISIBLE_OBJECTS
					&& (!obj->oinvis || See_invisible)
#endif
					) ? "off-color " : "smelly ") :
		                (rotted <= 3L) ? "slightly rotted " :
				(rotted <= 4L) ? "rotted " :
		                (rotted <= 5L) ? "very rotted " :
		                                 "thoroughly rotted ");
		    } else {
			if (obj->oerodeproof) {
				Strcat(prefix, "rotproof ");
			} else
		        Strcat(prefix, (rotted == 0L ||
		                 obj->corpsenm == PM_ACID_BLOB ||
				 obj->corpsenm == PM_LICHEN ||
				 obj->corpsenm == PM_LIZARD) ? "" :
		                (rotted == 1L) ? 
					((!Blind
#ifdef INVISIBLE_OBJECTS
					&& (!obj->oinvis || See_invisible)
#endif
					) ? "off-color " : "smelly ") :
		                (rotted <= 3L) ? "slightly rotted " :
				(rotted <= 4L) ? "rotted " :
		                (rotted <= 5L) ? "very rotted " :
		                                 "thoroughly rotted ");
			Strcat(prefix, mons[obj->corpsenm].mname);
			Strcat(prefix, " ");
		        Strcat(prefix, (obj->spe == -2) ? "zombie " :
		                       (obj->spe == -3) ? "wererat " :
			               (obj->spe == -4) ? "werejackal " :
				       (obj->spe == -5) ? "werewolf " : "");
		    }
		} else if (obj->otyp == EGG) {
#if 0	/* corpses don't tell if they're stale either */
		    if (obj->known && stale_egg(obj))
			Strcat(prefix, "stale ");
#endif
		    if (dump_ID_flag && stale_egg(obj))
			Strcat(prefix, "stale ");
		    if (obj->corpsenm >= LOW_PM &&
			    (obj->known ||
			    mvitals[obj->corpsenm].mvflags & MV_KNOWS_EGG)) {
			Strcat(prefix, mons[obj->corpsenm].mname);
			Strcat(prefix, " ");
			if (obj->spe)
			    Strcat(bp, " (laid by you)");
		    }
		}
		if (obj->otyp == MEAT_RING) goto ring;
		break;
	case BALL_CLASS:
	case CHAIN_CLASS:
		add_erosion_words(obj, prefix);
		if(obj->owornmask & W_BALL)
			Strcat(bp, " (chained to you)");
			break;
	}

end_descr:
	if((obj->owornmask & W_WEP) && !mrg_to_wielded) {
		if (obj->quan != 1L) {
			Strcat(bp, " (wielded)");
		} else {
			const char *hand_s = body_part(HAND);

			if (bimanual(obj)) hand_s = makeplural(hand_s);
			Sprintf(eos(bp), " (weapon in %s)", hand_s);
		}
	}
	if(obj->owornmask & W_SWAPWEP) {
		if (u.twoweap)
			Sprintf(eos(bp), " (wielded in other %s)",
				body_part(HAND));
		else
			Strcat(bp, " (alternate weapon; not wielded)");
	}
	if(obj->owornmask & W_QUIVER) Strcat(bp, " (in quiver)");
	if (obj->oclass != COIN_CLASS) {
		xchar ox, oy; 
		boolean on_floor = get_obj_location(
			obj, &ox, &oy, BURIED_TOO|CONTAINED_TOO);
		if (obj->unpaid ||
		    (!obj->no_charge && obj->where != OBJ_INVENT &&
		      on_floor && costly_spot(ox, oy))) {
			struct monst *shkp = (on_floor)
			    ? shop_keeper(*in_rooms(ox, oy, SHOPBASE))
			    : (struct monst *)0;
			long quotedprice =
			    (obj->unpaid) ? unpaid_cost(obj) :
			    (shkp)        ? get_cost(obj, shkp) : 0L;

			if (!obj->unpaid && shkp &&
			    Has_contents(obj))
			    quotedprice +=
			        contained_cost(obj, shkp, 0L, FALSE, FALSE);

			if (Has_contents(obj) &&
			    on_floor &&
		            costly_spot(ox, oy) && shkp)
			        quotedprice += contained_cost(
				    obj, shkp, 0L, FALSE, TRUE);
		    if (obj->unpaid)
		        Sprintf(eos(bp), " (unpaid, %ld %s)",
			        quotedprice, currency(quotedprice));
		    else if (quotedprice) {
		        Sprintf(eos(bp), " priced at %ld %s%s",
			        quotedprice, currency(quotedprice),
				obj->quan > 1 ? " each" : "");
		    }
		}
	}
	if (!strncmp(prefix, "a ", 2) &&
			(index(vowels, prefix[2] ? prefix[2] : *bp)
			 || (dump_ID_flag && !strncmp(prefix+2, "[uncursed", 9)))
			&& (*(prefix+2) || (strncmp(bp, "uranium", 7)
				&& strncmp(bp, "unicorn", 7)
				&& strncmp(bp, "eucalyptus", 10)))) {
		Strcpy(tmpbuf, prefix);
		Strcpy(prefix, "an ");
		Strcpy(prefix+3, tmpbuf+2);
	}
	/* merge bracketed attribs
	   eg. [rustproof] [+1] -> [rustproof +1] */
	tmp = prefix;
	while((tmp = strstr(tmp, "] ["))) {
		*tmp = ' ';
		memmove(tmp + 1, tmp + 3, strlen(tmp + 3) + 1);
	}
	bp = strprepend(bp, prefix);
	if(obj->otyp != SLIME_MOLD) {
		tmp = bp;
		while((tmp = strstr(tmp, "] ["))) {
			*tmp = ' ';
			memmove(tmp + 1, tmp + 3, strlen(tmp + 3) + 1);
		}
		/* turn [(n:n)] wand charges into [n:n] */
		if((tmp = strstr(bp, "[("))) {
			char *tmp2 = strstr(tmp, ")]");
			memmove(tmp2, tmp2 + 1, strlen(tmp2 + 1) + 1);
			memmove(tmp + 1, tmp + 2, strlen(tmp + 2) + 1);
		}
	}
	return(bp);
}

#endif /* OVL0 */
#ifdef OVLB

/* used from invent.c */
boolean
not_fully_identified(otmp)
register struct obj *otmp;
{
#ifdef GOLDOBJ
    /* gold doesn't have any interesting attributes [yet?] */
    if (otmp->oclass == COIN_CLASS) return FALSE;	/* always fully ID'd */
#endif
    /* check fundamental ID hallmarks first */
    if (!otmp->known || !otmp->dknown ||
#ifdef MAIL
	    (!otmp->bknown && otmp->otyp != SCR_MAIL) ||
#else
	    !otmp->bknown ||
#endif
	    !objects[otmp->otyp].oc_name_known)	/* ?redundant? */
	return TRUE;
    if (((otmp->oprops_known & otmp->oprops) & ITEM_PROP_MASK) != otmp->oprops)
    	return TRUE;
    if (otmp->oartifact && undiscovered_artifact(otmp->oartifact))
	return TRUE;
    /* otmp->rknown is the only item of interest if we reach here */
       /*
	*  Note:  if a revision ever allows scrolls to become fireproof or
	*  rings to become shockproof, this checking will need to be revised.
	*  `rknown' ID only matters if xname() will provide the info about it.
	*/
    if (otmp->rknown || (otmp->oclass != ARMOR_CLASS &&
			 otmp->oclass != WEAPON_CLASS &&
			 !is_weptool(otmp) &&		    /* (redunant) */
			 otmp->oclass != BALL_CLASS))	    /* (useless) */
	return FALSE;
    else	/* lack of `rknown' only matters for vulnerable objects */
	return (boolean)(is_rustprone(otmp) ||
			 is_corrodeable(otmp) ||
			 is_flammable(otmp));
}

char *
corpse_xname(otmp, ignore_oquan)
struct obj *otmp;
boolean ignore_oquan;	/* to force singular */
{
	char *nambuf = nextobuf();

	if (Hallucination)
	    return (ignore_oquan || otmp->quan < 2) ? xname(otmp)
	                                            : makeplural(xname(otmp));
	if (otmp->otyp == ROCK)
		Sprintf(nambuf, "chunk of %s%s",
			mons[otmp->corpsenm].mname,
		    	(vegetarian(&mons[otmp->corpsenm])) ? "" : " flesh");
	else
		Sprintf(nambuf, "%s corpse",
			type_is_pname(&mons[otmp->corpsenm])
			? s_suffix(mons[otmp->corpsenm].mname)
			: mons[otmp->corpsenm].mname);

	if (ignore_oquan || otmp->quan < 2)
	    return nambuf;
	else
	    return makeplural(nambuf);
}

/* xname, unless it's a corpse, then corpse_xname(obj, FALSE) */
char *
cxname(obj)
struct obj *obj;
{
	if (obj->otyp == CORPSE)
	    return corpse_xname(obj, FALSE);
	return xname(obj);
}
#ifdef SORTLOOT
char *
cxname2(obj)
struct obj *obj;
{
	if (obj->otyp == CORPSE)
	    return corpse_xname(obj, TRUE);
	return xname2(obj, TRUE);
}
#endif /* SORTLOOT */

/* treat an object as fully ID'd when it might be used as reason for death */
char *
killer_xname(obj, adj, prefix)
struct obj *obj;
const char *adj;
boolean prefix;
{
    struct obj save_obj;
    unsigned save_ocknown;
    char buf[BUFSZ], *save_ocuname;

    /* remember original settings for core of the object;
       oname and oattached extensions don't matter here--since they
       aren't modified they don't need to be saved and restored */
    save_obj = *obj;
    /* killer name should be more specific than general xname; however, exact
       info like blessed/cursed and rustproof makes things be too verbose */
    obj->known = obj->dknown =
#ifdef INVISIBLE_OBJECTS
			  obj->iknown = obj->opresenceknown =
#endif
    	1;
    obj->bknown = obj->rknown = obj->greased = 0;
    /* if character is a priest[ess], bknown will get toggled back on */
    obj->blessed = obj->cursed = 0;
    /* "killed by poisoned <obj>" would be misleading when poison is
       not the cause of death and "poisoned by poisoned <obj>" would
       be redundant when it is, so suppress "poisoned" prefix */
    obj->opoisoned = 0;
    /* strip user-supplied name; artifacts keep theirs */
    if (!obj->oartifact) obj->onamelth = 0;
    /* temporarily identify the type of object */
    save_ocknown = objects[obj->otyp].oc_name_known;
    objects[obj->otyp].oc_name_known = 1;
    save_ocuname = objects[obj->otyp].oc_uname;
    objects[obj->otyp].oc_uname = 0;	/* avoid "foo called bar" */

    Sprintf(buf, "%s%s%s", adj, strlen(adj) > 0 ? " " : "", cxname(obj));
    if (obj->quan == 1L && prefix)
    	Sprintf(buf, "%s", obj_is_pname(obj) ? the(buf) : an(buf));

    objects[obj->otyp].oc_name_known = save_ocknown;
    objects[obj->otyp].oc_uname = save_ocuname;
    *obj = save_obj;	/* restore object's core settings */

    return (char *)strdup(buf);
}

/*
 * Used if only one of a collection of objects is named (e.g. in eat.c).
 */
const char *
singular(otmp, func)
register struct obj *otmp;
char *FDECL((*func), (OBJ_P));
{
	long savequan;
	char *nam;

	/* Note: using xname for corpses will not give the monster type */
	if (otmp->otyp == CORPSE && func == xname)
		return corpse_xname(otmp, TRUE);

	savequan = otmp->quan;
	otmp->quan = 1L;
	nam = (*func)(otmp);
	otmp->quan = savequan;
	return nam;
}

char *
an(str)
register const char *str;
{
	char *buf = nextobuf();

	buf[0] = '\0';

	if (strncmpi(str, "the ", 4) &&
	    strcmp(str, "molten lava") &&
	    strcmp(str, "iron bars") &&
	    strcmp(str, "ice")) {
		if (index(vowels, *str) &&
		    strncmp(str, "one-", 4) &&
		    strncmp(str, "useful", 6) &&
		    strncmp(str, "unicorn", 7) &&
		    strncmp(str, "uranium", 7) &&
		    strncmp(str, "eucalyptus", 10))
			Strcpy(buf, "an ");
		else
			Strcpy(buf, "a ");
	}

	Strcat(buf, str);
	return buf;
}

char *
An(str)
const char *str;
{
	register char *tmp = an(str);
	*tmp = highc(*tmp);
	return tmp;
}

/*
 * Prepend "the" if necessary; assumes str is a subject derived from xname.
 * Use type_is_pname() for monster names, not the().  the() is idempotent.
 */
char *
the(str)
const char *str;
{
	char *buf = nextobuf();
	boolean insert_the = FALSE;

	if (!strncmpi(str, "the ", 4)) {
	    buf[0] = lowc(*str);
	    Strcpy(&buf[1], str+1);
	    return buf;
	} else if (*str < 'A' || *str > 'Z') {
	    /* not a proper name, needs an article */
	    insert_the = TRUE;
	} else {
	    /* Probably a proper name, might not need an article */
	    register char *tmp, *named, *called;
	    int l;

	    /* some objects have capitalized adjectives in their names */
	    if(((tmp = rindex(str, ' ')) || (tmp = rindex(str, '-'))) &&
	       (tmp[1] < 'A' || tmp[1] > 'Z'))
		insert_the = TRUE;
	    else if (tmp && index(str, ' ') < tmp) {	/* has spaces */
		/* it needs an article if the name contains "of" */
		tmp = strstri(str, " of ");
		named = strstri(str, " named ");
		called = strstri(str, " called ");
		if (called && (!named || called < named)) named = called;

		if (tmp && (!named || tmp < named))	/* found an "of" */
		    insert_the = TRUE;
		/* stupid special case: lacks "of" but needs "the" */
		else if (!named && (l = strlen(str)) >= 31 &&
		      !strcmp(&str[l - 31], "Platinum Yendorian Express Card"))
		    insert_the = TRUE;
	    }
	}
	if (insert_the)
	    Strcpy(buf, "the ");
	else
	    buf[0] = '\0';
	Strcat(buf, str);

	return buf;
}

char *
The(str)
const char *str;
{
    register char *tmp = the(str);
    *tmp = highc(*tmp);
    return tmp;
}

/* returns "count cxname(otmp)" or just cxname(otmp) if count == 1 */
char *
aobjnam(otmp,verb)
register struct obj *otmp;
register const char *verb;
{
	register char *bp = cxname(otmp);
	char prefix[PREFIX];

	if(otmp->quan != 1L) {
		Sprintf(prefix, "%ld ", otmp->quan);
		bp = strprepend(bp, prefix);
	}

	if(verb) {
	    Strcat(bp, " ");
	    Strcat(bp, otense(otmp, verb));
	}
	return(bp);
}

/* like aobjnam, but prepend "The", not count, and use xname */
char *
Tobjnam(otmp, verb)
register struct obj *otmp;
register const char *verb;
{
	char *bp = The(xname(otmp));

	if(verb) {
	    Strcat(bp, " ");
	    Strcat(bp, otense(otmp, verb));
	}
	return(bp);
}

/* return form of the verb (input plural) if xname(otmp) were the subject */
char *
otense(otmp, verb)
register struct obj *otmp;
register const char *verb;
{
	char *buf;

	/*
	 * verb is given in plural (without trailing s).  Return as input
	 * if the result of xname(otmp) would be plural.  Don't bother
	 * recomputing xname(otmp) at this time.
	 */
	if (!is_plural(otmp))
	    return vtense((char *)0, verb);

	buf = nextobuf();
	Strcpy(buf, verb);
	return buf;
}

/* various singular words that vtense would otherwise categorize as plural */
static const char * const special_subjs[] = {
	"erinys",
	"manes",		/* this one is ambiguous */
	"Cyclops",
	"Hippocrates",
	"Pelias",
	"aklys",
	"amnesia",
	"paralysis",
	0
};

/* return form of the verb (input plural) for present tense 3rd person subj */
char *
vtense(subj, verb)
register const char *subj;
register const char *verb;
{
	char *buf = nextobuf();
	int len, ltmp;
	const char *sp, *spot;
	const char * const *spec;

	/*
	 * verb is given in plural (without trailing s).  Return as input
	 * if subj appears to be plural.  Add special cases as necessary.
	 * Many hard cases can already be handled by using otense() instead.
	 * If this gets much bigger, consider decomposing makeplural.
	 * Note: monster names are not expected here (except before corpse).
	 *
	 * special case: allow null sobj to get the singular 3rd person
	 * present tense form so we don't duplicate this code elsewhere.
	 */
	if (subj) {
	    if (!strncmpi(subj, "a ", 2) || !strncmpi(subj, "an ", 3))
		goto sing;
	    spot = (const char *)0;
	    for (sp = subj; (sp = index(sp, ' ')) != 0; ++sp) {
		if (!strncmp(sp, " of ", 4) ||
		    !strncmp(sp, " from ", 6) ||
		    !strncmp(sp, " called ", 8) ||
		    !strncmp(sp, " named ", 7) ||
		    !strncmp(sp, " labeled ", 9)) {
		    if (sp != subj) spot = sp - 1;
		    break;
		}
	    }
	    len = (int) strlen(subj);
	    if (!spot) spot = subj + len - 1;

	    /*
	     * plural: anything that ends in 's', but not '*us' or '*ss'.
	     * Guess at a few other special cases that makeplural creates.
	     */
	    if ((*spot == 's' && spot != subj &&
			(*(spot-1) != 'u' && *(spot-1) != 's')) ||
		((spot - subj) >= 4 && !strncmp(spot-3, "eeth", 4)) ||
		((spot - subj) >= 3 && !strncmp(spot-3, "feet", 4)) ||
		((spot - subj) >= 2 && !strncmp(spot-1, "ia", 2)) ||
		((spot - subj) >= 2 && !strncmp(spot-1, "ae", 2))) {
		/* check for special cases to avoid false matches */
		len = (int)(spot - subj) + 1;
		for (spec = special_subjs; *spec; spec++) {
		    ltmp = strlen(*spec);
		    if (len == ltmp && !strncmpi(*spec, subj, len)) goto sing;
		    /* also check for <prefix><space><special_subj>
		       to catch things like "the invisible erinys" */
		    if (len > ltmp && *(spot - ltmp) == ' ' &&
			   !strncmpi(*spec, spot - ltmp + 1, ltmp)) goto sing;
		}

		return strcpy(buf, verb);
	    }
	    /*
	     * 3rd person plural doesn't end in telltale 's';
	     * 2nd person singular behaves as if plural.
	     */
	    if (!strcmpi(subj, "they") || !strcmpi(subj, "you"))
		return strcpy(buf, verb);
	}

 sing:
	len = strlen(verb);
	spot = verb + len - 1;

	if (!strcmp(verb, "are"))
	    Strcpy(buf, "is");
	else if (!strcmp(verb, "have"))
	    Strcpy(buf, "has");
	else if (index("zxs", *spot) ||
		 (len >= 2 && *spot=='h' && index("cs", *(spot-1))) ||
		 (len == 2 && *spot == 'o')) {
	    /* Ends in z, x, s, ch, sh; add an "es" */
	    Strcpy(buf, verb);
	    Strcat(buf, "es");
	} else if (*spot == 'y' && (!index(vowels, *(spot-1)))) {
	    /* like "y" case in makeplural */
	    Strcpy(buf, verb);
	    Strcpy(buf + len - 1, "ies");
	} else {
	    Strcpy(buf, verb);
	    Strcat(buf, "s");
	}

	return buf;
}

/* capitalized variant of doname() */
char *
Doname2(obj)
register struct obj *obj;
{
	register char *s = doname(obj);

	*s = highc(*s);
	return(s);
}

/* returns "your xname(obj)" or "Foobar's xname(obj)" or "the xname(obj)" */
char *
yname(obj)
struct obj *obj;
{
	char *outbuf = nextobuf();
	char *s = shk_your(outbuf, obj);	/* assert( s == outbuf ); */
	int space_left = BUFSZ - strlen(s) - sizeof " ";

	return strncat(strcat(s, " "), cxname(obj), space_left);
}

/* capitalized variant of yname() */
char *
Yname2(obj)
struct obj *obj;
{
	char *s = yname(obj);

	*s = highc(*s);
	return s;
}

/* returns "your simple_typename(obj->otyp)"
 * or "Foobar's simple_typename(obj->otyp)"
 * or "the simple_typename(obj-otyp)"
 */
char *
ysimple_name(obj)
struct obj *obj;
{
	char *outbuf = nextobuf();
	char *s = shk_your(outbuf, obj);	/* assert( s == outbuf ); */
	int space_left = BUFSZ - strlen(s) - sizeof " ";

	return strncat(strcat(s, " "), simple_typename(obj->otyp), space_left);
}

/* capitalized variant of ysimple_name() */
char *
Ysimple_name2(obj)
struct obj *obj;
{
	char *s = ysimple_name(obj);

	*s = highc(*s);
	return s;
}

static const char *wrp[] = {
	"wand", "ring", "potion", "scroll", "gem", "amulet",
	"spellbook", "spell book",
	/* for non-specific wishes */
	"weapon", "armor", "armour", "tool", "food", "comestible",
};
static const char wrpsym[] = {
	WAND_CLASS, RING_CLASS, POTION_CLASS, SCROLL_CLASS, GEM_CLASS,
	AMULET_CLASS, SPBOOK_CLASS, SPBOOK_CLASS,
	WEAPON_CLASS, ARMOR_CLASS, ARMOR_CLASS, TOOL_CLASS, FOOD_CLASS,
	FOOD_CLASS
};

#endif /* OVLB */
#ifdef OVL0

/* Plural routine; chiefly used for user-defined fruits.  We have to try to
 * account for everything reasonable the player has; something unreasonable
 * can still break the code.  However, it's still a lot more accurate than
 * "just add an s at the end", which Rogue uses...
 *
 * Also used for plural monster names ("Wiped out all homunculi.")
 * and body parts.
 *
 * Also misused by muse.c to convert 1st person present verbs to 2nd person.
 */
char *
makeplural(oldstr)
const char *oldstr;
{
	/* Note: cannot use strcmpi here -- it'd give MATZot, CAVEMeN,... */
	register char *spot;
	char *str = nextobuf();
	const char *excess = (char *)0;
	int len;

	while (*oldstr==' ') oldstr++;
	if (!oldstr || !*oldstr) {
		impossible("plural of null?");
		Strcpy(str, "s");
		return str;
	}
	Strcpy(str, oldstr);

	/*
	 * Skip changing "pair of" to "pairs of".  According to Webster, usual
	 * English usage is use pairs for humans, e.g. 3 pairs of dancers,
	 * and pair for objects and non-humans, e.g. 3 pair of boots.  We don't
	 * refer to pairs of humans in this game so just skip to the bottom.
	 */
	if (!strncmp(str, "pair of ", 8))
		goto bottom;

	/* Search for common compounds, ex. lump of royal jelly */
	for(spot=str; *spot; spot++) {
		if (!strncmp(spot, " of ", 4)
				|| !strncmp(spot, " labeled ", 9)
				|| !strncmp(spot, " called ", 8)
				|| !strncmp(spot, " named ", 7)
				|| !strcmp(spot, " above") /* lurkers above */
				|| !strncmp(spot, " versus ", 8)
				|| !strncmp(spot, " from ", 6)
				|| !strncmp(spot, " in ", 4)
				|| !strncmp(spot, " on ", 4)
				|| !strncmp(spot, " a la ", 6)
				|| !strncmp(spot, " with", 5)	/* " with "? */
				|| !strncmp(spot, " de ", 4)
				|| !strncmp(spot, " d'", 3)
				|| !strncmp(spot, " du ", 4)
		                || !strncmp(spot, " [of ", 5)
				|| !strncmp(spot, " [labeled ", 10)
				|| !strncmp(spot, " [called ", 9)
				|| !strncmp(spot, " [named ", 8)
				|| !strncmp(spot, " [versus ", 9)
				|| !strncmp(spot, " [from ", 5)
				|| !strncmp(spot, " [in ", 5)
				|| !strncmp(spot, " [on ", 5)
				|| !strncmp(spot, " [a la ", 7)
				|| !strncmp(spot, " [with", 6)	/* " [with "? */
				|| !strncmp(spot, " [de ", 5)
				|| !strncmp(spot, " [d'", 4)
				|| !strncmp(spot, " [du ", 5)) {
			excess = oldstr + (int) (spot - str);
			*spot = 0;
			break;
		}
	}
	spot--;
	while (*spot==' ') spot--; /* Strip blanks from end */
	*(spot+1) = 0;
	/* Now spot is the last character of the string */

	len = strlen(str);

	/* Single letters */
	if (len==1 || !letter(*spot)) {
		Strcpy(spot+1, "'s");
		goto bottom;
	}

	/* Same singular and plural; mostly Japanese words except for "manes" */
	if ((len == 2 && !strcmp(str, "ya")) ||
	    (len >= 2 && !strcmp(spot-1, "ai")) || /* samurai, Uruk-hai */
	    (len >= 3 && !strcmp(spot-2, " ya")) ||
	    (len >= 4 &&
	     (!strcmp(spot-3, "fish") || !strcmp(spot-3, "tuna") ||
	      !strcmp(spot-3, "deer") || !strcmp(spot-3, "yaki"))) ||
	    (len >= 5 && (!strcmp(spot-4, "sheep") ||
			!strcmp(spot-4, "ninja") ||
			!strcmp(spot-4, "ronin") ||
			!strcmp(spot-4, "shito") ||
			!strcmp(spot-7, "shuriken") ||
			!strcmp(spot-4, "tengu") ||
			!strcmp(spot-4, "manes"))) ||
	    (len >= 6 && !strcmp(spot-5, "ki-rin")) ||
	    (len >= 7 && !strcmp(spot-6, "gunyoki")))
		goto bottom;

	/* man/men ("Wiped out all cavemen.") */
	if (len >= 3 && !strcmp(spot-2, "man") &&
			(len<6 || strcmp(spot-5, "shaman")) &&
			(len<5 || strcmp(spot-4, "human"))) {
		*(spot-1) = 'e';
		goto bottom;
	}

	/* tooth/teeth */
	if (len >= 5 && !strcmp(spot-4, "tooth")) {
		Strcpy(spot-3, "eeth");
		goto bottom;
	}

	/* knife/knives, etc... */
	if (!strcmp(spot-1, "fe")) {
		Strcpy(spot-1, "ves");
		goto bottom;
	} else if (*spot == 'f') {
		if (index("lr", *(spot-1)) || index(vowels, *(spot-1))) {
			Strcpy(spot, "ves");
			goto bottom;
		} else if (len >= 5 && !strncmp(spot-4, "staf", 4)) {
			Strcpy(spot-1, "ves");
			goto bottom;
		}
	}

	/* foot/feet (body part) */
	if (len >= 4 && !strcmp(spot-3, "foot")) {
		Strcpy(spot-2, "eet");
		goto bottom;
	}

	/* ium/ia (mycelia, baluchitheria) */
	if (len >= 3 && !strcmp(spot-2, "ium")) {
		*(spot--) = (char)0;
		*spot = 'a';
		goto bottom;
	}

	/* algae, larvae, hyphae (another fungus part) */
	if ((len >= 4 && !strcmp(spot-3, "alga")) ||
	    (len >= 5 &&
	     (!strcmp(spot-4, "hypha") || !strcmp(spot-4, "larva")))) {
		Strcpy(spot, "ae");
		goto bottom;
	}

	/* fungus/fungi, homunculus/homunculi, but buses, lotuses, wumpuses */
	if (len > 3 && !strcmp(spot-1, "us") &&
	    (len < 5 || (strcmp(spot-4, "lotus") &&
			 (len < 6 || strcmp(spot-5, "wumpus"))))) {
		*(spot--) = (char)0;
		*spot = 'i';
		goto bottom;
	}

	/* vortex/vortices */
	if (len >= 6 && !strcmp(spot-3, "rtex")) {
		Strcpy(spot-1, "ices");
		goto bottom;
	}

	/* djinni/djinn (note: also efreeti/efreet) */
	if (len >= 6 && !strcmp(spot-5, "djinni")) {
		*spot = (char)0;
		goto bottom;
	}

	/* mumak/mumakil */
	if (len >= 5 && !strcmp(spot-4, "mumak")) {
		Strcpy(spot+1, "il");
		goto bottom;
	}

	/* sis/ses (nemesis) */
	if (len >= 3 && !strcmp(spot-2, "sis")) {
		*(spot-1) = 'e';
		goto bottom;
	}

	/* erinys/erinyes */
	if (len >= 6 && !strcmp(spot-5, "erinys")) {
		Strcpy(spot, "es");
		goto bottom;
	}

	/* mouse/mice,louse/lice (not a monster, but possible in food names) */
	if (len >= 5 && !strcmp(spot-3, "ouse") && index("MmLl", *(spot-4))) {
		Strcpy(spot-3, "ice");
		goto bottom;
	}

	/* The plural of Nazgul is Nazgul. */
	if (len >= 6 && !strcmp(spot-5, "Nazgul")) {
		goto bottom;
	}

	/* matzoh/matzot, possible food name */
	if (len >= 6 && (!strcmp(spot-5, "matzoh")
					|| !strcmp(spot-5, "matzah"))) {
		Strcpy(spot-1, "ot");
		goto bottom;
	}
	if (len >= 5 && (!strcmp(spot-4, "matzo")
					|| !strcmp(spot-5, "matza"))) {
		Strcpy(spot, "ot");
		goto bottom;
	}

	/* child/children (for wise guys who give their food funny names) */
	if (len >= 5 && !strcmp(spot-4, "child")) {
		Strcpy(spot, "dren");
		goto bottom;
	}


	/* note: -eau/-eaux (gateau, bordeau...) */
	/* note: ox/oxen, VAX/VAXen, goose/geese */

	/* Ends in z, x, s, ch, sh; add an "es" */
	if (index("zxs", *spot)
			|| (len >= 2 && *spot=='h' && index("cs", *(spot-1)))
	/* Kludge to get "tomatoes" and "potatoes" right */
			|| (len >= 4 && !strcmp(spot-2, "ato"))) {
		Strcpy(spot+1, "es");
		goto bottom;
	}

	/* Ends in y preceded by consonant (note: also "qu") change to "ies" */
	if (*spot == 'y' &&
	    (!index(vowels, *(spot-1)))) {
		Strcpy(spot, "ies");
		goto bottom;
	}

	/* Default: append an 's' */
	Strcpy(spot+1, "s");

bottom:	if (excess) Strcpy(eos(str), excess);
	return str;
}

#endif /* OVL0 */

struct o_range {
	const char *name, oclass;
	int  f_o_range, l_o_range;
};

#ifndef OVLB

STATIC_DCL const struct o_range o_ranges[];

#else /* OVLB */

/* wishable subranges of objects */
STATIC_OVL NEARDATA const struct o_range o_ranges[] = {
	{ "bag",	TOOL_CLASS,   SACK,	      BAG_OF_TRICKS },
	{ "lamp",	TOOL_CLASS,   OIL_LAMP,	      MAGIC_LAMP },
	{ "candle",	TOOL_CLASS,   TALLOW_CANDLE,  WAX_CANDLE },
	{ "horn",	TOOL_CLASS,   TOOLED_HORN,    HORN_OF_PLENTY },
	{ "shield",	ARMOR_CLASS,  SMALL_SHIELD,   SHIELD_OF_REFLECTION },
	{ "helm",	ARMOR_CLASS,  ELVEN_HELM,     HELM_OF_TELEPATHY },
	{ "gloves",	ARMOR_CLASS,  GLOVES, GAUNTLETS_OF_DEXTERITY },
	{ "gauntlets",	ARMOR_CLASS,  GLOVES, GAUNTLETS_OF_DEXTERITY },
	{ "boots",	ARMOR_CLASS,  LOW_BOOTS,      LEVITATION_BOOTS },
	{ "shoes",	ARMOR_CLASS,  LOW_BOOTS,      SHOES },
	{ "cloak",	ARMOR_CLASS,  MUMMY_WRAPPING, CLOAK_OF_DISPLACEMENT },
#ifdef TOURIST
	{ "shirt",	ARMOR_CLASS,  HAWAIIAN_SHIRT, T_SHIRT },
#endif
	{ "dragon scales",
			ARMOR_CLASS,  GRAY_DRAGON_SCALES, YELLOW_DRAGON_SCALES },
	{ "dragon scale mail",
			ARMOR_CLASS,  GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL },
	{ "sword",	WEAPON_CLASS, SHORT_SWORD,    KATANA },
#ifdef WIZARD
	{ "venom",	VENOM_CLASS,  BLINDING_VENOM, ACID_VENOM },
#endif
	{ "gray stone",	GEM_CLASS,    LUCKSTONE,      FLINT },
	{ "grey stone",	GEM_CLASS,    LUCKSTONE,      FLINT },
};

#define BSTRCMP(base,ptr,string) ((ptr) < base || strcmp((ptr),string))
#define BSTRCMPI(base,ptr,string) ((ptr) < base || strcmpi((ptr),string))
#define BSTRNCMP(base,ptr,string,num) ((ptr)<base || strncmp((ptr),string,num))
#define BSTRNCMPI(base,ptr,string,num) ((ptr)<base||strncmpi((ptr),string,num))

/*
 * Singularize a string the user typed in; this helps reduce the complexity
 * of readobjnam, and is also used in pager.c to singularize the string
 * for which help is sought.
 */
char *
makesingular(oldstr)
const char *oldstr;
{
	register char *p, *bp;
	char *str = nextobuf();

	if (!oldstr || !*oldstr) {
		impossible("singular of null?");
		str[0] = 0;
		return str;
	}
	Strcpy(str, oldstr);
	bp = str;

	while (*bp == ' ') bp++;
	/* find "cloves of garlic", "worthless pieces of blue glass" */
	if ((p = strstri(bp, "s of ")) != 0) {
	    /* but don't singularize "gauntlets", "boots", "Eyes of the.." */
	    if (BSTRNCMPI(bp, p-3, "Eye", 3) &&
		BSTRNCMP(bp, p-4, "boot", 4) &&
		BSTRNCMP(bp, p-8, "gauntlet", 8))
		while ((*p = *(p+1)) != 0) p++;
	    return bp;
	}

	/* remove -s or -es (boxes) or -ies (rubies) */
	p = eos(bp);
	if (p >= bp+1 && p[-1] == 's') {
		if (p >= bp+2 && p[-2] == 'e') {
			if (p >= bp+3 && p[-3] == 'i') {
				if(!BSTRCMP(bp, p-7, "cookies") ||
				   !BSTRCMP(bp, p-4, "pies"))
					goto mins;
				Strcpy(p-3, "y");
				return bp;
			}

			/* note: cloves / knives from clove / knife */
			if(!BSTRCMP(bp, p-6, "knives")) {
				Strcpy(p-3, "fe");
				return bp;
			}
			if(!BSTRCMP(bp, p-6, "staves")) {
				Strcpy(p-3, "ff");
				return bp;
			}
			if (!BSTRCMPI(bp, p-6, "leaves")) {
				Strcpy(p-3, "f");
				return bp;
			}
			if (!BSTRCMP(bp, p-8, "vortices")) {
				Strcpy(p-4, "ex");
				return bp;
			}

			/* note: nurses, axes but boxes */
			if (!BSTRCMP(bp, p-5, "boxes") ||
			    !BSTRCMP(bp, p-4, "ches")) {
				p[-2] = '\0';
				return bp;
			}

			if (!BSTRCMP(bp, p-6, "gloves") ||
			    !BSTRCMP(bp, p-6, "lenses") ||
			    !BSTRCMP(bp, p-5, "shoes") ||
			    !BSTRCMP(bp, p-6, "scales"))
				return bp;

		} else if (!BSTRCMP(bp, p-5, "boots") ||
			   !BSTRCMP(bp, p-9, "gauntlets") ||
			   !BSTRCMP(bp, p-6, "tricks") ||
			   !BSTRCMP(bp, p-9, "paralysis") ||
			   !BSTRCMP(bp, p-5, "glass") ||
			   !BSTRCMP(bp, p-4, "ness") ||
			   !BSTRCMP(bp, p-14, "shape changers") ||
			   !BSTRCMP(bp, p-15, "detect monsters") ||
			   !BSTRCMPI(bp, p-11, "Aesculapius") || /* staff */
			   !BSTRCMP(bp, p-10, "eucalyptus") ||
#ifdef WIZARD
			   !BSTRCMP(bp, p-9, "iron bars") ||
#endif
			   !BSTRCMP(bp, p-5, "aklys") ||
			   !BSTRCMP(bp, p-6, "fungus"))
				return bp;
	mins:
		p[-1] = '\0';

	} else {

		if(!BSTRCMP(bp, p-5, "teeth")) {
			Strcpy(p-5, "tooth");
			return bp;
		}

		if (!BSTRCMP(bp, p-5, "fungi")) {
			Strcpy(p-5, "fungus");
			return bp;
		}

		/* here we cannot find the plural suffix */
	}
	return bp;
}

/* compare user string against object name string using fuzzy matching */
static boolean
wishymatch(u_str, o_str, retry_inverted)
const char *u_str;	/* from user, so might be variant spelling */
const char *o_str;	/* from objects[], so is in canonical form */
boolean retry_inverted;	/* optional extra "of" handling */
{
	/* special case: wizards can wish for traps.  The object is "beartrap"
	 * and the trap is "bear trap", so to let wizards wish for both we
	 * must not fuzzymatch.
	 */
#ifdef WIZARD
	if (wizard && !strcmp(o_str, "beartrap"))
	    return !strncmpi(o_str, u_str, 8);
#endif

	/* ignore spaces & hyphens and upper/lower case when comparing */
	if (fuzzymatch(u_str, o_str, " -", TRUE)) return TRUE;

	if (retry_inverted) {
	    const char *u_of, *o_of;
	    char *p, buf[BUFSZ];

	    /* when just one of the strings is in the form "foo of bar",
	       convert it into "bar foo" and perform another comparison */
	    u_of = strstri(u_str, " of ");
	    o_of = strstri(o_str, " of ");
	    if (u_of && !o_of) {
		Strcpy(buf, u_of + 4);
		p = eos(strcat(buf, " "));
		while (u_str < u_of) *p++ = *u_str++;
		*p = '\0';
		return fuzzymatch(buf, o_str, " -", TRUE);
	    } else if (o_of && !u_of) {
		Strcpy(buf, o_of + 4);
		p = eos(strcat(buf, " "));
		while (o_str < o_of) *p++ = *o_str++;
		*p = '\0';
		return fuzzymatch(u_str, buf, " -", TRUE);
	    }
	}

	/* [note: if something like "elven speed boots" ever gets added, these
	   special cases should be changed to call wishymatch() recursively in
	   order to get the "of" inversion handling] */
	if (!strncmp(o_str, "dwarvish ", 9)) {
	    if (!strncmpi(u_str, "dwarven ", 8))
		return fuzzymatch(u_str + 8, o_str + 9, " -", TRUE);
	} else if (!strncmp(o_str, "elven ", 6)) {
	    if (!strncmpi(u_str, "elvish ", 7))
		return fuzzymatch(u_str + 7, o_str + 6, " -", TRUE);
	    else if (!strncmpi(u_str, "elfin ", 6))
		return fuzzymatch(u_str + 6, o_str + 6, " -", TRUE);
	} else if (!strcmp(o_str, "aluminum")) {
		/* this special case doesn't really fit anywhere else... */
		/* (note that " wand" will have been stripped off by now) */
	    if (!strcmpi(u_str, "aluminium"))
		return fuzzymatch(u_str + 9, o_str + 8, " -", TRUE);
	}

	return FALSE;
}

/* alternate spellings; if the difference is only the presence or
   absence of spaces and/or hyphens (such as "pickaxe" vs "pick axe"
   vs "pick-axe") then there is no need for inclusion in this list;
   likewise for ``"of" inversions'' ("boots of speed" vs "speed boots") */
struct alt_spellings {
	const char *sp;
	int ob;
} spellings[] = {
	{ "pickax", PICK_AXE },
	{ "whip", BULLWHIP },
	{ "sabre", SABER },
	{ "smooth shield", SHIELD_OF_REFLECTION },
	{ "grey dragon scale mail", GRAY_DRAGON_SCALE_MAIL },
	{ "grey dragon scales", GRAY_DRAGON_SCALES },
	{ "enchant armour", SCR_ENCHANT_ARMOR },
	{ "destroy armour", SCR_DESTROY_ARMOR },
	{ "scroll of enchant armour", SCR_ENCHANT_ARMOR },
	{ "scroll of destroy armour", SCR_DESTROY_ARMOR },
	{ "armour", ARMOR },
	{ "studded armour", STUDDED_ARMOR },
	{ "iron ball", HEAVY_IRON_BALL },
	{ "lantern", BRASS_LANTERN },
	{ "mattock", DWARVISH_MATTOCK },
	{ "amulet of poison resistance", AMULET_VERSUS_POISON },
	{ "stone", ROCK },
#ifdef TOURIST
	{ "camera", EXPENSIVE_CAMERA },
	{ "tee shirt", T_SHIRT },
#endif
	{ "can", TIN },
	{ "can opener", TIN_OPENER },
	{ "kelp", KELP_FROND },
	{ "eucalyptus", EUCALYPTUS_LEAF },
	{ "grapple", GRAPPLING_HOOK },
	{ "box", LARGE_BOX }, /* "large iron box" */
	{ (const char *)0, 0 },
};

/*
 * Return something wished for.  Specifying a null pointer for
 * the user request string results in a random object.  Otherwise,
 * if asking explicitly for "nothing" (or "nil") return no_wish;
 * if not an object return &zeroobj; if an error (no matching object),
 * return null.
 * If from_user is false, we're reading from the wizkit, nothing was typed in.
 */
struct obj *
readobjnam(bp, no_wish, from_user)
register char *bp;
struct obj *no_wish;
boolean from_user;
{
	register char *p;
	register int i;
	register struct obj *otmp;
	boolean resetmat = TRUE;
	int cnt, spe, spesgn, typ, very, rechrg;
	int blessed, uncursed, iscursed, ispoisoned, isgreased;
	int magical;
	int eroded, eroded2, erodeproof;
#ifdef INVISIBLE_OBJECTS
	int isinvisible, isvisible;
#endif
	int halfeaten, mntmp, contents;
	int islit, unlabeled, ishistoric, isdiluted;
	struct fruit *f;
	int ftype = current_fruit;
	char fruitbuf[BUFSZ];
	/* Fruits may not mess up the ability to wish for real objects (since
	 * you can leave a fruit in a bones file and it will be added to
	 * another person's game), so they must be checked for last, after
	 * stripping all the possible prefixes and seeing if there's a real
	 * name in there.  So we have to save the full original name.  However,
	 * it's still possible to do things like "uncursed burnt Alaska",
	 * or worse yet, "2 burned 5 course meals", so we need to loop to
	 * strip off the prefixes again, this time stripping only the ones
	 * possible on food.
	 * We could get even more detailed so as to allow food names with
	 * prefixes that _are_ possible on food, so you could wish for
	 * "2 3 alarm chilis".  Currently this isn't allowed; options.c
	 * automatically sticks 'candied' in front of such names.
	 */

	char oclass;
	char *un, *dn, *actualn;
	const char *name=0;

	long objprops = 0,
	     objpropcount = 0;

	int objmaterial = 0;

	cnt = spe = spesgn = typ = very = rechrg =
		blessed = uncursed = iscursed = magical =
#ifdef INVISIBLE_OBJECTS
		isinvisible = isvisible = 
#endif
		ispoisoned = isgreased = eroded = eroded2 = erodeproof =
		halfeaten = islit = unlabeled = ishistoric = isdiluted = 0;
	mntmp = NON_PM;
#define UNDEFINED 0
#define EMPTY 1
#define SPINACH 2
	contents = UNDEFINED;
	oclass = 0;
	actualn = dn = un = 0;

	if (!bp) goto any;
	/* first, remove extra whitespace they may have typed */
	(void)mungspaces(bp);
	/* allow wishing for "nothing" to preserve wishless conduct...
	   [now requires "wand of nothing" if that's what was really wanted] */
	if (!strcmpi(bp, "nothing") || !strcmpi(bp, "nil") ||
	    !strcmpi(bp, "none")) return no_wish;
	/* save the [nearly] unmodified choice string */
	Strcpy(fruitbuf, bp);

	for(;;) {
		register int l;

		if (!bp || !*bp) {
			if (objmaterial == GOLD) goto gold;

			goto any;
		}
		if (!strncmpi(bp, "an ", l=3) ||
		    !strncmpi(bp, "a ", l=2)) {
			cnt = 1;
		} else if (!strncmpi(bp, "the ", l=4)) {
			;	/* just increment `bp' by `l' below */
		} else if ((!strncmpi(bp, "large ", l=6) &&
		             strncmpi(bp, "large box", 9))) {
			;	/* "large iron box" */
		} else if (!cnt && digit(*bp) && strcmp(bp, "0")) {
			cnt = atoi(bp);
			while(digit(*bp)) bp++;
			while(*bp == ' ') bp++;
			l = 0;
		} else if (*bp == '+' || *bp == '-') {
			spesgn = (*bp++ == '+') ? 1 : -1;
			spe = atoi(bp);
			while(digit(*bp)) bp++;
			while(*bp == ' ') bp++;
			l = 0;
		} else if (!strncmpi(bp, "blessed ", l=8) ||
			   !strncmpi(bp, "holy ", l=5)) {
			blessed = 1;
		} else if (!strncmpi(bp, "cursed ", l=7) ||
			   !strncmpi(bp, "unholy ", l=7)) {
			iscursed = 1;
		} else if (!strncmpi(bp, "uncursed ", l=9)) {
			uncursed = 1;
		} else if (!strncmpi(bp, "magical ", l=8)) {
			magical = 1;
#ifdef INVISIBLE_OBJECTS
		} else if (!strncmpi(bp, "invisible ", l=10)) {
			isinvisible = 1;
		} else if (!strncmpi(bp, "visible ", l=8)) {
			isvisible = 1;
#endif
		} else if (!strncmpi(bp, "rustproof ", l=10) ||
			   !strncmpi(bp, "erodeproof ", l=11) ||
			   !strncmpi(bp, "corrodeproof ", l=13) ||
			   !strncmpi(bp, "fixed ", l=6) ||
			   !strncmpi(bp, "fireproof ", l=10) ||
			   !strncmpi(bp, "rotproof ", l=9)) {
			erodeproof = 1;
		} else if (!strncmpi(bp,"lit ", l=4) ||
			   !strncmpi(bp,"burning ", l=8)) {
			islit = 1;
		} else if (!strncmpi(bp,"unlit ", l=6) ||
			   !strncmpi(bp,"extinguished ", l=13)) {
			islit = 0;
		/* "unlabeled" and "blank" are synonymous */
		} else if (!strncmpi(bp,"unlabeled ", l=10) ||
			   !strncmpi(bp,"unlabelled ", l=11) ||
			   !strncmpi(bp,"blank ", l=6)) {
			unlabeled = 1;
		} else if(!strncmpi(bp, "poisoned ",l=9)
#ifdef WIZARD
			  || (wizard && !strncmpi(bp, "trapped ",l=8))
#endif
			  ) {
			ispoisoned=1;
		} else if(!strncmpi(bp, "greased ",l=8)) {
			isgreased=1;
		} else if (!strncmpi(bp, "very ", l=5)) {
			/* very rusted very heavy iron ball */
			very = 1;
		} else if (!strncmpi(bp, "thoroughly ", l=11)) {
			very = 2;
		} else if (!strncmpi(bp, "rusty ", l=6) ||
			   !strncmpi(bp, "rusted ", l=7) ||
			   !strncmpi(bp, "burnt ", l=6) ||
			   !strncmpi(bp, "burned ", l=7)) {
			eroded = 1 + very;
			very = 0;
		} else if (!strncmpi(bp, "corroded ", l=9) ||
			   !strncmpi(bp, "rotted ", l=7)) {
			eroded2 = 1 + very;
			very = 0;
		} else if (!strncmpi(bp, "partly eaten ", l=13)) {
			halfeaten = 1;
		} else if (!strncmpi(bp, "historic ", l=9)) {
			ishistoric = 1;
		} else if (!strncmpi(bp, "diluted ", l=8)) {
			isdiluted = 1;
		} else if(!strncmpi(bp, "empty ", l=6)) {
			contents = EMPTY;
		} else if(!strncmpi(bp, "vorpal ", l=7)) {
		    if (!objpropcount
#ifdef WIZARD
                        || wizard
#endif
		       )
		        objprops |= ITEM_VORPAL;
		    objpropcount++;
		} else if(!strncmpi(bp, "thirsty ", l=8)) {
		    if (!objpropcount
#ifdef WIZARD
                        || wizard
#endif
		       )
		        objprops |= ITEM_DRLI;
		    objpropcount++;
		} else if(!strncmpi(bp, "oilskin ", l=8) &&
		         !!strncmpi(bp, "oilskin cloak", 13) &&
			 !!strncmpi(bp, "oilskin sack", 12)) {
		    if (!objpropcount
#ifdef WIZARD
                        || wizard
#endif
		       )
		        objprops |= ITEM_OILSKIN;
		    objpropcount++;
		} else {
		    for (i = 1; i < MAX_MAT; i++)
		        if ((l = strlen(materialnm[i])) > 0 &&
			    !strncmpi(bp, materialnm[i], l) &&
			    !strstri(bp, " spellbook") &&
			    !strstri(bp, " ring") &&
			    !strstri(bp, " potion") &&
			    !strstri(bp, " wand") &&
			    !strstri(bp, " pieces") && /* gold pieces */
			    !strstri(bp, " dragon scale mail") &&
#ifdef WIZARD
			    !strstri(bp, "iron bars") &&
#endif
			    !strstri(bp, "Platinum Yendorian Express Card"))
			{
			    objmaterial = i;
			    l++;
			    break;
			}
		    if (i == MAX_MAT)
                        break;
		}
		bp += l;
	}
	if(!cnt) cnt = 1;		/* %% what with "gems" etc. ? */
	if (strlen(bp) > 1) {
	    if ((p = rindex(bp, '(')) != 0) {
		if (p > bp && p[-1] == ' ') p[-1] = 0;
		else *p = 0;
		p++;
		if (!strcmpi(p, "lit)")) {
		    islit = 1;
		} else {
		    spe = atoi(p);
		    while (digit(*p)) p++;
		    if (*p == ':') {
			p++;
			rechrg = spe;
			spe = atoi(p);
			while (digit(*p)) p++;
		    }
		    if (*p != ')') {
			spe = rechrg = 0;
		    } else {
			spesgn = 1;
			p++;
			if (*p) Strcat(bp, p);
		    }
		}
	    }
	}
/*
   otmp->spe is type schar; so we don't want spe to be any bigger or smaller.
   also, spe should always be positive  -- some cheaters may try to confuse
   atoi()
*/
	if (spe < 0) {
		spesgn = -1;	/* cheaters get what they deserve */
		spe = abs(spe);
	}
	if (spe > SCHAR_LIM)
		spe = SCHAR_LIM;
	if (rechrg < 0 || rechrg > 7) rechrg = 7;	/* recharge_limit */

	/* now we have the actual name, as delivered by xname, say
		green potions called whisky
		scrolls labeled "QWERTY"
		egg
		fortune cookies
		very heavy iron ball named hoei
		wand of wishing
		elven cloak
	*/
	if ((p = strstri(bp, " named ")) != 0) {
		*p = 0;
		name = p+7;
	}
	if ((p = strstri(bp, " called ")) != 0) {
		*p = 0;
		un = p+8;
		/* "helmet called telepathy" is not "helmet" (a specific type)
		 * "shield called reflection" is not "shield" (a general type)
		 */
		for(i = 0; i < SIZE(o_ranges); i++)
		    if(!strcmpi(bp, o_ranges[i].name)) {
			oclass = o_ranges[i].oclass;
			goto srch;
		    }
	}
	if ((p = strstri(bp, " labeled ")) != 0) {
		*p = 0;
		dn = p+9;
	} else if ((p = strstri(bp, " labelled ")) != 0) {
		*p = 0;
		dn = p+10;
	}
	if ((p = strstri(bp, " of spinach")) != 0) {
		*p = 0;
		contents = SPINACH;
	}

	/*
	Skip over "pair of ", "pairs of", "set of" and "sets of".

	Accept "3 pair of boots" as well as "3 pairs of boots". It is valid
	English either way.  See makeplural() for more on pair/pairs.

	We should only double count if the object in question is not
	refered to as a "pair of".  E.g. We should double if the player
	types "pair of spears", but not if the player types "pair of
	lenses".  Luckily (?) all objects that are refered to as pairs
	-- boots, gloves, and lenses -- are also not mergable, so cnt is
	ignored anyway.
	*/
	if(!strncmpi(bp, "pair of ",8)) {
		bp += 8;
		cnt *= 2;
	} else if(cnt > 1 && !strncmpi(bp, "pairs of ",9)) {
		bp += 9;
		cnt *= 2;
	} else if (!strncmpi(bp, "set of ",7)) {
		bp += 7;
	} else if (!strncmpi(bp, "sets of ",8)) {
		bp += 8;
	}

	/*
	 * Find corpse type using "of" (figurine of an orc, tin of orc meat)
	 * Don't check if it's a wand or spellbook.
	 * (avoid "wand/finger of death" confusion).
	 */
	if (!strstri(bp, "wand ")
	 && !strstri(bp, "spellbook ")
	 && !strstri(bp, "scroll ")
	 && !strstri(bp, "finger ")) {

	    int l = 0;
	    int of = 4;

	    char *tmpp;

	    p = bp;

	    while (p != 0) {
	        tmpp = strstri(p, " of ");
		if (tmpp) {
		  if ((tmpp - 6 >= bp && !strncmpi(tmpp - 6, "amulet", 6)) ||
		      (tmpp - 5 >= bp && !strncmpi(tmpp - 5, "cloak", 5)) ||
		      (tmpp - 9 >= bp && !strncmpi(tmpp - 9, "gauntlets", 9)) ||
		      (tmpp - 4 >= bp && !strncmpi(tmpp - 4, "helm", 4)) ||
		      (tmpp - 6 >= bp && !strncmpi(tmpp - 6, "potion", 6)) ||
		      (tmpp - 4 >= bp && !strncmpi(tmpp - 4, "ring", 4)) 
		      )
		  {
		    p = tmpp + 4;
		    continue;
		  }
		  of = 4;
		  p = tmpp;
		} else {
	            tmpp = strstri(p, " and ");
		    if (tmpp) of = 5, p = tmpp;
		    else {
		        p = 0;
		        break;
		    }
		}

	        l = 0;


	        if ((mntmp = name_to_mon(p+of)) >= LOW_PM)
		{
		    *p = 0;
		    p = 0;
		}
		else if (!strncmpi((p+of), "fire", l=4) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_FIRE;		    
		    objpropcount++;
		}
		else if ((!strncmpi((p+of), "frost", l=5) ||
		     !strncmpi((p+of), "cold", l=4) ))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_FROST;
		    objpropcount++;
		}
		else if ((!strncmpi((p+of), "detonations", l=11) ||
		          !strncmpi((p+of), "detonation", l=10) ))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_DETONATIONS;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "drain", l=5) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_DRLI;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "reflection", l=10) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_REFLECTION; 
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "speed", l=5) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_SPEED;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "power", l=5) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_POWER;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "dexterity", l=9) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_DEXTERITY;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "brilliance", l=10) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_BRILLIANCE;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "telepathy", l=9) ||
		    !strncmpi((p+of), "ESP", l=3))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_ESP;   
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "displacement", l=12) )
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_DISPLACEMENT;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "searching", l=9))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_SEARCHING;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "warning", l=7))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_WARNING;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "stealth", l=7))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_STEALTH;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "fumbling", l=8))
		{
		/* uh, if they really want it... */
/*
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    ) */
                    objprops |= ITEM_FUMBLING;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "clairvoyance", l=12))
		{
		    if (!objpropcount
#ifdef WIZARD
                    || wizard
#endif
		    )
                    objprops |= ITEM_CLAIRVOYANCE;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "hunger", l=8))
		{
                    objprops |= ITEM_HUNGER;
		    objpropcount++;
		}
		else if (!strncmpi((p+of), "aggravation", l=8))
		{
                    objprops |= ITEM_AGGRAVATE;
		    objpropcount++;
		} else l = 0;

		if (l > 0)
		*p = 0;

                if (p)
		    p += (of + l);
	}
	
	}


	/* Find corpse type w/o "of" (red dragon scale mail, yeti corpse) */
	if (strncmpi(bp, "samurai sword", 13)) /* not the "samurai" monster! */
	if (strncmpi(bp, "wizard lock", 11)) /* not the "wizard" monster! */
	if (strncmpi(bp, "ninja-to", 8)) /* not the "ninja" rank */
	if (strncmpi(bp, "master key", 10)) /* not the "master" rank */
	if (strncmpi(bp, "magenta", 7)) /* not the "mage" rank */
	if (strncmpi(bp, "mummy wrapping", 14)) /* not the "mummy" monster */
	if (mntmp < LOW_PM && strlen(bp) > 2) {
	    if ((mntmp = name_to_mon(bp)) >= LOW_PM) {
		int mntmptoo, mntmplen;	/* double check for rank title */
		char *obp = bp;
		mntmptoo = title_to_mon(bp, (int *)0, &mntmplen);
		bp += mntmp != mntmptoo ? (int)strlen(mons[mntmp].mname) : mntmplen;
		if (*bp == ' ') bp++;
		else if (!strncmpi(bp, "s ", 2)) bp += 2;
		else if (!strncmpi(bp, "es ", 3)) bp += 3;
		else if (!*bp && !actualn && !dn && !un && !oclass) {
		    /* no referent; they don't really mean a monster type */
		    bp = obp;
		    mntmp = NON_PM;
		}
	    }
	}

	/* first change to singular if necessary */
	if (*bp) {
		char *sng = makesingular(bp);
		if (strcmp(bp, sng)) {
			if (cnt == 1) cnt = 2;
			Strcpy(bp, sng);
		}
	}

	/* Alternate spellings (pick-ax, silver sabre, &c) */
    {
	struct alt_spellings *as = spellings;

	while (as->sp) {
		if (fuzzymatch(bp, as->sp, " -", TRUE)) {
			typ = as->ob;
			goto typfnd;
		}
		as++;
	}
	/* can't use spellings list for this one due to shuffling */
	if (!strncmpi(bp, "grey spell", 10))
		*(bp + 2) = 'a';
    }

	/* dragon scales - assumes order of dragons */
	if(!strcmpi(bp, "scales") &&
			mntmp >= PM_GRAY_DRAGON && mntmp <= PM_YELLOW_DRAGON) {
		typ = GRAY_DRAGON_SCALES + mntmp - PM_GRAY_DRAGON;
		mntmp = NON_PM;	/* no monster */
		goto typfnd;
	}

	p = eos(bp);
	if(!BSTRCMPI(bp, p-10, "holy water")) {
		typ = POT_WATER;
		if ((p-bp) >= 12 && *(p-12) == 'u')
			iscursed = 1; /* unholy water */
		else blessed = 1;
		goto typfnd;
	}
	if(unlabeled && !BSTRCMPI(bp, p-6, "scroll")) {
		typ = SCR_BLANK_PAPER;
		goto typfnd;
	}
	if(unlabeled && !BSTRCMPI(bp, p-9, "spellbook")) {
		typ = SPE_BLANK_PAPER;
		goto typfnd;
	}
	/*
	 * NOTE: Gold pieces are handled as objects nowadays, and therefore
	 * this section should probably be reconsidered as well as the entire
	 * gold/money concept.  Maybe we want to add other monetary units as
	 * well in the future. (TH)
	 */
	if(!BSTRCMPI(bp, p-10, "gold piece") || !BSTRCMPI(bp, p-7, "zorkmid") ||
	   !strcmpi(bp, "gold") || !strcmpi(bp, "money") ||
	   !strcmpi(bp, "coin") || *bp == GOLD_SYM) {
gold:
			if (cnt > 5000
#ifdef WIZARD
					&& !wizard
#endif
						) cnt=5000;
		if (cnt < 1) cnt=1;
#ifndef GOLDOBJ
		if (from_user)
		    pline("%d gold piece%s.", cnt, plur(cnt));
		u.ugold += cnt;
		flags.botl=1;
		return (&zeroobj);
#else
                otmp = mksobj(GOLD_PIECE, FALSE, FALSE);
		otmp->quan = cnt;
                otmp->owt = weight(otmp);
		flags.botl=1;
		return (otmp);
#endif
	}
	if (strlen(bp) == 1 &&
	   (i = def_char_to_objclass(*bp)) < MAXOCLASSES && i > ILLOBJ_CLASS
#ifdef WIZARD
	    && (wizard || i != VENOM_CLASS)
#else
	    && i != VENOM_CLASS
#endif
	    ) {
		oclass = i;
		goto any;
	}

	/* Search for class names: XXXXX potion, scroll of XXXXX.  Avoid */
	/* false hits on, e.g., rings for "ring mail". */
	if(strncmpi(bp, "enchant ", 8) &&
	   strncmpi(bp, "destroy ", 8) &&
	   strncmpi(bp, "food detection", 14) &&
	   strncmpi(bp, "ring mail", 9) &&
	   strncmpi(bp, "studded arm", 11) &&
	   strncmpi(bp, "tooled horn", 11) &&
	   strncmpi(bp, "food ration", 11) &&
	   strncmpi(bp, "meat ring", 9)
	)
	for (i = 0; i < (int)(sizeof wrpsym); i++) {
		register int j = strlen(wrp[i]);
		if(!strncmpi(bp, wrp[i], j)){
			oclass = wrpsym[i];
			if(oclass != AMULET_CLASS) {
			    bp += j;
			    if(!strncmpi(bp, " of ", 4)) actualn = bp+4;
			    /* else if(*bp) ?? */
			} else
			    actualn = bp;
			if (oclass != ARMOR_CLASS || !objmaterial)
			    goto srch; /*might wish for "leather armor"*/
			else {
			    oclass = 0;
			    typ = ARMOR; /* studded checked above */
			    goto typfnd;
			}
		}
		if(!BSTRCMPI(bp, p-j, wrp[i])){
			oclass = wrpsym[i];
			p -= j;
			*p = 0;
			if(p > bp && p[-1] == ' ') p[-1] = 0;
			actualn = dn = bp;
			if (oclass != ARMOR_CLASS || !objmaterial)
			    goto srch;
			else {
			    oclass = 0;
			    typ = ARMOR; /* studded checked above */
			    goto typfnd;
			}
		}
	}

	/* "grey stone" check must be before general "stone" */
	for (i = 0; i < SIZE(o_ranges); i++)
	    if(!strcmpi(bp, o_ranges[i].name)) {
		typ = rnd_class(o_ranges[i].f_o_range, o_ranges[i].l_o_range);
		goto typfnd;
	    }

	if (!BSTRCMPI(bp, p-6, " stone")) {
		p[-6] = 0;
		oclass = GEM_CLASS;
		dn = actualn = bp;
		goto srch;
	} else if (!strcmpi(bp, "looking glass")) {
		;	/* avoid false hit on "* glass" */
	} else if (!BSTRCMPI(bp, p-6, " glass") || !strcmpi(bp, "glass")) {
		register char *g = bp;
		if (strstri(g, "broken")) return (struct obj *)0;
		if (!strncmpi(g, "worthless ", 10)) g += 10;
		if (!strncmpi(g, "piece of ", 9)) g += 9;
		if (!strncmpi(g, "colored ", 8)) g += 8;
		else if (!strncmpi(g, "coloured ", 9)) g += 9;
		if (!strcmpi(g, "glass")) {	/* choose random color */
			/* 9 different kinds */
			typ = LAST_GEM + rnd(9);
			if (objects[typ].oc_class == GEM_CLASS) goto typfnd;
			else typ = 0;	/* somebody changed objects[]? punt */
		} else {		/* try to construct canonical form */
			char tbuf[BUFSZ];
			Strcpy(tbuf, "worthless piece of ");
			Strcat(tbuf, g);  /* assume it starts with the color */
			Strcpy(bp, tbuf);
		}
	}

	actualn = bp;
	if (!dn) dn = actualn; /* ex. "skull cap" */
srch:
	/* check real names of gems first */
	if(!oclass && actualn) {
	    for(i = bases[GEM_CLASS]; i <= LAST_GEM; i++) {
		register const char *zn;

		if((zn = OBJ_NAME(objects[i])) && !strcmpi(actualn, zn)) {
		    typ = i;
		    goto typfnd;
		}
	    }
	}
	i = oclass ? bases[(int)oclass] : 1;
	while(i < NUM_OBJECTS && (!oclass || objects[i].oc_class == oclass)){
		register const char *zn;

		if (actualn && (zn = OBJ_NAME(objects[i])) != 0 &&
			    wishymatch(actualn, zn, TRUE)) {
			typ = i;
			goto typfnd;
		}
		if (dn && (zn = OBJ_DESCR(objects[i])) != 0 &&
			    wishymatch(dn, zn, FALSE)) {
			/* don't match extra descriptions (w/o real name) */
			if (!OBJ_NAME(objects[i])) return (struct obj *)0;
			typ = i;
			goto typfnd;
		}
		if (un && (zn = objects[i].oc_uname) != 0 &&
			    wishymatch(un, zn, FALSE)) {
			typ = i;
			goto typfnd;
		}
		i++;
	}
	if (actualn) {
		struct Jitem *j = Japanese_items;
		while(j->item) {
			if (actualn && !strcmpi(actualn, j->name)) {
				typ = j->item;
				goto typfnd;
			}
			j++;
		}
	}
	if (!strcmpi(bp, "spinach")) {
		contents = SPINACH;
		typ = TIN;
		goto typfnd;
	}
	/* Note: not strncmpi.  2 fruits, one capital, one not, are possible. */
	{
	    char *fp;
	    int l, cntf;
	    int blessedf, iscursedf, uncursedf, halfeatenf;

	    blessedf = iscursedf = uncursedf = halfeatenf = 0;
	    cntf = 0;

	    fp = fruitbuf;
	    for(;;) {
		if (!fp || !*fp) break;
		if (!strncmpi(fp, "an ", l=3) ||
		    !strncmpi(fp, "a ", l=2)) {
			cntf = 1;
		} else if (!cntf && digit(*fp)) {
			cntf = atoi(fp);
			while(digit(*fp)) fp++;
			while(*fp == ' ') fp++;
			l = 0;
		} else if (!strncmpi(fp, "blessed ", l=8)) {
			blessedf = 1;
		} else if (!strncmpi(fp, "cursed ", l=7)) {
			iscursedf = 1;
		} else if (!strncmpi(fp, "uncursed ", l=9)) {
			uncursedf = 1;
		} else if (!strncmpi(fp, "partly eaten ", l=13)) {
			halfeatenf = 1;
		} else break;
		fp += l;
	    }

	    for(f=ffruit; f; f = f->nextf) {
		char *f1 = f->fname, *f2 = makeplural(f->fname);

		if(!strncmp(fp, f1, strlen(f1)) ||
					!strncmp(fp, f2, strlen(f2))) {
			typ = SLIME_MOLD;
			blessed = blessedf;
			iscursed = iscursedf;
			uncursed = uncursedf;
			halfeaten = halfeatenf;
			cnt = cntf;
			ftype = f->fid;
			goto typfnd;
		}
	    }
	}

	if(!oclass && actualn) {
	    short objtyp;

	    /* Perhaps it's an artifact specified by name, not type */
	    name = artifact_name(actualn, &objtyp);
	    if(name) {
		typ = objtyp;
		goto typfnd;
	    }
	}
#ifdef WIZARD
	/* Let wizards wish for traps --KAA */
	/* must come after objects check so wizards can still wish for
	 * trap objects like beartraps
	 */
	if (wizard && from_user) {
		int trap;

		for (trap = NO_TRAP+1; trap < TRAPNUM; trap++) {
			const char *tname;

			tname = defsyms[trap_to_defsym(trap)].explanation;
			if (!strncmpi(tname, bp, strlen(tname))) {
				/* avoid stupid mistakes */
				if((trap == TRAPDOOR || trap == HOLE)
				      && !Can_fall_thru(&u.uz)) trap = ROCKTRAP;
				(void) maketrap(u.ux, u.uy, trap);
				pline("%s.", An(tname));
				return(&zeroobj);
			}
		}
		/* or some other dungeon features -dlc */
		p = eos(bp);
		if(!BSTRCMP(bp, p-8, "fountain")) {
			levl[u.ux][u.uy].typ = FOUNTAIN;
			level.flags.nfountains++;
			if(!strncmpi(bp, "magic ", 6))
				levl[u.ux][u.uy].blessedftn = 1;
			pline("A %sfountain.",
			      levl[u.ux][u.uy].blessedftn ? "magic " : "");
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
		if(!BSTRCMP(bp, p-6, "throne")) {
			levl[u.ux][u.uy].typ = THRONE;
			pline("A throne.");
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
# ifdef SINKS
		if(!BSTRCMP(bp, p-4, "sink")) {
			levl[u.ux][u.uy].typ = SINK;
			level.flags.nsinks++;
			pline("A sink.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
# endif
		if(!BSTRCMP(bp, p-4, "pool")) {
			levl[u.ux][u.uy].typ = POOL;
			del_engr_at(u.ux, u.uy);
			pline("A pool.");
			/* Must manually make kelp! */
			water_damage(NULL, level.objects[u.ux][u.uy], FALSE, TRUE);
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if (!BSTRCMP(bp, p-4, "lava")) {  /* also matches "molten lava" */
			levl[u.ux][u.uy].typ = LAVAPOOL;
			del_engr_at(u.ux, u.uy);
			pline("A pool of molten lava.");
			if (!(Levitation || Flying)) (void) lava_effects();
			newsym(u.ux, u.uy);
			return &zeroobj;
		}

		if(!BSTRCMP(bp, p-5, "altar")) {
		    aligntyp al;

		    levl[u.ux][u.uy].typ = ALTAR;
		    if(!strncmpi(bp, "chaotic ", 8))
			al = A_CHAOTIC;
		    else if(!strncmpi(bp, "neutral ", 8))
			al = A_NEUTRAL;
		    else if(!strncmpi(bp, "lawful ", 7))
			al = A_LAWFUL;
		    else if(!strncmpi(bp, "unaligned ", 10))
			al = A_NONE;
		    else /* -1 - A_CHAOTIC, 0 - A_NEUTRAL, 1 - A_LAWFUL */
			al = (!rn2(6)) ? A_NONE : rn2((int)A_LAWFUL+2) - 1;
		    levl[u.ux][u.uy].altarmask = Align2amask( al );
		    pline("%s altar.", An(align_str(al)));
		    newsym(u.ux, u.uy);
		    return(&zeroobj);
		}

		if(!BSTRCMP(bp, p-5, "grave") || !BSTRCMP(bp, p-9, "headstone")) {
		    make_grave(u.ux, u.uy, (char *) 0);
		    pline("A grave.");
		    newsym(u.ux, u.uy);
		    return(&zeroobj);
		}

		if(!BSTRCMP(bp, p-4, "tree")) {
		    levl[u.ux][u.uy].typ = TREE;
		    pline("A tree.");
		    newsym(u.ux, u.uy);
		    block_point(u.ux, u.uy);
		    return &zeroobj;
		}

		if(!BSTRCMP(bp, p-4, "bars")) {
		    levl[u.ux][u.uy].typ = IRONBARS;
		    pline("Iron bars.");
		    newsym(u.ux, u.uy);
		    return &zeroobj;
		}
	}
#endif
	if (!oclass && objmaterial == GOLD)
	    goto gold; /* probably wished for e.g. "5000 gold" */
	if(!oclass) return((struct obj *)0);
any:
	if(!oclass) oclass = wrpsym[rn2((int)sizeof(wrpsym))];
typfnd:
	if (typ) oclass = objects[typ].oc_class;

	/* check for some objects that are not allowed */
	if (typ && objects[typ].oc_unique) {
#ifdef WIZARD
	    if (wizard)
		;	/* allow unique objects */
	    else
#endif
	    switch (typ) {
		case AMULET_OF_YENDOR:
		    typ = FAKE_AMULET_OF_YENDOR;
		    break;
		case CANDELABRUM_OF_INVOCATION:
		    typ = rnd_class(TALLOW_CANDLE, WAX_CANDLE);
		    break;
		case BELL_OF_OPENING:
		    typ = BELL;
		    break;
		case SPE_BOOK_OF_THE_DEAD:
		    typ = SPE_BLANK_PAPER;
		    break;
	    }
	}

	/* catch any other non-wishable objects */
	if (objects[typ].oc_nowish
#ifdef WIZARD
	    && !wizard
#endif
	    )
	    return((struct obj *)0);

	/* convert magic lamps to regular lamps before lighting them or setting
	   the charges */
	if (typ == MAGIC_LAMP
#ifdef WIZARD
				&& !wizard
#endif
						)
	    typ = OIL_LAMP;

	if(typ) {
		otmp = mksobj(typ, TRUE, FALSE);
	} else {
		otmp = mkobj(oclass, NO_MO_FLAGS);
		if (otmp) typ = otmp->otyp;
	}

	if (islit &&
		(typ == OIL_LAMP || typ == MAGIC_LAMP || typ == BRASS_LANTERN ||
		 Is_candle(otmp) || typ == POT_OIL)) {
	    place_object(otmp, u.ux, u.uy);  /* make it viable light source */
	    begin_burn(otmp, FALSE);
	    obj_extract_self(otmp);	 /* now release it for caller's use */
	}

	if(cnt > 0 && objects[typ].oc_merge && oclass != SPBOOK_CLASS &&
		(cnt < rnd(6) ||
#ifdef WIZARD
		wizard ||
#endif
		 (cnt <= 7 && Is_candle(otmp)) ||
		 (cnt <= 20 &&
		  ((oclass == WEAPON_CLASS && is_ammo(otmp))
				|| typ == ROCK || is_missile(otmp)))))
			otmp->quan = (long) cnt;

#ifdef WIZARD
	if (oclass == VENOM_CLASS) otmp->spe = 1;
#endif

	if (spesgn == 0) spe = otmp->spe;
#ifdef WIZARD
	else if (wizard) /* no alteration to spe */ ;
#endif
	else if (oclass == ARMOR_CLASS || oclass == WEAPON_CLASS ||
		 is_weptool(otmp) ||
			(oclass==RING_CLASS && objects[typ].oc_charged)) {
		if(spe > rnd(5) && spe > otmp->spe) spe = 0;
		if(spe > 2 && Luck < 0) spesgn = -1;
	} else {
		if (oclass == WAND_CLASS) {
			if (spe > 1 && spesgn == -1) spe = 1;
		} else {
			if (spe > 0 && spesgn == -1) spe = 0;
		}
		if (spe > otmp->spe) spe = otmp->spe;
	}

	if (spesgn == -1) spe = -spe;

	/* set otmp->spe.  This may, or may not, use spe... */
	switch (typ) {
		case TIN: if (contents==EMPTY) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 0;
			} else if (contents==SPINACH) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 1;
			}
			break;
		case SLIME_MOLD: otmp->spe = ftype;
			/* Fall through */
		case SKELETON_KEY: case CHEST: case LARGE_BOX:
		case HEAVY_IRON_BALL: case IRON_CHAIN: case STATUE:
			/* otmp->cobj already done in mksobj() */
				break;
#ifdef MAIL
		case SCR_MAIL: otmp->spe = 1; break;
#endif
		case WAN_WISHING:
#ifdef WIZARD
			if (!wizard) {
#endif
				otmp->spe = (rn2(10) ? -1 : 0);
				break;
#ifdef WIZARD
			}
			/* fall through, if wizard */
#endif
		default: otmp->spe = spe;
	}

	/* set otmp->corpsenm or dragon scale [mail] */
	if (mntmp >= LOW_PM) {
		if (mntmp == PM_LONG_WORM_TAIL) mntmp = PM_LONG_WORM;

		switch (typ) {
		case TIN:
			otmp->spe = 0; /* No spinach */
			if (dead_species(mntmp, FALSE)) {
			    otmp->corpsenm = NON_PM;	/* it's empty */
			} else if (!(mons[mntmp].geno & G_UNIQ) &&
				   !(mvitals[mntmp].mvflags & G_NOCORPSE) &&
				   mons[mntmp].cnutrit != 0) {
			    otmp->corpsenm = mntmp;
			}
			break;
		case CORPSE:
			if (!(mons[mntmp].geno & G_UNIQ) &&
				   !(mvitals[mntmp].mvflags & G_NOCORPSE)) {
			    /* beware of random troll or lizard corpse,
			       or of ordinary one being forced to such */
			    if (otmp->timed) obj_stop_timers(otmp);
			    if (mons[mntmp].msound == MS_GUARDIAN)
			    	otmp->corpsenm = genus(mntmp,1);
			    else
				otmp->corpsenm = mntmp;
			    if (!otmp->oerodeproof)
			    	start_corpse_timeout(otmp);
			}
			break;
		case FIGURINE:
			if (!(mons[mntmp].geno & G_UNIQ)
			    && !(mons[mntmp].mflags2 & M2_HUMAN)
#ifdef MAIL
			    && mntmp != PM_MAIL_DAEMON
#endif
							)
				otmp->corpsenm = mntmp;
			break;
		case EGG:
			mntmp = can_be_hatched(mntmp);
			if (mntmp != NON_PM) {
			    otmp->corpsenm = mntmp;
			    if (!dead_species(mntmp, TRUE))
				attach_egg_hatch_timeout(otmp);
			    else
				kill_egg(otmp);
			}
			break;
		case STATUE: otmp->corpsenm = mntmp;
			if (Has_contents(otmp) && verysmall(&mons[mntmp]))
			    delete_contents(otmp);	/* no spellbook */
			otmp->spe = ishistoric ? STATUE_HISTORIC : 0;
			break;
		case SCALE_MAIL:
			/* Dragon mail - depends on the order of objects */
			/*		 & dragons.			 */
			if (mntmp >= PM_GRAY_DRAGON &&
						mntmp <= PM_YELLOW_DRAGON)
			    otmp->otyp = GRAY_DRAGON_SCALE_MAIL +
						    mntmp - PM_GRAY_DRAGON;
			break;
		}
	}

	/* set blessed/cursed -- setting the fields directly is safe
	 * since weight() is called below and addinv() will take care
	 * of luck */
	if (iscursed) {
		curse(otmp);
	} else if (uncursed) {
		otmp->blessed = 0;
		otmp->cursed = (Luck < 0
#ifdef WIZARD
					 && !wizard
#endif
							);
	} else if (blessed) {
		otmp->blessed = (Luck >= 0
#ifdef WIZARD
					 || wizard
#endif
							);
		otmp->cursed = (Luck < 0
#ifdef WIZARD
					 && !wizard
#endif
							);
	} else if (spesgn < 0) {
		curse(otmp);
	}

	otmp->oprops = 0;

	if (magical && (!objpropcount
#ifdef WIZARD
					 || wizard
#endif
                       ))
	{
	    create_oprop(otmp, TRUE);
	}

#ifdef INVISIBLE_OBJECTS
	if (isinvisible && can_be_invisible(otmp)) {
		otmp->oinvis = 1;
	} else if (isvisible) {
		otmp->oinvis = 0;
	}
#endif

	/* set eroded */
	if (is_damageable(otmp) || otmp->otyp == CRYSKNIFE) {
	    if (eroded && (is_flammable(otmp) || is_rustprone(otmp)))
		    otmp->oeroded = eroded;
	    if (eroded2 && (is_corrodeable(otmp) || is_rottable(otmp)))
		    otmp->oeroded2 = eroded2;

	    /* set erodeproof */
	    if (erodeproof && !eroded && !eroded2)
		    otmp->oerodeproof = (Luck >= 0
#ifdef WIZARD
					     || wizard
#endif
					);
	}

	/* set otmp->recharged */
	if (oclass == WAND_CLASS) {
	    /* prevent wishing abuse */
	    if (otmp->otyp == WAN_WISHING
#ifdef WIZARD
		    && !wizard
#endif
		) rechrg = 1;
	    otmp->recharged = (unsigned)rechrg;
	}

	/* set poisoned */
	if (ispoisoned) {
	    if (is_poisonable(otmp))
		otmp->opoisoned = (Luck >= 0);
	    else if (Is_box(otmp) || typ == TIN)
		otmp->otrapped = 1;
	    else if (oclass == FOOD_CLASS)
		/* try to taint by making it as old as possible */
		otmp->age = 1L;
	}

	if (isgreased) otmp->greased = 1;

	if (isdiluted && otmp->oclass == POTION_CLASS &&
			otmp->otyp != POT_WATER)
		otmp->odiluted = 1;

	if (name) {
		const char *aname;
		short objtyp;

		/* an artifact name might need capitalization fixing */
		aname = artifact_name(name, &objtyp);
		if (aname && objtyp == otmp->otyp) name = aname;

		otmp = oname(otmp, name, TRUE);
		if (otmp->oartifact) {
			otmp->quan = 1L;
			u.uconduct.wisharti++;	/* KMH, conduct */
		}
		if (aname &&
		    (!strcmp(aname, "Grayswandir") ||
		     !strcmp(aname, "Werebane"))) {
		    otmp->omaterial = SILVER;
		    resetmat = FALSE;
		}
#ifdef INVISIBLE_OBJECTS
		if (aname && !strcmp(aname, "Magicbane"))
			otmp->oinvis = FALSE;
		else if (otmp->oinvis)
			otmp->opresenceknown = TRUE;
#endif
	}

	/* more wishing abuse: don't allow wishing for certain artifacts */
	/* and make them pay; charge them for the wish anyway! */
	if ((is_quest_artifact(otmp) ||
	     (otmp->oartifact && rn2(nartifact_exist()) > 1))
#ifdef WIZARD
	    && !wizard
#endif
	    ) {
	    artifact_exists(otmp, ONAME(otmp), FALSE, FALSE);
	    obfree(otmp, (struct obj *) 0);
	    otmp = &zeroobj;
	    pline("For a moment, you feel %s in your %s, but it disappears!",
		  something,
		  makeplural(body_part(HAND)));
	}

	if (!otmp->oartifact && resetmat &&
	    objmaterial > LIQUID &&
	  (((otmp->oclass == ROCK_CLASS) &&
	    objmaterial > FLESH) ||
	  ((otmp->otyp == PICK_AXE ||
	    otmp->otyp == DWARVISH_MATTOCK ||
	    otmp->otyp == CHEST ||
	    otmp->otyp == LARGE_BOX ||
	    is_elven_weapon(otmp) ||
	    otmp->otyp == ELVEN_SHIELD ||
	    (is_dwarvish_obj(otmp) && otmp->otyp != DWARVISH_CLOAK) ||
	    ((otmp->oclass == WEAPON_CLASS ||
	    otmp->oclass == ARMOR_CLASS) && 
	    (objects[otmp->otyp].oc_material >= IRON ||
	     objects[otmp->otyp].oc_material == WOOD))) &&
	    (objmaterial >= IRON ||
	     objmaterial == BONE ||
	     objmaterial == WOOD)) ||
	    (otmp->oclass == ARMOR_CLASS &&
	     (objects[otmp->otyp].oc_material == LEATHER ||
	      objects[otmp->otyp].oc_material == CLOTH) &&
	     ((objmaterial > VEGGY &&
	       objmaterial < WOOD) ||
	       (objmaterial == DRAGON_HIDE &&
	        !is_suit(otmp))))))
	   
	{
	    /* TODO: implement material restrictions */
	    otmp->omaterial = objmaterial;
	}
	else if (!otmp->oartifact && resetmat) 
	    otmp->omaterial = objects[otmp->otyp].oc_material;

        if (otmp->oclass == WEAPON_CLASS || 
            is_weptool(otmp) || 
    	    otmp->oclass == AMULET_CLASS || 
    	    otmp->oclass == RING_CLASS || 
    	    otmp->oclass == ARMOR_CLASS)
        {
    	    /* check for restrictions */
    	    if (!((otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) &&
    	          is_blade(otmp))) 
	        objprops &= ~ITEM_VORPAL;

	    if (objprops & ITEM_FROST)
	        objprops &= ~(ITEM_FIRE|ITEM_DRLI);
	    else if (objprops & ITEM_FIRE)
	        objprops &= ~(ITEM_FROST|ITEM_DRLI);
	    else if (objprops & ITEM_DRLI)
	        objprops &= ~(ITEM_FIRE|ITEM_FROST);

	    if (objprops & ITEM_DETONATIONS &&
	        !is_ammo(otmp) && !is_missile(otmp) && !is_launcher(otmp))
		objprops &= ~ITEM_DETONATIONS;
    
    	    if (is_boots(otmp) || is_gloves(otmp) || is_helmet(otmp))
	        objprops &= ~ITEM_REFLECTION;
    	
    	    if (otmp->oclass == WEAPON_CLASS || is_weptool(otmp))
	        objprops &= 
    	          ~(ITEM_DISPLACEMENT|ITEM_POWER|ITEM_DEXTERITY|ITEM_BRILLIANCE
    	           |ITEM_SPEED);
    	
    	    if (otmp->omaterial != CLOTH)
	        objprops &= ~ITEM_OILSKIN;    
    
    	    otmp->oprops |= objprops;

	    if (objprops & ITEM_REFLECTION &&
	        (otmp->oclass == ARMOR_CLASS || otmp->oclass == WEAPON_CLASS ||
		 is_weptool(otmp)) &&
		objects[otmp->otyp].oc_material >= IRON &&
		rn2(5))
		otmp->omaterial = SILVER,
		otmp->owt = weight(otmp);
        }

	if (halfeaten && otmp->oclass == FOOD_CLASS) {
		if (otmp->otyp == CORPSE)
			otmp->oeaten = mons[otmp->corpsenm].cnutrit;
		else otmp->oeaten = objects[otmp->otyp].oc_nutrition;
		/* (do this adjustment before setting up object's weight) */
		consume_oeaten(otmp, 1);
	}
	otmp->owt = weight(otmp);
	if (very && otmp->otyp == HEAVY_IRON_BALL) otmp->owt += 160;

	return(otmp);
}

int
rnd_class(first,last)
int first,last;
{
	int i, x, sum=0;

	if (first == last)
	    return (first);
	for(i=first; i<=last; i++)
		sum += objects[i].oc_prob;
	if (!sum) /* all zero */
		return first + rn2(last-first+1);
	x = rnd(sum);
	for(i=first; i<=last; i++)
		if (objects[i].oc_prob && (x -= objects[i].oc_prob) <= 0)
			return i;
	return 0;
}

STATIC_OVL const char *
Japanese_item_name(i)
int i;
{
	struct Jitem *j = Japanese_items;

	while(j->item) {
		if (i == j->item)
			return j->name;
		j++;
	}
	return (const char *)0;
}

const char *
cloak_simple_name(cloak)
struct obj *cloak;
{
    if (cloak) {
	switch (cloak->otyp) {
	case ROBE:
	    return "robe";
	case MUMMY_WRAPPING:
	    return "wrapping";
	case ALCHEMY_SMOCK:
	    return (objects[cloak->otyp].oc_name_known &&
			cloak->dknown) ? "smock" : "apron";
	default:
	    break;
	}
    }
    return "cloak";
}

const char *
mimic_obj_name(mtmp)
struct monst *mtmp;
{
	if (mtmp->m_ap_type == M_AP_OBJECT && mtmp->mappearance != STRANGE_OBJECT) {
		int idx = objects[mtmp->mappearance].oc_descr_idx;
		if (mtmp->mappearance == GOLD_PIECE) return "gold";
		return obj_descr[idx].oc_name;
	}
	return "whatcha-may-callit";
}
#endif /* OVLB */

/*objnam.c*/
