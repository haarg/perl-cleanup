/*    hv.c
 *
 *    Copyright (c) 1991-1997, Larry Wall
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 */

/*
 * "I sit beside the fire and think of all that I have seen."  --Bilbo
 */

#include "EXTERN.h"
#include "perl.h"

static void hsplit _((HV *hv));
static void hfreeentries _((HV *hv));

static HE* more_he();

#if defined(STRANGE_MALLOC) || defined(MYMALLOC)
#  define ARRAY_ALLOC_BYTES(size) ( (size)*sizeof(HE*) )
#else
#  define MALLOC_OVERHEAD 16
#  define ARRAY_ALLOC_BYTES(size) ( (size)*sizeof(HE*)*2 - MALLOC_OVERHEAD )
#endif

static HE*
new_he()
{
    HE* he;
    if (he_root) {
        he = he_root;
        he_root = HeNEXT(he);
        return he;
    }
    return more_he();
}

static void
del_he(p)
HE* p;
{
    HeNEXT(p) = (HE*)he_root;
    he_root = p;
}

static HE*
more_he()
{
    register HE* he;
    register HE* heend;
    New(54, he_root, 1008/sizeof(HE), HE);
    he = he_root;
    heend = &he[1008 / sizeof(HE) - 1];
    while (he < heend) {
        HeNEXT(he) = (HE*)(he + 1);
        he++;
    }
    HeNEXT(he) = 0;
    return new_he();
}

static HEK *
save_hek(str, len, hash)
char *str;
I32 len;
U32 hash;
{
    char *k;
    register HEK *hek;
    
    New(54, k, HEK_BASESIZE + len + 1, char);
    hek = (HEK*)k;
    Copy(str, HEK_KEY(hek), len, char);
    *(HEK_KEY(hek) + len) = '\0';
    HEK_LEN(hek) = len;
    HEK_HASH(hek) = hash;
    return hek;
}

void
unshare_hek(hek)
HEK *hek;
{
    unsharepvn(HEK_KEY(hek),HEK_LEN(hek),HEK_HASH(hek));
}

/* (klen == HEf_SVKEY) is special for MAGICAL hv entries, meaning key slot
 * contains an SV* */

SV**
hv_fetch(hv,key,klen,lval)
HV *hv;
char *key;
U32 klen;
I32 lval;
{
    register XPVHV* xhv;
    register U32 hash;
    register HE *entry;
    SV *sv;

    if (!hv)
	return 0;

    if (SvRMAGICAL(hv)) {
	if (mg_find((SV*)hv,'P')) {
	    static SV *mysv;
	    sv = sv_newmortal();
	    mg_copy((SV*)hv, sv, key, klen);
	    mysv = sv;
	    return &mysv;
	}
#ifdef ENV_IS_CASELESS
	else if (mg_find((SV*)hv,'E')) {
	    U32 i;
	    for (i = 0; i < klen; ++i)
		if (isLOWER(key[i])) {
		    char *nkey = strupr(SvPVX(sv_2mortal(newSVpv(key,klen))));
		    SV **ret = hv_fetch(hv, nkey, klen, 0);
		    if (!ret && lval)
			ret = hv_store(hv, key, klen, NEWSV(61,0), 0);
		    return ret;
		}
	}
#endif
    }

    xhv = (XPVHV*)SvANY(hv);
    if (!xhv->xhv_array) {
	if (lval 
#ifdef DYNAMIC_ENV_FETCH  /* if it's an %ENV lookup, we may get it on the fly */
	         || (HvNAME(hv) && strEQ(HvNAME(hv),ENV_HV_NAME))
#endif
	                                                          )
	    Newz(503,xhv->xhv_array, ARRAY_ALLOC_BYTES(xhv->xhv_max + 1), char);
	else
	    return 0;
    }

    PERL_HASH(hash, key, klen);

    entry = ((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    for (; entry; entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	return &HeVAL(entry);
    }
#ifdef DYNAMIC_ENV_FETCH  /* %ENV lookup?  If so, try to fetch the value now */
    if (HvNAME(hv) && strEQ(HvNAME(hv),ENV_HV_NAME)) {
      char *gotenv;

      if ((gotenv = ENV_getenv(key)) != Nullch) {
        sv = newSVpv(gotenv,strlen(gotenv));
        SvTAINTED_on(sv);
        return hv_store(hv,key,klen,sv,hash);
      }
    }
#endif
    if (lval) {		/* gonna assign to this, so it better be there */
	sv = NEWSV(61,0);
	return hv_store(hv,key,klen,sv,hash);
    }
    return 0;
}

/* returns a HE * structure with the all fields set */
/* note that hent_val will be a mortal sv for MAGICAL hashes */
HE *
hv_fetch_ent(hv,keysv,lval,hash)
HV *hv;
SV *keysv;
I32 lval;
register U32 hash;
{
    register XPVHV* xhv;
    register char *key;
    STRLEN klen;
    register HE *entry;
    SV *sv;

    if (!hv)
	return 0;

    if (SvRMAGICAL(hv)) {
	if (mg_find((SV*)hv,'P')) {
	    static HE mh;

	    sv = sv_newmortal();
	    keysv = sv_2mortal(newSVsv(keysv));
	    mg_copy((SV*)hv, sv, (char*)keysv, HEf_SVKEY);
	    if (!HeKEY_hek(&mh)) {
		char *k;
		New(54, k, HEK_BASESIZE + sizeof(SV*), char);
		HeKEY_hek(&mh) = (HEK*)k;
	    }
	    HeSVKEY_set(&mh, keysv);
	    HeVAL(&mh) = sv;
	    return &mh;
	}
#ifdef ENV_IS_CASELESS
	else if (mg_find((SV*)hv,'E')) {
	    U32 i;
	    key = SvPV(keysv, klen);
	    for (i = 0; i < klen; ++i)
		if (isLOWER(key[i])) {
		    SV *nkeysv = sv_2mortal(newSVpv(key,klen));
		    (void)strupr(SvPVX(nkeysv));
		    entry = hv_fetch_ent(hv, nkeysv, 0, 0);
		    if (!entry && lval)
			entry = hv_store_ent(hv, keysv, NEWSV(61,0), hash);
		    return entry;
		}
	}
#endif
    }

    xhv = (XPVHV*)SvANY(hv);
    if (!xhv->xhv_array) {
	if (lval 
#ifdef DYNAMIC_ENV_FETCH  /* if it's an %ENV lookup, we may get it on the fly */
	         || (HvNAME(hv) && strEQ(HvNAME(hv),ENV_HV_NAME))
#endif
	                                                          )
	    Newz(503,xhv->xhv_array, ARRAY_ALLOC_BYTES(xhv->xhv_max + 1), char);
	else
	    return 0;
    }

    key = SvPV(keysv, klen);
    
    if (!hash)
	PERL_HASH(hash, key, klen);

    entry = ((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    for (; entry; entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	return entry;
    }
#ifdef DYNAMIC_ENV_FETCH  /* %ENV lookup?  If so, try to fetch the value now */
    if (HvNAME(hv) && strEQ(HvNAME(hv),ENV_HV_NAME)) {
      char *gotenv;

      if ((gotenv = ENV_getenv(key)) != Nullch) {
        sv = newSVpv(gotenv,strlen(gotenv));
        SvTAINTED_on(sv);
        return hv_store_ent(hv,keysv,sv,hash);
      }
    }
#endif
    if (lval) {		/* gonna assign to this, so it better be there */
	sv = NEWSV(61,0);
	return hv_store_ent(hv,keysv,sv,hash);
    }
    return 0;
}

SV**
hv_store(hv,key,klen,val,hash)
HV *hv;
char *key;
U32 klen;
SV *val;
register U32 hash;
{
    register XPVHV* xhv;
    register I32 i;
    register HE *entry;
    register HE **oentry;

    if (!hv)
	return 0;

    xhv = (XPVHV*)SvANY(hv);
    if (SvMAGICAL(hv)) {
	mg_copy((SV*)hv, val, key, klen);
	if (!xhv->xhv_array
	    && (SvMAGIC(hv)->mg_moremagic
		|| (SvMAGIC(hv)->mg_type != 'E'
#ifdef OVERLOAD
		    && SvMAGIC(hv)->mg_type != 'A'
#endif /* OVERLOAD */
		    )))
	    return 0;
#ifdef ENV_IS_CASELESS
	else if (mg_find((SV*)hv,'E')) {
	    SV *sv = sv_2mortal(newSVpv(key,klen));
	    key = strupr(SvPVX(sv));
	    hash = 0;
	}
#endif
    }
    if (!hash)
	PERL_HASH(hash, key, klen);

    if (!xhv->xhv_array)
	Newz(505, xhv->xhv_array, ARRAY_ALLOC_BYTES(xhv->xhv_max + 1), char);

    oentry = &((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    i = 1;

    for (entry = *oentry; entry; i=0, entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	SvREFCNT_dec(HeVAL(entry));
	HeVAL(entry) = val;
	return &HeVAL(entry);
    }

    entry = new_he();
    if (HvSHAREKEYS(hv))
	HeKEY_hek(entry) = share_hek(key, klen, hash);
    else                                       /* gotta do the real thing */
	HeKEY_hek(entry) = save_hek(key, klen, hash);
    HeVAL(entry) = val;
    HeNEXT(entry) = *oentry;
    *oentry = entry;

    xhv->xhv_keys++;
    if (i) {				/* initial entry? */
	++xhv->xhv_fill;
	if (xhv->xhv_keys > xhv->xhv_max)
	    hsplit(hv);
    }

    return &HeVAL(entry);
}

HE *
hv_store_ent(hv,keysv,val,hash)
HV *hv;
SV *keysv;
SV *val;
register U32 hash;
{
    register XPVHV* xhv;
    register char *key;
    STRLEN klen;
    register I32 i;
    register HE *entry;
    register HE **oentry;

    if (!hv)
	return 0;

    xhv = (XPVHV*)SvANY(hv);
    if (SvMAGICAL(hv)) {
	bool save_taint = tainted;
	if (tainting)
	    tainted = SvTAINTED(keysv);
	keysv = sv_2mortal(newSVsv(keysv));
	mg_copy((SV*)hv, val, (char*)keysv, HEf_SVKEY);
	TAINT_IF(save_taint);
	if (!xhv->xhv_array
	    && (SvMAGIC(hv)->mg_moremagic
		|| (SvMAGIC(hv)->mg_type != 'E'
#ifdef OVERLOAD
		    && SvMAGIC(hv)->mg_type != 'A'
#endif /* OVERLOAD */
		    )))
	  return Nullhe;
#ifdef ENV_IS_CASELESS
	else if (mg_find((SV*)hv,'E')) {
	    key = SvPV(keysv, klen);
	    keysv = sv_2mortal(newSVpv(key,klen));
	    (void)strupr(SvPVX(keysv));
	    hash = 0;
	}
#endif
    }

    key = SvPV(keysv, klen);

    if (!hash)
	PERL_HASH(hash, key, klen);

    if (!xhv->xhv_array)
	Newz(505, xhv->xhv_array, ARRAY_ALLOC_BYTES(xhv->xhv_max + 1), char);

    oentry = &((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    i = 1;

    for (entry = *oentry; entry; i=0, entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	SvREFCNT_dec(HeVAL(entry));
	HeVAL(entry) = val;
	return entry;
    }

    entry = new_he();
    if (HvSHAREKEYS(hv))
	HeKEY_hek(entry) = share_hek(key, klen, hash);
    else                                       /* gotta do the real thing */
	HeKEY_hek(entry) = save_hek(key, klen, hash);
    HeVAL(entry) = val;
    HeNEXT(entry) = *oentry;
    *oentry = entry;

    xhv->xhv_keys++;
    if (i) {				/* initial entry? */
	++xhv->xhv_fill;
	if (xhv->xhv_keys > xhv->xhv_max)
	    hsplit(hv);
    }

    return entry;
}

SV *
hv_delete(hv,key,klen,flags)
HV *hv;
char *key;
U32 klen;
I32 flags;
{
    register XPVHV* xhv;
    register I32 i;
    register U32 hash;
    register HE *entry;
    register HE **oentry;
    SV *sv;

    if (!hv)
	return Nullsv;
    if (SvRMAGICAL(hv)) {
	sv = *hv_fetch(hv, key, klen, TRUE);
	/* If sv isn't magical, do nothing.  This enables the HV to have
	 * magic that doesn't propagate to the elements (such as '~').
	 */
	if (SvRMAGICAL(sv)) {
	    mg_clear(sv);
	    if (mg_find(sv, 's')) {
		return Nullsv;		/* %SIG elements cannot be deleted */
	    }
	    if (mg_find(sv, 'p')) {
		sv_unmagic(sv, 'p');	/* No longer an element */
		return sv;
	    }
#ifdef ENV_IS_CASELESS
	    if (mg_find((SV*)hv,'E')) {
		sv = sv_2mortal(newSVpv(key,klen));
		key = strupr(SvPVX(sv));
	    }
#endif
	}
    }
    xhv = (XPVHV*)SvANY(hv);
    if (!xhv->xhv_array)
	return Nullsv;

    PERL_HASH(hash, key, klen);

    oentry = &((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    entry = *oentry;
    i = 1;
    for (; entry; i=0, oentry = &HeNEXT(entry), entry = *oentry) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	*oentry = HeNEXT(entry);
	if (i && !*oentry)
	    xhv->xhv_fill--;
	if (flags & G_DISCARD)
	    sv = Nullsv;
	else
	    sv = sv_mortalcopy(HeVAL(entry));
	if (entry == xhv->xhv_eiter)
	    HvLAZYDEL_on(hv);
	else
	    hv_free_ent(hv, entry);
	--xhv->xhv_keys;
	return sv;
    }
    return Nullsv;
}

SV *
hv_delete_ent(hv,keysv,flags,hash)
HV *hv;
SV *keysv;
I32 flags;
U32 hash;
{
    register XPVHV* xhv;
    register I32 i;
    register char *key;
    STRLEN klen;
    register HE *entry;
    register HE **oentry;
    SV *sv;
    
    if (!hv)
	return Nullsv;
    if (SvRMAGICAL(hv)) {
	entry = hv_fetch_ent(hv, keysv, TRUE, hash);
	sv = HeVAL(entry);
	/* If sv isn't magical, do nothing.  This enables the HV to have
	 * magic that doesn't propagate to the elements (such as '~').
	 */
	if (SvRMAGICAL(sv)) {	     
	    mg_clear(sv);
	    if (mg_find(sv, 'p')) {
		sv_unmagic(sv, 'p');	/* No longer an element */
		return sv;
	    }
#ifdef ENV_IS_CASELESS
	    else if (mg_find((SV*)hv,'E')) {
		key = SvPV(keysv, klen);
		keysv = sv_2mortal(newSVpv(key,klen));
		(void)strupr(SvPVX(keysv));
		hash = 0; 
	    }
#endif
	}
    }
    xhv = (XPVHV*)SvANY(hv);
    if (!xhv->xhv_array)
	return Nullsv;

    key = SvPV(keysv, klen);
    
    if (!hash)
	PERL_HASH(hash, key, klen);

    oentry = &((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    entry = *oentry;
    i = 1;
    for (; entry; i=0, oentry = &HeNEXT(entry), entry = *oentry) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	*oentry = HeNEXT(entry);
	if (i && !*oentry)
	    xhv->xhv_fill--;
	if (flags & G_DISCARD)
	    sv = Nullsv;
	else
	    sv = sv_mortalcopy(HeVAL(entry));
	if (entry == xhv->xhv_eiter)
	    HvLAZYDEL_on(hv);
	else
	    hv_free_ent(hv, entry);
	--xhv->xhv_keys;
	return sv;
    }
    return Nullsv;
}

bool
hv_exists(hv,key,klen)
HV *hv;
char *key;
U32 klen;
{
    register XPVHV* xhv;
    register U32 hash;
    register HE *entry;
    SV *sv;

    if (!hv)
	return 0;

    if (SvRMAGICAL(hv)) {
	if (mg_find((SV*)hv,'P')) {
	    sv = sv_newmortal();
	    mg_copy((SV*)hv, sv, key, klen); 
	    magic_existspack(sv, mg_find(sv, 'p'));
	    return SvTRUE(sv);
	}
#ifdef ENV_IS_CASELESS
	else if (mg_find((SV*)hv,'E')) {
	    sv = sv_2mortal(newSVpv(key,klen));
	    key = strupr(SvPVX(sv));
	}
#endif
    }

    xhv = (XPVHV*)SvANY(hv);
    if (!xhv->xhv_array)
	return 0; 

    PERL_HASH(hash, key, klen);

    entry = ((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    for (; entry; entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	return TRUE;
    }
    return FALSE;
}


bool
hv_exists_ent(hv,keysv,hash)
HV *hv;
SV *keysv;
U32 hash;
{
    register XPVHV* xhv;
    register char *key;
    STRLEN klen;
    register HE *entry;
    SV *sv;

    if (!hv)
	return 0;

    if (SvRMAGICAL(hv)) {
	if (mg_find((SV*)hv,'P')) {
	    sv = sv_newmortal();
	    keysv = sv_2mortal(newSVsv(keysv));
	    mg_copy((SV*)hv, sv, (char*)keysv, HEf_SVKEY); 
	    magic_existspack(sv, mg_find(sv, 'p'));
	    return SvTRUE(sv);
	}
#ifdef ENV_IS_CASELESS
	else if (mg_find((SV*)hv,'E')) {
	    key = SvPV(keysv, klen);
	    keysv = sv_2mortal(newSVpv(key,klen));
	    (void)strupr(SvPVX(keysv));
	    hash = 0; 
	}
#endif
    }

    xhv = (XPVHV*)SvANY(hv);
    if (!xhv->xhv_array)
	return 0; 

    key = SvPV(keysv, klen);
    if (!hash)
	PERL_HASH(hash, key, klen);

    entry = ((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    for (; entry; entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != klen)
	    continue;
	if (memNE(HeKEY(entry),key,klen))	/* is this it? */
	    continue;
	return TRUE;
    }
    return FALSE;
}

static void
hsplit(hv)
HV *hv;
{
    register XPVHV* xhv = (XPVHV*)SvANY(hv);
    I32 oldsize = (I32) xhv->xhv_max + 1; /* sic(k) */
    register I32 newsize = oldsize * 2;
    register I32 i;
    register char *a = xhv->xhv_array;
    register HE **aep;
    register HE **bep;
    register HE *entry;
    register HE **oentry;

    nomemok = TRUE;
#if defined(STRANGE_MALLOC) || defined(MYMALLOC)
    Renew(a, ARRAY_ALLOC_BYTES(newsize), char);
    if (!a) {
      nomemok = FALSE;
      return;
    }
#else
#define MALLOC_OVERHEAD 16
    New(2, a, ARRAY_ALLOC_BYTES(newsize), char);
    if (!a) {
      nomemok = FALSE;
      return;
    }
    Copy(xhv->xhv_array, a, oldsize * sizeof(HE*), char);
    if (oldsize >= 64 && !nice_chunk) {
	nice_chunk = xhv->xhv_array;
	nice_chunk_size = ARRAY_ALLOC_BYTES(oldsize);
    }
    else
	Safefree(xhv->xhv_array);
#endif

    nomemok = FALSE;
    Zero(&a[oldsize * sizeof(HE*)], (newsize-oldsize) * sizeof(HE*), char);	/* zero 2nd half*/
    xhv->xhv_max = --newsize;
    xhv->xhv_array = a;
    aep = (HE**)a;

    for (i=0; i<oldsize; i++,aep++) {
	if (!*aep)				/* non-existent */
	    continue;
	bep = aep+oldsize;
	for (oentry = aep, entry = *aep; entry; entry = *oentry) {
	    if ((HeHASH(entry) & newsize) != i) {
		*oentry = HeNEXT(entry);
		HeNEXT(entry) = *bep;
		if (!*bep)
		    xhv->xhv_fill++;
		*bep = entry;
		continue;
	    }
	    else
		oentry = &HeNEXT(entry);
	}
	if (!*aep)				/* everything moved */
	    xhv->xhv_fill--;
    }
}

void
hv_ksplit(hv, newmax)
HV *hv;
IV newmax;
{
    register XPVHV* xhv = (XPVHV*)SvANY(hv);
    I32 oldsize = (I32) xhv->xhv_max + 1; /* sic(k) */
    register I32 newsize;
    register I32 i;
    register I32 j;
    register char *a;
    register HE **aep;
    register HE *entry;
    register HE **oentry;

    newsize = (I32) newmax;			/* possible truncation here */
    if (newsize != newmax || newmax <= oldsize)
	return;
    while ((newsize & (1 + ~newsize)) != newsize) {
	newsize &= ~(newsize & (1 + ~newsize));	/* get proper power of 2 */
    }
    if (newsize < newmax)
	newsize *= 2;
    if (newsize < newmax)
	return;					/* overflow detection */

    a = xhv->xhv_array;
    if (a) {
	nomemok = TRUE;
#if defined(STRANGE_MALLOC) || defined(MYMALLOC)
	Renew(a, ARRAY_ALLOC_BYTES(newsize), char);
        if (!a) {
	  nomemok = FALSE;
	  return;
	}
#else
	New(2, a, ARRAY_ALLOC_BYTES(newsize), char);
        if (!a) {
	  nomemok = FALSE;
	  return;
	}
	Copy(xhv->xhv_array, a, oldsize * sizeof(HE*), char);
	if (oldsize >= 64 && !nice_chunk) {
	    nice_chunk = xhv->xhv_array;
	    nice_chunk_size = ARRAY_ALLOC_BYTES(oldsize);
	}
	else
	    Safefree(xhv->xhv_array);
#endif
	nomemok = FALSE;
	Zero(&a[oldsize * sizeof(HE*)], (newsize-oldsize) * sizeof(HE*), char); /* zero 2nd half*/
    }
    else {
	Newz(0, a, ARRAY_ALLOC_BYTES(newsize), char);
    }
    xhv->xhv_max = --newsize;
    xhv->xhv_array = a;
    if (!xhv->xhv_fill)				/* skip rest if no entries */
	return;

    aep = (HE**)a;
    for (i=0; i<oldsize; i++,aep++) {
	if (!*aep)				/* non-existent */
	    continue;
	for (oentry = aep, entry = *aep; entry; entry = *oentry) {
	    if ((j = (HeHASH(entry) & newsize)) != i) {
		j -= i;
		*oentry = HeNEXT(entry);
		if (!(HeNEXT(entry) = aep[j]))
		    xhv->xhv_fill++;
		aep[j] = entry;
		continue;
	    }
	    else
		oentry = &HeNEXT(entry);
	}
	if (!*aep)				/* everything moved */
	    xhv->xhv_fill--;
    }
}

HV *
newHV()
{
    register HV *hv;
    register XPVHV* xhv;

    hv = (HV*)NEWSV(502,0);
    sv_upgrade((SV *)hv, SVt_PVHV);
    xhv = (XPVHV*)SvANY(hv);
    SvPOK_off(hv);
    SvNOK_off(hv);
#ifndef NODEFAULT_SHAREKEYS    
    HvSHAREKEYS_on(hv);         /* key-sharing on by default */
#endif    
    xhv->xhv_max = 7;		/* start with 8 buckets */
    xhv->xhv_fill = 0;
    xhv->xhv_pmroot = 0;
    (void)hv_iterinit(hv);	/* so each() will start off right */
    return hv;
}

void
hv_free_ent(hv, entry)
HV *hv;
register HE *entry;
{
    SV *val;

    if (!entry)
	return;
    val = HeVAL(entry);
    if (val && isGV(val) && GvCVu(val) && HvNAME(hv))
	sub_generation++;	/* may be deletion of method from stash */
    SvREFCNT_dec(val);
    if (HeKLEN(entry) == HEf_SVKEY) {
	SvREFCNT_dec(HeKEY_sv(entry));
        Safefree(HeKEY_hek(entry));
    }
    else if (HvSHAREKEYS(hv))
	unshare_hek(HeKEY_hek(entry));
    else
	Safefree(HeKEY_hek(entry));
    del_he(entry);
}

void
hv_delayfree_ent(hv, entry)
HV *hv;
register HE *entry;
{
    if (!entry)
	return;
    if (isGV(HeVAL(entry)) && GvCVu(HeVAL(entry)) && HvNAME(hv))
	sub_generation++;	/* may be deletion of method from stash */
    sv_2mortal(HeVAL(entry));	/* free between statements */
    if (HeKLEN(entry) == HEf_SVKEY) {
	sv_2mortal(HeKEY_sv(entry));
	Safefree(HeKEY_hek(entry));
    }
    else if (HvSHAREKEYS(hv))
	unshare_hek(HeKEY_hek(entry));
    else
	Safefree(HeKEY_hek(entry));
    del_he(entry);
}

void
hv_clear(hv)
HV *hv;
{
    register XPVHV* xhv;
    if (!hv)
	return;
    xhv = (XPVHV*)SvANY(hv);
    hfreeentries(hv);
    xhv->xhv_fill = 0;
    xhv->xhv_keys = 0;
    if (xhv->xhv_array)
	(void)memzero(xhv->xhv_array, (xhv->xhv_max + 1) * sizeof(HE*));

    if (SvRMAGICAL(hv))
	mg_clear((SV*)hv); 
}

static void
hfreeentries(hv)
HV *hv;
{
    register HE **array;
    register HE *entry;
    register HE *oentry = Null(HE*);
    I32 riter;
    I32 max;

    if (!hv)
	return;
    if (!HvARRAY(hv))
	return;

    riter = 0;
    max = HvMAX(hv);
    array = HvARRAY(hv);
    entry = array[0];
    for (;;) {
	if (entry) {
	    oentry = entry;
	    entry = HeNEXT(entry);
	    hv_free_ent(hv, oentry);
	}
	if (!entry) {
	    if (++riter > max)
		break;
	    entry = array[riter];
	} 
    }
    (void)hv_iterinit(hv);
}

void
hv_undef(hv)
HV *hv;
{
    register XPVHV* xhv;
    if (!hv)
	return;
    xhv = (XPVHV*)SvANY(hv);
    hfreeentries(hv);
    Safefree(xhv->xhv_array);
    if (HvNAME(hv)) {
	Safefree(HvNAME(hv));
	HvNAME(hv) = 0;
    }
    xhv->xhv_array = 0;
    xhv->xhv_max = 7;		/* it's a normal hash */
    xhv->xhv_fill = 0;
    xhv->xhv_keys = 0;

    if (SvRMAGICAL(hv))
	mg_clear((SV*)hv); 
}

I32
hv_iterinit(hv)
HV *hv;
{
    register XPVHV* xhv;
    HE *entry;

    if (!hv)
	croak("Bad hash");
    xhv = (XPVHV*)SvANY(hv);
    entry = xhv->xhv_eiter;
#ifdef DYNAMIC_ENV_FETCH  /* set up %ENV for iteration */
    if (HvNAME(hv) && strEQ(HvNAME(hv), ENV_HV_NAME))
	prime_env_iter();
#endif
    if (entry && HvLAZYDEL(hv)) {	/* was deleted earlier? */
	HvLAZYDEL_off(hv);
	hv_free_ent(hv, entry);
    }
    xhv->xhv_riter = -1;
    xhv->xhv_eiter = Null(HE*);
    return xhv->xhv_keys;	/* used to be xhv->xhv_fill before 5.004_65 */
}

HE *
hv_iternext(hv)
HV *hv;
{
    register XPVHV* xhv;
    register HE *entry;
    HE *oldentry;
    MAGIC* mg;

    if (!hv)
	croak("Bad hash");
    xhv = (XPVHV*)SvANY(hv);
    oldentry = entry = xhv->xhv_eiter;

    if (SvRMAGICAL(hv) && (mg = mg_find((SV*)hv,'P'))) {
	SV *key = sv_newmortal();
	if (entry) {
	    sv_setsv(key, HeSVKEY_force(entry));
	    SvREFCNT_dec(HeSVKEY(entry));	/* get rid of previous key */
	}
	else {
	    char *k;
	    HEK *hek;

	    xhv->xhv_eiter = entry = new_he();  /* one HE per MAGICAL hash */
	    Zero(entry, 1, HE);
	    Newz(54, k, HEK_BASESIZE + sizeof(SV*), char);
	    hek = (HEK*)k;
	    HeKEY_hek(entry) = hek;
	    HeKLEN(entry) = HEf_SVKEY;
	}
	magic_nextpack((SV*) hv,mg,key);
        if (SvOK(key)) {
	    /* force key to stay around until next time */
	    HeSVKEY_set(entry, SvREFCNT_inc(key));
	    return entry;		/* beware, hent_val is not set */
        }
	if (HeVAL(entry))
	    SvREFCNT_dec(HeVAL(entry));
	Safefree(HeKEY_hek(entry));
	del_he(entry);
	xhv->xhv_eiter = Null(HE*);
	return Null(HE*);
    }

    if (!xhv->xhv_array)
	Newz(506,xhv->xhv_array, ARRAY_ALLOC_BYTES(xhv->xhv_max + 1), char);
    if (entry)
	entry = HeNEXT(entry);
    while (!entry) {
	++xhv->xhv_riter;
	if (xhv->xhv_riter > xhv->xhv_max) {
	    xhv->xhv_riter = -1;
	    break;
	}
	entry = ((HE**)xhv->xhv_array)[xhv->xhv_riter];
    }

    if (oldentry && HvLAZYDEL(hv)) {		/* was deleted earlier? */
	HvLAZYDEL_off(hv);
	hv_free_ent(hv, oldentry);
    }

    xhv->xhv_eiter = entry;
    return entry;
}

char *
hv_iterkey(entry,retlen)
register HE *entry;
I32 *retlen;
{
    if (HeKLEN(entry) == HEf_SVKEY) {
	STRLEN len;
	char *p = SvPV(HeKEY_sv(entry), len);
	*retlen = len;
	return p;
    }
    else {
	*retlen = HeKLEN(entry);
	return HeKEY(entry);
    }
}

/* unlike hv_iterval(), this always returns a mortal copy of the key */
SV *
hv_iterkeysv(entry)
register HE *entry;
{
    if (HeKLEN(entry) == HEf_SVKEY)
	return sv_mortalcopy(HeKEY_sv(entry));
    else
	return sv_2mortal(newSVpv((HeKLEN(entry) ? HeKEY(entry) : ""),
				  HeKLEN(entry)));
}

SV *
hv_iterval(hv,entry)
HV *hv;
register HE *entry;
{
    if (SvRMAGICAL(hv)) {
	if (mg_find((SV*)hv,'P')) {
	    SV* sv = sv_newmortal();
	    if (HeKLEN(entry) == HEf_SVKEY)
		mg_copy((SV*)hv, sv, (char*)HeKEY_sv(entry), HEf_SVKEY);
	    else mg_copy((SV*)hv, sv, HeKEY(entry), HeKLEN(entry));
	    return sv;
	}
    }
    return HeVAL(entry);
}

SV *
hv_iternextsv(hv, key, retlen)
    HV *hv;
    char **key;
    I32 *retlen;
{
    HE *he;
    if ( (he = hv_iternext(hv)) == NULL)
	return NULL;
    *key = hv_iterkey(he, retlen);
    return hv_iterval(hv, he);
}

void
hv_magic(hv, gv, how)
HV* hv;
GV* gv;
int how;
{
    sv_magic((SV*)hv, (SV*)gv, how, Nullch, 0);
}

char*	
sharepvn(sv, len, hash)
char* sv;
I32 len;
U32 hash;
{
    return HEK_KEY(share_hek(sv, len, hash));
}

/* possibly free a shared string if no one has access to it
 * len and hash must both be valid for str.
 */
void
unsharepvn(str, len, hash)
char* str;
I32 len;
U32 hash;
{
    register XPVHV* xhv;
    register HE *entry;
    register HE **oentry;
    register I32 i = 1;
    I32 found = 0;
    
    /* what follows is the moral equivalent of:
    if ((Svp = hv_fetch(strtab, tmpsv, FALSE, hash))) {
	if (--*Svp == Nullsv)
	    hv_delete(strtab, str, len, G_DISCARD, hash);
    } */
    xhv = (XPVHV*)SvANY(strtab);
    /* assert(xhv_array != 0) */
    oentry = &((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    for (entry = *oentry; entry; i=0, oentry = &HeNEXT(entry), entry = *oentry) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != len)
	    continue;
	if (memNE(HeKEY(entry),str,len))	/* is this it? */
	    continue;
	found = 1;
	if (--HeVAL(entry) == Nullsv) {
	    *oentry = HeNEXT(entry);
	    if (i && !*oentry)
		xhv->xhv_fill--;
	    Safefree(HeKEY_hek(entry));
	    del_he(entry);
	    --xhv->xhv_keys;
	}
	break;
    }
    
    if (!found)
	warn("Attempt to free non-existent shared string");    
}

/* get a (constant) string ptr from the global string table
 * string will get added if it is not already there.
 * len and hash must both be valid for str.
 */
HEK *
share_hek(str, len, hash)
char *str;
I32 len;
register U32 hash;
{
    register XPVHV* xhv;
    register HE *entry;
    register HE **oentry;
    register I32 i = 1;
    I32 found = 0;

    /* what follows is the moral equivalent of:
       
    if (!(Svp = hv_fetch(strtab, str, len, FALSE)))
    	hv_store(strtab, str, len, Nullsv, hash);
    */
    xhv = (XPVHV*)SvANY(strtab);
    /* assert(xhv_array != 0) */
    oentry = &((HE**)xhv->xhv_array)[hash & (I32) xhv->xhv_max];
    for (entry = *oentry; entry; i=0, entry = HeNEXT(entry)) {
	if (HeHASH(entry) != hash)		/* strings can't be equal */
	    continue;
	if (HeKLEN(entry) != len)
	    continue;
	if (memNE(HeKEY(entry),str,len))	/* is this it? */
	    continue;
	found = 1;
	break;
    }
    if (!found) {
	entry = new_he();
	HeKEY_hek(entry) = save_hek(str, len, hash);
	HeVAL(entry) = Nullsv;
	HeNEXT(entry) = *oentry;
	*oentry = entry;
	xhv->xhv_keys++;
	if (i) {				/* initial entry? */
	    ++xhv->xhv_fill;
	    if (xhv->xhv_keys > xhv->xhv_max)
		hsplit(strtab);
	}
    }

    ++HeVAL(entry);				/* use value slot as REFCNT */
    return HeKEY_hek(entry);
}


