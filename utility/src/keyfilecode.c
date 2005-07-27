
/*                                                               */
/* Copyright 1984,1985,1986,1988,1989,1990,2003,2004,2005 by     */
/*   Howard Turtle                                               */

#define boolean int
#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "keyerr.h"
#include "keydef.h"
#include "keyprint.h"
#include "keyfile.h"
#include "fileio.h"
#ifdef min
#undef min
#endif
/* #define maxint 32767*/
#define show_errors true
#define trace_io false


int
  get_buffer_cnt=0,
  replace_buffer_cnt=0,
  get_index_cnt=0,
  hash_chain_cnt=0;

int
  read_cnt=0,
  write_cnt=0;

static struct level0_pntr
  null0_ptr = {max_segments,0,0},
  dummy_ptr = {0,0,0};
static struct leveln_pntr
  nulln_ptr = {max_segments,0};

static char
  index_caption[3][10]={"user    \0","free_rec\0","free_lc \0"};

void print_buffer_caption();

static int allocate_block();
static boolean allocate_rec();
static void replace_max_key();
static void deallocate_rec();
static int compress_ix_block();
static void check_ix_block_compression();
static void update_index();
static void index_delete();
static void split_block();
static FILE *file_index();


#define eq_pntr(p1,p2) ((p1.block==p2.block) && (p1.segment==p2.segment))
#define null_pntr(p1) ((p1.segment==max_segments) && (p1.block==0))

/* Miscellaneous procedures */

static int min(int i, int j)
{
  if ( i<j ) return(i);
  else return(j);
}

#define mvc(t1,sc1,t2,sc2,lc) memmove((unsigned char *)t2+sc2,(unsigned char *)t1+sc1,(size_t)lc);

/*void mvc(t1,sc1,t2,sc2,lc)
unsigned char t1[],t2[]; int sc1,sc2,lc;
{
  memmove(t2+sc2,t1+sc1,lc);
}*/


static void print_leveln_pntr(char caption[], struct leveln_pntr *pn)
{
  printf("%s%u/",caption,pn->segment);
  printf(UINT64_format,pn->block);
  printf(" ");
}

static void print_level0_pntr(char caption[], struct level0_pntr *p0)
{
  printf("%s%u/",caption,p0->segment);
  printf(UINT64_format,p0->sc);
  printf("/%u ",p0->lc);
}

/* compressed_int_lc returns the length of a compressed integer */
/*   without actually uncompressing it.  Note that it doesn't   */
/*   care what the size of the uncompressed int would be.       */

static int compressed_int_lc(char buf[], unsigned offset)
{int i=0;

  while ( (buf[offset+i] & 128)!=0 ) i++;
  return(i+1);
}

/* uncompress_UINT16 uncompresses an integer compressed in      */
/*    array p and returns the number of bytes consumed to       */
/*    decompress the int.                                       */


static int uncompress_UINT16(UINT16 *i, unsigned char p[])
{int j=0; boolean done=false;

  *i = 0;
  do {
    *i = *i | (p[j] & 127);
    if ( (p[j] & 128)!=0 ) {
      *i = *i << 7;
      j++;
    }
    else done = true;
  } while ( !done );
  return(j+1);
}

/* uncompress_UINT32 uncompresses an integer compressed in      */
/*    array p and returns the number of bytes consumed to       */
/*    decompress the int.                                       */


static int uncompress_UINT32(UINT32 *i, unsigned char p[])
{int j=0; boolean done=false;

  *i = 0;
  do {
    *i = *i | (p[j] & 127);
    if ( (p[j] & 128)!=0 ) {
      *i = *i << 7;
      j++;
    }
    else done = true;
  } while ( !done );
  return(j+1);
}

/* uncompress_UINT64 uncompresses an integer compressed in      */
/*    array p and returns the number of bytes consumed to       */
/*    decompress the int.                                       */

static int uncompress_UINT64(UINT64 *i, unsigned char p[])
{int j=0; boolean done=false;

  *i = 0;
  do {
    *i = *i | (p[j] & 127);
    if ( (p[j] & 128)!=0 ) {
      *i = *i << 7;
      j++;
    }
    else done = true;
  } while ( !done );
  return(j+1);
}

/* compress_UINT32 compresses UINT32 i into a byte string and     */
/*   returns the length of the compressed string. Note that ptr   */
/*   points to the rightmost character in the compressed string,  */
/*   i.e. the int is compressed from ptr to the left.             */


static int compress_UINT32(UINT32 i, unsigned char *ptr)
{unsigned char *p;

  p = ptr;
  do {
    *p = (i & 127) | 128;
    p--;
    i = i>>7;
  } while ( i>0 );
  *ptr = *ptr & 127; /* high bit off */
  return((int)(ptr-p));
}

/* compress_UINT64 compresses UINT64 i into a byte string and     */
/*   returns the length of the compressed string. Note that ptr   */
/*   points to the rightmost character in the compressed string,  */
/*   i.e. the int is compressed from ptr to the left.             */


static int compress_UINT64(UINT64 i, unsigned char *ptr)
{unsigned char *p;

  p = ptr;
  do {
    *p = (i & 127) | 128;
    p--;
    i = i>>7;
  } while ( i>0 );
  *ptr = *ptr & 127; /* high bit off */
  return((int)(ptr-p));
}

/* UINT32_lc_if_compressed returns the length that UINT32 i      */
/*   will occupy once compressed.                                */

static int UINT32_lc_if_compressed(UINT32 i)
{
  if      ( i<128    )    return(1);
  else if ( i<16384  )    return(2);
  else if ( i<2097152 )   return(3);
  else if ( i<268435456 ) return(4);
  else return(5);
}

/* UINT64_lc_if_compressed returns the length that UINT64 i      */
/*   will occupy once compressed.                                */

static int UINT64_lc_if_compressed(UINT64 i)
{
  if      ( i<                UINT64_C(128) ) return(1);
  else if ( i<              UINT64_C(16384) ) return(2);
  else if ( i<            UINT64_C(2097152) ) return(3);
  else if ( i<          UINT64_C(268435456) ) return(4);
  else if ( i<        UINT64_C(34359738368) ) return(5);
  else if ( i<      UINT64_C(4398046511104) ) return(6);
  else if ( i<    UINT64_C(562949953421312) ) return(7);
  else if ( i<  UINT64_C(72057594037927936) ) return(8);
  else if ( i<UINT64_C(9223372036854775808) ) return(9);
  else return(10);
}

static unsigned allocation_lc(unsigned lc, unsigned unit)
{
  if ( lc==0 ) return(0);
  else return(((lc-1) / unit + 1) * unit);
}

static unsigned rec_allocation_lc(unsigned lc)
{
  if ( lc==0 ) return(0);
  else return(((lc-1) / rec_allocation_unit + 1) * rec_allocation_unit);
}

/* Error handling */

static void fatal_error(f,err)
struct fcb *f; int err;
{
   f->error_code = err; f->file_ok = false;
}

static void set_error(struct fcb *f, int err, char caption[])
{
  f->error_code = err;
  if ( show_errors ) printf("%s",caption);
}

static void set_error1(struct fcb *f, int err, char caption[], int code)
{
  f->error_code = err;
  if ( show_errors ) printf("%s%d\n",caption,code);
}

/* Error checking.  Three fields in the fcb are used for error */
/*   management.  f->marker is set to keyf when the file is    */
/*   created and is never changed.  Any fcb with a different   */
/*   value is not useable.  f->file_ok is set true when the    */
/*   file is created and is turned off if an error occurs that */
/*   is so serious that the file is probably damaged (call to  */
/*   fatal_error).  f->error_code is set for any error         */
/*   condition.  Some errors are considered transient (e.g.,   */
/*   ateof, atbof, xxnokey,...) and are reset on the next call */
/*   to the package.  All others are considered permanent and  */
/*   are not reset.                                            */

static boolean check_fcb(struct fcb *f)
{ boolean ok;

  ok = (f->marker==keyf) && f->file_ok;
  switch ( f->error_code ) {
    case  0: break;
    case  1: /* badopen_err */      ok = false; break;
    case  2: /* badcreate_err */    ok = false; break;
    case  3: /* smallfcb_err */     ok = false; break;
    case  4: /* dltnokey_err */                 break;
    case  5: /* getnokey_err */                 break;
    case  6: /* notkeyfil_err */    ok = false; break;
    case  7: /* filenotok_err */    ok = false; break;
    case  8: /* badkey_err */                   break;
    case  9: /* maxlevel_err */     ok = false; break;
    case 10: /* ateof_err */                    break;
    case 11: /* atbof_err */                    break;
    case 12: /* longrec_err */                  break;
    case 13: /* longkey_err */                  break;
    case 14: /* version_err */      ok = false; break;
    case 15: /* seek_err */         ok = false; break;
    case 16: /* read_err */         ok = false; break;
    case 17: /* write_err */        ok = false; break;
    case 18: /* segment_open_err */ ok = false; break;
    case 19: /* segment_create_err */ok = false; break;
    case 20: /* bad_name_err */     ok = false; break;
    case 21: /* bad_dlt_err */      ok = false; break;
    case 22: /* max_key_err */      ok = false; break;
    case 23: /* nospace_err */      ok = false; break;
    case 24: /* free_insrt_err */   ok = false; break;
    case 25: /* free_dlt_err */     ok = false; break;
    case 26: /* alloc_rec_err */    ok = false; break;
    case 27: /* dealloc_rec_err */  ok = false; break;
    case 28: /* alloc_buf_err */    ok = false; break;
    case 29: /* move_rec_err */     ok = false; break;
    case 30: /* bad_close_err */    ok = false; break;
    case 31: /* ix_struct_err */    ok = false; break;
    case 32: /* read_only_err */                break;
    case 33: /* repl_max_key_err */ ok = false; break;
    default: /* illegal_err code */ ok = false; break;
  }
  if ( ok ) f->error_code = no_err;
  return(ok);
}

static boolean set_up(struct fcb *f, unsigned char key[], int key_lc, struct key *k)
{
  if ( !check_fcb(f) ) return(false);
  k->lc = key_lc;
  if ( k->lc>0 && k->lc<maxkey_lc ) {
    memcpy(k->text,key,(size_t)key_lc); return(true);
  }
  else {
    f->error_code = badkey_err; k->lc = 0;
    return(false);
  }
}

/* Pointer manipulation */

static boolean gt_pntr(struct leveln_pntr p1, struct leveln_pntr p2)
{
  if ( p1.segment<p2.segment ) return(false);
  else if ( p1.segment>p2.segment ) return(true);
  else return( p1.block>p2.block );
}

/* compress0_pntr compresses a level0 pointer p into a byte */
/*   string.  cp points to the last byte in the compressed  */
/*   string and compression will proceed from right to left */
/*   Note that segment and lc shrink when compressed but sc */
/*   will usually expand.  We tolerate the expansion since  */
/*   it obviates the need for byte swapping                 */

static int compress0_pntr(struct level0_pntr *p, unsigned char *cp)
{int lc,lc1,lc2;

  lc = compress_UINT32(p->lc,cp);
  lc1 = compress_UINT64(p->sc,cp-lc);
  /*  memcpy((char *)cp-lc-sizeof(int)+1,&(p->sc),sizeof(int));*/
  lc2 = compress_UINT32(p->segment,cp-lc-lc1);
  return( lc+lc1+lc2 );
}

/* level0_pntr_lc returns the compressed length of level0_pntr p */

static int level0_pntr_lc(struct level0_pntr *p)
{/*int lc,lc1,lc2;

  lc =  UINT32_lc_if_compressed(p->lc);
  lc1 = UINT32_lc_if_compressed(p->sc);
  lc2 = UINT32_lc_if_compressed(p->segment);
  return( lc+lc1+lc2);*/
  return( UINT32_lc_if_compressed(p->lc) + UINT64_lc_if_compressed(p->sc) + UINT32_lc_if_compressed(p->segment) );
}

/* uncompress0_pntr uncompresses a level0 pointer.  Note   */
/*   cp points to the first character in the compressed    */
/*   string (unlike compress0_pntr).                       */

static int uncompress0_pntr(struct level0_pntr *p, unsigned char *cp)
{int lc,lc1,lc2;

  lc = uncompress_UINT16(&(p->segment),cp);
  lc1 = uncompress_UINT64(&(p->sc),cp+lc);
  /*  memcpy(&(p->sc),cp+lc,sizeof(int));*/
  lc2 = uncompress_UINT32(&(p->lc),cp+lc+lc1);
  return( lc+lc1+lc2 );
}

/* compressn_pntr compresses a leveln pointer p into a byte */
/*   string.  cp points to the last byte in the compressed  */
/*   string and compression will proceed from right to left */

static int compressn_pntr(struct leveln_pntr *p, unsigned char *cp)
{int lc,lc1;

  lc = compress_UINT64(p->block,cp);
  lc1 = compress_UINT32(p->segment,cp-lc);
  return( lc+lc1 );
}

/* leveln_pntr_lc returns the compressed length of leveln_pntr p */

static int leveln_pntr_lc(struct leveln_pntr *p)
{/*int lc,lc1;

  lc = UINT32_lc_if_compressed(p->block);
  lc1 = UINT32_lc_if_compressed(p->segment);
  return( lc+lc1 );*/
  return( UINT64_lc_if_compressed(p->block) + UINT32_lc_if_compressed(p->segment) );
}

/*static*/ int pntr_lc(levelx_pntr *p, int level)
{
  if ( level==0 ) return(level0_pntr_lc(&(p->p0)));
  else return(leveln_pntr_lc(&(p->pn)));
}

/* uncompressn_pntr uncompresses a leveln pointer.  Note   */
/*   cp points to the first character in the compressed    */
/*   string (unlike compressn_pntr).                       */

static int uncompressn_pntr(struct leveln_pntr *p, unsigned char *cp)
{int lc,lc1;

  lc = uncompress_UINT16(&(p->segment),cp);
  lc1 = uncompress_UINT64(&(p->block),cp+lc);
  /*  memcpy(&(p->block),cp+lc,sizeof(int));*/
  return( lc+lc1 );
}

static int packn_ptr(struct ix_block *b, struct leveln_pntr p)
{int lc;

  lc = compressn_pntr(&p,(unsigned char *) b->keys+(keyspace_lc-b->chars_in_use-1));
  return(lc);
}

int unpackn_ptr(struct ix_block *b, int ix, struct leveln_pntr *p)
{int lc;

  lc = uncompressn_pntr(p,(char *)b->keys+(b->keys[ix].sc+b->keys[ix].lc));
  return(lc);
}

static int pack0_ptr(struct ix_block *b, struct level0_pntr p)
{int lc;

  lc = compress0_pntr(&p,(unsigned char *) b->keys+(keyspace_lc-b->chars_in_use-1));
  return(lc);
}

int unpack0_ptr(struct ix_block *b, int ix, struct level0_pntr *p)
{int lc;

  lc = uncompress0_pntr(p,(char *)b->keys+(b->keys[ix].sc+b->keys[ix].lc));
  return(lc);
}

int unpackx_ptr(struct ix_block *b, int ix, levelx_pntr *p)
{int lc;

  if ( b->level==0 ) lc = uncompress0_pntr((struct level0_pntr *)p,(char *)b->keys+(b->keys[ix].sc+b->keys[ix].lc));
  else lc = uncompressn_pntr((struct leveln_pntr *)p,(char *)b->keys+(b->keys[ix].sc+b->keys[ix].lc));
  return(lc);
}

static int copy_ptr(struct ix_block *b, int ix, struct ix_block *b1)
{unsigned sc,lc;

  sc = b->keys[ix].sc + b->keys[ix].lc;
  lc = compressed_int_lc((char *) b->keys,sc);
  lc = lc + compressed_int_lc((char *) b->keys,sc+lc);
  if ( b->level==0 ) lc = lc + compressed_int_lc((char *) b->keys,sc+lc);
  /*  printf("  in copy_ptr, b.sc=%d, b1.sc=%d, lc=%d\n",
    sc,keyspace_lc-b1->chars_in_use-lc,lc);*/
  mvc(b->keys,sc,b1->keys,keyspace_lc-b1->chars_in_use-lc,lc);
  return(lc);
}

/* Key handling */

boolean eq_key(struct key *k1, struct key *k2)
{
  if ( k1->lc!=k2->lc ) return(false);
  else return( memcmp(&(k1->text),&(k2->text),(size_t)k1->lc)==0 );
}

static void print_lc_key(struct key *k, char caption[])
{struct level0_pntr p;

  unpack_lc_key(k->text,&p);
  printf("%s(free_lc)%u, %u/",caption,p.lc,p.segment);
  printf(UINT64_format,p.sc);
}

static void print_rec_key(struct key *k, char caption[])
{struct level0_pntr p;

  unpack_rec_key(k->text,&p);
  printf("%s(free_rec)%u/",caption,p.segment);
  printf(UINT64_format,p.sc);
}

static void print_user_key(struct key *k, char caption[])
{int i; boolean is_text;

  is_text = true;
  for (i=0; i<k->lc; i++) if ( !isprint(k->text[i]) ) is_text = false;
  printf("%s",caption);
  if ( k->lc==0 ) printf("null");
  else if ( is_text ) for (i=0; i<k->lc; i++) printf("%c",k->text[i]);
  else {
    printf("0x");
    for (i=0; i<k->lc; i++) printf("%2x",k->text[i]);
  }
}

static void print_key(int index, struct key *k, char caption[])
{
  if ( index==user_ix ) print_user_key(k,caption);
  else if ( index==free_rec_ix ) print_rec_key(k,caption);
  else if ( index==free_lc_ix )  print_lc_key(k,caption);
}

/* static void print_nth_key(struct ix_block *b, int n, char caption[])
{struct key k;

  printf("%s",caption);
  if ( n<0 || n>=b->keys_in_block ) printf("undefined");
  else {
    k.lc = b->keys[n].lc;
    mvc(b->keys,b->keys[n].sc,k.text,0,k.lc);
    if ( b->index_type==user_ix ) print_user_key(&k,caption);
    else if ( b->index_type==free_rec_ix ) print_rec_key(&k,caption);
    else if ( b->index_type==free_lc_ix ) print_lc_key(&k,caption);
  }
}*/

/* compare_key compares a key k with the ix^th entry stored in an   */
/*   index block.                                                   */

enum comparison compare_key(struct key *k, struct ix_block *b, int ix)
{int r; unsigned lc1,lc2;

  if ( k->lc<b->prefix_lc ) {
    r = memcmp(k->text,(char *) b->keys+keyspace_lc-b->prefix_lc,(size_t)k->lc );
    if ( r<0 ) return(less);
    else if (r>0) return(greater);
    else return(less);
  }
  else {
    r = memcmp(k->text,(char *) b->keys+keyspace_lc-b->prefix_lc,(size_t)b->prefix_lc );
    if (r<0) return(less);
    else if (r>0) return(greater);
    else {

      lc1 = k->lc - b->prefix_lc;
      lc2 = b->keys[ix].lc;
      if ( lc1<=lc2 ) {
        r = memcmp(k->text+b->prefix_lc,(char *) b->keys+b->keys[ix].sc,(size_t)lc1 );
        if (r<0) return(less);
        else if (r>0) return(greater);
        else if ( lc1==lc2 ) return(equal);
        else return(less);
      }
      else {
        r = memcmp(k->text+b->prefix_lc,(char *) b->keys+b->keys[ix].sc,(size_t)lc2 );
        if (r<0) return(less);
        else if (r>0) return(greater);
        else if ( lc1==lc2 ) return(equal);
        else return(greater);
      }

    }
  }
}

void get_nth_key(struct ix_block *b, struct key *k, int n)
{
  if ( n<0 || n>=b->keys_in_block ) k->lc = 0;
  else {
    mvc(b->keys,keyspace_lc-b->prefix_lc,k->text,0,b->prefix_lc);
    k->lc = b->keys[n].lc + b->prefix_lc;
    mvc(b->keys,b->keys[n].sc,k->text,b->prefix_lc,b->keys[n].lc);
  }
}

/* get_nth_key_lc returns the nth key in block b in k.  It also  */
/*   returns the compressed length of the pointer assocated with */
/*   that key                                                    */

static int get_nth_key_lc(struct ix_block *b, struct key *k, int n)
{int lc=0; struct level0_pntr p0; struct leveln_pntr pn;

  get_nth_key(b,k,n);
  if ( k->lc > 0 ) {
    if ( b->level==0 ) lc = unpack0_ptr(b,n,&p0);
    else lc = unpackn_ptr(b,n,&pn);
  }
  return(lc);
}

/* get_nth_key_and_pntr extracts the nth key and pointer from    */
/*   block b and places them in k and p..  It returns the length */
/*   of the entry (compressed key lc + compressed pointer lc +   */
/*   key_ptr_lc)                                                 */

int get_nth_key_and_pntr(struct ix_block *b, struct key *k, int n, levelx_pntr *p)
{int lc;

  get_nth_key(b,k,n);
  lc = k->lc - b->prefix_lc;
  if ( k->lc > 0 ) {
    if ( b->level==0 ) lc = lc + unpack0_ptr(b,n,&(p->p0)) + key_ptr_lc;
    else lc = lc + unpackn_ptr(b,n,&(p->pn)) + key_ptr_lc;
  }
  return(lc);
}

void get_max_key(struct ix_block *b, struct key *k)
{
  if ( b->keys_in_block<1 ) k->lc = 0;
  else {
    get_nth_key(b,k,b->keys_in_block-1);
  }
}

/* static boolean eq_ix_block(struct ix_block *b, struct ix_block *b1)
{int i,b_lc,b1_lc,temp,key_lc=0,p_lc=0; boolean same; struct key k,k1; levelx_pntr px,px1;

  same = b->keys_in_block==b1->keys_in_block;
  same = same && b->chars_in_use==b1->chars_in_use;
  same = same && b->index_type==b1->index_type;
  same = same && b->prefix_lc==b1->prefix_lc;
  same = same && b->level==b1->level;
  same = same && eq_pntr(b->next,b1->next);
  same = same && eq_pntr(b->prev,b1->prev);
  b_lc = b->prefix_lc;
  b1_lc = b1->prefix_lc;
  same = same && (memcmp((char *)b->keys+keyspace_lc-b_lc,
    (char *)b1->keys+keyspace_lc-b1_lc,(size_t) b_lc)==0);
  for (i=0; i<b->keys_in_block; i++) {
    temp = get_nth_key_and_pntr(b,&k,i,&px);
    key_lc = key_lc + k.lc - b->prefix_lc;
    p_lc = p_lc + pntr_lc(&px,b->level);
    if ( temp!=(k.lc-b->prefix_lc+pntr_lc(&px,b->level)+key_ptr_lc ) )
      printf("    get_nth_lc=%d, k->lc=%d, pntr_lc=%d\n",temp,k.lc,pntr_lc(&px,b->level));
    b_lc = b_lc + get_nth_key_and_pntr(b,&k,i,&px);
    b1_lc = b1_lc + get_nth_key_and_pntr(b1,&k1,i,&px1);
    same = same && eq_key(&k,&k1) && eq_pntr(px.pn,px1.pn);
  }
  same = same && b_lc==b1_lc;
  same = same && b_lc==(b->chars_in_use+(key_ptr_lc*b->keys_in_block));
  if ( !same ) printf("  b_lc=%d, b1.lc=%d, b/b1.keys=%d/%d, key_ptr_lc=%d, b/b1.chars_in_use=%d/%d, tot_key_lc=%d, pntr_lc=%d\n",
    b_lc,b1_lc,b->keys_in_block,b1->keys_in_block,key_ptr_lc,b->chars_in_use,b1->chars_in_use,key_lc,p_lc);
  return(same);
}*/


/**** I/O ***/


/* init_file_name separates the file name and any extension */
/*   and saves the two parts in the fcb                     */

static void init_file_name(struct fcb *f, char id[])
{int i; unsigned name_lc, f_lc, ext_lc = 0;

  name_lc = (unsigned) strlen(id);
  if (name_lc > max_filename_lc + max_extension_lc)
    fatal_error(f,bad_name_err); /* whole thing too long */
  i = name_lc - 1;
  /* scan  from right to left */
  while ( i >= 0 && id[i] != '.') {i--; ext_lc++;}
  if (i >= 0) {
    f_lc = i;
    ext_lc++;
  }
  else {
    f_lc = name_lc;
    ext_lc = 0;
  }
  if (f_lc >= max_filename_lc) fatal_error(f,bad_name_err);
  else {
    strncpy(f->file_name, id, (size_t)f_lc);
    f->file_name[f_lc] = '\0';
  }
  if ( ext_lc >= max_extension_lc ) fatal_error(f,bad_name_err);
  else {
    strncpy(f->file_extension, id + i, (size_t)ext_lc);
    f->file_extension[ext_lc] = '\0';
  }
}

/* build_segment_name builds a segment name by appending the segment */
/*   number to the file name and then appending any extension.       */

static void build_segment_name(struct fcb *f, unsigned segment, char name[])
{int suffix_lc; size_t name_lc;

  strcpy(name,f->file_name);
  if (segment>0) {
    name_lc = strlen(name);
    suffix_lc = sprintf(name+name_lc,"$%d",segment);
    name[name_lc+suffix_lc] = '\0';
  }
  strcat(name,f->file_extension);
}

static void byte_swap_UINT16s(unsigned char s[], int cnt)
{unsigned int i=0; unsigned char ch;

  while ( i<cnt*sizeof(UINT16) ) {
    ch = s[i];
    s[i] = s[i+1];
    s[i+1] = ch;
    i = i + sizeof(UINT16);
  }
}

static void byte_swap_UINT32(unsigned char n[])
{unsigned char ch;

/*  printf("int %d after swapping is",n); */
  ch = n[0];
  n[0] = n[3];
  n[3] = ch;
  ch = n[1];
  n[1] = n[2];
  n[2] = ch;
/*  printf(" %d\n",n); */
 /*  printf("int %d after swapping is %d\n",n.as_int,n1.as_int);*/
}

static void byte_swap_UINT64(unsigned char n[])
{unsigned char ch;

/*  printf("int %d after swapping is",n); */
  ch = n[0];
  n[0] = n[7];
  n[7] = ch;
  ch = n[1];
  n[1] = n[6];
  n[6] = ch;
  ch = n[2];
  n[2] = n[5];
  n[5] = ch;
  ch = n[3];
  n[3] = n[4];
  n[4] = ch;
/*  printf(" %d\n",n); */
 /*  printf("int %d after swapping is %d\n",n.as_int,n1.as_int);*/
}

static unsigned char read_byte(struct fcb *f, FILE *file)
{unsigned char ch=0;

  if ( fread(&ch,sizeof(char),(size_t)1,file)!=1 )
    set_error(f,read_err,"read_byte failed\n");
  return(ch);
}


static UINT16 read_UINT16(struct fcb *f, FILE *file)
{UINT16 n; unsigned char ch;
 unsigned char *p = (unsigned char *)&n;

  if ( fread(&n,sizeof(UINT16),(size_t)1,file)!=1 ) {
    set_error(f,read_err,"read_UINT16 failed\n");
    return(0);
  }
  if ( f->byte_swapping_required ) {
    ch = p[1];
    p[1] = p[0];
    p[0] = ch;
  }
  return(n);
}


static UINT32 read_UINT32(struct fcb *f, FILE *file)
{UINT32 n;

  if ( fread(&n,sizeof(UINT32),(size_t)1,file)!=1 ) {
    set_error(f,read_err,"read_UINT32 failed\n");
    return(0);
  }
  if ( f->byte_swapping_required ) byte_swap_UINT32((unsigned char *) &n);
  return(n);
}

static UINT64 read_UINT64(struct fcb *f, FILE *file)
{UINT64 n;

  if ( fread(&n,sizeof(UINT64),(size_t)1,file)!=1 ) {
    set_error(f,read_err,"read_UINT64 failed\n");
    return(0);
  }
  if ( f->byte_swapping_required ) byte_swap_UINT64((unsigned char *) &n);
  return(n);
}

static boolean read_fib(struct fcb *f,char id[], boolean byte_swapping_required,
  boolean read_only)
{int i,j; FILE_OFFSET position; FILE *file;

  file = fopen(id,"rb");
  if ( file==NULL ) f->error_code = badopen_err;
  else if ( fseeko(file,(FILE_OFFSET) 0,0)!=0 ) f->error_code = badopen_err;
  else {
    f->byte_swapping_required = byte_swapping_required;
    f->read_only = read_only;

    f->error_code = read_UINT32(f,file);
    f->version = read_UINT32(f,file);
    f->segment_cnt = read_UINT32(f,file);
    for ( i=0; i<max_index; i++) f->primary_level[i] = read_UINT32(f,file);
    f->marker = read_UINT32(f,file);
    f->file_ok = read_UINT32(f,file);
    for (i=0; i<max_level; i++)
      for (j=0; j<max_index; j++) {
        f->first_free_block[i][j].segment = read_UINT16(f,file);
        f->first_free_block[i][j].block = read_UINT64(f,file);
      }
    for (i=0; i<max_level; i++)
      for (j=0; j<max_index; j++) {
        f->first_at_level[i][j].segment = read_UINT16(f,file);
        f->first_at_level[i][j].block = read_UINT64(f,file);
      }
    for (i=0; i<max_level; i++)
      for (j=0; j<max_index; j++) {
        f->last_pntr[i][j].segment = read_UINT16(f,file);
        f->last_pntr[i][j].block = read_UINT64(f,file);
      }
    f->max_file_lc = read_UINT64(f,file);
    for (i=0; i<max_segments; i++) f->segment_length[i] = read_UINT64(f,file);
    position = ftello(file);
    if ( position!=fib_lc ) set_error1(f,badopen_err,"**Unable to read fib, position=",(int)position);
    fclose(file);
  }
  return(f->error_code==no_err);
}

static void write_UINT16(struct fcb *f, FILE *file, UINT16 *i)
{UINT16 n; unsigned char ch;
 unsigned char *p = (unsigned char *)&n;

  n = *i;
  if ( f->byte_swapping_required ) {
    ch = p[0];
    p[0] = p[1];
    p[1] = ch;
  }
  if ( fwrite(&n,sizeof(UINT16),(size_t)1,file)!=1 )
    set_error(f,write_err,"write failed in write_UINT16\n");
}

static void write_UINT16s(struct fcb *f, FILE *file, unsigned char s[], unsigned int cnt)
{unsigned int i; unsigned char swapped[keyspace_lc];

  if ( f->byte_swapping_required ) {
    i = 0;
    while ( i<cnt*sizeof(UINT16) ) {
      swapped[i] = s[i+1];
      swapped[i+1] = s[i];
      i = i + sizeof(UINT16);
    }
    if ( fwrite(swapped,sizeof(UINT16),(size_t)cnt,file)!=cnt )
    set_error(f,write_err,"write_UINT16s failed\n");
  }
  else {
    if ( fwrite(s,sizeof(UINT16),(size_t)cnt,file)!=cnt )
      set_error(f,write_err,"write_UINT16s failed\n");
  }
}

static void write_UINT32(struct fcb *f, FILE *file, UINT32 i)
{UINT32 n;

  n = i;
  if ( f->byte_swapping_required ) byte_swap_UINT32((unsigned char *) &n);
  if ( fwrite(&n,sizeof(UINT32),(size_t)1,file)!= 1 )
    set_error(f,write_err,"write failed in write_UINT32\n");
}

static void write_UINT64(struct fcb *f, FILE *file, UINT64 i)
{UINT64 n;

  n = i;
  if ( f->byte_swapping_required ) byte_swap_UINT64((unsigned char *) &n);
  if ( fwrite(&n,sizeof(UINT64),(size_t)1,file)!= 1 )
    set_error(f,write_err,"write failed in write_UINT64\n");
}

static void write_fib(struct fcb *f)
{int i,j,fill_cnt; FILE_OFFSET position; FILE *file;

  if ( f->error_code!=no_err ) return;
  else if ( f->read_only ) return;
  else {
    file = file_index(f,0);
    if ( file==NULL ) set_error(f,bad_close_err,"**Bad file in write_fib\n");
    else if ( fseeko(file,(FILE_OFFSET) 0,0)!=0 ) set_error(f,bad_close_err,"**Couldn't seek to fib\n");
    else {
      /*      f->byte_swapping_required = false;*/

      write_UINT32(f,file,f->error_code);
      write_UINT32(f,file,f->version);
      write_UINT32(f,file,f->segment_cnt);
      for ( i=0; i<max_index; i++) write_UINT32(f,file,f->primary_level[i]);
      write_UINT32(f,file,f->marker);
      write_UINT32(f,file,f->file_ok);
      for (i=0; i<max_level; i++)
        for (j=0; j<max_index; j++) {
          write_UINT16(f,file,&(f->first_free_block[i][j].segment));
          write_UINT64(f,file,f->first_free_block[i][j].block);
        }
      for (i=0; i<max_level; i++)
        for (j=0; j<max_index; j++) {
          write_UINT16(f,file,&(f->first_at_level[i][j].segment));
          write_UINT64(f,file,f->first_at_level[i][j].block);
        }
      for (i=0; i<max_level; i++)
        for (j=0; j<max_index; j++) {
          write_UINT16(f,file,&(f->last_pntr[i][j].segment));
          write_UINT64(f,file,f->last_pntr[i][j].block);
        }
      write_UINT64(f,file,f->max_file_lc);
      for (i=0; i<max_segments; i++) write_UINT64(f,file,f->segment_length[i]);
      position = ftello(file);
      if ( position!=fib_lc ) set_error(f,bad_close_err,"**Wrong fib length on close\n");

      /*      printf("**fib_lc=%d, position=%u\n",(int)fib_lc,(int)position);*/

      fill_cnt = ((fib_blocks * block_lc) - fib_lc) / sizeof(int);
      for (i=0; i<fill_cnt; i++) write_UINT32(f,file,0);
    }
  }
}

void read_page(struct fcb *f, struct leveln_pntr p, block_type_t *buf)
{FILE *file; FILE_OFFSET offset;

 read_cnt++;
  if ( f->trace ) {
    print_leveln_pntr("reading page ",&p);
    printf("\n");
  }
  file = file_index(f,p.segment);
  offset = (p.block) << f->block_shift;
  if ( file==NULL ) set_error(f,read_err,"**Bad file in read_page\n");
  else if ( fseeko(file,offset,0)!=0 )
    set_error(f,seek_err,"**Seek failed in read_page\n");
  else {
    buf->ix.keys_in_block = read_UINT16(f,file);
    buf->ix.chars_in_use = read_UINT16(f,file);
    buf->ix.index_type = read_byte(f,file);
    buf->ix.prefix_lc = read_byte(f,file);
    buf->ix.unused = read_byte(f,file);
    buf->ix.level = read_byte(f,file);
    buf->ix.next.segment = read_UINT16(f,file);
    buf->ix.next.block = read_UINT64(f,file);
    buf->ix.prev.segment = read_UINT16(f,file);
    buf->ix.prev.block = read_UINT64(f,file);
    fread(buf->ix.keys,(size_t) 1, (size_t) keyspace_lc,file);
    if ( ftello(file)!=(FILE_OFFSET)(offset+block_lc) )
      set_error1(f,read_err,"**I/O failure in read_page, bytes read=",(int)(ftello(file)-offset));
    if ( f->byte_swapping_required )
      byte_swap_UINT16s((char *)buf->ix.keys,buf->ix.keys_in_block*2);
  }
}

static void write_page(struct fcb *f, struct leveln_pntr p, block_type_t *buf)
{int pntr_lc,remaining; FILE *file; FILE_OFFSET offset;

  write_cnt++;
  if ( f->read_only ) {
    f->error_code = read_only_err;
    return;
  }
  if ( f->trace ) {
    print_leveln_pntr("writing page ",&p);
    printf("\n");
  }
  file = file_index(f,p.segment);
  offset = (p.block) << f->block_shift;
  if ( file==NULL ) set_error(f,write_err,"**Bad file in write_page\n");
  else if ( fseeko(file,offset,0)!=0 )
    set_error(f,seek_err,"**Seek error in write_page\n");
  else {
    write_UINT16(f,file,&(buf->ix.keys_in_block));
    write_UINT16(f,file,&(buf->ix.chars_in_use));
    if ( fwrite(&(buf->ix.index_type),sizeof(char),(size_t)1,file)!=1 ) set_error(f,write_err,"write byte failed\n");
    if ( fwrite(&(buf->ix.prefix_lc), sizeof(char),(size_t)1,file)!=1 ) set_error(f,write_err,"write byte failed\n");
    if ( fwrite(&(buf->ix.unused),sizeof(char),(size_t)1,file)!=1 ) set_error(f,write_err,"write byte failed\n");
    if ( fwrite(&(buf->ix.level),     sizeof(char),(size_t)1,file)!=1 ) set_error(f,write_err,"write byte failed\n");
    write_UINT16(f,file,&(buf->ix.next.segment));
    write_UINT64(f,file,buf->ix.next.block);
    write_UINT16(f,file,&(buf->ix.prev.segment));
    write_UINT64(f,file,buf->ix.prev.block);
    write_UINT16s(f,file,(char *)buf->ix.keys,(unsigned)buf->ix.keys_in_block*2);
    pntr_lc = buf->ix.keys_in_block * key_ptr_lc;
    remaining = keyspace_lc - pntr_lc;
    fwrite(buf->ix.keys+buf->ix.keys_in_block,(size_t) 1, (size_t) remaining,file);
    if ( ftello(file)!=(FILE_OFFSET)(offset+block_lc) )
      set_error1(f,read_err,"**I/O failure in write_page, bytes written=",(int)(ftello(file)-offset));
  }
}

/*void flush_kf_buffers(struct fcb *f)
{int i;

  for (i=0; i<f->buffers_in_use; i++)
    if (f->buffer[i].modified && (f->buffer[i].lock_cnt==0) ) {
      write_page(f,f->buffer[i].contents,&(f->buffer[i].b) );
      f->buffer[i].modified = false;
    }
}*/


#define flush_window 256

/*static int write_page_and_flush(struct fcb *f, struct leveln_pntr p, void *buf)
{int i,min_block,max_block,nbr_cnt=0,nbr_list[flush_window]; struct leveln_pntr pn;

  min_block = (p.block / flush_window) * flush_window;
  max_block = min_block + flush_window;
  for (i=0; i<f->buffers_in_use; i++)
    if (f->buffer[i].modified && (f->buffer[i].lock_cnt==0) ) {
      pn = f->buffer[i].contents;
      if ( pn.segment==p.segment && pn.block>=min_block && pn.block<=max_block ) {
        nbr_list[nbr_cnt] = i;
        nbr_cnt++;
      }
    }
  for (i=0; i<nbr_cnt; i++) {
    write_page(f,f->buffer[nbr_list[i]].contents,&(f->buffer[nbr_list[i]].b) );
    f->buffer[nbr_list[i]].modified = false;
  }
  return(nbr_cnt);
}*/

/* vacate_file_index finds the LRU file_index, closes the segment */
/*   currently in use, marks the segment as closed and returns an */
/*   index into open_file to be used for segment I/O              */

static int vacate_file_index(struct fcb *f)
{int i,oldest,age,max_age;

  oldest = 0; max_age = 0;
  for ( i=0; i<f->open_file_cnt; i++ ) {
    age = f->current_age - f->file_age[i];
    if ( age>max_age ) {
      oldest = i; max_age = age;
    }
  }
  f->segment_ix[f->open_segment[oldest]] = max_files;
  fclose(f->open_file[oldest]);
  return(oldest);
}

/* open_segment opens a file segment.  If it is new it is opened in */
/*   write+ mode otherwise it is opened in read+ mode.  If the open */
/*   fails then f->open_file[ix] is set to NULL and f->error_code is*/
/*   set.  In any case the directories segment_ix[] and             */
/*   open_segment[] are set.  */

static void open_segment(struct fcb *f, unsigned segment, int ix)
{char name[max_filename_lc+10]; char* mode;

  build_segment_name(f,segment,name);
  if ( segment >= (unsigned)f->segment_cnt ) {
    if ( f->read_only ) {
      f->error_code = read_only_err;
      return;
    }
    else f->open_file[ix] = fopen(name,"wb+");
  }
  else {
    mode = f->read_only ? "rb" : "rb+";
    f->open_file[ix] = fopen(name,mode);
  }
  if (f->open_file[ix]==NULL) {
    f->error_code = segment_open_err;
  }
  f->segment_ix[segment] = ix;
  f->open_segment[ix] = segment;
  if ( f->trace)  printf("Opening segment %s on file index %d\n",name,ix);
}

static int file_ix(struct fcb *f, unsigned segment)
{int ix;

  ix = f->segment_ix[segment];
  if (ix<max_files) /* have a file open */;
  else if (f->open_file_cnt<max_files) {
    ix = f->open_file_cnt;
    f->open_file_cnt++;
    open_segment(f,segment,ix);
  }
  else {
    ix = vacate_file_index(f);
    open_segment(f,segment,ix);
  }
  f->file_age[ix] = f->current_age;
  if ( f->trace ) printf("  segment %d open on file index %d\n",segment,ix);
  return(ix);
}


/* file_index returns a file index to be used for I/O to a given   */
/*   segment of the keyed file.  If there is a file index open for */
/*   the segment, it is returned.  If there is an unused file   */
/*   index it is opened for the segment and returned.  Otherwise   */
/*   the LRU index is closed and reopened for the segment.         */

static FILE *file_index(struct fcb *f, unsigned segment)
{int ix;

  if ( segment<max_segments ) {
    ix = file_ix(f,segment);
    return(f->open_file[ix]);
  }
  else return(NULL);
}

static void set_position(struct fcb *f, int index, struct leveln_pntr b, int ix)
{
  f->position[index] = b; f->position_ix[index] = ix;
}


/* Buffer handling */

/* reset_ages is called when f->current_age reaches INT_MAX.    */
/*   The age of all open files is set to 0 and                  */
/*   f->current_age is set to 0.                                */

static void reset_ages(struct fcb *f)
{int i;

/*  printf("**Resetting ages\n");*/
  for (i=0; i<f->open_file_cnt; i++) f->file_age[i] = 0;
  f->current_age = 0;
}


/* static int hash_value(struct fcb *f, struct leveln_pntr block)
{int k;

  k =  ((block.block / block_lc) + block.segment)% f->buffers_allocated;
  return(k);
} */

#define hash_value(b,limit)  ((((int)b.block) + b.segment) % limit)


static int search_hash_chain(struct fcb *f, struct leveln_pntr block)
{int k,next,bufix=-1/*,cnt=0*/;

  
  k =  hash_value(block,f->buf_hash_entries);
  next = f->buf_hash_table[k];
  while ( next>=0 ) {
    if ( eq_pntr(f->buffer[next].contents,block) ) {
      bufix = next; next = -1;
    }
    else next = f->buffer[next].hash_next;
  }
  /*  printf("  searched hash chain %d for %d/%d, ",k,block.segment,block.block);
  if ( bufix<0 ) printf("not found, cnt=%d\n",cnt);
  else  printf("found in bufix=%d, cnt=%d\n",bufix,cnt);*/
  return(bufix);
}

/* hash_chain_insrt inserts a buffer in a hash_chain  */
/*   The block number is hashed and the hash table is */
/*   checked.  If the hash table entry<0 then the     */
/*   chain is empty and the buffer inserted as a      */
/*   chain of length one.  If the entry>=0 then the   */
/*   chain is searched and the buffer inserted.       */

static void hash_chain_insert(struct fcb *f, int bufix)
{int k,next,last=-1; struct leveln_pntr block;

  block = f->buffer[bufix].contents;
  k = hash_value(block,f->buf_hash_entries);
  next = f->buf_hash_table[k];
  if ( next<0 ) {
    f->buf_hash_table[k] = bufix;
    f->buffer[bufix].hash_next = -1;
  }
  else {
    while ( next>=0 && gt_pntr(block,f->buffer[next].contents) ) {
      last = next; next = f->buffer[next].hash_next;
    }
    if ( last<0 ) {
      f->buffer[bufix].hash_next = f->buf_hash_table[k];
      f->buf_hash_table[k] = bufix;
    }
    else {
      f->buffer[last].hash_next = bufix;
      f->buffer[bufix].hash_next = next;
    }
  }
  if ( f->trace ) {
    printf("  inserted buffer %d (",bufix);
    print_leveln_pntr("",&block);
    printf(") into hash chain %d\n",k);
    print_hash_chain(stdout,f,k);
  }
}

/* remove the buffer with index bufix from its hash chain */

static void hash_chain_remove(struct fcb *f, int bufix)
{int k,next,last=0; struct leveln_pntr block;

  block = f->buffer[bufix].contents;
  k = hash_value(block,f->buf_hash_entries);
  if ( f->trace ) {
    printf("Removing buffer %d from hash chain %d\n",bufix,k);
    printf("  old "); print_hash_chain(stdout,f,k);
  }
  next = f->buf_hash_table[k];
  if ( next==bufix ) f->buf_hash_table[k] = f->buffer[bufix].hash_next;
  else {
    while ( (next>=0) && !eq_pntr(block,f->buffer[next].contents) ) {
      last = next; next = f->buffer[next].hash_next;
    }
    if ( next<0 ) {
      if ( show_errors )
        printf("**Tried to remove nonexistent buffer (%d) from hash chain %d\n",bufix,k);
    }
    else f->buffer[last].hash_next = f->buffer[next].hash_next;
  }
  if ( f->trace ) {
    printf("  new "); print_hash_chain(stdout,f,k);
  }
  f->buffer[bufix].hash_next = -1;
}

/* make_buffer_youngest removes the buffer in bufix from the */
/*   age chain and inserts it as the youngest buffer         */

static void make_buffer_youngest(struct fcb *f,int bufix)
{int older,younger;

  older = f->buffer[bufix].older;
  younger = f->buffer[bufix].younger;
  if ( younger>=0 ) { /* not allready youngest */
    if ( older==-1 ) {
      f->oldest_buffer = younger;
      f->buffer[younger].older = -1;
    }
    else {
      f->buffer[older].younger = younger;
      f->buffer[younger].older = older;
    }
    f->buffer[f->youngest_buffer].younger = bufix;
    f->buffer[bufix].younger = -1;
    f->buffer[bufix].older = f->youngest_buffer;
    f->youngest_buffer = bufix;
  }
}

static void init_buffer(struct fcb *f,int i)
{
  f->buffer[i].contents = nulln_ptr;
  f->buffer[i].lock_cnt = 0;
  f->buffer[i].modified = false;
  f->buffer[i].older = -1;
  f->buffer[i].younger = -1;
  f->buffer[i].hash_next = -1;
}

/* vacate_oldest_buffer is called when a new buffer is needed */
/*   if there are unallocated buffers then the next one is   */
/*   added to the buffer chain and returned.  If all buffers */
/*   are in use then the oldest unlocked buffer is flushed   */
/*   (if necessary) and returned                             */

static int vacate_oldest_buffer(struct fcb *f)
{int oldest,cnt=0,locked_cnt=0,i; boolean done;

  if ( f->buffers_in_use < f->buffers_allocated ) {
    oldest = f->buffers_in_use;
    init_buffer(f,oldest);
    if ( f->buffers_in_use==0 ) {
      f->youngest_buffer = oldest; f->oldest_buffer = oldest;
    }
    else {
      f->buffer[f->youngest_buffer].younger = oldest;
      f->buffer[oldest].older = f->youngest_buffer;
      f->youngest_buffer = oldest;
    }
    f->buffers_in_use++;
  }
  else {
    do {
      oldest = f->oldest_buffer;
      make_buffer_youngest(f,oldest);
      cnt++;
      if ( cnt>f->buffers_allocated ) {
        done = true; 
        f->error_code = alloc_buf_err;
        if ( show_errors )
          printf("**couldn't allocate a buffer, allocated=%d, locked=%d\n",
            f->buffers_allocated,locked_cnt);
      }
      else if ( f->buffer[oldest].lock_cnt>0 ) {
        done = false; locked_cnt++;
      }
      else done = true;
    }
    while ( !done );
    if ( f->buffer[oldest].modified ) {
      write_page(f,f->buffer[oldest].contents,&(f->buffer[oldest].b));

      /*      printf("  wrote block %u/%u ",f->buffer[oldest].contents.segment,f->buffer[oldest].contents.block);
      print_buffer_caption(stdout,f,oldest);
      printf(" from buffer %d\n",oldest);*/


      /*      i = write_page_and_flush(f,f->buffer[oldest].contents,&(f->buffer[oldest].b));*/
      if ( trace_io ) {
        print_leveln_pntr("  wrote block ",&(f->buffer[oldest].contents));
        print_buffer_caption(stdout,f,oldest);
        printf(" from buffer %d, %d others in window\n",oldest,i);
      }
    }
    hash_chain_remove(f,oldest);
  }
  if ( f->trace ) printf("  just vacated oldest buffer, bufix=%d\n",oldest);
  return(oldest);
}

static int get_buffer(struct fcb *f, struct leveln_pntr block, boolean *not_found)
{int bufix;

/*  get_buffer_cnt++;*/

  f->current_age++;
  if ( f->current_age==INT_MAX ) reset_ages(f);
  bufix = search_hash_chain(f,block);
  *not_found = bufix<0 ;

  if ( *not_found ) {

    /*    replace_buffer_cnt++;*/

    bufix = vacate_oldest_buffer(f);
    f->buffer[bufix].contents = block;
    f->buffer[bufix].modified = false;
    hash_chain_insert(f,bufix);
  }
  else make_buffer_youngest(f,bufix);
  return(bufix);
}

static void set_empty_block_prefix(struct ix_block *b, struct key *prefix, unsigned prefix_lc)
{
  mvc(prefix->text,0,b->keys,keyspace_lc-prefix_lc,prefix_lc);
  b->chars_in_use = prefix_lc;
  b->prefix_lc = prefix_lc;
}

static void initialize_index_block(struct ix_block *b, int index, unsigned lvl,
  struct key *prefix, unsigned prefix_lc)
{
  set_empty_block_prefix(b,prefix,prefix_lc);
  b->keys_in_block = 0;
  b->index_type = index;
  b->level = lvl;
  b->next = nulln_ptr;
  b->prev = nulln_ptr;
}

static int get_index(struct fcb *f, struct leveln_pntr b)
{boolean not_found; int bufix,index_type; struct key dummy;

/*  get_index_cnt++; */
  bufix = get_buffer(f,b,&not_found);
  if ( not_found ) {
    read_page(f,b,&(f->buffer[bufix].b));
    if ( trace_io ) {
      print_leveln_pntr("  read block ",&b);
      print_buffer_caption(stdout,f,bufix);
      printf(" into buffer %d\n",bufix);
    }
  }
  if ( f->error_code==no_err ) {
    index_type = f->buffer[bufix].b.ix.index_type;
    f->mru_at_level[f->buffer[bufix].b.ix.level][index_type] = b;
  }
  else initialize_index_block(&(f->buffer[bufix].b.ix),user_ix,level_zero,&dummy,(unsigned)0);
  return(bufix);
}

int kf_wire_index(struct fcb *f, struct leveln_pntr b)
{int bufix;

/*  get_index_cnt++; */
  bufix = get_index(f,b);
  f->buffer[bufix].lock_cnt = 1;
  return(bufix);
}

/* Prefix compression.  Keys in a block may have a common prefix that */
/*   is omitted when stored.   */
/*                                                                    */
/*   On block splits the prefix can never get shorter since the max   */
/*   key for the split blocks is the same as for the original block.  */

static int find_prefix_lc(struct key *k1, struct key *k2)
{int i=0,max_lc;

  max_lc = min(k1->lc,k2->lc);
  max_lc = min(max_lc,max_prefix_lc);
  while ( i<max_lc && (k1->text[i]==k2->text[i]) ) i++;
  return(i);
}

static int block_prefix_lc(struct ix_block *b)
{struct key first,last;

  if ( b->keys_in_block>1 ) {
    get_nth_key(b,&first,0);
    get_max_key(b,&last);
    return( find_prefix_lc(&first,&last) );
  }
  else return(b->prefix_lc);
}

static unsigned ix_pool_lc(struct ix_block *b)
{
  return(b->chars_in_use + (b->keys_in_block * key_ptr_lc));
}

/* ix_pool_lc_after_insert returns the size that an ix_pool would */
/*   be if key k and pointer p were inserted in entry ix of block */
/*   b.  If the insertion is at either the beginning or end of    */
/*   block then any change in length due to a new prefix must be  */
/*   computed.  The returned size may exceed keyspace_lc.  The    */
/*   prefix_lc of the block after insertion is returned in        */
/*   new_prefix_lc.                                               */

static int ix_pool_lc_after_insert(struct ix_block *b, struct key *k,
  levelx_pntr *p, int ix, int *new_prefix_lc, boolean trace)
{int needed,pool_lc,prefix_difference=0,save_pool_lc; struct key min,max;

  needed = k->lc - b->prefix_lc + pntr_lc(p,b->level) + key_ptr_lc;

  pool_lc = ix_pool_lc(b) + needed;
  save_pool_lc = pool_lc;
  if ( ix==0 ) {
    get_max_key(b,&max);
    prefix_difference = b->prefix_lc - find_prefix_lc(k,&max);
    pool_lc = pool_lc + (prefix_difference * (b->keys_in_block+1)) - prefix_difference;
  }
  else if ( ix==b->keys_in_block ) {
    get_nth_key(b,&min,0);
    prefix_difference = b->prefix_lc - find_prefix_lc(&min,k);
    pool_lc = pool_lc + (prefix_difference * (b->keys_in_block+1)) - prefix_difference;
  }
  if ( trace ) {
    printf("in ix_pool_aft_insert, needed=%d, k->lc=%d, orig ix_pool(b)=%d, prefix_lc=%d/%d, ",
      needed,k->lc,ix_pool_lc(b),b->prefix_lc,b->prefix_lc-prefix_difference);
    printf("keys_in_block=%d, ix=%d\n  insert pool before prefix adjustment=%d, after=%d\n",
      b->keys_in_block,ix,save_pool_lc,pool_lc);
  }
  *new_prefix_lc = b->prefix_lc - prefix_difference;
  return( pool_lc );
}

/* ix_pool_lc_after_replace returns the size that an ix_pool would */
/*   be if key k and pointer p replace those currently in entry ix */
/*   of block b.  If the replace is at either the beginning or end */
/*   of block then any change in length due to a new prefix must be*/
/*   computed.  The returned size may exceed keyspace_lc.  The     */
/*   prefix_lc of the block after replacement is returned in       */
/*   new_prefix_lc.                                                */

static int ix_pool_lc_after_replace(struct ix_block *b, struct key *k, levelx_pntr *p,
  int ix, int *new_prefix_lc, boolean trace)
{int pool_lc,difference,pntr_difference,key_difference,prefix_difference=0,save_pool_lc;
struct key min,max; struct leveln_pntr pn; struct level0_pntr p0;

  if ( b->level==0 ) pntr_difference = pntr_lc(p,b->level) - unpack0_ptr(b,ix,&p0);
  else pntr_difference = pntr_lc(p,b->level) - unpackn_ptr(b,ix,&pn);
  key_difference =  (k->lc - b->prefix_lc - b->keys[ix].lc);
  difference =  key_difference + pntr_difference;

  pool_lc = ix_pool_lc(b) + difference;
  save_pool_lc = pool_lc;
  if ( ix==0 ) {
    get_max_key(b,&max);
    prefix_difference = b->prefix_lc - find_prefix_lc(k,&max);
    pool_lc = pool_lc + (prefix_difference * b->keys_in_block) - prefix_difference;
  }
  else if ( ix==b->keys_in_block-1 ) {
    get_nth_key(b,&min,0);
    prefix_difference = b->prefix_lc - find_prefix_lc(&min,k);
    pool_lc = pool_lc + (prefix_difference * b->keys_in_block) - prefix_difference;
  }
  if ( trace ) {
    printf("in ix_pool_aft_repl, diff=%d, k->lc=%d, orig ix_pool(b)=%d, prefix_lc=%d/%d, keys_in_block=%d, ix=%d\n",
      difference,k->lc,ix_pool_lc(b),b->prefix_lc,b->prefix_lc-prefix_difference,b->keys_in_block,ix);
    printf("  insert pool before prefix adjustment=%d, after=%d\n",save_pool_lc,pool_lc);
  }
  *new_prefix_lc = b->prefix_lc - prefix_difference;
  return( pool_lc );
}

/* ix_pool_lc_after_change returns the size that an ix_pool would */
/*   be if block b is modified with key k and pointer p.  If      */
/*   insert=true then k and p are to be inserted in entry ix.  If */
/*   insert=false then k and p replace entry ix */
/*   If the change is at either the beginning or end */
/*   of block then any change in length due to a new prefix must be*/
/*   computed.  The returned size may exceed keyspace_lc.  The     */
/*   prefix_lc of the block after change is returned in       */
/*   new_prefix_lc.                                                */

static int ix_pool_lc_after_change(struct ix_block *b, struct key *k, levelx_pntr *p,
  int ix, int *new_prefix_lc, boolean insert, boolean trace)
{
  if ( insert ) return(ix_pool_lc_after_insert(b,k,p,ix,new_prefix_lc,trace));
  else  return(ix_pool_lc_after_replace(b,k,p,ix,new_prefix_lc,trace));
}

/* Index searching */

/* Interior index blocks contain entries that point to the subtree
 *   containing keys <= entry key (and greater than the previous
 *   entry key).  Each level in the index has an additional pointer
 *   that points to the last subtree -- the subtree containing keys
 *   <= the largest possible key.  This last pointer is not stored
 *   in the index block but is found in last_pntr[].
 */

/*
 * search_block searches the block for the first entry>=k
 *   if k = some entry then  found=true   and ix is entry
 *   if k < some entry then  found=false  and ix is entry
 *   if k > all entries then found=false ix = keys_in_block
 */

static int search_block(struct fcb *f, int bufix, struct key *k, boolean *found, char caller[])
{int mid,high,ix=0; struct ix_block *b;

  *found = false;
  b = &(f->buffer[bufix].b.ix);
  high = b->keys_in_block-1;
  while ( ix<=high ) {
    mid = (ix + high) / 2;
    switch ( compare_key(k,b,mid) ) {
      case greater: ix = mid + 1; break;
      case equal:   ix = mid; *found = true; high = -1; break;
      case less:    high = mid - 1; break;
    }
  }
  /* now ix points to first entry>=k or keys_in_block */
  if ( f->trace ) {
    printf("(%s)searched level %d ",caller,f->buffer[bufix].b.ix.level);
    print_leveln_pntr("block",&(f->buffer[bufix].contents));
    print_key(f->buffer[bufix].b.ix.index_type,k,"for k=");
    if (ix>=b->keys_in_block) printf(" its larger that any in block\n");
    else if ( *found ) printf(" found it, ix=%d\n",ix);
    else printf(" not found, ix=%d\n",ix);
  }
  return(ix);
}

/*
 * search_index searches index blocks down to stop_lvl and returns
 *   a pointer to the block at stop_lvl-1 in which the key lies.
 *   By construction, the key must be smaller than some key in
 *   each block searched unless it is in the rightmost block at
 *   this level.  If a key is larger than any in this level, then
 *   the last_pntr pointer is the returned.
 */

static struct leveln_pntr search_index(struct fcb *f, int index, UINT32 stop_lvl,
  struct key *k)
{struct leveln_pntr child; int ix,bufix; boolean done=false,found;

  child = f->first_at_level[f->primary_level[index]][index];
  if ( stop_lvl<=f->primary_level[index] )
    do {
      bufix = get_index(f,child);
      ix = search_block(f,bufix,k,&found,"search_index");
      done = f->buffer[bufix].b.ix.level<=stop_lvl;
      if ( ix>=f->buffer[bufix].b.ix.keys_in_block ) { /* larger than any key */
        if ( null_pntr(f->buffer[bufix].b.ix.next) )
          child = f->last_pntr[f->buffer[bufix].b.ix.level][index];
	else {
          done = true; f->error_code = max_key_err; child = nulln_ptr;
	  if ( show_errors ) {
            print_key(index,k,"Uh Oh.  k=");
            printf(" is larger than any key in block\n");
	  }
	}
      }
      else unpackn_ptr(&(f->buffer[bufix].b.ix),ix,&child);
    }
    while ( !done );
  return(child);
}

/* static void get_parent_key(struct fcb *f, struct ix_block *mid, struct key *parent_key)
{int parent_level,parent_ix,index,ix; boolean found; struct leveln_pntr p; struct key k;

  parent_key->lc = 0;
  get_nth_key(mid,&k,mid->keys_in_block-1);
  parent_level = mid->level + 1;
  index = mid->index_type;
  p = nulln_ptr;
  if ( parent_level<=f->primary_level[index] ) {
    p = search_index(f,index,parent_level+1,&k);
    parent_ix = get_index(f,p);

    ix = search_block(f,parent_ix,&k,&found,"get_prnt_key");
    get_nth_key(&(f->buffer[parent_ix].b.ix),parent_key,ix);
  }
  if ( f->trace ) {
    print_key(index,&k," parent key for k=");
    print_key(index,parent_key," is ");
    printf(", parent_block is %d/%lu",p.segment,p.block);
    printf("\n");
  }
}*/

static struct leveln_pntr parent_block(struct fcb *f, int bufix, struct key *k)
{int parent_level,index;

/* int parent_ix,ix; boolean found; struct leveln_pntr p;
  parent_level = f->buffer[bufix].b.ix.level + 1;
  index = f->buffer[bufix].b.ix.index_type;
  parent_key->lc = 0; prev_key->lc = 0;
  p = nulln_ptr;
  if ( parent_level<=f->primary_level[index] ) {
    p = search_index(f,index,parent_level+1,k);
    parent_ix = get_index(f,p);

    ix = search_block(f,&(f->buffer[parent_ix].b.ix),k,&found);
    get_nth_key(&(f->buffer[parent_ix].b.ix),parent_key,ix);
    get_nth_key(&(f->buffer[parent_ix].b.ix),prev_key,ix-1);
  }
  if ( f->trace ) {
    print_key(index,k," parent key for k=");
    print_key(index,parent_key," is ");
    print_key(index,prev_key,", prev is ");
    printf("\n");
  }
  if ( !eq_pntr(p,f->mru_at_level[parent_level][index]) )
    printf("**Uh Oh. parent doesn't match MRU pointer, prnt_level=%d,primary_level=%d\n",
      parent_level,f->primary_level[index]);
  return(p);*/

  parent_level = f->buffer[bufix].b.ix.level + 1;
  index = f->buffer[bufix].b.ix.index_type;
  return(f->mru_at_level[parent_level][index]);
}

/* file initialization */

static void set_block_shift(struct fcb *f)
{int i;

  i = block_lc;
  f->block_shift = 0;
  while (i>0) {
    i = i>>1;
    if ( i>0 ) f->block_shift++;
  }
}

static boolean machine_is_little_endian()
{int i=1; unsigned char *p=(char *)&i;

  if ( p[0]==1 ) return(true);
  else return(false);
}


/* init_key initializes the temporary part of the fcb and a few static  */
/*   variables.  It assumes that the fib has been initialized and that  */
/*   the endedness of the machine has been set.                         */

static void init_key(struct fcb *f, char id[], int lc)
{int i,j,hash_target,hash_blocks;;

  if ( !check_fcb(f) ) return;
  if ( f->version!=current_version) { f->error_code = version_err; return; }
  if ( lc<(int)min_fcb_lc ) { f->error_code = smallfcb_err; return; }
  set_block_shift(f);
  f->trace = false; f->trace_freespace = false;
  /*  f->trace = false; f->trace_freespace = true;*/
  f->open_file_cnt = 0;
  init_file_name(f,id);
  for (i=0; i<max_segments; i++) f->segment_ix[i] = max_files;
  f->current_age = 0;
  if ( lc==min_fcb_lc ) f->buffers_allocated = min_buffer_cnt;
  else f->buffers_allocated = min_buffer_cnt + (lc-min_fcb_lc) / buffer_lc;
  f->buffers_allocated = min(f->buffers_allocated,max_buffer_cnt);
  hash_target = f->buffers_allocated * buf_hash_load_factor;
  hash_blocks = ((hash_target - 1) / hash_entries_per_buf) + 1;
  f->buf_hash_table = (int *) &(f->buffer[f->buffers_allocated-hash_blocks]);
  f->buf_hash_entries = hash_blocks * hash_entries_per_buf;
  f->buffers_allocated = f->buffers_allocated - hash_blocks;
  for (i=0; i<f->buf_hash_entries; i++) f->buf_hash_table[i] = -1;
  f->buffers_in_use = 0;
  f->oldest_buffer = -1;
  f->youngest_buffer = -1;
  for (i=0; i<max_level; i++)
    for (j=0; j<max_index; j++)  f->mru_at_level[i][j] = nulln_ptr;
}

/* record moving */

int copy_rec(struct fcb *f,struct level0_pntr *p, unsigned char rec[], int *rec_lc ,unsigned max_rec_lc)
{size_t size; FILE *file;

  if ( check_fcb(f) ) {
    *rec_lc = p->lc;
    if ( (unsigned)*rec_lc>max_rec_lc ) {
      f->error_code = longrec_err; *rec_lc = max_rec_lc;
    }
    if ( *rec_lc<min_disk_rec_lc ) memcpy(rec,&p->sc,(size_t) *rec_lc);
    else {
      file = file_index(f,p->segment);
      if ( f->error_code!=no_err ) return(f->error_code);
      if ( fseeko(file,(FILE_OFFSET)p->sc,0)!=0 ) {
        f->error_code = seek_err; return(f->error_code);
      }
      size = fread(rec,(size_t) 1,(size_t) *rec_lc,file);
      if ( size!=(size_t)*rec_lc ) f->error_code = read_err;
    }
  }
  return(f->error_code);
}


/*** buffer handling ***/

static void lock_buffer(f,bufix)
struct fcb *f; int bufix;
{
  f->buffer[bufix].lock_cnt++;
}

static void unlock_buffer(f,bufix)
struct fcb *f; int bufix;
{
  f->buffer[bufix].lock_cnt--;
}

#define mark_modified(f,bufix) f->buffer[bufix].modified = true

/* static void mark_modified(f,bufix)
struct fcb *f; int bufix;
{
  f->buffer[bufix].modified = true;
}*/

static int get_index_update(struct fcb *f, struct leveln_pntr b)
{int bufix;

  bufix = get_index(f,b); mark_modified(f,bufix); return(bufix);
}

/* Space allocation */

/* extend_file extends the current segment by lc characters or   */
/*   opens a new segment if the current segment is full.  A      */
/*   pointer to the newly allocated block is returned in p.      */
/*   allocate_rec and allocate_block manage                      */
/*   segment_cnt.  Note that nospace_err is set when we allocate */
/*   space in the last possible segment even though the          */
/*   allocation succeeds.  It simplifies error handling.         */
/*   Space is always allocated in units that are an integral     */
/*   number of blocks.  The pointer returned by extend file is a */
/*   block number rather than the offset within the segment.     */
/*   This pointer is used directly for block structured data     */
/*   (index and freespace blocks) but must be converted to an    */
/*   offset for data records.                                    */

static boolean extend_file(struct fcb *f, unsigned lc, struct leveln_pntr *p)
{int current,ix;

  current = f->segment_cnt - 1;
  if ( (f->max_file_lc - f->segment_length[current]) < lc ) { /* segment full */
    if ( f->segment_cnt>=(max_segments-1) ) {
      set_error1(f,nospace_err,"  extend_file failed, segment=",current);
    }
    current++;
    ix = file_ix(f,(unsigned)current); /* opens current segment */
    f->segment_cnt++;
  }
  p->segment =current;
  p->block = f->segment_length[current] >> f->block_shift;
  /*  p->block = f->segment_length[current];*/
  f->segment_length[current] = f->segment_length[current] + lc;
  if ( f->trace ) {
    print_leveln_pntr("  extended file,",p);
    printf("lc=%d\n",lc);
  }
  return(f->error_code!=nospace_err);
}

/* deallocate_block places the block in buffer bufix at the front of the free */
/*   block chain.  It assumes that the buffer is marked modified (it needs    */
/*   to be written to preserve the chain).  Any locking required is handled   */
/*   by the caller.   */

static void deallocate_block(struct fcb *f, int bufix)
{int index_type,level; struct leveln_pntr p;

  index_type = f->buffer[bufix].b.ix.index_type;
  level = f->buffer[bufix].b.ix.level;
  p = f->buffer[bufix].contents;

  if ( f->trace_freespace ) {
    print_leveln_pntr("deallocating block",&p);
    printf("\n");
  }
  f->buffer[bufix].b.ix.next = f->first_free_block[level][index_type];
  f->first_free_block[level][index_type] = p;

}

static int allocate_index_block(struct fcb *f, int index, struct leveln_pntr *b, unsigned lvl,
  struct key *prefix, unsigned prefix_lc)
{int bufix;
/* printf("  allocating index block at level %d\n",lvl); */
  bufix = allocate_block(f,index,lvl);
  initialize_index_block(&(f->buffer[bufix].b.ix),index,lvl,prefix,prefix_lc);

  f->mru_at_level[lvl][index] = f->buffer[bufix].contents;
  *b = f->buffer[bufix].contents;
  return(bufix);
}

/*** index deletion ***/

/* simple_delete deletes the ix^th key from block b and returns */
/*   the number of bytes deleted (compressed key+compressed     */
/*   pointer+key_ptr_lc                                         */

static int simple_delete(struct ix_block *b, int ix)
{int i,key_sc,deleted_lc,deleted_sc,plc; struct level0_pntr p0; struct leveln_pntr pn;

  if (ix>=b->keys_in_block ) {
    if ( show_errors )
      printf("**attempted to simple delete nonexistent key, ix=%d, keys_in_block=%d\n",
        ix,b->keys_in_block);
    return(0);
  }
  else {
    key_sc = keyspace_lc - b->chars_in_use;
    if ( b->level>0 ) plc = unpackn_ptr(b,ix,&pn);
    else plc = unpack0_ptr(b,ix,&p0);
    deleted_lc = b->keys[ix].lc + plc;
    deleted_sc = b->keys[ix].sc;
    mvc(b->keys,key_sc,b->keys,key_sc+deleted_lc,(unsigned)deleted_sc-key_sc);
    b->chars_in_use = b->chars_in_use - deleted_lc;
    b->keys_in_block--;
    for (i=ix; i<b->keys_in_block; i++) b->keys[i] = b->keys[i+1];
    for (i=0; i<b->keys_in_block; i++)
      if (b->keys[i].sc<deleted_sc )
        b->keys[i].sc = b->keys[i].sc + deleted_lc;
    return(deleted_lc+key_ptr_lc);
  }
}

/* remove_primary removes the primary index block and reduces primary_level */
/*   by one (if it is>0).  Note that we do not remove the free_block chain  */
/*   at the old primary level.  If the depth of the index increases later   */
/*   then these free blocks will be reused otherwise they will be unused.   */
/*   They could be removed and added to a chain at a lower level but there  */
/*   cases where the primary is repeatedly removed by a delete then         */
/*   immediately recreated by an insert (esp. in freespace management)      */
/*   which could result in very long freespace chains.                      */

static void remove_primary(struct fcb *f, int index_type)
{int old_primary_level,bufix; struct leveln_pntr b;

  if ( f->trace )
    printf("  removing primary block at level %u, index_type=%d\n",
      f->primary_level[index_type],index_type);
  old_primary_level = f->primary_level[index_type];
  if ( old_primary_level>0 ) {
    b = f->first_at_level[old_primary_level][index_type];
    bufix = get_index_update(f,b);
    f->primary_level[index_type]--;
    deallocate_block(f,bufix);
  }
}

/* remove index block is called when a delete results in an empty index */
/*   block.  Ordinarily the block is removed and the reference to the   */
/*   block in any parent level is removed.  If the block is the last in */
/*   the chain at any level then it is pointed to by last_pntr entry    */
/*   in the parent level and we must make last_pntr point to the        */
/*   previous block at this level and delete the parent reference to    */
/*   that block.  When removing a block from a chain at primary_level-1 */
/*   results in a chain containing a single block then primary_level is */
/*   reduced by one and that singleton becomes the primary.             */

static void remove_index_block(struct fcb *f, struct key *parent_key, int bufix)
{int i,index_type,level,primary_level; struct leveln_pntr next,prev;
struct key max; struct level0_pntr dummy;

  index_type = f->buffer[bufix].b.ix.index_type;
  level = f->buffer[bufix].b.ix.level;
  primary_level = f->primary_level[index_type];
  next = f->buffer[bufix].b.ix.next;
  prev = f->buffer[bufix].b.ix.prev;
  if ( null_pntr(next) && null_pntr(prev) ) { /* only block at this level */
    if ( level==0 && primary_level==0 ) ; /* OK */
    else {
      set_error1(f,bad_dlt_err,"**Hmmm, singleton block at level=",level); 
    }
  }
  else if ( null_pntr(prev) ) { /* first block in chain */
    f->first_at_level[level][index_type] = next;
    i = get_index_update(f,next);
    f->buffer[i].b.ix.prev = nulln_ptr;
    if ( level==primary_level-1 && null_pntr(f->buffer[i].b.ix.next) )
      remove_primary(f,index_type);
    else if ( level<primary_level ) index_delete(f,index_type,*parent_key,&dummy,level+1);
    deallocate_block(f,bufix);
  }
  else if ( null_pntr(next) ) { /* last block in chain */
    i= get_index_update(f,prev);
    f->buffer[i].b.ix.next = nulln_ptr;
    if ( level==primary_level-1 && null_pntr(f->buffer[i].b.ix.prev) )
      remove_primary(f,index_type);
    else if ( level<primary_level ) {
      f->last_pntr[level+1][index_type] = prev;
      get_max_key(&(f->buffer[i].b.ix),&max);
      index_delete(f,index_type,max,&dummy,level+1);
    }
    deallocate_block(f,bufix);
  }
  else { /* somewhere in the middle of chain */
    i= get_index_update(f,prev);
    f->buffer[i].b.ix.next = next;
    i = get_index_update(f,next);
    f->buffer[i].b.ix.prev = prev;
    if ( level<primary_level ) index_delete(f,index_type,*parent_key,&dummy,level+1);
    deallocate_block(f,bufix);
  }
}



/* index_delete removes key k from index block b at level level and   */
/*   returns the pointer p associated with that key.  If key k is not */
/*   found at the given level then error_code is set to dltnokey_err  */
/*   and p is set to null.                                            */

static void index_delete(struct fcb *f, int index_type, struct key k,
  struct level0_pntr *p, UINT32 level)
{int bufix,ix; boolean found,at_end,update_parent; struct key old_max_key,new_separator;
 struct leveln_pntr b;

  *p = null0_ptr;
  b = search_index(f,index_type,level+1,&k);
  bufix = get_index(f,b);
  if ( f->buffer[bufix].b.ix.level!=level ){
    set_error(f,bad_dlt_err,"**Uh oh, wrong level in index delete\n");
  }
  ix = search_block(f,bufix,&k,&found,"ix_delete");
  if ( f->trace ) {
    print_key(index_type,&k,"deleting key=");
    print_leveln_pntr(" from block",&b);
    printf("keys_in_block(before)=%d, found=%d, ix=%d\n",
      f->buffer[bufix].b.ix.keys_in_block,found,ix);
  }
  if ( !found ) f->error_code = dltnokey_err;
  else {
    lock_buffer(f,bufix);
    mark_modified(f,bufix);
    at_end = ix==(f->buffer[bufix].b.ix.keys_in_block-1);
    update_parent = at_end && (ix>0) && (f->primary_level[index_type]>0);
    get_max_key(&(f->buffer[bufix].b.ix),&old_max_key);
    unpack0_ptr(&(f->buffer[bufix].b.ix),ix,p);
    simple_delete(&(f->buffer[bufix].b.ix),ix);
    if ( f->buffer[bufix].b.ix.keys_in_block==0 ) { /* empty block */
      remove_index_block(f,&old_max_key,bufix);
    }
    else if ( ix==0 ) { /* block not empty, deleted first key */
      check_ix_block_compression(&(f->buffer[bufix].b.ix));
    }
    else if ( at_end ) { /* block not empty, deleted last key */
      get_max_key(&(f->buffer[bufix].b.ix),&new_separator);
      check_ix_block_compression(&(f->buffer[bufix].b.ix));
      if ( f->primary_level[index_type]>0 && !null_pntr(f->buffer[bufix].b.ix.next) )
        replace_max_key(f,index_type,&old_max_key,&new_separator,
          f->buffer[bufix].contents,level+1);
    }
    unlock_buffer(f,bufix);
  }
}

/*** index insertion ***/

/* simple_insert inserts key k and pointer p in entry ix of block b.   */
/*   It can be used when a) it is known that the key and pointer will  */
/*   fit in the block and b) when it is known that the key has the     */
/*   same prefix as the block.  Otherwise, prefix_simple_insert should */
/*   be used.                                                          */

static boolean simple_insert(struct ix_block *b, int ix, struct key *k, levelx_pntr p)
{int i,plc,entry_lc; boolean ok=true;

/*  char t[max_prefix_lc];
    if( k->lc<b->prefix_lc ||
      memcmp(k->text,(char *) b->keys+keyspace_lc-b->prefix_lc,(size_t)b->prefix_lc)!=0 ) {
      ok = false;
    if ( show_errors ) {
      print_key(b->index_type,k,"**Uh oh, trying to insert key=");
      printf(" into a block with prefix_lc=%d, prefix=",b->prefix_lc);
      mvc(b->keys,keyspace_lc-b->prefix_lc,t,0,b->prefix_lc);
      for (i=0; i<b->prefix_lc; i++) 
        if ( b->index_type==user_ix ) printf("%c",t[i]);
        else printf("%2x ",t[i]);
      printf("\n");
    }
    }*/

  if ( b->level==0 ) plc = pack0_ptr(b,p.p0);
  else plc = packn_ptr(b,p.pn);
  entry_lc = k->lc - b->prefix_lc + plc;
  if ( (b->chars_in_use+entry_lc+(b->keys_in_block+1)*key_ptr_lc)<=keyspace_lc ) {
    b->keys_in_block++;
    b->chars_in_use = b->chars_in_use + entry_lc;
    for (i=b->keys_in_block-1; i>ix; i--) b->keys[i] = b->keys[i-1];
    b->keys[ix].lc = k->lc - b->prefix_lc;
    b->keys[ix].sc = keyspace_lc - b->chars_in_use;
    mvc(k->text,b->prefix_lc,b->keys,b->keys[ix].sc,b->keys[ix].lc);
  }
  else {
    ok = false;
    if ( show_errors ) printf("**Uh oh, simple_insert overflow\n");
  }
  return(ok);
}

/* move_keys moves cnt keys and pointers from b to b1.  The first key */
/*   moved is the ix_th entry in b which is moved to the ix1_th entry */
/*   in b1.  Entries in b1 are moved if necessary and any prefix      */
/*   difference between the blocks is accounted for.                  */

static boolean move_keys(struct ix_block *b, int ix, struct ix_block *b1, int ix1,
  int cnt)
{int i,current_lc,old_cnt,move_cnt,entry_lc,plc; boolean ok=true; struct key k;

/*  printf("**in move_keys, ix=%d, ix1=%d, cnt=%d, b1 before ptr move is\n",ix,ix1,cnt);
    print_index_block(stdout,b1);*/
  current_lc = ix_pool_lc(b1);
  if ( current_lc+(cnt*key_ptr_lc)<=keyspace_lc ) {
    old_cnt = b1->keys_in_block;
    move_cnt = old_cnt - ix1;
    for (i=1; i<=move_cnt; i++) b1->keys[old_cnt+cnt-i] = b1->keys[old_cnt-i];
    b1->keys_in_block = b1->keys_in_block + cnt;
  }
  /*  printf("**in move_keys, b1 after ptr move is\n");
      print_index_block(stdout,b1);*/

  i = 0;
  while ( ok && i<cnt ) {
    plc = copy_ptr(b,ix+i,b1);
    get_nth_key(b,&k,ix+i);

    entry_lc = k.lc - b1->prefix_lc + plc;
    if ( (b1->chars_in_use+entry_lc+(b1->keys_in_block*key_ptr_lc))<=keyspace_lc ) {
      b1->chars_in_use = b1->chars_in_use + entry_lc;
      b1->keys[ix1+i].lc = k.lc - b1->prefix_lc;
      b1->keys[ix1+i].sc = keyspace_lc - b1->chars_in_use;
      mvc(k.text,b1->prefix_lc,b1->keys,b1->keys[ix1+i].sc,b1->keys[ix1+i].lc);
    }
    else {
      ok = false;
      if ( show_errors ) printf("**Uh oh, move overflow\n");
    }
    i++;
  }
  /*  printf("**in move_keys, b1 after key move is\n");
      print_index_block(stdout,b1);*/

  return(ok);
}

static void delete_keys(struct ix_block *b, int ix, int cnt)
{int i,plc,entry_lc,sc; struct ix_block buf; struct key k;
#define delete_threshold 1

/*  printf("**in delete_keys, ix=%d, cnt=%d, b before is\n",ix,cnt);
    print_index_block(stdout,b);*/
  if ( cnt<=delete_threshold ) {
    for (i=0; i<cnt; i++) simple_delete(b,ix);
  }
  else {
    k.lc = b->prefix_lc;
    mvc((char *)b->keys,keyspace_lc-b->prefix_lc,k.text,0,b->prefix_lc);
    initialize_index_block(&buf,b->index_type,b->level,&k,(unsigned)b->prefix_lc);
    for (i=0; i<ix; i++) {
      plc = copy_ptr(b,i,&buf);
      get_nth_key(b,&k,i);
      entry_lc = k.lc - b->prefix_lc + plc;
      buf.chars_in_use = buf.chars_in_use + entry_lc;
      b->keys[i].lc = k.lc - b->prefix_lc;
      b->keys[i].sc = keyspace_lc - buf.chars_in_use;
      mvc(k.text,b->prefix_lc,buf.keys,b->keys[i].sc,b->keys[i].lc);
    }
    /*    buf.keys_in_block = ix;
  printf("**in delete_keys, ix=%d, cnt=%d, buf after reloc is\n",ix,cnt);
  print_index_block(stdout,&buf);*/
    for (i=ix+cnt; i<b->keys_in_block; i++) {
      plc = copy_ptr(b,i,&buf);
      get_nth_key(b,&k,i);
      entry_lc = k.lc - b->prefix_lc + plc;
      buf.chars_in_use = buf.chars_in_use + entry_lc;
      b->keys[i-cnt].lc = k.lc - b->prefix_lc;
      b->keys[i-cnt].sc = keyspace_lc - buf.chars_in_use;
      mvc(k.text,b->prefix_lc,buf.keys,b->keys[i-cnt].sc,b->keys[i-cnt].lc);
    }
    sc = keyspace_lc - buf.chars_in_use;
    mvc(buf.keys,sc,b->keys,sc,buf.chars_in_use);
    b->keys_in_block = b->keys_in_block - cnt;
    b->chars_in_use = buf.chars_in_use;
  }
  
  /*  printf("**in delete_keys, ix=%d, cnt=%d, b after is\n",ix,cnt);
      print_index_block(stdout,b);*/



  /*  for (i=0; i<cnt; i++) {
    simple_delete(b,ix);
    }*/
}

/* compress_ix_block adds or removes prefixes from keys in an index block  */
/*   to give the block a given prefix_lc.  We assume that all keys in the  */
/*   block have a common prefix of length>=prefix_lc.  Decreasing the      */
/*   prefix length (inserts at either end) is never a problem (all keys    */
/*   have the prefix).  Increasing the prefix length (delete or block      */
/*   split) can only occur if all keys have the new prefix length.         */

static int compress_ix_block(struct ix_block *b, unsigned prefix_lc, boolean trace)
{int i,prefix_difference,pool_sc,old_key_sc,chars_in_use;
 unsigned expected_pool_lc,pntr_lc,old_pntr_sc;
 struct ix_block copy; struct key prefix; char expansion[max_prefix_lc];

  if ( b->prefix_lc==prefix_lc ) { /* do nothing */ }
  else {
    prefix_difference = b->prefix_lc - prefix_lc;
    expected_pool_lc = ix_pool_lc(b) + (prefix_difference * b->keys_in_block) - prefix_difference;
    if ( trace ) {
      printf("Compressing ix block from prefix_lc=%d to %d, pool_lc before=%d, after=%d\n",
        b->prefix_lc,prefix_lc,ix_pool_lc(b),expected_pool_lc);
    }
    if ( expected_pool_lc>keyspace_lc ) {
      if ( show_errors ) printf("**overflow in compress_ix_block\n");
    }
    else if (b->keys_in_block==0 ) { /* nothing to compress */
      b->chars_in_use = 0;
      b->prefix_lc = 0;
    }
    else {
      pool_sc = keyspace_lc - b->chars_in_use;
      mvc(b->keys,pool_sc,copy.keys,pool_sc,b->chars_in_use);
      get_nth_key(b,&prefix,0);
      if ( prefix.lc<prefix_lc && show_errors )
        printf("**key used for prefix compression too short, lc=%d, prefix_lc=%d\n",prefix.lc,prefix_lc);
      b->prefix_lc = prefix_lc;
      b->chars_in_use = prefix_lc;
      chars_in_use = b->chars_in_use;
      mvc(prefix.text,0,b->keys,keyspace_lc-prefix_lc,prefix_lc);
      if ( prefix_difference>0 ) mvc(prefix.text,prefix_lc,expansion,0,(unsigned)prefix_difference);
      for (i=0; i<b->keys_in_block; i++) {

        old_key_sc = b->keys[i].sc;
        old_pntr_sc = old_key_sc + b->keys[i].lc;
        pntr_lc = compressed_int_lc((char *) copy.keys,old_pntr_sc);
        pntr_lc = pntr_lc + compressed_int_lc((char *) copy.keys,old_pntr_sc+pntr_lc);
        if ( b->level==0 ) pntr_lc = pntr_lc + compressed_int_lc((char *) copy.keys,old_pntr_sc+pntr_lc);

        chars_in_use = chars_in_use + pntr_lc + b->keys[i].lc + prefix_difference;

        b->keys[i].sc = keyspace_lc - chars_in_use;
        b->keys[i].lc = b->keys[i].lc + prefix_difference;
        if ( prefix_difference>0 ) {
          mvc(expansion,0,b->keys,b->keys[i].sc,(unsigned)prefix_difference);;
          mvc(copy.keys,old_key_sc,b->keys,b->keys[i].sc+prefix_difference,b->keys[i].lc+pntr_lc-prefix_difference);
	}
        else mvc(copy.keys,old_key_sc-prefix_difference,b->keys,b->keys[i].sc,b->keys[i].lc+pntr_lc);
      }
      b->chars_in_use = chars_in_use;
      /*      if ( ix_pool_lc(b)!=expected_pool_lc && show_errors)
        printf("**compressed ix block lc=%d doesn't match expected=%d, original prefix_lc=%d, prefix_lc=%d, keys=%d, original pool=%d\n",
	ix_pool_lc(b),expected_pool_lc,original_prefix_lc,prefix_lc,b->keys_in_block,original_pool_lc);*/
    }
  }
  return(b->chars_in_use);
}

/* check_ix_block_compression checks that the prefix length recorded in */
/*   the block is consistent with the keys in the block.  If it is not  */
/*   then the block is recompressed.                                    */

static void check_ix_block_compression(struct ix_block *b)
{unsigned prefix_lc;

  if ( b->keys_in_block>1 ) {
    prefix_lc = block_prefix_lc(b);
    if ( prefix_lc!=b->prefix_lc ) {
      compress_ix_block(b,prefix_lc,false);
    }
  }
}


/* prefix_simple_insert inserts key k and pointer p in entry ix of block b.  */
/*   If the insert succeeds then true is returned otherwise false is         */
/*   is returned and block b is unchanged.  If the key prefix does not match */
/*   the prefix used in b then b is uncompressed as far as necessary.  The   */
/*   key is inserted and the block is recompressed.                          */


static boolean prefix_simple_insert(struct ix_block *b, int ix, struct key *k, levelx_pntr p)
{int prefix_difference,actual_lc,expected_lc; unsigned prefix_lc; boolean fits,ok=true;

  fits = (unsigned)ix_pool_lc_after_insert(b,k,&p,ix,&prefix_lc,false) <= keyspace_lc;
  actual_lc = b->chars_in_use;
  if ( fits ) {
    if ( ix==0 || ix==b->keys_in_block ) { /* k inserted at beginning or end */
      prefix_difference = b->prefix_lc - prefix_lc;
      expected_lc = b->chars_in_use + (prefix_difference * b->keys_in_block) - prefix_difference;
      if ( prefix_lc!=b->prefix_lc ) actual_lc = compress_ix_block(b,prefix_lc,false);
      if ( expected_lc!=actual_lc ) {
        ok = false;
        if ( show_errors ) printf("**prefix_simple_insert at end got wrong compressed ix block length\n");
      }
    }
    ok = ok && simple_insert(b,ix,k,p);
    if ( !ok && show_errors ) printf("**insert failed in prefix_simple_insert\n");
  }
  return(fits);
}

/* replace_max_key is used to replace a max_key in a parent block at level. */
/*   The old max_key is used to search for the entry in the parent block.   */
/*   The pointer found must match the child.  The key value is replaced and */
/*   propagated upward if it replaces the max_key in this (parent) block.   */
/*   However, if the key replacement causes this block to split then it is  */
/*   not necessary to propagate further since split_block will update the   */
/*   parent max_key values.  replace_max_key is similar to         */ 
/*   replace_max_key_and_pntr except that it only replaces the key and can  */
/*   ignore the request if the old and new keys are identical.              */

static void replace_max_key(struct fcb *f, int index, struct key *old_key, struct key *new_key,
  struct leveln_pntr child, unsigned level)
{int ix,bufix,new_prefix_lc; unsigned new_lc;
boolean found=false,propagate; struct leveln_pntr p; struct key k; levelx_pntr px,childx;

  if ( level>f->primary_level[index] ){
    set_error1(f,repl_max_key_err,"**trying to replace_max_key in level above primary=",(int) level);
  }
  else if ( !eq_key(old_key,new_key) ) {
    p = search_index(f,index,level+1,old_key);
    bufix = get_index_update(f,p);
    ix = search_block(f,bufix,old_key,&found,"rep_max_key");
    propagate = (ix==f->buffer[bufix].b.ix.keys_in_block-1) && (!null_pntr(f->buffer[bufix].b.ix.next));
    get_nth_key_and_pntr(&(f->buffer[bufix].b.ix),&k,ix,&px);

    if ( f->trace ) {
      printf("  replacing max_key\n");
      print_key(index,old_key,"    old="); printf("\n");
      print_key(index,new_key,"    new=");
      printf("\n    level=%u,",level);
      print_leveln_pntr("child=",&child);
      printf("propagate=%d\n",propagate);
    }
    if ( !found || !eq_pntr(child,px.pn) ) {
      if ( ix==f->buffer[bufix].b.ix.keys_in_block && null_pntr(f->buffer[bufix].b.ix.next) ) {/* ok */}
      else {
        f->error_code = ix_struct_err;
        if ( show_errors ) {  
          printf("**Uh oh. Couldn't find entry in replace_max_key, found=%d, level=%u\n",found,level);
          print_key(index,old_key,"  old key=");
          print_key(index,new_key,", new key=");
          print_leveln_pntr(" child=",&child);
          print_leveln_pntr("px=",&(px.pn));
          printf("\n ix=%d, keys_in_block=%d, ",ix,f->buffer[bufix].b.ix.keys_in_block);
          print_leveln_pntr("next_ptr=",&(f->buffer[bufix].b.ix.next));
          printf("\n");
	}
      }
    }
    else {
      childx.pn = child;
      new_lc = ix_pool_lc_after_replace(&(f->buffer[bufix].b.ix),new_key,&childx,ix,&new_prefix_lc,false);
      if ( new_lc <= keyspace_lc) {
        simple_delete(&(f->buffer[bufix].b.ix),ix);
        if ( !prefix_simple_insert(&(f->buffer[bufix].b.ix),ix,new_key,childx) ) {
          set_error(f,repl_max_key_err,"**prefix_simple_insert failed in replace_max_key\n");
	}
      }
      else {
        split_block(f,new_key,childx,bufix,ix,false);
	propagate = false;
      }
      if ( propagate && level<f->primary_level[index] ) replace_max_key(f,index,old_key,new_key,p,level+1);
    }
  }
}

/* replace_max_key_and_pntr replaces old_key and old_child in an index block */
/*   at level level with new_key and new_child and propogates new_key upward */
/*   if necessary.  Note that the pointer replacement occurs even if         */
/*   old_key==new_key and that it is only the key that is propagated upward. */

static boolean replace_max_key_and_pntr(struct fcb *f, int index, struct key *old_key, struct key *new_key,
  struct leveln_pntr old_child, struct leveln_pntr new_child, unsigned level)
{int ix,bufix,new_prefix_lc; unsigned new_lc;
boolean found=false,propagate,split=false; struct leveln_pntr p; struct key k; levelx_pntr px,childx;

  if ( level>f->primary_level[index] ){
    set_error1(f,repl_max_key_err,"**trying to replace_max_key_and_pntr in level above primary=",(int)level);
  }
  else {
    p = search_index(f,index,level+1,old_key);
    bufix = get_index_update(f,p);
    ix = search_block(f,bufix,old_key,&found,"rep_max_key&pntr");
    propagate = (ix==f->buffer[bufix].b.ix.keys_in_block-1) && (!null_pntr(f->buffer[bufix].b.ix.next));
    get_nth_key_and_pntr(&(f->buffer[bufix].b.ix),&k,ix,&px);

    if ( f->trace ) {
      printf("  replacing max_key_and_pntr\n");
      print_key(index,old_key,"    old=");
      printf("\n");
      print_key(index,new_key,"    new=");
      printf("\n    level=%d, ",level);
      print_leveln_pntr("old_child=",&old_child);
      print_leveln_pntr("new_child=",&new_child);
      printf("propagate=%d\n",propagate);
    }
    if ( !found || !eq_pntr(old_child,px.pn) ) {
      if ( ix==f->buffer[bufix].b.ix.keys_in_block && null_pntr(f->buffer[bufix].b.ix.next) ) {
        if ( !eq_pntr(f->last_pntr[level][index],old_child) ) {
          f->error_code = ix_struct_err;
          if ( show_errors ) {
            printf("**last_pntr[%d][%d]",level,index);
            print_leveln_pntr("=",&(f->last_pntr[level][index]));
            print_leveln_pntr("doesn't match old_child=",&old_child);
            printf("\n");
	  }
	}
        f->last_pntr[level][index] = new_child;
      }
      else {
        f->error_code = ix_struct_err;
        if ( show_errors ) {
          printf("**Uh oh. Couldn't find entry in replace_max_key_and_pntr, found=%d, level=%d\n",found,level);
          print_key(index,old_key,"  old key=");
          print_key(index,new_key,", new key=");
          print_leveln_pntr(" old_child=",&old_child);
          print_leveln_pntr("px=",&(px.pn));
          printf("\n");
	}
      }
    }
    else {
      childx.pn = new_child;
      new_lc = ix_pool_lc_after_replace(&(f->buffer[bufix].b.ix),new_key,&childx,ix,&new_prefix_lc,false);
      if ( new_lc<=keyspace_lc) {
        simple_delete(&(f->buffer[bufix].b.ix),ix);
        if ( !prefix_simple_insert(&(f->buffer[bufix].b.ix),ix,new_key,childx) ) {
          set_error(f,repl_max_key_err,"**prefix_simple_insert failed in replace_max_key_and_pntr\n");
	}
      }
      else {
        split = true;
        split_block(f,new_key,childx,bufix,ix,false);
	propagate = false;
      }
      if ( propagate && level<f->primary_level[index] ) replace_max_key(f,index,old_key,new_key,p,level+1);
    }
  }
  return(split);
}

/* moving keys -- on input we are given two blocks; a mid block from */
/*   which keys will be moved and either a rt or lt block into which */
/*   they will be moved.  We are also given a new key and pointer that */
/*   will either be inserted (insert=true) in position ix or will    */
/*   replace (insert=false) the entry in position ix.  We always do  */
/*   cnt inserts to the new block; one of these inserts will be the  */
/*   new key if ix is in the move range.  When insert=true the total */
/*   number of keys to be distributed between blocks is one greater  */
/*   than the original content.  If ix is in the move range we will  */
/*   do cnt-1 deletes to mid otherwise we will do cnt deletes.  When */
/*   insert is false the end number of keys is the same as the       */
/*   original and we will always do cnt deletes to mid.  Note that   */
/*   if ix is not in the move range then the caller must decide what */
/*   to do with the new key.                                         */

static boolean move_keys_to_right(struct ix_block *mid, struct ix_block *rt, int cnt,
  struct key *new_key, levelx_pntr *new_p, int ix, boolean insert)
{int first,above_ix_cnt,below_ix_cnt; boolean ok=true,move_new_key=false;

  first = mid->keys_in_block - cnt;
  move_new_key = ix >= (first+insert);
  /*  printf("  move_keys_to_right, mid/rt_keys=%d/%d, insert=%d, ix=%d, cnt=%d, move_new=%d\n",mid->keys_in_block,
      rt->keys_in_block,insert,ix,cnt,move_new_key);*/
  if ( cnt==0 ) { /* do nothing */ }
  else if ( move_new_key ) {
    above_ix_cnt = mid->keys_in_block - (ix+1-insert);
    below_ix_cnt = cnt - above_ix_cnt - 1;
    ok = move_keys(mid,ix+1-insert,rt,0,above_ix_cnt);
    ok = ok && simple_insert(rt,0,new_key,*new_p);
    ok = ok && move_keys(mid,ix-below_ix_cnt,rt,0,below_ix_cnt);
    /*    printf("    after move, mid_keys=%d, rt_keys=%d\n",mid->keys_in_block,rt->keys_in_block);*/
    delete_keys(mid,ix-below_ix_cnt,cnt-insert);
    /*    printf("    after delete, mid_keys=%d, rt_keys=%d\n",mid->keys_in_block,rt->keys_in_block);*/
  }
  else {
    ok = move_keys(mid,first,rt,0,cnt);
    /*    printf("    after move, mid_keys=%d, rt_keys=%d\n",mid->keys_in_block,rt->keys_in_block);*/
    delete_keys(mid,first,cnt);
    /*    printf("    after delete, mid_keys=%d, rt_keys=%d\n",mid->keys_in_block,rt->keys_in_block);*/
  }
  if ( !ok && show_errors ) printf("**move_keys failed in move_keys_to_right\n");
  return(move_new_key);
}

static boolean move_keys_to_left(struct ix_block *lt, struct ix_block *mid, int cnt,
  struct key *new_key, levelx_pntr *new_p, int ix, boolean insert)
{int i,next=0,delete_cnt=0; boolean ok=true,moved_new_key=false; struct key k; levelx_pntr p;

  for (i=0; i<cnt; i++)
    if ( next<ix ) {
      get_nth_key_and_pntr(mid,&k,next,&p);
      delete_cnt++;
      ok = ok && simple_insert(lt,lt->keys_in_block,&k,p);
      next++;
    }
    else if ( next==ix ) {
      moved_new_key = true;
      if ( !insert ) delete_cnt++;
      ok = ok && simple_insert(lt,lt->keys_in_block,new_key,*new_p);
      next++;
    }
    else { 
      get_nth_key_and_pntr(mid,&k,next-insert,&p);
      delete_cnt++;
      ok = ok && simple_insert(lt,lt->keys_in_block,&k,p);
      next++;
    }
  delete_keys(mid,0,delete_cnt);
  /*  for (i=0; i<delete_cnt; i++) simple_delete(mid,0);*/
  if ( !ok && show_errors ) printf("**simple insert failed in move_keys_to_left\n");
  return(moved_new_key);
}

/* choose_key selects the nth key from block mid.  If insert==1 then   */
/*   new_key and new_p contain a key/ptr pair that are to be inserted  */
/*   in ix_th entry of mid and n can have values 0..mid->keys_in_block.*/
/*   if insert==0 then the new key/ptr pair will replace the ix^th     */
/*   entry and n can have values 0..mid->keys_in_block-1.  The length  */
/*   of the entire entry selected (key+ptr+key_ptr) is returned.       */

static int choose_key(struct ix_block *mid, struct key *new_key, levelx_pntr *new_p,
  int ix, int n, struct key *choice, boolean insert)
{int lc=0;

  if ( n>=(mid->keys_in_block+insert) && show_errors ) printf("**choosing key from rt block\n");
  if ( n<ix ) lc = get_nth_key_lc(mid,choice,n);
  else if ( n==ix ) {
    *choice = *new_key;
    lc = pntr_lc(new_p,mid->level);
  }
  else if ( n<(mid->keys_in_block+insert) ) lc = get_nth_key_lc(mid,choice,n-insert);
  else choice->lc = 0;
  if ( lc==0 ) return(0);
  else return(lc + choice->lc + key_ptr_lc);
}

/* choose_right_move_cnt figures out how many keys should be moved from */
/*  index block mid to rt in order to make rt have at least target      */
/*  bytes in its index pool.  If insert=true then k is a new key to     */
/*  be inserted in location ix.  Otherwise, k will replace the entry    */
/*  in ix. The number of keys to move is returned, the sizes of mid     */
/*  and rt after moving are returned in mid_lc_in_out and rt_lc_out.       */
/*  and the prefix_lc's after moving are returned in mid_prefix_lc_out  */
/*  and rt_prefix_lc_out.                                               */
/*  Note that new_mid_lc may still exceed max_keyspace.                 */

static int choose_right_move_cnt(struct fcb *f, struct ix_block *mid, struct ix_block *rt,
struct key *k, levelx_pntr *new_p, int ix, boolean insert, unsigned target,
int *mid_lc_in_out, int *mid_prefix_lc_in_out, int *rt_lc_out, int *rt_prefix_lc_out)
{int rt_cnt=0,lc,mid_prefix_lc,rt_prefix_lc,prefix_difference,mid_lc,
 mid_change,rt_change,new_mid_prefix_lc,new_rt_prefix_lc,move_ix;
 unsigned rt_lc;
 boolean done=false;
 struct key min_key,max_key,rt_mid_key,move_key;

 /*  if ( insert ) mid_lc = ix_pool_lc_after_insert(mid,k,new_p,ix,&mid_prefix_lc,false);
     else  mid_lc = ix_pool_lc_after_replace(mid,k,new_p,ix,&mid_prefix_lc,false);*/
  mid_lc = *mid_lc_in_out;
  mid_prefix_lc = *mid_prefix_lc_in_out;
  rt_lc = ix_pool_lc(rt);
  rt_prefix_lc = rt->prefix_lc;

  if ( f->trace ) {
    print_key(mid->index_type,k,"finding right_mov_cnt, insert key=");
    printf(", lc=%d, pntr_lc=%d, ix=%d, level=%d",k->lc,pntr_lc(new_p,mid->level),ix,mid->level);
    printf(" target=%d, mid_lc=%d, rt_lc=%d\n",target,mid_lc,rt_lc);
  }

  choose_key(mid,k,new_p,ix,0,&min_key,insert);

  if ( rt->keys_in_block>0 ) get_max_key(rt,&max_key);
  else choose_key(mid,k,new_p,ix,mid->keys_in_block+insert-1,&max_key,insert);

  done = rt_lc>target;
  while ( !done) {
    move_ix = mid->keys_in_block - rt_cnt + insert - 1;
    lc = choose_key(mid,k,new_p,ix,move_ix,&move_key,insert);/*key that will move*/
    choose_key(mid,k,new_p,ix,move_ix-1,&rt_mid_key,insert);
    new_mid_prefix_lc = find_prefix_lc(&min_key,&rt_mid_key);
    if ( mid_prefix_lc!=new_mid_prefix_lc ) {
      prefix_difference = mid_prefix_lc - new_mid_prefix_lc;
      /*      mid_change = -(lc-new_mid_prefix_lc) + prefix_difference * (mid->keys_in_block - rt_cnt + 1) - prefix_difference;*/
      mid_change = -(lc-new_mid_prefix_lc) + prefix_difference * (mid->keys_in_block - rt_cnt + insert) - prefix_difference;
    }
    else mid_change = -(lc-new_mid_prefix_lc);
    new_rt_prefix_lc = find_prefix_lc(&move_key,&max_key);
    if ( rt_prefix_lc!=new_rt_prefix_lc ) {
      prefix_difference = rt_prefix_lc - new_rt_prefix_lc;
      rt_change = (lc-new_rt_prefix_lc) + prefix_difference * (rt->keys_in_block + rt_cnt) - prefix_difference;
    }
    else rt_change = (lc-new_rt_prefix_lc);
    if ( f->trace ) {
      print_key(mid->index_type,&move_key,"  checked move key=");
      printf(", lc=%d, mid_lc=%d, mid_change=%d, rt_lc=%d, rt_change=%d\n",
        lc,mid_lc,mid_change,rt_lc,rt_change);
    }
    if ( (unsigned)rt_lc+rt_change>keyspace_lc ) done = true;
    else if ( rt_cnt>=(mid->keys_in_block+insert-1) ) done = true; /* leave at least one key in mid */
    else {
      mid_prefix_lc = new_mid_prefix_lc;
      rt_prefix_lc = new_rt_prefix_lc;
      rt_lc = rt_lc + rt_change;
      mid_lc = mid_lc + mid_change;
      rt_cnt++;
      if ( rt_lc>=target) done = true;
    }
  }
  if ( f->trace ) printf(" should move %d keys to rt, mid_lc=%d, rt_lc=%d, mid_prefix_lc=%d, rt_prefix_lc=%d\n",
    rt_cnt,mid_lc,rt_lc,mid_prefix_lc,rt_prefix_lc);

  *mid_lc_in_out = mid_lc;
  *mid_prefix_lc_in_out = mid_prefix_lc;
  *rt_lc_out = rt_lc;
  *rt_prefix_lc_out = rt_prefix_lc;
  return(rt_cnt);

}


/* choose_left_move_cnt figures out how many keys should be moved from */
/*  index block mid to lt in order to make lt have at least target      */
/*  bytes in its index pool.  If insert=true then k is a new key to     */
/*  be inserted in location ix.  Otherwise, k will replace the entry    */
/*  in ix. The number of keys to move is returned, the sizes of mid     */
/*  and lt after moving are returned in mid_lc_out and lt_lc_out.       */
/*  and the prefix_lc's after moving are returned in mid_prefix_lc_out  */
/*  and lt_prefix_lc_out.                                               */
/*  Note that new_mid_lc may still exceed max_keyspace.                 */
/*  ***this version is klugy in that keys_in_block, mid_lc, and         */
/*  ***mid_prefix_lc are take from input parameters rather than the     */
/*  ***block itself.  This is to allow mid to remain unmodified in case */
/*  ***the move won't actually be done                                  */

static int choose_left_move_cnt(struct fcb *f, struct ix_block *lt, struct ix_block *mid,
  struct key *k, levelx_pntr *new_p, int ix, boolean insert, int mid_keys_in_block,
  int *mid_lc_in_out, int *mid_prefix_lc_in_out, int *lt_lc_out, int *lt_prefix_lc_out)
{int lt_cnt=0,lc,lt_prefix_lc,mid_prefix_lc,prefix_difference,lt_lc,mid_lc,
 lt_change,mid_change,new_lt_prefix_lc,new_mid_prefix_lc,target;
 boolean done=false;
 struct key min_key,rt_mid_key,move_key,mid_min_key;

  get_nth_key(lt,&min_key,0);
  lt_lc = ix_pool_lc(lt);
  lt_prefix_lc = lt->prefix_lc;
  mid_lc = *mid_lc_in_out;
  mid_prefix_lc = *mid_prefix_lc_in_out;
  target = (lt_lc + mid_lc) / 2;

  if ( block_prefix_lc(lt)!=lt->prefix_lc && show_errors )
    printf("**lt prefix length has changed, expected=%d, actual=%d, index=%d\n",block_prefix_lc(lt),lt->prefix_lc,lt->index_type);
  if ( f->trace ) {
    print_key(mid->index_type,k,"finding left_mov_cnt, insert key=");
    printf(", lc=%d, pntr_lc=%d, ix=%d, level=%d",k->lc,pntr_lc(new_p,mid->level),ix,mid->level);
    printf(" target=%d, mid_lc=%d, mid_prefix_lc=%d, lt_lc=%d\n",target,mid_lc,mid_prefix_lc,lt_lc);
  }

  choose_key(mid,k,new_p,ix,mid_keys_in_block+insert-1,&rt_mid_key,insert);
  done = lt_lc>target;
  while ( !done ) {
    lc = choose_key(mid,k,new_p,ix,lt_cnt,&move_key,insert);
    choose_key(mid,k,new_p,ix,lt_cnt+1,&mid_min_key,insert);
    new_lt_prefix_lc = find_prefix_lc(&min_key,&move_key);
    if ( lt_prefix_lc!=new_lt_prefix_lc ) {
      prefix_difference = lt_prefix_lc - new_lt_prefix_lc;
      lt_change = (lc-new_lt_prefix_lc) + prefix_difference * (lt->keys_in_block + lt_cnt) - prefix_difference;
    }
    else lt_change = lc - new_lt_prefix_lc;
    new_mid_prefix_lc = find_prefix_lc(&mid_min_key,&rt_mid_key);
    if ( mid_prefix_lc!=new_mid_prefix_lc ) { 
      prefix_difference = mid_prefix_lc - new_mid_prefix_lc;
      /*      mid_change = -(lc-new_mid_prefix_lc) + prefix_difference * (mid_keys_in_block - lt_cnt + 1) - prefix_difference;*/
      mid_change = -(lc-new_mid_prefix_lc) + prefix_difference * (mid_keys_in_block - lt_cnt + insert) - prefix_difference;
    }
    else mid_change = -(lc-new_mid_prefix_lc);
    if ( f->trace ) {
      print_key(mid->index_type,&move_key,"  checked move key=");
      printf(", lc=%d, mid_lc=%d, mid_change=%d, lt_lc=%d, lt_change=%d\n",
        lc,mid_lc,mid_change,lt_lc,lt_change);
    }
    if ( (unsigned)lt_lc+lt_change>keyspace_lc ) done = true;
    else if ( lt_cnt>=(mid_keys_in_block+insert-1) ) done = true; /* leave at least one key in mid */
    else {
      lt_prefix_lc = new_lt_prefix_lc;
      mid_prefix_lc = new_mid_prefix_lc;
      lt_lc = lt_lc + lt_change;
      mid_lc = mid_lc + mid_change;
      /*      if ( lt_cnt==0 && lt->keys_in_block==0 ) min_key = move_key;*/
      lt_cnt++;
      if ( lt_lc+lt_change>=target) done = true;
    }
  }
  if ( f->trace ) {
    printf(" should move %d keys to lt, ",lt_cnt);
    printf("lt_lc=%d, lt_prefix=%d, mid_lc=%d, mid_prefix=%d\n",lt_lc,lt_prefix_lc,mid_lc,mid_prefix_lc);
  }

  *mid_lc_in_out = mid_lc;
  *mid_prefix_lc_in_out = mid_prefix_lc;
  *lt_lc_out = lt_lc;
  *lt_prefix_lc_out = lt_prefix_lc;
  return(lt_cnt);

}

static void shuffle_length_mismatch(struct ix_block *b, char caption[], unsigned expected_lc, struct key *k, boolean insert)
{
  if ( show_errors ) {
    printf("**shuffle length mismatch for %s, index_type=%d, level=%d\n",caption,b->index_type,b->level);
    print_key(b->index_type,k,"    key=");
    printf(", actual_lc=%d, expected_lc=%u, insert=%d\n",ix_pool_lc(b),expected_lc,insert);
  }
}

static boolean choose_split_points(struct fcb *f, struct ix_block *lt, struct ix_block *mid,
struct ix_block *rt, struct key *k, levelx_pntr *new_p, int ix, boolean insert)
{int lt_cnt=0,rt_cnt=0;
 unsigned lt_lc,mid_lc,rt_lc,target,lt_prefix_lc,mid_prefix_lc,rt_prefix_lc;
 boolean fits,moved_key_left,moved_key_right,key_not_moved;
 struct key mid_max_key,move_key,temp;

  if ( f->trace ) {
    print_key(mid->index_type,k,"trying to move to nbrs, key=");
    printf(", lc=%d, pntr_lc=%d, ix=%d, level=%d, insert=%d\n",k->lc,pntr_lc(new_p,mid->level),ix,mid->level,insert);
  }

  lt_lc = ix_pool_lc(lt);
  lt_prefix_lc = lt->prefix_lc;
  /*  if ( insert ) mid_lc = ix_pool_lc_after_insert(mid,k,new_p,ix,&mid_prefix_lc,false);
      else mid_lc = ix_pool_lc_after_replace(mid,k,new_p,ix,&mid_prefix_lc,false);*/
  mid_lc = ix_pool_lc_after_change(mid,k,new_p,ix,&mid_prefix_lc,insert,false);
  rt_lc = ix_pool_lc(rt);
  target = ( lt_lc + mid_lc + rt_lc) / 3;
  if ( lt_lc>target ) target = (mid_lc + rt_lc) / 2;
  else if ( rt_lc>target ) target = (lt_lc + mid_lc) / 2;

  if ( f->trace ) printf("target=%d, lt_lc=%d, mid_lc=%d, rt_lc=%d\n",target,lt_lc,mid_lc,rt_lc);

  rt_cnt = choose_right_move_cnt(f,mid,rt,k,new_p,ix,insert,target,&mid_lc,&mid_prefix_lc,&rt_lc,&rt_prefix_lc);
  /*  choose_key(mid,k,new_p,ix,mid->keys_in_block-rt_cnt,&mid_max_key,true);*/
  choose_key(mid,k,new_p,ix,mid->keys_in_block-1+insert-rt_cnt,&mid_max_key,insert);
  lt_cnt = choose_left_move_cnt(f,lt,mid,k,new_p,ix,insert,mid->keys_in_block-rt_cnt,&mid_lc,&mid_prefix_lc,&lt_lc,&lt_prefix_lc);
  /*  lt_cnt = choose_left_move_cnt(f,lt,mid,k,new_p,ix,insert,mid->keys_in_block-rt_cnt+insert,&mid_lc,&mid_prefix_lc,&lt_lc,&lt_prefix_lc);*/

  if ( f->trace ) {
    printf(" should move %d keys to lt, %d to rt ",lt_cnt,rt_cnt);
    printf("lt_lc/prefix=%d/%d, mid_lc/prefix=%d/%d, rt_lc/prefix=%d/%d\n",
      lt_lc,lt_prefix_lc,mid_lc,mid_prefix_lc,rt_lc,rt_prefix_lc);
  }

  if ( lt_cnt==0 ) get_nth_key(lt,&move_key,lt->keys_in_block-1);
  else choose_key(mid,k,new_p,ix,lt_cnt-1,&move_key,insert);
  fits = (lt_lc<=keyspace_lc && mid_lc<=keyspace_lc && rt_lc<keyspace_lc);
  if ( fits ) {
    /*    key_not_moved = (ix>(lt_cnt-1)) && (ix<=mid->keys_in_block-rt_cnt);*/
    /*    key_not_moved = (ix>(lt_cnt-1)) && (ix<mid->keys_in_block-rt_cnt);*/
    compress_ix_block(rt,rt_prefix_lc,f->trace);
    moved_key_right = move_keys_to_right(mid,rt,rt_cnt,k,new_p,ix,insert);
    if (block_prefix_lc(rt)!=rt->prefix_lc && show_errors)
      printf("   rt_prefix after move should be=%d, is=%d\n",block_prefix_lc(rt),rt->prefix_lc);
    compress_ix_block(lt,lt_prefix_lc,f->trace);
    moved_key_left = move_keys_to_left(lt,mid,lt_cnt,k,new_p,ix,insert);
    if (block_prefix_lc(lt)!=lt->prefix_lc && show_errors)
      printf("   lt_prefix after move should be=%d, is=%d\n",block_prefix_lc(lt),lt->prefix_lc);
    compress_ix_block(mid,mid_prefix_lc,f->trace);
    key_not_moved = !moved_key_right && !moved_key_left;
    if ( key_not_moved ) {
      if ( !insert && (ix-lt_cnt)<mid->keys_in_block ) simple_delete(mid,ix-lt_cnt);
      if ( simple_insert(mid,ix-lt_cnt,k,*new_p) ) ;
      else if ( show_errors ) printf("**simple_insert failed in choose_split_points, ix=%d, lt_cnt=%d, rt_cnt=%d\n",ix,lt_cnt,rt_cnt);
    }
    if (block_prefix_lc(mid)!=mid->prefix_lc && show_errors )
      printf("   mid_prefix after moves and insert=%d, in_block=%d\n",block_prefix_lc(mid),mid->prefix_lc);
    get_max_key(lt,&temp);
    if ( !eq_key(&temp,&move_key) && show_errors ) {
      print_key(mid->index_type,&temp,"**actual lt max_key=");
      print_key(mid->index_type,&move_key," doesn't match expected=");
      printf("\n");
    }
    get_max_key(mid,&temp);
    if ( !eq_key(&temp,&mid_max_key) && show_errors ) {
      print_key(mid->index_type,&temp,"**actual mid max_key=");
      print_key(mid->index_type,&mid_max_key," doesn't match expected=");
      printf("\n");
    }
    if ( ix_pool_lc(lt)!=lt_lc )   shuffle_length_mismatch(lt ,"lt", lt_lc ,k,insert);
    if ( ix_pool_lc(mid)!=mid_lc ) shuffle_length_mismatch(mid,"mid",mid_lc,k,insert);
    if ( ix_pool_lc(rt)!=rt_lc )   shuffle_length_mismatch(rt, "rt", rt_lc, k,insert);
    }
  return(fits);
}

static boolean shuffle_keys(struct fcb *f, int mid_ix, struct key *k, levelx_pntr *p, int ix, boolean insert)
{int lt_ix,rt_ix,index_type; unsigned level;
 boolean shuffled=false; struct key lt_sep,old_lt_sep,mid_sep,old_mid_sep;

  lock_buffer(f,mid_ix);
  get_max_key(&(f->buffer[mid_ix].b.ix),&old_mid_sep);
  level = f->buffer[mid_ix].b.ix.level;
  index_type = f->buffer[mid_ix].b.ix.index_type;
  if ( null_pntr(f->buffer[mid_ix].b.ix.prev) ) shuffled = false;
  else if ( null_pntr(f->buffer[mid_ix].b.ix.next) ) shuffled = false;
  else {
    lt_ix = get_index(f,f->buffer[mid_ix].b.ix.prev);
    lock_buffer(f,lt_ix);
    get_max_key(&(f->buffer[lt_ix].b.ix),&old_lt_sep);
    rt_ix = get_index(f,f->buffer[mid_ix].b.ix.next);
    lock_buffer(f,rt_ix);

    if ( f->trace ) {
      print_index_block(stdout,&(f->buffer[lt_ix].b.ix));
      print_index_block(stdout,&(f->buffer[mid_ix].b.ix));
      print_index_block(stdout,&(f->buffer[rt_ix].b.ix));
    }
    if ( choose_split_points(f,&(f->buffer[lt_ix].b.ix),&(f->buffer[mid_ix].b.ix),&(f->buffer[rt_ix].b.ix),k,p,ix,insert) ) {
      shuffled = true;
      if ( level<f->primary_level[index_type] ) {
        get_max_key(&(f->buffer[lt_ix].b.ix),&lt_sep);
        replace_max_key(f,index_type,&old_lt_sep,&lt_sep,f->buffer[lt_ix].contents,(unsigned)f->buffer[lt_ix].b.ix.level+1);
        get_max_key(&(f->buffer[mid_ix].b.ix),&mid_sep);
        replace_max_key(f,index_type,&old_mid_sep,&mid_sep,f->buffer[mid_ix].contents,(unsigned)f->buffer[mid_ix].b.ix.level+1);
      }
      mark_modified(f,lt_ix);
      mark_modified(f,rt_ix);
    }
    if ( f->trace ) {
      if ( shuffled ) {
        printf("reshuffle succeeded, shuffled blocks are:\n");
        print_index_block(stdout,&(f->buffer[lt_ix].b.ix));
        print_index_block(stdout,&(f->buffer[mid_ix].b.ix));
        print_index_block(stdout,&(f->buffer[rt_ix].b.ix));
      }
      else printf("reshuffle failed\n");
    }
    unlock_buffer(f,rt_ix);
    unlock_buffer(f,lt_ix);
  }
  unlock_buffer(f,mid_ix);

  return(shuffled);
}

/* create_new_primary creates a new primary block containing the key max  */
/*   and sets last_pntr */

static void create_new_primary(struct fcb *f, int index, struct leveln_pntr b, struct key *max,
  struct leveln_pntr newb)
{int ix; levelx_pntr bx; struct key dummy;

  if ( f->primary_level[index]>=max_level-1 ) fatal_error(f,maxlevel_err);
  else {
    f->primary_level[index]++;
    ix = allocate_index_block(f,index,&(f->first_at_level[f->primary_level[index]][index]),
      f->primary_level[index],&dummy,0);
    bx.pn = b;
    simple_insert(&(f->buffer[ix].b.ix),0,max,bx);
    f->last_pntr[f->primary_level[index]][index] = newb;

    if ( f->trace )
      printf("  creating new %s primary, levels are %s=%u,%s=%u,%s=%u\n",index_caption[index],
	   index_caption[user_ix],f->primary_level[user_ix],
	   index_caption[free_rec_ix],f->primary_level[free_rec_ix],
	   index_caption[free_lc_ix],f->primary_level[free_lc_ix]);
  }
}

/* split_block splits the block in buffer[bufix]. On entry key k    */
/*   belongs in entry ix and must be <= the */
/*   max_key in the parent block that points to this block.  It     */
/*   creates a new block, moves keys to it, and adjusts prev and    */
/*   next pointers. It then updates the parent block with a new     */
/*   pointer for the right block and a new max_key for the left or  */
/*   creates a new primary, as appropriate.                         */
/*   If we are inserting at eof (next=null, ix=keys_in_block)       */
/*   then sequential inserts are assumed, otherwise random.         */
/*   Note, however, that we do not use sequential mode for free     */
/*   space blocks.  This is because freespace management repeatedly */
/*   deletes the last key in the index then reinserts the last key  */
/*   (with a new lc) which leads to degenerate behavior when block  */
/*   splits result in a new primary.                                */

static void split_block(struct fcb *f, struct key *k, levelx_pntr p, int bufix, int ix, boolean insert)
{int new_ix,i,index_type,new_prefix_lc,
  old_block_lc,cnt,new_block_lc;
 unsigned parent_level,old_block_prefix_lc,new_block_prefix_lc,target;
 boolean split,seq,new_on_right,moved_new_key;
 struct leveln_pntr parent,oldb,newb,save_next,leftb,rightb;
 struct key left_max,right_max,original_max_key,old_max,new_max,temp; levelx_pntr leftbx;
 struct ix_block *old_block,*new_block;

  lock_buffer(f,bufix);
  index_type = f->buffer[bufix].b.ix.index_type;
  oldb = f->buffer[bufix].contents;
  old_block = &(f->buffer[bufix].b.ix);
  get_max_key(&(f->buffer[bufix].b.ix),&original_max_key);
  target = ix_pool_lc_after_change(&(f->buffer[bufix].b.ix),k,&p,ix,&new_prefix_lc,insert,false) / 2;

  seq = insert && null_pntr(f->buffer[bufix].b.ix.next) && ix==f->buffer[bufix].b.ix.keys_in_block;
  seq = seq && (index_type==user_ix);

  parent_level = f->buffer[bufix].b.ix.level + 1;
  parent = parent_block(f,bufix,k);
  new_ix = allocate_index_block(f,index_type,&newb,f->buffer[bufix].b.ix.level,k,0);
  lock_buffer(f,new_ix);
  new_block = &(f->buffer[new_ix].b.ix);

  if ( f->trace ) {
    print_leveln_pntr("splitting block ",&oldb);
    print_leveln_pntr("new block is ",&newb);
    printf("insert=%d, target=%d, seq=%d, ix=%d\n",insert,target,seq,ix);
    print_key(index_type,k,"  ins/rep key=");
    printf("\n");
    print_key(index_type,&original_max_key,"  orig_max_key=");
    printf("\n");
    print_index_block(stdout,old_block);
  }
  if ( ix<(old_block->keys_in_block/2) ) { /* add block to left */
    new_on_right = false;
    new_block->next = oldb;
    new_block->prev = old_block->prev;
    if ( null_pntr(old_block->prev) ) {
      f->first_at_level[old_block->level][index_type] = newb;
    }
    else {
      i = get_index_update(f,old_block->prev);
      f->buffer[i].b.ix.next = newb;
    }
    old_block->prev = newb;
    old_block_lc = ix_pool_lc(old_block);
    old_block_prefix_lc = old_block->prefix_lc;
    cnt = choose_left_move_cnt(f,new_block,old_block,k,&p,ix,insert,
      old_block->keys_in_block,&old_block_lc,&old_block_prefix_lc,&new_block_lc,&new_block_prefix_lc);
    get_nth_key(old_block,&temp,0);
    set_empty_block_prefix(new_block,&temp,new_block_prefix_lc);
    moved_new_key = move_keys_to_left(new_block,old_block,cnt,k,&p,ix,insert);
    compress_ix_block(old_block,old_block_prefix_lc,f->trace);
    if ( !moved_new_key ) {
      if ( !insert ) simple_delete(old_block,ix-cnt);
      if ( !simple_insert(old_block,ix-cnt,k,p) && show_errors ) printf("**insert failed in split_block, new is on rt\n");
    }
  }
  else { /* add new block to right */
    new_on_right = true;
    new_block->next = old_block->next;
    new_block->prev = oldb;
    if ( !null_pntr(new_block->next) ) {
      i = get_index_update(f,new_block->next);
      f->buffer[i].b.ix.prev = newb;
    }
    old_block->next = newb;
    if ( seq ) simple_insert(new_block,0,k,p);
    else {
      cnt = choose_right_move_cnt(f,old_block,new_block,k,&p,ix,insert,target,
        &old_block_lc,&old_block_prefix_lc,&new_block_lc,&new_block_prefix_lc);
      set_empty_block_prefix(new_block,&original_max_key,new_block_prefix_lc);
      moved_new_key = move_keys_to_right(old_block,new_block,cnt,k,&p,ix,insert);
      compress_ix_block(old_block,old_block_prefix_lc,f->trace);
      if ( !moved_new_key ) {
        if ( !insert ) simple_delete(old_block,ix);
        if ( !simple_insert(old_block,ix,k,p) && show_errors ) printf("**insert failed in split_block, new is on rt\n");
      }
    }
  }

  check_ix_block_compression(old_block);
  check_ix_block_compression(new_block);
  
  if ( f->trace ) {
    printf("after split, new_on_right=%d, seq=%d, old block is\n",new_on_right,seq);
    print_index_block(stdout,old_block);
    printf("after split, new block is\n");
    print_index_block(stdout,new_block);
  }

  /*now propogate upward*/
  get_max_key(old_block,&old_max);
  get_max_key(new_block,&new_max);
  if ( new_on_right ) {
    leftb = oldb;
    rightb = newb;
    get_max_key(old_block,&left_max);
    get_max_key(new_block,&right_max);
    save_next = new_block->next;
  }
  else {
    leftb = newb;
    rightb = oldb;
    get_max_key(old_block,&right_max);
    get_max_key(new_block,&left_max);
    save_next = old_block->next;
  }
  unlock_buffer(f,bufix);
  unlock_buffer(f,new_ix);

  if ( parent_level>f->primary_level[index_type] )
    create_new_primary(f,index_type,leftb,&left_max,rightb);
  else {
    leftbx.pn = leftb;
    if ( null_pntr(save_next) ) {
      f->last_pntr[parent_level][index_type] = rightb;
      update_index(f,&left_max,parent,leftbx);
    }
    else {
      split = replace_max_key_and_pntr(f,index_type,&original_max_key,&right_max,oldb,rightb,parent_level);
      if ( split ) {
        parent = search_index(f,index_type,parent_level+1,&left_max);
        if ( f->trace ) {
          print_leveln_pntr("  parent after replace_max split is ",&parent);
          printf("\n");
	}
      }
      update_index(f,&left_max,parent,leftbx);
    }
  }
}

/* update_index1 inserts key k and pointer p into entry ix in the  */
/*   index block in buffer[bufix].  It assumes that the buffer has */
/*   been marked modified, locked and will be unlocked upon        */
/*   return              */  

static void update_index1(struct fcb *f, struct key *k, levelx_pntr p, int bufix, int ix, boolean insert)
{int index_type,update_type,new_prefix_lc;
 unsigned level,new_lc;
 boolean at_end,update_parent=false;
 struct key old_max_key; struct leveln_pntr b;

  level = f->buffer[bufix].b.ix.level;
  index_type = f->buffer[bufix].b.ix.index_type;
  at_end = ix==f->buffer[bufix].b.ix.keys_in_block;
  if ( at_end && f->primary_level[index_type]>level && !null_pntr(f->buffer[bufix].b.ix.next) && insert) {
    update_parent = true;
    get_max_key(&(f->buffer[bufix].b.ix),&old_max_key);
  }
  if ( f->trace ) {
    b = f->buffer[bufix].contents;
    print_leveln_pntr("  updating block ",&b);
    print_key(index_type,k,"with key="); printf(", ");
  }
  if ( insert )
    new_lc = ix_pool_lc_after_insert(&(f->buffer[bufix].b.ix),k,&p,ix,&new_prefix_lc,false);
  else new_lc = ix_pool_lc_after_replace(&(f->buffer[bufix].b.ix),k,&p,ix,&new_prefix_lc,false);
  if ( new_lc<=keyspace_lc ) {
    if ( !insert ) simple_delete(&(f->buffer[bufix].b.ix),ix);
    prefix_simple_insert(&(f->buffer[bufix].b.ix),ix,k,p);
    update_type = 0;
    if ( f->trace ) printf("simple insert\n");
    if ( update_parent )
      replace_max_key(f,index_type,&old_max_key,k,f->buffer[bufix].contents,(unsigned)level+1);
  }
  else if ( shuffle_keys(f,bufix,k,&p,ix,insert) ) {
    update_type = 1;
    if ( f->trace ) printf("shuffled keys to insert\n");
  }
  else {
    update_type = 2;
    if ( f->trace ) {
      print_key(index_type,k,"block split, k=");
      printf(", keys_in_block=%d, chars_in_use=%d\n",
        f->buffer[bufix].b.ix.keys_in_block,f->buffer[bufix].b.ix.chars_in_use);
    }
    split_block(f,k,p,bufix,ix,insert);
  }
  /*  if ( block_prefix_lc(&(f->buffer[bufix].b.ix))!=f->buffer[bufix].b.ix.prefix_lc && show_errors)
    printf("  after update of block %d/%lu, prefix_lc=%d, expected=%d, update_type=%d\n",b.segment,b.block,
    f->buffer[bufix].b.ix.prefix_lc,block_prefix_lc(&(f->buffer[bufix].b.ix)),update_type);*/
}

/* update_index inserts key k and pointer p into index block b. */

static void update_index(struct fcb *f, struct key *k, struct leveln_pntr b, levelx_pntr p)
{int bufix,ix; boolean found;

  bufix = get_index_update(f,b);
  lock_buffer(f,bufix);
  ix = search_block(f,bufix,k,&found,"update_index");
  update_index1(f,k,p,bufix,ix,!found);
  unlock_buffer(f,bufix);
}

/*static void update_index2(struct fcb *f, struct key *k, struct leveln_pntr b, levelx_pntr p)
{int bufix,ix,level,index_type,update_type,new_lc,new_prefix_lc;
boolean found,at_end,update_parent=false,insert=true;
struct key old_max_key;

  bufix = get_index_update(f,b);
  lock_buffer(f,bufix);
  level = f->buffer[bufix].b.ix.level;
  index_type = f->buffer[bufix].b.ix.index_type;
  ix = search_block(f,bufix,k,&found,"update_index");
  at_end = ix==f->buffer[bufix].b.ix.keys_in_block;
  if ( at_end && f->primary_level[index_type]>level && !null_pntr(f->buffer[bufix].b.ix.next) && !found) {
    update_parent = true;
    get_max_key(&(f->buffer[bufix].b.ix),&old_max_key);
  }

  if ( f->trace ) {
    printf("  updating block %d/%lu with key=",b.segment,b.block);
    print_key(index_type,k,""); printf(", ");
  }

  if ( found ) {
    insert = false;
    new_lc = ix_pool_lc_after_replace(&(f->buffer[bufix].b.ix),k,&p,ix,&new_prefix_lc,false);
  }
  else new_lc = ix_pool_lc_after_insert(&(f->buffer[bufix].b.ix),k,&p,ix,&new_prefix_lc,false);
  if ( new_lc<=keyspace_lc ) {
    if ( found ) simple_delete(&(f->buffer[bufix].b.ix),ix);
    prefix_simple_insert(&(f->buffer[bufix].b.ix),ix,k,p);
    update_type = 0;
    if ( f->trace ) printf("simple insert\n");
    if ( update_parent )
      replace_max_key(f,index_type,&old_max_key,k,f->buffer[bufix].contents,level+1);
  }
  else if ( shuffle_keys(f,bufix,k,&p,ix,insert) ) {
    update_type = 1;
    if ( f->trace ) printf("shuffled keys to insert\n");
  }
  else {
    update_type = 2;
    if ( f->trace ) {
      print_key(index_type,k,"block split, k=");
      printf(", keys_in_block=%d, chars_in_use=%d\n",
        f->buffer[bufix].b.ix.keys_in_block,f->buffer[bufix].b.ix.chars_in_use);
    }
    split_block(f,k,p,bufix,ix,insert);
  }
  if ( block_prefix_lc(&(f->buffer[bufix].b.ix))!=f->buffer[bufix].b.ix.prefix_lc && show_errors)
    printf("  after update of block %d/%lu, prefix_lc=%d, expected=%d, update_type=%d\n",b.segment,b.block,
      f->buffer[bufix].b.ix.prefix_lc,block_prefix_lc(&(f->buffer[bufix].b.ix)),update_type);
  unlock_buffer(f,bufix);
}*/

/* record moving -- if the record is so short that it fits in a UINT32 */
/*   then it is kept in the sc field of the pointer, otherwise the   */
/*   record is written to disk     */

static void move_from_rec(struct fcb *f, char r[], struct level0_pntr *p)
{size_t size,lc; FILE *file;

  if ( f->read_only ) f->error_code = read_only_err;
  else if ( p->lc<min_disk_rec_lc ) memcpy(&p->sc,r,(size_t)p->lc);
  else {
    file = file_index(f,p->segment);
    if ( f->error_code!=no_err ) return;
    if ( fseeko(file,(FILE_OFFSET)p->sc,0)!=0 ) {
      f->error_code = seek_err; return;
    }
    lc = p->lc;
    size = fwrite(r,(size_t) 1,lc,file);
    if ( size!=lc ) {
      set_error(f,move_rec_err,"**move_from_rec failed\n");
    }
  }
}

/* intermediate calls */

/* extract_next extracts the key and pointer identified by the */
/*   current file position and advances the file position.     */

static void extract_next(struct fcb *f, int index, int bufix, char t[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{struct key k;

  if ( f->position_ix[index]>=f->buffer[bufix].b.ix.keys_in_block ) {
    t[0] = '\0'; *key_lc = 0; *p = null0_ptr;
    if ( null_pntr(f->buffer[bufix].b.ix.next) ) f->error_code = ateof_err;
    else if ( show_errors ) {
      print_leveln_pntr("**Uh Oh.  Error in extract_next, block=",&(f->buffer[bufix].contents));
      printf("index=%d, position=%d, keys=%d, ",index,f->position_ix[index],f->buffer[bufix].b.ix.keys_in_block);
      print_leveln_pntr("next",&(f->buffer[bufix].b.ix.next));
      printf("\n");
    }
  }
  else {
    get_nth_key(&(f->buffer[bufix].b.ix),&k,f->position_ix[index]);
    if ( k.lc<=max_key_lc ) *key_lc = k.lc;
    else {
      f->error_code = longkey_err; *key_lc = max_key_lc;
    }
    unpack0_ptr(&(f->buffer[bufix].b.ix),f->position_ix[index],p);
    mvc(k.text,0,t,0,(unsigned)*key_lc);
    f->position_ix[index]++;
    if ( f->position_ix[index]>=f->buffer[bufix].b.ix.keys_in_block && !null_pntr(f->buffer[bufix].b.ix.next) )
      set_position(f,index,f->buffer[bufix].b.ix.next,0);
  }
}

void kf_set_bof(struct fcb *f, int index)
{
  f->position[index] = f->first_at_level[0][index];
  f->position_ix[index] = 0;
}

int kf_next_ptr(struct fcb *f, int index, unsigned char key[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{int ix; struct leveln_pntr next;

  if ( check_fcb(f) ) {
    ix = get_index(f,f->position[index]);
    next = f->buffer[ix].b.ix.next;
    while ( f->position_ix[index]>=f->buffer[ix].b.ix.keys_in_block && !null_pntr(next) ) {
      ix = get_index(f,next);
      set_position(f,index,next,0);
      next = f->buffer[ix].b.ix.next;
    }
    extract_next(f,index,ix,key,key_lc,max_key_lc,p);
  }
  return(f->error_code);
}

int kf_next_rec(struct fcb *f, int index, unsigned char t[], int *key_lc, int max_key_lc,
   char r[],int *rlc, int max_lc)
{struct level0_pntr p;

  kf_next_ptr(f,index,t,key_lc,max_key_lc,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,(unsigned)max_lc);
  else *rlc = 0;
  return(f->error_code);
}

static int kf_prev_ptr(struct fcb *f, int index, unsigned char t[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{int bufix; boolean done=false; struct leveln_pntr prev; struct key k;

  if ( check_fcb(f) ) {
    bufix = get_index(f,f->position[index]);
    if ( f->position_ix[index]==0 ) {
      prev = f->buffer[bufix].b.ix.prev;
      do {
        if ( null_pntr(prev) ) {
          done = true; f->error_code = atbof_err;
          *key_lc = 0; t[0] = '\0'; *p = null0_ptr;
        }
        else {
          bufix = get_index(f,prev);
          set_position(f,index,prev,f->buffer[bufix].b.ix.keys_in_block);
          prev = f->buffer[bufix].b.ix.prev;
          done = f->buffer[bufix].b.ix.keys_in_block > 0;
        }
      } while ( !done );
    }
    if ( f->error_code==no_err && f->position_ix[index]>0 ) {
      f->position_ix[index]--;
      get_nth_key(&(f->buffer[bufix].b.ix),&k,f->position_ix[index]);
      *key_lc = f->buffer[bufix].b.ix.keys[f->position_ix[index]].lc;
      if ( k.lc<=max_key_lc ) *key_lc = k.lc;
      else {
        *key_lc = max_key_lc; f->error_code = longkey_err;
      }
      unpack0_ptr(&(f->buffer[bufix].b.ix),f->position_ix[index],p);
      mvc(k.text,0,t,0,(unsigned)*key_lc);
    }
  }
  return(f->error_code);
}

static int kf_prev_rec(struct fcb *f, int index, char t[], int *key_lc, int max_key_lc,
   char r[],int *rlc, unsigned max_lc)
{struct level0_pntr p;

  kf_prev_ptr(f,index,t,key_lc,max_key_lc,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

static int kf_get_ptr(struct fcb *f, int index, unsigned char t[], int key_lc, struct level0_pntr *p)
{struct leveln_pntr b; int ix,lc,bufix; struct key k; boolean found;
char t1[maxkey_lc];

  set_up(f,t,key_lc,&k);
  if ( f->error_code==no_err ) {
    b = search_index(f,index,level_one,&k);
    bufix = get_index(f,b);
    ix = search_block(f,bufix,&k,&found,"kf_get_ptr");
    set_position(f,index,b,ix);
    if ( found ) extract_next(f,index,bufix,t1,&lc,maxkey_lc,p);
    else if ( f->error_code==no_err ) {
      f->error_code = getnokey_err; *p = null0_ptr;
    }
  }
  return(f->error_code);
}

static int kf_get_rec(struct fcb *f,int index, unsigned char t[],int key_lc, unsigned char r[], unsigned *rlc,int max_lc)
{struct level0_pntr p;

  kf_get_ptr(f,index,t,key_lc,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,(unsigned)max_lc);
  else *rlc = 0;
  return(f->error_code);
}

static int kf_put_ptr(struct fcb *f, int index, char t[], int key_lc, struct level0_pntr p)
{struct leveln_pntr b; levelx_pntr px; struct key k;

  if ( f->read_only ) f->error_code = read_only_err;
  else {
    set_up(f,t,key_lc,&k);
    if ( f->error_code==no_err ) {
      b = search_index(f,index,level_one,&k);
      px.p0 = p;
      update_index(f,&k,b,px);
      kf_set_bof(f,index);
    }
  }
  return(f->error_code);
}

static int kf_delete_ptr(struct fcb *f, int index, char t[], int key_lc)
{struct key k; struct level0_pntr p;

  if ( f->read_only ) f->error_code = read_only_err;
  else {
    set_up(f,t,key_lc,&k);
    if (f->error_code==no_err ) {
      index_delete(f,index,k,&p,level_zero);
      kf_set_bof(f,index);
    }
  }
  return(f->error_code);
}

static int kf_put_rec(struct fcb *f,int index, unsigned char t[], int key_lc, char r[], unsigned rlc)
{int ix,bufix,lc; boolean have_space=false,found; char t1[maxkey_lc];
 struct key k; struct leveln_pntr b; struct level0_pntr p; levelx_pntr px;

  if ( f->read_only ) f->error_code = read_only_err;
  else {
    set_up(f,t,key_lc,&k);
    if ( f->error_code==no_err ) {
      b = search_index(f,index,level_one,&k);
      bufix = get_index_update(f,b);
      lock_buffer(f,bufix);
      ix = search_block(f,bufix,&k,&found,"kf_put_rec");
      set_position(f,index,b,ix);
      if ( found  ) {
        extract_next(f,index,bufix,t1,&lc,maxkey_lc,&p);
        if ( rlc<min_disk_rec_lc ) { /* new rec goes into pointer */
          if ( p.lc>=min_disk_rec_lc ) deallocate_rec(f,p);
          have_space = true; p = dummy_ptr; p.lc = rlc;
        }
        else { /* new rec goes on disk */
          if ( p.lc>=min_disk_rec_lc && (rec_allocation_lc(rlc)==rec_allocation_lc(p.lc)) ) {
            have_space = true; p.lc = rlc;
          }
          else {
            if ( p.lc>=min_disk_rec_lc ) deallocate_rec(f,p);
            have_space =  allocate_rec(f,rlc,&p);
          }
        }
      }
      else {
        have_space = allocate_rec(f,rlc,&p);
      }
      if ( have_space ) {
        move_from_rec(f,r,&p); 
        px.p0 = p;
	/*        update_index(f,&k,b,px);*/
	update_index1(f,&k,px,bufix,ix,!found);
        kf_set_bof(f,index);
      }
      unlock_buffer(f,bufix);
    }
  }
  return(f->error_code);
}

static int kf_delete_rec(struct fcb *f, int index, unsigned char t[], int key_lc)
{struct level0_pntr p; struct key k;

  if ( f->read_only ) f->error_code = read_only_err;
  else {
    set_up(f,t,key_lc,&k);
    if (f->error_code==no_err ) {
      index_delete(f,index,k,&p,level_zero);
      if (f->error_code==no_err ) deallocate_rec(f,p);
      kf_set_bof(f,index);
    }
  }
  return(f->error_code);
}


/* Freespace management */

static boolean contiguous(struct level0_pntr p1, struct level0_pntr p2)
{
  if ( p1.segment!=p2.segment ) return(false);
  if ( (p1.sc+p1.lc)==p2.sc ) return(true);
  else return(false);
}

/* allocate_block allocates a new block and returns the buffer index */
/*   in which it lies.  If there are any blocks on the free block    */
/*   chain for this index level then the first block is returned.  If the */
/*   free block chain is empty then block_allocation_unit blocks are */
/*   allocated.  One is returned and the rest are initialized,       */
/*   and placed on the free block chain for this level.     */

static int allocate_block(struct fcb *f, int index_type, unsigned level)
{int i,bufix=0,temp; struct leveln_pntr p,p1; block_type_t b; struct key k;

  if ( null_pntr(f->first_free_block[level][index_type]) ) {
    if ( extend_file(f,(unsigned)(block_allocation_unit*block_lc),&p) ) {
      /* add blocks after the first to a free_block chain */
      p1.segment = p.segment;
      p1.block = p.block+1;
      initialize_index_block(&(b.ix),index_type,level,&k,(unsigned)0);
      for (i=0; i<block_allocation_unit-1; i++) {
        temp = vacate_oldest_buffer(f);
        initialize_index_block(&(f->buffer[temp].b.ix),index_type,level,&k,(unsigned)0);
        f->buffer[temp].contents = p1;
        f->buffer[temp].modified = true;
        f->buffer[temp].b.ix.next = f->first_free_block[level][index_type];
        f->first_free_block[level][index_type] = p1;
        hash_chain_insert(f,temp);
        p1.block++;
      }
      /*      for (i=0; i<block_allocation_unit-1; i++) {
        b.ix.next = f->first_free_block[level][index_type];
        f->first_free_block[level][index_type] = p1;
        write_page(f,p1,&b);
        p1.block++;
	}*/
      bufix = vacate_oldest_buffer(f);
      f->buffer[bufix].contents = p;
      f->buffer[bufix].modified = true;
      hash_chain_insert(f,bufix);
    }
  }
  else { /* we have a previously deallocated block */
    p = f->first_free_block[level][index_type];
    /*    printf("  reallocating empty block %d/%d\n",p.segment,p.block);*/
    bufix = get_index_update(f,p);
    f->first_free_block[level][index_type] = f->buffer[bufix].b.ix.next;
  }
  if ( f->trace_freespace ) {
    print_leveln_pntr("  just allocated block ",&p);
    printf("in buf=%d, seg_cnt=%u\n",bufix,f->segment_cnt);
  }
  return(bufix);
}

static int unpack_16bit(unsigned char key[], UINT16 *n)
{unsigned i;

  *n = 0;
  for (i=0; i<sizeof(UINT16); i++) {
    *n = (*n << 8) + key[i];
  }
  return(sizeof(UINT16));
}

static int unpack_32bit(unsigned char key[], UINT32 *n)
{unsigned i;

  *n = 0;
  for (i=0; i<sizeof(UINT32); i++) {
    *n = (*n << 8) + key[i];
  }
  return(sizeof(UINT32));
}

static int unpack_64bit(unsigned char key[], UINT64 *n)
{unsigned i;

  *n = 0;
  for (i=0; i<sizeof(UINT64); i++) {
    *n = (*n << 8) + key[i];
  }
  return(sizeof(UINT64));
}

static int pack_16bit(unsigned char key[], UINT32 n)
{int i;

  for (i=1; i>=0 ; i--) {
    key[i] = n & 255;
    n = n >> 8;
  }
  return((int)sizeof(UINT16));
}

static int pack_32bit(unsigned char key[], UINT32 n)
{int i;

  for (i=sizeof(UINT32)-1; i>=0 ; i--) {
    key[i] = n & 255;
    n = n >> 8;
  }
  return((int)sizeof(UINT32));
}

static int pack_64bit(unsigned char key[], UINT64 n)
{int i;

  for (i=sizeof(UINT64)-1; i>=0 ; i--) {
    key[i] = n & 255;
    n = n >> 8;
  }
  return((int)sizeof(UINT64));
}

int unpack_lc_key(unsigned char key[], struct level0_pntr *p)
{int lc; UINT16 segment; UINT32 plc; UINT64 sc;

  lc = unpack_32bit(key,&plc);
  p->lc = plc;
  lc = lc + unpack_16bit(key+lc,&segment);
  p->segment = segment;
  lc = lc + unpack_64bit(key+lc,&sc);
  p->sc = sc;
  /*  printf("unpacked lc key %d/%d/%d, lc=%d\n",p->lc,p->segment,p->sc,lc);*/
  return(lc);
}

static int pack_lc_key(unsigned char key[], struct level0_pntr p)
{int lc;

  lc = pack_32bit(key,p.lc);
  lc = lc + pack_16bit(key+lc,p.segment);
  lc = lc + pack_64bit(key+lc,p.sc);
  return(lc);
}

int unpack_rec_key(unsigned char key[], struct level0_pntr *p)
{int lc; UINT16 segment; UINT64 sc;

  lc = unpack_16bit(key,&segment);
  p->segment = segment;
  lc = lc + unpack_64bit(key+lc,&sc);
  p->sc = sc;
  /* printf("unpacked rec key %d/%d, lc=%d\n",p->segment,p->sc,lc);*/
  return(lc);
}

static int pack_rec_key(unsigned char key[], struct level0_pntr p)
{int lc;

  lc = pack_16bit(key,p.segment);
  lc = lc + pack_64bit(key+lc,p.sc);
  return(lc);
}

static void insert_freespace_entry(struct fcb *f, struct level0_pntr *p0)
{int err,key_lc; unsigned char key[maxkey_lc]; struct level0_pntr p;

  if ( f->trace_freespace ) {
    print_level0_pntr("inserting freespace entry ",p0);
    printf("\n");
  }
  p = *p0;
  p.lc = rec_allocation_lc(p0->lc);
  key_lc = pack_lc_key(key,p);
  err = kf_put_ptr(f,free_lc_ix,key,key_lc,dummy_ptr);
  if ( err!=no_err ) {
    set_error1(f,free_insrt_err,"**Couldn't insert free_lc entry, err=",err);
  }
  else {
    key_lc = pack_rec_key(key,p);
    err = kf_put_rec(f,free_rec_ix,key,key_lc,(char *) &(p.lc),(unsigned)sizeof(int));
    if ( err!=no_err ) {
      set_error1(f,free_insrt_err,"**Couldn't insert free_rec entry, err=",err);
    }
  }
}

static void delete_freespace_entry(struct fcb *f, struct level0_pntr *p0)
{int err,key_lc; unsigned char key[maxkey_lc]; struct level0_pntr p;

  if ( f->trace_freespace ) {
    print_level0_pntr("deleting freespace entry ",p0);
    printf("\n");
  }
  p = *p0;
  p.lc = rec_allocation_lc(p0->lc);
  key_lc = pack_lc_key(key,p);
  err = kf_delete_ptr(f,free_lc_ix,key,key_lc);
  if ( err!=no_err ) {
    f->error_code = free_dlt_err;
    if ( show_errors ) {
      print_level0_pntr("**Couldn't delete free_lc entry ",&p);
      printf("err=%d\n",err);
    }
  }
  else {
    key_lc = pack_rec_key(key,p);
    err = kf_delete_rec(f,free_rec_ix,key,key_lc);
    if ( err!=no_err ) {
      f->error_code = free_dlt_err;
      if ( show_errors ) {
        print_level0_pntr("**Couldn't delete free_rec entry ",&p);
        printf("err=%d, free_rec_ix=%d\n",err,free_rec_ix);
      }
    }
  }
}

static boolean allocate_rec(struct fcb *f, unsigned lc, struct level0_pntr *p)
{int err,key_lc; unsigned block_allocate_lc,rec_allocate_lc; boolean have_space=false;
 unsigned char key[maxkey_lc];
 struct level0_pntr p0,p1; struct leveln_pntr pn;

  if ( f->trace_freespace ) printf("allocating rec lc=%u\n",lc);
  p->segment = 0; p->sc = 0; p->lc = lc;
  if ( lc<min_disk_rec_lc ) have_space = true;
  else {
    rec_allocate_lc = rec_allocation_lc(lc);
    p0.segment = 0; p0.sc = 0; p0.lc = rec_allocate_lc;
    key_lc = pack_lc_key(key,p0);
    kf_get_ptr(f,free_lc_ix,key,key_lc,&p1);
    err = kf_next_ptr(f,free_lc_ix,key,&key_lc,maxkey_lc,&p1);
    if ( err!=ateof_err ) {
      if ( err!=no_err ) {
        set_error1(f,alloc_rec_err,"**Couldn't get free_lc entry, err=",err);
      }
      if ( key_lc!=freespace_lc_key_lc ) set_error1(f,alloc_rec_err,"**Uh Oh. Expected free_lc_key to be 14, is=",key_lc);
      unpack_lc_key(key,&p0);
      if ( p0.lc>=rec_allocate_lc ) {
        have_space = true;
        p->segment = p0.segment;
        p->sc = p0.sc;
        delete_freespace_entry(f,&p0);
        p0.sc = p0.sc + rec_allocate_lc;
        p0.lc = p0.lc - rec_allocate_lc;
        if ( p0.lc>0 ) insert_freespace_entry(f,&p0);
      }
    }
    if ( !have_space ) {
      block_allocate_lc = allocation_lc(lc,(unsigned)(16*block_lc));
      have_space = extend_file(f,block_allocate_lc,&pn);
      p->segment = pn.segment;
      p->sc = pn.block << f->block_shift;
      p0.segment = pn.segment;
      p0.sc = p->sc + rec_allocate_lc;
      p0.lc = block_allocate_lc - rec_allocate_lc;
      /*      printf("old scheme p=%d/%lu/%lu, p0=%d/%lu/%lu\n",p->segment,p->sc,p->lc,p0.segment,p0.sc,p0.lc);*/

      /*      p->segment = pn.segment;
      p->sc = (pn.block << f->block_shift) + (block_allocate_lc - rec_allocate_lc);
      p0.segment = pn.segment;
      p0.sc = pn.block << f->block_shift;
      p0.lc = block_allocate_lc - rec_allocate_lc;*/
      /*      printf("new scheme p=%d/%lu/%lu, p0=%d/%lu/%lu\n",p->segment,p->sc,p->lc,p0.segment,p0.sc,p0.lc);*/
      if ( f->trace_freespace ) printf("new space, block_lc=%d, residual=%d\n",
        block_allocate_lc,block_allocate_lc-rec_allocate_lc);
      if ( p0.lc>0 ) insert_freespace_entry(f,&p0);
    }
  }
  return(have_space);
}

/* deallocate_rec deallocates any space allocated with pointer p. */
/*   If p.lc<min_disk_rec_lc then p doesn't point to a disk record, the  */
/*   data has been stored in p.sc.                                */


static void deallocate_rec(struct fcb *f, struct level0_pntr p)
{int err,key_lc,start_key_lc,lc,rec_lc; struct level0_pntr p0,p1; char start_key[maxkey_lc],key[maxkey_lc];

  if ( f->trace_freespace ) {
    print_level0_pntr("deallocating rec ",&p);
    printf("\n");
  }
  if ( p.lc >= min_disk_rec_lc ) { /* a real disk record */
    p0 = p;
    p0.lc = rec_allocation_lc(p.lc);
    start_key_lc = pack_rec_key(start_key,p0);
    err = kf_get_rec(f,free_rec_ix,start_key,start_key_lc,(char *) &lc,&rec_lc,(unsigned)sizeof(int));
    if ( err==no_err ) {
      f->error_code = dealloc_rec_err;
      if ( show_errors ) {
        print_level0_pntr("**Uh Oh. Trying to deallocate entry allready in free list ",&p0);
        printf("\n");
      }
    }
    err = kf_prev_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,(char *) &lc,&rec_lc,(unsigned)sizeof(int));
    if ( err==atbof_err ) /* nothing to merge */ { if ( f->trace_freespace ) printf("prev is bof\n"); }
    else if ( err!=no_err ) {
      set_error1(f,dealloc_rec_err,"**couldn't get prev rec in deallocate_rec, err=",err);
    }
    else {
      key_lc = unpack_rec_key(key,&p1);
      p1.lc = lc;
      if ( f->trace_freespace ) print_level0_pntr("prev rec is ",&p1);
      if ( contiguous(p1,p0) ) {
        delete_freespace_entry(f,&p1);
        p0.sc = p1.sc;
        p0.lc = p0.lc + p1.lc;
        if ( f->trace_freespace ) print_level0_pntr("contiguous, merged entry is ",&p0);
        err = kf_get_rec(f,free_rec_ix,start_key,start_key_lc,(char *) &lc,&rec_lc,(unsigned)sizeof(int));
      }
      else err = kf_next_ptr(f,free_rec_ix,key,&key_lc,maxkey_lc,&p1 /*get past current entry*/);
      if ( f->trace_freespace ) printf("\n");
    }
    err = kf_next_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,(char *) &lc,&rec_lc,(int)sizeof(int));
    if ( err==ateof_err ) /* nothing to merge */ { if ( f->trace_freespace ) printf("next is eof\n"); }
    else if ( err!=no_err ) {
      set_error1(f,dealloc_rec_err,"**couldn't get next rec in deallocate_rec, err=",err);
    }
    else {
      key_lc = unpack_rec_key(key,&p1);
      p1.lc = lc;
      if ( f->trace_freespace ) print_level0_pntr("next rec is ",&p1);
      if ( contiguous(p0,p1) ) {
        delete_freespace_entry(f,&p1);
        p0.lc = p0.lc + p1.lc;
        if ( f->trace_freespace ) print_level0_pntr(" contiguous, merged entry is ",&p0);
      }
      if ( f->trace_freespace ) printf("\n");
    }
    insert_freespace_entry(f,&p0);
  }
}

static void init_free_space(struct fcb *f)
{int bufix; struct leveln_pntr b; struct key dummy;

  bufix = allocate_index_block(f,free_rec_ix,&b,level_zero,&dummy,0);
  f->first_at_level[level_zero][free_rec_ix] = f->buffer[bufix].contents;
  kf_set_bof(f,free_rec_ix);
  bufix = allocate_index_block(f,free_lc_ix,&b,level_zero,&dummy,0);
  f->first_at_level[level_zero][free_lc_ix] = f->buffer[bufix].contents;
  kf_set_bof(f,free_lc_ix);
}


/* user callable entries */

int set_bof(struct fcb *f)
{
  if ( check_fcb(f) ) {
    kf_set_bof(f,user_ix);
  }
  return(f->error_code);
}

int set_eof(struct fcb *f)
{int ix;

  if ( check_fcb(f) ) {
    if ( f->primary_level[user_ix]==0 ) f->position[user_ix] = f->first_at_level[0][user_ix];
    else f->position[user_ix] = f->last_pntr[1][user_ix];
    ix = get_index(f,f->position[user_ix]);
    f->position_ix[user_ix] = f->buffer[ix].b.ix.keys_in_block;
  }
  return(f->error_code);
}

int open_key(struct fcb *f, char id[], int lc, int read_only)
{  
  /*  read_fib(f,id,false,read_only);*/
  read_fib(f,id,machine_is_little_endian(),read_only);
  if ( f->error_code!=no_err ) fatal_error(f,badopen_err);
  else {
    init_key(f,id,lc);
    kf_set_bof(f,user_ix);
    kf_set_bof(f,free_rec_ix);
    kf_set_bof(f,free_lc_ix);
  }
  return(f->error_code);
}

int close_key(struct fcb *f)
{int i; FILE *temp;

/* printf("  get_buffer_cnt=%d, replace_buffer_cnt=%d\n",get_buffer_cnt,replace_buffer_cnt);*/
/* printf("  get_index_cnt=%d\n",get_index_cnt); */
  if ( f->trace ) {
    printf("  read_cnt =%d\n",read_cnt);
    printf("  write_cnt=%d\n",write_cnt);
  }
  if ( f->marker!=keyf ) f->error_code = notkeyfil_err;
  else {
    f->error_code = no_err;
    for (i=0; i<f->buffers_in_use; i++) {
      if (f->buffer[i].modified){
        write_page(f,f->buffer[i].contents,&(f->buffer[i].b) );
        if ( trace_io ) {
          print_leveln_pntr("  wrote block ",&(f->buffer[i].contents));
          print_buffer_caption(stdout,f,i);
          printf(" from buffer %d\n",i);
        }
      }
      if (f->buffer[i].lock_cnt>0) {
        set_error(f,bad_close_err,"**Buffer locked at close\n");
      }
    }
    write_fib(f);
    for (i=0; i<f->open_file_cnt; i++) {
      if (f->trace) printf("  closing segment %d\n",f->open_segment[i]);
      temp = f->open_file[i];
      fclose(temp);
    }
    f->marker = 0;
  }
  return(f->error_code);
}

int create_key(struct fcb *f, char id[], int lc)
{int i,j,bufix; struct leveln_pntr b; struct key dummy;

  f->error_code = no_err; f->version = current_version;
  f->segment_cnt = 0;
  f->marker = keyf; f->file_ok = true;
  f->read_only = false;
  for (i=0; i<max_index; i++) {
    f->primary_level[i] = level_zero;
    for (j=0; j<max_level; j++) {
      f->first_free_block[j][i] = nulln_ptr;
      f->first_at_level[j][i] = nulln_ptr;
      f->last_pntr[j][i] = nulln_ptr;
    }
  }
  f->max_file_lc = 1;
  for (i=0; i<file_lc_bits; i++) f->max_file_lc = f->max_file_lc * 2;
  f->max_file_lc = f->max_file_lc - 1;
  for (i=0; i<max_segments; i++) f->segment_length[i] = 0;

  if ( f->error_code==no_err ) {
    f->byte_swapping_required = machine_is_little_endian();
    /*    f->byte_swapping_required = false; */

    init_key(f,id,lc);
    write_fib(f);
    f->segment_cnt = 1; f->segment_length[0] = fib_blocks * block_lc;
    bufix = allocate_index_block(f,user_ix,&b,level_zero,&dummy,0);
    f->first_at_level[level_zero][user_ix] = f->buffer[bufix].contents;
    init_free_space(f);
    kf_set_bof(f,user_ix);
  }
  return(f->error_code);
}

int keyrec_lc(struct level0_pntr *p)
{
  if ( p->segment >= max_segments ) return(-1);
  else return(p->lc);
}

int next_ptr(struct fcb *f, char key[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{
  return( kf_next_ptr(f,user_ix,key,key_lc,max_key_lc,p) );
}

int prev_ptr(struct fcb *f, char key[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{
  return( kf_prev_ptr(f,user_ix,key,key_lc,max_key_lc,p) );
}

int get_ptr(struct fcb *f, char t[], int key_lc, struct level0_pntr *p)
{
  return( kf_get_ptr(f,user_ix,t,key_lc,p) );
}


int get_rec(struct fcb *f,char t[],int key_lc, char r[],int *rlc,int max_lc)
{
  return( kf_get_rec(f,user_ix,t,key_lc,r,rlc,max_lc) );
}

int next_rec(struct fcb *f, char t[], int *key_lc, int max_key_lc,
   char r[],int *rlc,int max_lc)
{
  return( kf_next_rec(f,user_ix,t,key_lc,max_key_lc,r,rlc,max_lc) );
}

int prev_rec(struct fcb *f, char t[], int *key_lc, int max_key_lc,
   char r[],int *rlc,int max_lc)
{
  return( kf_prev_rec(f,user_ix,t,key_lc,max_key_lc,r,rlc,(unsigned)max_lc) );
}

int put_ptr(struct fcb *f, char t[], int key_lc, struct level0_pntr *p)
{
  return( kf_put_ptr(f,user_ix,t,key_lc,*p) );
}

int delete_ptr(struct fcb *f, char t[], int key_lc)
{
  return( kf_delete_ptr(f,user_ix,t,key_lc) );
}

int put_rec(struct fcb *f,char t[], int key_lc, char r[], int rlc)
{
  return( kf_put_rec(f,user_ix,t,key_lc,r,(unsigned)rlc) );
}

int delete_rec(struct fcb *f, unsigned char key[], int key_lc)
{
  return( kf_delete_rec(f,user_ix,key,key_lc) );
}

/* Functions to support subrecords: */

/* Function: get_subrec
   Retrieve a part of a record.  This is provided for those cases where the 
   records are so large they must be read into memory from the disk in smaller
   pieces.  Use get_ptr to get the keyfile_pointer, then call get_subrec 
   repeatedly with bytes_to_read set to some manageable size and incrementing 
   offset by bytes_actually_read each time until p->lc bytes have been read.  
   (You must keep track of offset and bytes_to_read so that you don't ask for 
   too much.)

   Returns no_err if all went well, else a non-zero err code.
*/

int get_subrec(
   struct fcb *f, 
   struct level0_pntr *p,      // IN - the original pointer from get_ptr
   int offset,                 // IN - offset of data to get within the rec 
   int bytes_to_read,          // IN - how many bytes of the rec to get
   unsigned char *rec,         // IN-OUT - where to put the bytes from the rec
   int *bytes_actually_read,   // OUT - how many bytes were actually transferred
   int max_lc)                 // IN - max bytes to transfer
{size_t size; FILE *file;

  if ( check_fcb(f) ) {
    *bytes_actually_read = bytes_to_read;
    if ( *bytes_actually_read>max_lc ) {
      f->error_code = longrec_err; *bytes_actually_read = max_lc;
    }
    if ( p->lc<min_disk_rec_lc ) {
      if ( offset+bytes_to_read>=min_disk_rec_lc ) {
        f->error_code = longrec_err; *bytes_actually_read = min_disk_rec_lc - offset - 1;
      }
      memcpy(rec,(unsigned char *)(&p->sc)+offset,(size_t) *bytes_actually_read);
    }
    else {
      file = file_index(f,p->segment);
      if ( f->error_code!=no_err ) return(f->error_code);
      if ( fseeko(file,(FILE_OFFSET)p->sc+offset,0)!=0 ) {
        f->error_code = seek_err; return(f->error_code);
      }
      size = fread(rec,(size_t) 1,(size_t) *bytes_actually_read,file);
      if ( size!=(size_t)*bytes_actually_read ) f->error_code = read_err;
    }
  }
  return(f->error_code);
}


void kf_test(struct fcb *f)
{

}
