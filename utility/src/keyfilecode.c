
/*                                                               */
/* Copyright 1984,1985,1986,1988,1989,1990,2003,2004 by Howard Turtle */
/*                                                               */

#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "keyerr.h"
#include "ht.h"
#include "keydef.h"

#define size_t unsigned
#define maxint 32767
#define show_errors false

/*int
  get_buffer_cnt=0,
  replace_buffer_cnt=0,
  get_index_cnt=0,
  hash_chain_cnt=0;*/

int
read_cnt=0,
  write_cnt=0;

static struct level0_pntr
  null0_ptr = {max_segments,0,0},
  dummy_ptr = {0,0,0};
static struct leveln_pntr
  nulln_ptr = {max_segments,0};
static int
  block_shift;
static char
  index_caption[3][10]={"user    \0","free_rec\0","free_lc \0"};


static int allocate_block();
static boolean allocate_rec();
static void deallocate_block();
static void deallocate_rec();
void update_index();
static void split_block();
static FILE *file_index();
int unpack_lc_key();
int unpack_rec_key();


/* miscellaneous procedures */

static int min(int i, int j)
{
  if ( i<j ) return(i);
  else return(j);
}

void mvc(t1,sc1,t2,sc2,lc)
unsigned char t1[],t2[]; int sc1,sc2,lc;
{int i,p1,p2;

  memmove(t2+sc2,t1+sc1,lc);
/*  printf("in mvc, t2=%ld, t2+sc2=%ld\n",t2,t2+sc2); */
/*  if ( t1+sc1 > t2+sc2 )
    for (i=0; i<lc; i++) t2[sc2++] = t1[sc1++];
  else {
    p1 = sc1 + lc; p2 = sc2 + lc;
    for (i=0; i<lc; i++) t2[--p2] = t1[--p1];
  }
*/
}

static void fatal_error(f,err)
struct fcb *f; int err;
{
   f->error_code = err; f->file_ok = false;
}

static void print_lc_key(struct key *k, char caption[])
{int i; struct level0_pntr p;

  unpack_lc_key(k->text,&p);
  printf("%s lc=%d, %d/%d",caption,p.lc,p.segment,p.sc);
}

static void print_rec_key(struct key *k, char caption[])
{int i; struct level0_pntr p;

  unpack_rec_key(k->text,&p);
  printf("%s %d/%d",caption,p.segment,p.sc);
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

static void print_user_key1(struct key *k, char caption[])
{
  print_user_key(k,caption);
  printf("\n");
}

void print_nth_key(struct ix_block *b, int n, char caption[])
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
}

static void print_pntr0(struct level0_pntr p, char caption[])
{
  printf("%s",caption);
  printf("%d/%d/%d",p.segment,p.sc,p.lc);
}


static long allocation_lc(int lc, int unit)
{
  if ( lc<=0 ) return(0);
  else return(((lc-1) / unit + 1) * unit);
}

static long rec_allocation_lc(int lc)
{
  if ( lc<=0 ) return(0);
  else return(((lc-1) / rec_allocation_unit + 1) * rec_allocation_unit);
}

/* init_file_name separates the file name and any extension */
/*   and saves the two parts in the fcb                     */

static void init_file_name(struct fcb *f, char id[])
{
  int i, name_lc, f_lc, ext_lc = 0;
  name_lc = strlen(id);
  if (name_lc > max_filename_lc + max_extension_lc) 
    fatal_error(f,bad_name_err); /* whole thing too long */
  i = name_lc - 1;  
  /* scan from right to left */
  while ( i >= 0 && id[i] != '.') {i--; ext_lc++;}
  if (i >= 0) {
    f_lc = i; 
    ext_lc++;
  } else {
    f_lc = name_lc;
    ext_lc = 0;
  }
  if (f_lc >= max_filename_lc) 
    fatal_error(f,bad_name_err);
  else {
    strncpy(f->file_name, id, f_lc);
    f->file_name[f_lc] = '\0';
  }
  if ( ext_lc >= max_extension_lc ) 
    fatal_error(f,bad_name_err);
  else {
      strncpy(f->file_extension, id + i, ext_lc);
      f->file_extension[ext_lc] = '\0';
  }
}

/* compare_key compares a key k with the suffix portion stored in an   */
/*   index block.  It is used for searching when we allready know that */
/*   the block prefix matches the key.  compare_full_key compares the  */
/*   entire key including the prefix                                   */

enum comparison compare_key(struct key *k, struct ix_block *b, int ix)
{int r,lc1,lc2;

  lc1 = k->lc - b->prefix_lc;
  lc2 = b->keys[ix].lc;
  if ( lc1<=lc2 ) {
    r = memcmp(k->text+b->prefix_lc,(char *) b->keys+b->keys[ix].sc,lc1 );
    if (r<0) return(less);
    else if (r>0) return(greater);
    else if ( lc1==lc2 ) return(equal);
    else return(less);
  }
  else {
    r = memcmp(k->text+b->prefix_lc,(char *) b->keys+b->keys[ix].sc,lc2 );
    if (r<0) return(less);
    else if (r>0) return(greater);
    else if ( lc1==lc2 ) return(equal);
    else return(greater);
  }

}

enum comparison compare_full_key(struct key *k, struct ix_block *b, int ix)
{int r,lc1;

  if ( k->lc<b->prefix_lc ) {
    r = memcmp(k->text,(char *) b->keys+keyspace_lc-b->prefix_lc,k->lc );
    if ( r<0 ) return(less);
    else if (r>0) return(greater);
    else return(less);
  }
  else {
    r = memcmp(k->text,(char *) b->keys+keyspace_lc-b->prefix_lc,b->prefix_lc );
    if (r<0) return(less);
    else if (r>0) return(greater);
    else return(compare_key(k,b,ix));
  }
}

/* static enum comparison cmpstr(t1,lc1,t2,sc2,lc2)
char t1[],t2[]; int lc1,sc2,lc2;
{int r;

  r = memcmp(t1,t2+sc2,min(lc1,lc2) );
  if (r<0) return(less);
  else if (r>0) return(greater);
  else {
    if ( lc1==lc2 ) return(equal);
    else if ( lc1<lc2 ) return(less);
    else return(greater);
  }
} */

static boolean read_fib(struct fcb *f,char id[])
{size_t size,lc; FILE *file;

  lc = allocation_lc(fib_lc,block_lc);
  file = fopen(id,"rb+");
  if ( file==NULL ) f->error_code = badopen_err;
  else if ( fseek(file,(long) 0 /* * block_lc */,0)!=0 ) f->error_code = badopen_err;
  else {
    size = fread(f,(size_t) 1,lc,file);
    if ( size!=lc ) f->error_code = notkeyfil_err;
    fclose(file);
  }
  return(f->error_code==no_err);
}

static void write_fib(struct fcb *f)
{size_t size=0,lc; FILE *file;

  lc = allocation_lc(fib_lc,block_lc);
  file = file_index(f,0);
  if ( f->error_code==no_err ) {
    fseek(file,(long) 0 /* * block_lc */,0);
    size = fwrite(f,(size_t) 1,lc,file);
  }
  if ( size!=lc ) printf("**write_fib failed\n");
}

void read_page(struct fcb *f, struct leveln_pntr p, void *buf)
{size_t size=0; FILE *file; long b;

 read_cnt++;
  if ( f->trace ) printf("reading page %d/%ld\n",p.segment,p.block);
  /*  printf("reading page %d/%ld\n",p.segment,p.block); */
  file = file_index(f,p.segment);
  b = (p.block) << block_shift;
  if ( file==NULL ) f->error_code = read_err;
  else if ( fseek(file,b,0)!=0 ) f->error_code = seek_err;
  /*  else if ( fseek(file,p.block,0)!=0 ) f->error_code = seek_err;*/
  else size = fread(buf,(size_t) 1,(size_t) block_lc,file);
  if ( size!=block_lc ) {
    f->error_code = read_err;
    if ( show_errors )
      printf("**fread failed, block=%d/%ld, requested lc=%d (%ud)\n",p.segment,p.block,block_lc,size);
  }
}

static void write_page(struct fcb *f, struct leveln_pntr p, void *buf)
{size_t size=0; FILE *file; long b;

 write_cnt++;
  if ( f->trace ) printf("writing page %d/%ld\n",p.segment,p.block);
  /* printf("writing page %d/%ld\n",p.segment,p.block); */
  file = file_index(f,p.segment);
  b = (p.block) << block_shift;
  if ( file==NULL ) f->error_code = write_err;
  else if ( fseek(file,b,0)!=0 ) f->error_code = seek_err;
  /*  else if ( fseek(file,p.block,0)!=0 ) f->error_code = seek_err;*/
  else size = fwrite(buf,(size_t) 1,(size_t) block_lc,file);
  if ( size!=block_lc ) {
    f->error_code = write_err;
    if ( show_errors )
      printf("**fwrite failed, block=%d/%ld, requested lc=%d (%ud)\n",p.segment,p.block,block_lc,size);
  }
}

static void write_page_and_flush(struct fcb *f, struct leveln_pntr p, void *buf)
{int i,min_block,max_block; struct leveln_pntr pn;

  min_block = p.block - 8;
  max_block = min_block + 16;
  for (i=0; i<f->buffers_in_use; i++)
    if (f->buffer[i].modified && (!f->buffer[i].locked) ) {
      pn = f->buffer[i].contents;
      if ( pn.segment==p.segment && pn.block>=min_block && pn.block<=max_block ) {
        write_page(f,f->buffer[i].contents,&(f->buffer[i].b) );
	if ( pn.block!=p.block ) printf("writing extra block, base=%d/%d, extra=%d/%d\n",p.segment,p.block,pn.segment,pn.block);
        f->buffer[i].modified = false;
      }
    }
}

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

static void build_segment_name(struct fcb *f,int segment,char name[])
{
  strcpy(name,f->file_name);
  if (segment>0) {
    sprintf(name+strlen(name),"$%d\0",segment);
  }
  strcat(name,f->file_extension);
}

/* open_segment opens a file segment.  If it is new it is opened in */
/*   write+ mode otherwise it is opened in read+ mode.  If the open */
/*   fails then f->open_file[ix] is set to NULL and f->error_code is*/
/*   set.  In any case the directories segment_ix[] and             */
/*   open_segment[] are set.  */

static void open_segment(struct fcb *f,int segment,int ix)
{char name[max_filename_lc];

  build_segment_name(f,segment,name);
  if ( segment >= f->segment_cnt ) f->open_file[ix] = fopen(name,"wb+");
  else f->open_file[ix] = fopen(name,"rb+");
  if (f->open_file[ix]==NULL) {
    f->error_code = segment_open_err;
  }
  f->segment_ix[segment] = ix;
  f->open_segment[ix] = segment;
  if ( f->trace)  printf("Opening segment %s on file index %d\n",name,ix);
}

static int file_ix(struct fcb *f,int segment)
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

static FILE *file_index(struct fcb *f,int segment)
{int ix;

  ix = file_ix(f,segment);
  return(f->open_file[ix]);
}


static int pntr_lc(int level)
{
  if ( level==0 ) return(level0_lc);
  else return(leveln_lc);
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
    default: /* illegal_err code */ ok = false; break;
  }
  if ( ok ) f->error_code = no_err;
  return(ok);
}

static boolean set_up(struct fcb *f, unsigned char key[],int key_lc, struct key *k)
{
  if ( !check_fcb(f) ) return(false);
  k->lc = key_lc;
  if ( k->lc>0 && k->lc<maxkey_lc ) {
    memcpy(k->text,key,key_lc); return(true);
  }
  else {
    f->error_code = badkey_err; k->lc = 0;
    return(false);
  }
}

static void set_position(struct fcb *f, int index, struct leveln_pntr b, int ix)
{
  f->position[index] = b; f->position_ix[index] = ix;
}

/* pointer conversion copying between record and block pointers  */
/*   require conversion from block offset to character offset    */

static void copy_ptr(struct leveln_pntr p1, struct level0_pntr *p2)
{
  p2->segment = p1.segment; p2->sc = p1.block; p2->lc = 0;
}

/*static void copy_0ton_ptr(struct level0_pntr p1, struct leveln_pntr *p2)
{
  p2->segment = p1.segment; p2->block = p1.sc >> block_shift; p2->lc = 0;
}*/

static boolean gt_pntr(struct leveln_pntr p1, struct leveln_pntr p2)
{
  if ( p1.segment<p2.segment ) return(false);
  else if ( p1.segment>p2.segment ) return(true);
  else return( p1.block>p2.block );
}

static boolean gt_pntr0(struct level0_pntr p1, struct level0_pntr p2)
{
  if ( p1.segment<p2.segment ) return(false);
  else if ( p1.segment>p2.segment ) return(true);
  else return( p1.sc>p2.sc );
}

/* static boolean eq_pntr(struct leveln_pntr p1, struct leveln_pntr p2)
{
  return( (p1.segment==p2.segment) && (p1.block==p2.block) );
}
*/

#define eq_pntr(p1,p2) ((p1.block==p2.block) && (p1.segment==p2.segment))
#define null_pntr(p1) ((p1.segment==max_segments) && (p1.block==0))

/* uncompress_int uncompresses an integer compressed in array p */
/*   and returns the number of bytes consumed to decompress     */
/*   the int.  */


static int uncompress_int(unsigned long *i, unsigned char p[])
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

/* compress_int compresses integer i into a byte string and       */
/*   returns the length of the compressed string. Note that ptr   */
/*   points to the rightmost character in the compressed string,  */
/*   i.e. the int is compressed from ptr to the left.             */


static int compress_int(int i, unsigned char *ptr)
{int lc=0; boolean done=false; unsigned char *p;

  p = ptr;
  do {
    *p = (i & 127) | 128;
    lc++; p--;
    i = i>>7;
  } while ( i>0 );
  *ptr = *ptr & 127; /* high bit off */
  return(lc);
}

/* compress0_pntr compresses a level0 pointer p into a byte */
/*   string.  cp points to the last byte in the compressed  */
/*   string and compression will proceed from right to left */
/*   Note that segment and lc are compressed but sc is not  */
/*   since it would usually expand under compression.       */

static int compress0_pntr(struct level0_pntr *p, unsigned char *cp)
{int lc,lc1;

  lc = compress_int(p->lc,cp);
  memcpy((char *)cp-lc-sizeof(int)+1,&(p->sc),sizeof(int));
  lc1 = compress_int(p->segment,cp-lc-sizeof(int));
  return( lc+lc1+sizeof(int) );
}

/* uncompress0_pntr uncompresses a level0 pointer.  Note   */
/*   cp points to the first character in the compressed    */
/*   string (unlike compress0_pntr).                       */

static int uncompress0_pntr(struct level0_pntr *p, unsigned char *cp)
{int lc,lc1;

  lc = uncompress_int((unsigned long *)&(p->segment),cp);
  memcpy(&(p->sc),cp+lc,sizeof(int));
  lc1 = uncompress_int(&(p->lc),cp+lc+sizeof(int));
  return( lc+lc1+sizeof(int) );
}

/* compressn_pntr compresses a leveln pointer p into a byte */
/*   string.  cp points to the last byte in the compressed  */
/*   string and compression will proceed from right to left */

static int compressn_pntr(struct leveln_pntr *p, unsigned char *cp)
{int lc,lc1;

/*  memcpy((char *)cp-sizeof(int)+1,&(p->block),sizeof(int));*/
  lc = compress_int(p->block,cp);
  lc1 = compress_int(p->segment,cp-lc);
  return( lc+lc1 );
}

/* uncompressn_pntr uncompresses a leveln pointer.  Note   */
/*   cp points to the first character in the compressed    */
/*   string (unlike compressn_pntr).                       */

static int uncompressn_pntr(struct leveln_pntr *p, unsigned char *cp)
{int lc,lc1;

  lc = uncompress_int((unsigned long *)&(p->segment),cp);
  lc1 = uncompress_int(&(p->block),cp+lc);
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

/* buffer handling */

/* reset_ages is called when f->current_age reaches maxint.     */
/*   The age of all open files is set to 0 and  */
/*   f->current_age is set to 0.                                */

static void reset_ages(struct fcb *f)
{int i;

  for (i=0; i<f->open_file_cnt; i++) f->file_age[i] = 0;
  f->current_age = 0;
}


/* static int hash_value(struct fcb *f, struct leveln_pntr block)
{int k;

  k =  ((block.block / block_lc) + block.segment)% f->buffers_allocated;
  return(k);
} */

/* #define hash_value(b,limit)  (((b.block / block_lc) + b.segment) % limit) */
#define hash_value(b,limit)  (((b.block) + b.segment) % limit)


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

static void hash_chain_insert(struct fcb *f, int bufix)
{int k,next,last=-1; struct leveln_pntr block;

  block = f->buffer[bufix].contents;
  k = hash_value(block,f->buf_hash_entries);
  next = f->buf_hash_table[k];
  if ( next<0 ) f->buf_hash_table[k] = bufix;
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
    printf("  inserted buffer %d (%d/%d) into hash chain %d\n",bufix,block.segment,block.block,k);
    print_hash_chain(stdout,f,k);
  }
}

/* remove the buffer with index bufix from its hash chain */

static void hash_chain_remove(struct fcb *f, int bufix)
{int k,next,last; struct leveln_pntr block;

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
  /*  printf("just made buffer %d youngest, chain=",bufix); */
  /*  print_buffer_MRU_chain(stdout,f); */
}

static void init_buffer(struct fcb *f,int i)
{
  f->buffer[i].contents = nulln_ptr;
  f->buffer[i].locked = false;
  f->buffer[i].modified = false;
  f->buffer[i].older = -1;
  f->buffer[i].younger = -1;
  f->buffer[i].hash_next = -1;
  /* f->buffer[i].b.ix.keys_in_block = 0;
  f->buffer[i].b.ix.chars_in_use = 0;
  f->buffer[i].b.ix.level = 0;
  f->buffer[i].b.ix.next = nulln_ptr;
  f->buffer[i].b.ix.prev = nulln_ptr; */
}

/* vacate_oldest_buffer is called when a new buffer is needed */
/*   if there are unallocated buffers then the next one is   */
/*   added to the buffer chain and returned.  If all buffers */
/*   are in use then the oldest unlocked buffer is flushed   */
/*   (if necessary) and returned                             */

static int vacate_oldest_buffer(struct fcb *f)
{int oldest,cnt=0; boolean done;

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
        if ( show_errors ) printf("**couldn't allocate a buffer\n");
      }
      else if ( f->buffer[oldest].locked ) done = false;
      else done = true;
    }
    while ( !done );
    if ( f->buffer[oldest].modified )
      write_page(f,f->buffer[oldest].contents,&(f->buffer[oldest].b));
    /* write_page_and_flush(f,f->buffer[oldest].contents,&(f->buffer[oldest].b));*/
    hash_chain_remove(f,oldest);
  }
  if ( f->trace ) printf("  just vacated oldest buffer, bufix=%d\n",oldest);
  return(oldest);
}

static int get_buffer(struct fcb *f, struct leveln_pntr block, boolean *not_found)
{int i,bufix;

/*  get_buffer_cnt++; */

  f->current_age++;
  if ( f->current_age==maxint ) reset_ages(f);
  bufix = search_hash_chain(f,block);
  *not_found = bufix<0 ;

  if ( *not_found ) {

    /*    replace_buffer_cnt++; */

    bufix = vacate_oldest_buffer(f);
    f->buffer[bufix].contents = block;
    f->buffer[bufix].modified = false;
    hash_chain_insert(f,bufix);
  }
  else make_buffer_youngest(f,bufix);
  /*  printf("  just got buf=%d, chain=",bufix); print_buffer_MRU_chain(stdout,f);printf("\n"); */
  return(bufix);
}

static int get_index(struct fcb *f, struct leveln_pntr b)
{boolean not_found; int bufix,index_type;

/*  get_index_cnt++; */
  bufix = get_buffer(f,b,&not_found);
  /*  printf("just got %d/%d, not_found=%d\n",b.segment,b.block,not_found); */
  if ( not_found ) read_page(f,b,&(f->buffer[bufix].b));
  /*  print_ix_block(stdout,&(f->buffer[bufix].b));*/
  index_type = f->buffer[bufix].b.ix.index_type;
  f->mru_at_level[f->buffer[bufix].b.ix.level][index_type] = b;
  return(bufix);
}

void kf_set_bof(struct fcb *f, int index)
{
  f->position[index] = f->first_at_level[0][index];
  f->position_ix[index] = 0;
}

int kf_wire_index(struct fcb *f, struct leveln_pntr b)
{boolean not_found; int bufix;

/*  get_index_cnt++; */
  bufix = get_index(f,b);
  f->buffer[bufix].locked = true;
  return(bufix);
}

/*key manipulation*/

void get_nth_key(struct ix_block *b, struct key *k, int n)
{
  if ( n<0 || n>=b->keys_in_block ) k->lc = 0;
  else {
    mvc(b->keys,keyspace_lc-b->prefix_lc,k->text,0,b->prefix_lc);
    k->lc = b->keys[n].lc + b->prefix_lc;
    mvc(b->keys,b->keys[n].sc,k->text,b->prefix_lc,b->keys[n].lc);
  }
}

static void get_max_key(struct ix_block *b, struct key *k)
{
  if ( b->keys_in_block<1 ) k->lc = 0;
  else {
    get_nth_key(b,k,b->keys_in_block-1);
  }
}

/* Prefix compression.  Keys in a block may have a common prefix that */
/*   can be omitted when stored.  We make use of prefixes that are    */
/*   logically necessary given the keys present in the parent block.  */
/*   The first child of a parent gets no compression (not worth       */
/*   getting the prev block to compute the prefix).  The last block   */
/*   in any level gets no compression (no max key). Other children    */
/*   are compressed using the common prefix of their entry and the    */
/*   previous entry in the parent (may be suboptimal).  Using this    */
/*   strategy, compression for a block changes only when a block      */
/*   splits on insertion or when an an index block is removed on      */
/*   deletion (not considered here but must be if index blocks can be */
/*   removed.)                                                        */
/*                                                                    */
/*   On block splits the prefix can never get shorter since the max   */
/*   key for the split blocks is the same as for the original block.  */
/*   On splits we get the prev key when finding the parent, split the */
/*   blocks, and further compress the split blocks when possible.     */

static int find_prefix_lc(struct key *k1, struct key *k2)
{int i=0,max_lc;

  max_lc = min(k1->lc,k2->lc);
  max_lc = min(max_lc,max_prefix_lc);
  while ( i<max_lc && (k1->text[i]==k2->text[i]) ) i++;
  /*  print_user_key(k1,"prefix lc of ");
  print_user_key(k2," and ");
  printf(" is %d\n",i);*/
  return(i);
}

/* Index searching */

/* Interior index blocks contain entries that point to the subtree
 *   containing keys <= entry key (and greater than the previous
 *   entry key.  Each level in the index has an additional pointer
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

static int search_block(struct fcb *f, struct ix_block *b, struct key *k, boolean *found)
{int mid,high,ix=0;

  *found = false;
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
    if ( b->index_type==user_ix )          print_user_key(k,"  searched user block for ");
    else if ( b->index_type==free_lc_ix )  print_lc_key  (k,"  searched f_lc block for ");
    else if ( b->index_type==free_rec_ix ) print_rec_key (k,"  searched f_rec block for ");
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
 *   this level.  If a key is larger than any in a block, then
 *   the last_pntr pointer is the returned.
 */

static struct leveln_pntr search_index(struct fcb *f, int index, int stop_lvl, struct key *k)
{struct leveln_pntr child; int ix,bufix; boolean done=false,found;

  child = f->first_at_level[f->primary_level[index]][index];
  if ( stop_lvl<=f->primary_level[index] )
    do {
      bufix = get_index(f,child);
      ix = search_block(f,&(f->buffer[bufix].b.ix),k,&found);
      done = f->buffer[bufix].b.ix.level<=stop_lvl;
      if ( ix>=f->buffer[bufix].b.ix.keys_in_block ) { /* larger than any key */
        if ( null_pntr(f->buffer[bufix].b.ix.next) )
          child = f->last_pntr[f->buffer[bufix].b.ix.level][index];
	else {
          done = true; f->error_code = max_key_err; child = nulln_ptr;
	  if ( show_errors ) {
            print_user_key(k,"Uh Oh.  ");
            printf(" is larger than any key in block\n");
	  }
	}
      }
      else unpackn_ptr(&(f->buffer[bufix].b.ix),ix,&child);
    }
    while ( !done );
  return(child);
}

static struct leveln_pntr parent_block_and_prefix(struct fcb *f, int bufix,
  struct key *k, struct key *parent_key, struct key *prev_key)
{int parent_level,parent_ix,index,ix; boolean found; struct leveln_pntr p;

  parent_key->lc = 0; prev_key->lc = 0;
  parent_level = f->buffer[bufix].b.ix.level + 1;
  index = f->buffer[bufix].b.ix.index_type;
  p = nulln_ptr;
  if ( parent_level<=f->primary_level[index] ) {
    p = search_index(f,index,parent_level+1,k);
    parent_ix = get_index(f,p);

    ix = search_block(f,&(f->buffer[parent_ix].b.ix),k,&found);
    get_nth_key(&(f->buffer[parent_ix].b.ix),parent_key,ix);
    get_nth_key(&(f->buffer[parent_ix].b.ix),prev_key,ix-1);
  }
  /*  print_user_key(k," parent key for k=");
  print_user_key(parent_key," is ");
  print_user_key(prev_key,", prev is ");
  printf("\n");*/
  /*  if ( prev_key->lc==0 ) {
    print_user_key1(k,"prev key is null for ");
    print_index_block(stdout,&(f->buffer[parent_ix].b.ix));
    }*/
  if ( !eq_pntr(p,f->mru_at_level[parent_level][index]) )
    printf("**Uh Oh. parent doesn't match MRU pointer, parent_level=%d,primary_level=%d\n",parent_level,f->primary_level[index]);
  return(p);
}

/* file initialization */

/* init_key initializes the temporary part of the fcb and a few static  */
/*   variables.  It assumes that the fib has been initialized.          */

static void init_key(struct fcb *f, char id[], int lc)
{int i,j,bufix,hash_target,hash_blocks;;
  block_shift = log(block_lc) / log(2);
  /*  printf("block_shift=%d\n",block_shift);*/
  if ( !check_fcb(f) ) return;
  if ( f->version!=current_version) { f->error_code = version_err; return; }
  if ( lc<min_fcb_lc ) { f->error_code = smallfcb_err; return; }
  f->trace = 0;
  f->trace_freespace = 0;
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
  /*  printf("MRU chain="); print_buffer_MRU_chain(stdout,f);printf("\n"); */
}

/* record moving */

int copy_rec(struct fcb *f,struct level0_pntr *p, unsigned char rec[], int *rec_lc ,int max_rec_lc)
{size_t size; FILE *file;

  if ( check_fcb(f) ) {
    *rec_lc = p->lc;
    if ( *rec_lc>max_rec_lc ) {
      f->error_code = longrec_err; *rec_lc = max_rec_lc;
    }
    if ( *rec_lc<=long_lc ) memcpy(rec,&p->sc,(size_t) *rec_lc);
    else {
      file = file_index(f,p->segment);
      if ( f->error_code!=no_err ) return(f->error_code);
      if ( fseek(file,p->sc,0)!=0 ) {
        f->error_code = seek_err; return(f->error_code);
      }
      size = fread(rec,(size_t) 1,(size_t) *rec_lc,file);
      if ( size!=*rec_lc ) f->error_code = read_err;
    }
  }
  return(f->error_code);
}


/*** buffer handling ***/

static void lock_buffer(f,bufix)
struct fcb *f; int bufix;
{
  f->buffer[bufix].locked = true;
}

static void unlock_buffer(f,bufix)
struct fcb *f; int bufix;
{
  f->buffer[bufix].locked = false;
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

static boolean extend_file(struct fcb *f,long lc,struct leveln_pntr *p)
{int current,ix;

  current = f->segment_cnt - 1;
  /*  if ( ((lc >> block_shift) << block_shift) != lc )
      printf("**Uh oh.  Tried to use bad  block size=%ld\n",lc); */
  if ( (max_segment_lc - f->segment_length[current]) < lc ) { /* segment full */
    if ( f->segment_cnt>=(max_segments-1) ) {
      f->error_code = nospace_err;
      if ( f->trace ) printf("  extend_file failed, segment=%d\n",current);
    }
    current++;
    ix = file_ix(f,current); /* opens current segment */
    f->segment_cnt++;
  }
  p->segment =current;
  /*  temp = f->segment_length[current] >> block_shift;*/
  p->block = f->segment_length[current] >> block_shift;
  /*  p->block = f->segment_length[current];*/
  f->segment_length[current] = f->segment_length[current] + lc;
  if ( f->trace )
    printf("  extended file, seg=%d, block=%ld, lc=%ld\n",p->segment,p->block,lc);
  return(f->error_code!=nospace_err);
}

static int allocate_index_block(struct fcb *f, int index, struct leveln_pntr *b, int lvl)
{int bufix;
/* printf("  allocating index block at level %d\n",lvl); */
  bufix = allocate_block(f);
  f->buffer[bufix].b.ix.keys_in_block = 0;
  f->buffer[bufix].b.ix.chars_in_use = 0;
  f->buffer[bufix].b.ix.index_type = index;
  f->buffer[bufix].b.ix.prefix_lc = 0;
  f->buffer[bufix].b.ix.level = lvl;
  f->buffer[bufix].b.ix.next = nulln_ptr;
  f->buffer[bufix].b.ix.prev = nulln_ptr;
  f->mru_at_level[lvl][index] = f->buffer[bufix].contents;
  *b = f->buffer[bufix].contents;
  return(bufix);
}

/*** index deletion ***/

/* simple_delete deletes the ix^th key from the given block.
 */

static void simple_delete(struct ix_block *b, int ix)
{int i,key_sc,deleted_lc,deleted_sc,plc; struct level0_pntr p0; struct leveln_pntr pn;

  key_sc = keyspace_lc - b->chars_in_use;
  if ( b->level>0 ) plc = unpackn_ptr(b,ix,&pn);
  else plc = unpack0_ptr(b,ix,&p0);
  deleted_lc = b->keys[ix].lc + plc;
  deleted_sc = b->keys[ix].sc;
  mvc(b->keys,key_sc,b->keys,key_sc+deleted_lc,deleted_sc-key_sc);
  b->chars_in_use = b->chars_in_use - deleted_lc;
  b->keys_in_block--;
  for (i=ix; i<b->keys_in_block; i++) b->keys[i] = b->keys[i+1];
  for (i=0; i<b->keys_in_block; i++)
    if (b->keys[i].sc<deleted_sc )
      b->keys[i].sc = b->keys[i].sc + deleted_lc;
}

static void remove_index(struct fcb *f, struct leveln_pntr b)
{int index; struct leveln_pntr parent,save_next,save_prev; int ix=0,bufix,i; boolean done=false;
 struct leveln_pntr p;

  bufix = get_index(f,b); f->buffer[bufix].modified = false;
  index = f->buffer[bufix].b.ix.index_type;
  parent = f->mru_at_level[f->buffer[bufix].b.ix.level+1][index];
  /*printf("  removing index block %d/%d, level=%d, parent=%d/%d\n",
    b.segment,b.block,f->buffer[bufix].b.ix.level,parent.segment,parent.block); */
  save_next = f->buffer[bufix].b.ix.next;
  save_prev = f->buffer[bufix].b.ix.prev;
  if ( null_pntr(save_prev) )
    f->first_at_level[f->buffer[bufix].b.ix.level][index] = save_next;
  else {
    bufix = get_index_update(f,save_prev);
    f->buffer[bufix].b.ix.next = save_next;
  }
  if ( !null_pntr(save_next) ) {
    bufix = get_index_update(f,save_next);
    f->buffer[bufix].b.ix.prev = save_prev;
  }
  deallocate_block(f,b);
  /*now remove reference from parent*/
  bufix = get_index_update(f,parent);
  do {
    unpackn_ptr(&(f->buffer[bufix].b.ix),ix,&p);
    if ( eq_pntr(p,b) ) done = true;
    else if ( ix>=f->buffer[bufix].b.ix.keys_in_block ) {
      done = true; f->error_code = bad_dlt_err;
      if ( show_errors ) printf("**Delete failed - couldn't find pointer in parent\n");
    }
    else ix++;
  }
  while ( !done );
  simple_delete(&(f->buffer[bufix].b.ix),ix);
  if (f->buffer[bufix].b.ix.keys_in_block<=0 ) {
    /* printf("  recursive index removal\n"); */
    remove_index(f,parent);
  }
}

/* index_delete removes key k from level0 block b.  The current version
     does not reclaim empty level0 blocks in order to avoid problems
     of propagating max_key values up the tree */

static void index_delete(struct fcb *f, struct key k, struct leveln_pntr b, struct level0_pntr *p)
{int bufix,ix; boolean found;

  *p = null0_ptr;
  bufix = get_index(f,b);
  if ( f->trace ) {
    printf("  deleting %s ",index_caption[f->buffer[bufix].b.ix.index_type]);
    print_user_key(&k,"block key "); printf(" from block %d/%d\n",b.segment,b.block);
  }
  ix = search_block(f,&(f->buffer[bufix].b.ix),&k,&found);
  if ( !found ) f->error_code = dltnokey_err;
  else {
    unpack0_ptr(&(f->buffer[bufix].b.ix),ix,p);
    simple_delete(&(f->buffer[bufix].b.ix),ix);
    mark_modified(f,bufix);
    /*    if ( f->buffer[bufix].b.ix.keys_in_block==0 ) {
      printf("  delete resulted in empty level0 block, index=%d, p=%d/%d\n",
        f->buffer[bufix].b.ix.index_type,f->buffer[bufix].contents.segment,f->buffer[bufix].contents.block);
	}*/
    /* if (f->buffer[bufix].b.ix.keys_in_block>0 ) mark_modified(f,bufix);
       else remove_index(f,b);*/
  }
}

/*** index insertion ***/

static void simple_insert(struct ix_block *b, int ix, struct key *k, levelx_pntr p)
{int i,plc;

  if ( b->level==0 ) plc = pack0_ptr(b,p.p0);
  else plc = packn_ptr(b,p.pn);
  b->keys_in_block++;
  b->chars_in_use = b->chars_in_use + k->lc - b->prefix_lc + plc;
  for (i=b->keys_in_block-1; i>ix; i--) b->keys[i] = b->keys[i-1];
  b->keys[ix].lc = k->lc - b->prefix_lc;
  b->keys[ix].sc = keyspace_lc - b->chars_in_use;
  mvc(k->text,b->prefix_lc,b->keys,b->keys[ix].sc,b->keys[ix].lc);
}

static boolean will_fit(struct key *k, struct ix_block *b)
{int needed,have;

  needed = k->lc - b->prefix_lc + pntr_lc(b->level) + key_ptr_lc;
  have = keyspace_lc - (b->chars_in_use
    + (b->keys_in_block * key_ptr_lc));
  return( needed<=have );
}

static boolean replace_max_key_pntr(struct fcb *f, struct leveln_pntr parent, struct leveln_pntr old_ptr,
  struct leveln_pntr new_ptr)
{int ix=0,bufix; boolean found=false,split=false; struct level0_pntr p0; struct leveln_pntr p; struct key k;

  bufix = get_index_update(f,parent);
  lock_buffer(f,bufix);
  while ( ix<f->buffer[bufix].b.ix.keys_in_block ) {
    unpackn_ptr(&(f->buffer[bufix].b.ix),ix,&p);
    if ( eq_pntr(old_ptr,p) ) {
      found = true;
      get_nth_key(&(f->buffer[bufix].b.ix),&k,ix);

      simple_delete(&(f->buffer[bufix].b.ix),ix);
      if ( will_fit(&k,&(f->buffer[bufix].b.ix)) ) {
        if ( f->trace ) printf("simple insert\n");
	//        simple_insert(&(f->buffer[bufix].b.ix),ix,&k,(levelx_pntr)new_ptr);
	simple_insert(&(f->buffer[bufix].b.ix),ix,&k,*((levelx_pntr *)&new_ptr));
      }
      else {
        if ( f->trace ) {
          print_user_key(&k,"replace_max_key forced block split, k=");
          printf(", keys_in_block=%d, chars_in_use=%d\n",
            f->buffer[bufix].b.ix.keys_in_block,f->buffer[bufix].b.ix.chars_in_use);
	}
        split = true;
        copy_ptr(new_ptr,&p0);
        split_block(f,&k,p0,bufix,ix);
      }

      ix = f->buffer[bufix].b.ix.keys_in_block;
    }
    else ix++;
  }
  unlock_buffer(f,bufix);
  if ( !found ) {
    f->error_code = max_key_err;
    if ( show_errors )
      printf("**Couldn't replace max_key pntr for %d/%d in %d/%d\n",
        old_ptr.segment,old_ptr.block,parent.segment,parent.block);
  }
  return(split);
}

static void compress_ix_block(struct fcb *f, int bufix, struct key *prefix, int prefix_lc)
{int i; boolean prefix_ok=true; struct ix_block b; struct key k; levelx_pntr p;

  if ( f->buffer[bufix].b.ix.prefix_lc==prefix_lc ) { /* do nothing */ }
  else if ( f->buffer[bufix].b.ix.prefix_lc>prefix_lc ) {
    /*    if( show_errors ) {
      printf("Uh oh. Trying to compress block with longer prefix=");
      for (i=0; i<prefix_lc; i++) printf("%c",prefix->text[i]);
      printf("\n");
      print_index_block(stdout,&(f->buffer[bufix].b.ix));
      }*/
  }
  else { /* make prefix longer */
    /*    printf("Compressing index block, prefix_lc=%d, before\n",prefix_lc);
	  print_index_block(stdout,&(f->buffer[bufix].b.ix));*/
    b.keys_in_block = 0;
    b.chars_in_use = prefix_lc;
    mvc(prefix->text,0,b.keys,keyspace_lc-b.chars_in_use,prefix_lc);
    b.index_type = f->buffer[bufix].b.ix.index_type;
    b.prefix_lc = prefix_lc;
    b.level = f->buffer[bufix].b.ix.level;
    b.next = f->buffer[bufix].b.ix.next;
    b.prev = f->buffer[bufix].b.ix.prev;
    for (i=0; i<f->buffer[bufix].b.ix.keys_in_block; i++) {
      get_nth_key(&(f->buffer[bufix].b.ix),&k,i);
      if ( memcmp(k.text,prefix->text,(size_t)prefix_lc)!=0 ) prefix_ok = false;
      if ( f->buffer[bufix].b.ix.level==0 ) unpack0_ptr(&(f->buffer[bufix].b.ix),i,&(p.p0));
      else  unpackn_ptr(&(f->buffer[bufix].b.ix),i,&(p.pn));
      simple_insert(&b,i,&k,p);
    }
    if ( !prefix_ok ) printf("Uh oh. Bad prefix in compress_ix_block\n");
    /*    printf("  after\n");
	  print_index_block(stdout,&b);*/
    f->buffer[bufix].b.ix = b;
  }
}

/* choose_split_point attempts to figure out how many keys should */
/*   be moved to the new block to get a reasonable balance        */
/*   between the blocks while favoring a choice that gives better */
/*   prefix compression.  It finds the point at which the block   */
/*   evenly split then looks one key either way to see if either  */
/*   results in better overall compression.                       */

static int choose_split_point(struct fcb *f, int bufix, struct key *min_key, struct key *mid_key, struct key *max_key)
{int i,left_lc,right_lc=0;

  left_lc = f->buffer[bufix].b.ix.chars_in_use;

}

/* move_keys_to_new_block moves keys from the block in buffer[ix] */
/*   to the block in buffer[new_ix] then inserts key k and        */
/*   pointer p into the appropriate block.                        */

static void move_keys_to_new_block(struct fcb *f, struct key *k,
  levelx_pntr p,int index, int bufix, int new_ix, boolean seq)
{int freed=0,needed,entry_lc,level,plc; struct key max; struct level0_pntr p0;
 struct leveln_pntr pn; levelx_pntr px;

  level = f->buffer[bufix].b.ix.level;
  entry_lc = k->lc + pntr_lc(level) + key_ptr_lc;
  if ( seq ) needed = entry_lc;
  else if ( entry_lc>(keyspace_lc/3) ) needed = entry_lc;
  else needed = keyspace_lc / 3;
  do {
    get_max_key(&(f->buffer[bufix].b.ix),&max);
    if ( level==0 )
      plc = unpack0_ptr(&(f->buffer[bufix].b.ix),f->buffer[bufix].b.ix.keys_in_block-1,&(px.p0));
    else plc = unpackn_ptr(&(f->buffer[bufix].b.ix),f->buffer[bufix].b.ix.keys_in_block-1,&(px.pn));
    simple_insert(&(f->buffer[new_ix].b.ix),0,&max,px);
    freed = freed + f->buffer[bufix].b.ix.keys[f->buffer[bufix].b.ix.keys_in_block-1].lc +
      plc + key_ptr_lc;
    simple_delete(&(f->buffer[bufix].b.ix),f->buffer[bufix].b.ix.keys_in_block-1);
  } while ( freed<needed );
  if ( f->trace ) {
    printf("  moved %d keys to new block, insert is to ",f->buffer[new_ix].b.ix.keys_in_block);
  }
  if ( index<f->buffer[bufix].b.ix.keys_in_block ) {
    if ( f->trace ) printf("old block\n");
    simple_insert(&(f->buffer[bufix].b.ix),index,k,p);
  }
  else {
    if ( f->trace ) printf("new block\n");
    simple_insert(&(f->buffer[new_ix].b.ix),index-f->buffer[bufix].b.ix.keys_in_block,k,p);
  }
  if ( f->trace ) {
    get_max_key(&(f->buffer[bufix].b.ix),&max);
    pn = f->buffer[bufix].contents;
    printf("  old(%d/%d): %d keys, max is ",pn.segment,pn.block,
      f->buffer[bufix].b.ix.keys_in_block);
    print_user_key1(&max,"");
    get_max_key(&(f->buffer[new_ix].b.ix),&max);
    pn = f->buffer[new_ix].contents;
    printf("  new(%d/%d): %d keys, max is ",pn.segment,pn.block,
      f->buffer[new_ix].b.ix.keys_in_block);
    print_user_key1(&max,"");
  }
}

/* create_new_primary creates a new primary block containing the key max  */
/*   and sets last_pntr */

static void create_new_primary(struct fcb *f, int index, struct leveln_pntr b, struct key *max,
  struct leveln_pntr newb)
{int ix,ix1,ix2,old_level; struct leveln_pntr oldb;

  if ( f->primary_level[index]>=max_level-1 ) fatal_error(f,maxlevel_err);
  else {
    f->primary_level[index]++;
    ix = allocate_index_block(f,index,&(f->first_at_level[f->primary_level[index]][index]),
      f->primary_level[index]);
    //simple_insert(&(f->buffer[ix].b.ix),0,max,(levelx_pntr)b);
    simple_insert(&(f->buffer[ix].b.ix),0,max,*((levelx_pntr *)&b));
    f->last_pntr[f->primary_level[index]][index] = newb;

    /*   lock_buffer(f,ix);
    ix2 = get_index(f,b);
    lock_buffer(f,ix2);
    old_level = f->primary_level[index] - 1;
    ix1 = allocate_index_block(f,index,&oldb,old_level);
    f->buffer[ix1].b = f->buffer[ix2].b;
    mark_modified(f,ix1);
    f->buffer[ix2].modified = false;
    f->first_at_level[old_level][index] = oldb;
    unlock_buffer(f,ix2);
    simple_insert(&(f->buffer[ix].b.ix),0,max,(levelx_pntr)oldb);
    unlock_buffer(f,ix);
    deallocate_block(f,b);*/

    if ( f->trace )
      printf("  creating new %s primary, levels are %s=%d,%s=%d,%s=%d\n",index_caption[index],
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
/*   creates a new primary, as appropriate.  Note that the max_key  */
/*   value in the parent may not match the child (child could have  */
/*   been emptied).  But it is still a valid max_key.  The parent   */
/*   entry is changed to point to the newly allocated block.        */
/*   If we are inserting at eof (next=null, ix=keys_in_block)    */
/*   then sequential inserts are assumed, otherwise random.         */

static void split_block(struct fcb *f, struct key *k, levelx_pntr p, int bufix, int ix)
{int new_ix,parent_level,i,index_type,prefix_lc; boolean split,seq; struct leveln_pntr parent,oldb,newb,save_next;
 struct key max,new_max,min_key,mid_key,max_key;

  lock_buffer(f,bufix);
  index_type = f->buffer[bufix].b.ix.index_type;
  oldb = f->buffer[bufix].contents;
  seq = null_pntr(f->buffer[bufix].b.ix.next) && ix==f->buffer[bufix].b.ix.keys_in_block;
  /*  printf("in split_block, null(next)=%d, ix=%d, keys_in_block=%d, seq=%d\n",
      null_pntr(f->buffer[bufix].b.ix.next),ix,f->buffer[bufix].b.ix.keys_in_block,seq);*/
  parent_level = f->buffer[bufix].b.ix.level + 1;
  /*  parent = f->mru_at_level[parent_level][index_type];*/
  parent = parent_block_and_prefix(f,bufix,k,&max_key,&min_key);
  new_ix = allocate_index_block(f,index_type,&newb,f->buffer[bufix].b.ix.level);
  lock_buffer(f,new_ix);
  if ( f->trace ) printf("  splitting block %d/%d, new block is %d/%d\n",
    oldb.segment,oldb.block,newb.segment,newb.block);
  f->buffer[new_ix].b.ix.next = f->buffer[bufix].b.ix.next;
  f->buffer[new_ix].b.ix.prev = oldb;
  save_next = f->buffer[new_ix].b.ix.next;
  f->buffer[bufix].b.ix.next = newb;
  move_keys_to_new_block(f,k,p,ix,bufix,new_ix,seq);
  get_max_key(&(f->buffer[bufix].b.ix),&mid_key);
  if ( seq ) {
    /*    move_keys_to_new_block1();*/
    prefix_lc = find_prefix_lc(&min_key,&max_key);
    compress_ix_block(f,bufix,&max_key,prefix_lc);
  }
  else {
    /*    n = choose_split_point(f,bufix,&min_key,&mid_key,&max_key);*/
    prefix_lc = find_prefix_lc(&mid_key,&max_key);
    compress_ix_block(f,new_ix,&max_key,prefix_lc);
    /*    move_keys_to_new_block1();*/
    prefix_lc = find_prefix_lc(&min_key,&mid_key);
    compress_ix_block(f,bufix,&mid_key,prefix_lc);
  }
  get_max_key(&(f->buffer[bufix].b.ix),&max);
  unlock_buffer(f,bufix);
  unlock_buffer(f,new_ix);
  if ( !null_pntr(save_next) ) {
    i = get_index_update(f,save_next); f->buffer[i].b.ix.prev = newb;
  }
  /*now propogate upward*/
  if ( parent_level>f->primary_level[index_type] )
    create_new_primary(f,index_type,oldb,&max,newb);
  else { /*update parent*/
    if ( null_pntr(save_next) ) { /* last block at this level */
      f->last_pntr[parent_level][index_type] = newb;
      update_index(f,&max,parent,*((levelx_pntr*) &oldb));
      //update_index(f,&max,parent,(levelx_pntr) oldb);
    }
    else {
      split = replace_max_key_pntr(f,parent,oldb,newb);
      if ( split ) {
        /* replace_max_key_pntr split the parent block so we */
        /* have to search for the right one */
        parent = search_index(f,index_type,parent_level+1,&max);
        if ( f->trace ) printf("  parent after replace_max split is %d/%d\n",parent.segment,parent.block);
      }
      update_index(f,&max,parent,*((levelx_pntr *)&oldb));
      //update_index(f,&max,parent,(levelx_pntr)oldb);
    }
  }
}

/* update_index inserts key k and pointer p into index block b. */

void update_index(struct fcb *f, struct key *k, struct leveln_pntr b, levelx_pntr p)
{int bufix,ix; boolean found; struct level0_pntr p0;

  bufix = get_index_update(f,b);
  ix = search_block(f,&(f->buffer[bufix].b.ix),k,&found);
  if ( f->trace ) {
    printf("  updating %s ",index_caption[f->buffer[bufix].b.ix.index_type]);
    printf("block %ld/%ld with key ",b.segment,b.block);
    print_user_key(k,""); printf(", ");
  }
  if ( found  ) {
    /*    if ( f->trace ) printf(" pointer replacement\n");
	  packn_ptr(&(f->buffer[bufix].b.ix),ix,p);*/
    simple_delete(&(f->buffer[bufix].b.ix),ix);
  }
  if ( will_fit(k,&(f->buffer[bufix].b.ix)) ) {
    if ( f->trace ) printf("simple insert\n");
    simple_insert(&(f->buffer[bufix].b.ix),ix,k,p);
  }
  else {
    if ( f->trace ) {
      print_user_key(k,"block split, k=");
      printf(", keys_in_block=%d, chars_in_use=%d\n",
        f->buffer[bufix].b.ix.keys_in_block,f->buffer[bufix].b.ix.chars_in_use);
    }
    /*    split_block(f,k,p0,bufix,ix);*/
    split_block(f,k,p,bufix,ix);
  }
}

/* record moving -- if the record is so short that it fits in a long */
/*   then it is kept in the sc field of the pointer, otherwise the   */
/*   record is written to disk     */

static void move_from_rec(struct fcb *f, char r[], struct level0_pntr *p)
{size_t size; FILE *file;

  if ( p->lc<=long_lc ) memcpy(&p->sc,r,(size_t)p->lc);
  else {
    file = file_index(f,p->segment);
    if ( f->error_code!=no_err ) return;
    if ( fseek(file,p->sc,0)!=0 ) {
      f->error_code = seek_err; return;
    }
    size = fwrite(r,(size_t) 1,(size_t) p->lc,file);
    if ( size!=p->lc ) {
      f->error_code = move_rec_err;
      if ( show_errors ) printf("**move_from_rec failed\n");
    }
  }
}

/* intermediate calls */

static void extract_next(struct fcb *f, int index, int bufix, char t[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{int lc; struct key k;

  if ( f->position_ix[index]>=f->buffer[bufix].b.ix.keys_in_block ) {
    t[0] = '\0'; *key_lc = 0; *p = null0_ptr;
    if ( null_pntr(f->buffer[bufix].b.ix.next) ) f->error_code = ateof_err;
    else if ( show_errors ) {
      printf("**Uh Oh.  Error in extract_next, block=%d/%d, index=%d, position=%d, keys=%d, next=%d/%d\n",
        f->buffer[bufix].contents.segment,f->buffer[bufix].contents.block,index,f->position_ix[index],
        f->buffer[bufix].b.ix.keys_in_block,f->buffer[bufix].b.ix.next.segment,f->buffer[bufix].b.ix.next.block);
    }
  }
  else {
    get_nth_key(&(f->buffer[bufix].b.ix),&k,f->position_ix[index]);
    if ( k.lc<=max_key_lc ) *key_lc = k.lc;
    else {
      f->error_code = longkey_err; *key_lc = max_key_lc;
    }
    unpack0_ptr(&(f->buffer[bufix].b.ix),f->position_ix[index],p);
    mvc(k.text,0,t,0,*key_lc);
    f->position_ix[index]++;
    if ( f->position_ix[index]>=f->buffer[bufix].b.ix.keys_in_block && !null_pntr(f->buffer[bufix].b.ix.next) )
      set_position(f,index,f->buffer[bufix].b.ix.next,0);
  }
}

int kf_next_ptr(struct fcb *f, int index, unsigned char key[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{int ix,lc; struct leveln_pntr next;

  if ( check_fcb(f) ) {
    ix = get_index(f,f->position[index]);
    next = f->buffer[ix].b.ix.next;
    while ( f->position_ix[index]>=f->buffer[ix].b.ix.keys_in_block && !null_pntr(next) ) {
      /*      printf("  skipping block %d/%d\n",f->buffer[ix].contents.segment,f->buffer[ix].contents.block);*/
      ix = get_index(f,next);
      set_position(f,index,next,0);
      next = f->buffer[ix].b.ix.next;
    }
    extract_next(f,index,ix,key,key_lc,max_key_lc,p);
  }
  return(f->error_code);
}

int kf_next_rec(struct fcb *f, int index, unsigned char t[], int *key_lc, int max_key_lc,
   char r[],int *rlc,int max_lc)
{struct level0_pntr p;

  kf_next_ptr(f,index,t,key_lc,max_key_lc,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

static int kf_prev_ptr(struct fcb *f, int index, unsigned char t[], int *key_lc, int max_key_lc, struct level0_pntr *p)
{int bufix,lc; boolean done=false; struct leveln_pntr prev; struct key k;

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
      mvc(k.text,0,t,0,*key_lc);
    }
  }
  return(f->error_code);
}

static int kf_prev_rec(struct fcb *f, int index, char t[], int *key_lc, int max_key_lc,
   char r[],int *rlc,int max_lc)
{struct level0_pntr p;

  kf_prev_ptr(f,index,t,key_lc,max_key_lc,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

static int kf_get_ptr(struct fcb *f, int index, unsigned char t[], int key_lc, struct level0_pntr *p)
{struct leveln_pntr b; int ix,lc,bufix; struct key k; boolean found;
char t1[maxkey_lc]; struct level0_pntr p1;

  set_up(f,t,key_lc,&k);
  if ( f->error_code==no_err ) {
    b = search_index(f,index,level_one,&k);
    bufix = get_index(f,b);
    ix = search_block(f,&(f->buffer[bufix].b.ix),&k,&found);
    set_position(f,index,b,ix);
    if ( found ) extract_next(f,index,bufix,t1,&lc,maxkey_lc,p);
    else {
      f->error_code = getnokey_err; *p = null0_ptr;
    }
  }
  return(f->error_code);
}

static int kf_get_rec(struct fcb *f,int index, unsigned char t[],int key_lc, unsigned char r[],int *rlc,int max_lc)
{struct level0_pntr p;

  kf_get_ptr(f,index,t,key_lc,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

static int kf_put_ptr(struct fcb *f, int index, char t[], int key_lc, struct level0_pntr p)
{struct leveln_pntr b; struct key k;

  set_up(f,t,key_lc,&k);
  if ( f->error_code==no_err ) {
    b = search_index(f,index,level_one,&k);
    update_index(f,&k,b,*((levelx_pntr *) &p));
    //update_index(f,&k,b,(levelx_pntr) p);
    kf_set_bof(f,index);
    }
  return(f->error_code);
}

static int kf_delete_ptr(struct fcb *f, int index, char t[], int key_lc)
{struct leveln_pntr b; struct key k; struct level0_pntr p;

  set_up(f,t,key_lc,&k);
  if (f->error_code==no_err ) {
    b = search_index(f,index,level_one,&k);
    index_delete(f,k,b,&p);
    kf_set_bof(f,index);
  }
  return(f->error_code);
}

static int kf_put_rec(struct fcb *f,int index, unsigned char t[], int key_lc, char r[], int rlc)
{struct level0_pntr p; boolean have_space=false;

  kf_get_ptr(f,index,t,key_lc,&p);
  if (f->error_code==no_err ) {
    if ( rlc<=long_lc ) { /* new rec goes into pointer */
      if ( p.lc>long_lc ) deallocate_rec(f,p);
      have_space = true; p = dummy_ptr; p.lc = rlc;
    }
    else { /* new rec goes on disk */
      if ( p.lc>long_lc && (rec_allocation_lc(rlc)==rec_allocation_lc(p.lc)) ) {
        have_space = true; p.lc = rlc;
      }
      else {
        if ( p.lc>long_lc ) deallocate_rec(f,p);
        have_space =  allocate_rec(f,(long)rlc,&p);
      }
    }
  }
  else if ( f->error_code==getnokey_err ) {
    f->error_code = no_err;
    have_space = allocate_rec(f,(long)rlc,&p);
  }
  if ( have_space ) {
    move_from_rec(f,r,&p); 
    kf_put_ptr(f,index,t,key_lc,p);
  }
  return(f->error_code);
}

static int kf_delete_rec(struct fcb *f, int index, unsigned char t[], int key_lc)
{struct level0_pntr p; struct key k; struct leveln_pntr b;

  set_up(f,t,key_lc,&k);
  if (f->error_code==no_err ) {
    b = search_index(f,index,level_one,&k);
    index_delete(f,k,b,&p);
    if (f->error_code==no_err ) deallocate_rec(f,p);
    kf_set_bof(f,index);
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

static int allocate_block(struct fcb *f)
{int bufix; struct leveln_pntr p;

  extend_file(f,(long)block_lc,&p);
  bufix = vacate_oldest_buffer(f);
  f->buffer[bufix].contents = p;
  f->buffer[bufix].modified = true;
  hash_chain_insert(f,bufix);
  if ( f->trace_freespace )
    printf("  just allocated block in buf=%d, seg=%d, block=%ld, seg_cnt=%d\n",
      bufix,p.segment,p.block,f->segment_cnt);
  return(bufix);
}

static int unpack_int(unsigned char key[], unsigned int *n)
{int i;

  *n = 0;
  for (i=0; i<sizeof(int); i++) {
    *n = (*n << 8) + key[i];
  }
  return(sizeof(int));
}

static int unpack_short(unsigned char key[], unsigned int *n)
{int i;

  *n = 0;
  for (i=0; i<sizeof(short); i++) {
    *n = (*n << 8) + key[i];
  }
  return(sizeof(short));
}

static int pack_int(unsigned char key[], unsigned int n)
{int i;  unsigned int n1;

  for (i=sizeof(int)-1; i>=0 ; i--) {
    key[i] = n & 255;
    n = n >> 8;
  }
  return(sizeof(int));
}

static int pack_short(unsigned char key[], unsigned int n)
{int i;

  for (i=sizeof(short)-1; i>=0 ; i--) {
    key[i] = n & 255;
    n = n >> 8;
  }
  return(sizeof(short));
}

int unpack_lc_key(unsigned char key[], struct level0_pntr *p)
{int lc,n; struct level0_pntr p0;

  lc = unpack_int(key,&n);
  p->lc = n;
  lc = lc + unpack_short(key+lc,&n);
  p->segment = n;
  lc = lc + unpack_int(key+lc,&n);
  p->sc = n;
  /*  printf("unpacked lc key %d/%d/%d, lc=%d\n",p->lc,p->segment,p->sc,lc);*/
  return(lc);
}

static int pack_lc_key(unsigned char key[], struct level0_pntr p)
{int lc; struct level0_pntr p0;

  lc = pack_int(key,p.lc);
  lc = lc + pack_short(key+lc,p.segment);
  lc = lc + pack_int(key+lc,p.sc);
  /*  unpack_lc_key(key,&p0);
      printf("packed lc key %d/%d/%d, lc=%d, unpacked is %d/%d/%d\n",p.lc,p.segment,p.sc,lc,p0.lc,p0.segment,p0.sc);*/
  return(lc);
}

int unpack_rec_key(unsigned char key[], struct level0_pntr *p)
{int lc,n; struct level0_pntr p0;

  lc = unpack_short(key,&n);
  p->segment = n;
  lc = lc + unpack_int(key+lc,&n);
  p->sc = n;
  /* printf("unpacked rec key %d/%d, lc=%d\n",p->segment,p->sc,lc);*/
  return(lc);
}

static int pack_rec_key(unsigned char key[], struct level0_pntr p)
{int lc; struct level0_pntr p0;

  lc = pack_short(key,p.segment);
  lc = lc + pack_int(key+lc,p.sc);
  /*  unpack_rec_key(key,&p0);
      printf("packed rec key %d/%d, lc=%d, unpacked=%d/%d\n",p.segment,p.sc,lc,p0.segment,p0.sc);*/
  return(lc);
}

static void insert_freespace_entry(struct fcb *f, struct level0_pntr *p0)
{int err,lc,key_lc; unsigned char key[maxkey_lc]; struct level0_pntr p;

  if ( f->trace_freespace ) printf("inserting freespace entry %d/%d/%d\n",p0->segment,p0->sc,p0->lc);
  p = *p0;
  p.lc = rec_allocation_lc(p0->lc);
  key_lc = pack_lc_key(key,p);
  err = kf_put_ptr(f,free_lc_ix,key,key_lc,dummy_ptr);
  if ( err!=no_err ) {
    f->error_code = free_insrt_err;
    if ( show_errors) printf("**Couldn't insert free_lc entry, err=%d\n",err);
  }
  else {
    key_lc = pack_rec_key(key,p);
    err = kf_put_rec(f,free_rec_ix,key,key_lc,(char *) &(p.lc),sizeof(int));
    if ( err!=no_err ) {
      f->error_code = free_insrt_err;
      if ( show_errors ) printf("**Couldn't insert free_rec entry, err=%d\n",err);
    }
  }
}

static void delete_freespace_entry(struct fcb *f, struct level0_pntr *p0)
{int err,lc,key_lc; unsigned char key[maxkey_lc]; struct level0_pntr p;

  if ( f->trace_freespace ) printf("deleting freespace entry %d/%d/%d\n",p0->segment,p0->sc,p0->lc);
  p = *p0;
  p.lc = rec_allocation_lc(p0->lc);
  key_lc = pack_lc_key(key,p);
  err = kf_delete_ptr(f,free_lc_ix,key,key_lc);
  if ( err!=no_err ) {
    f->error_code = free_dlt_err;
    if ( show_errors ) printf("**Couldn't delete free_lc entry %d/%d/%d, err=%d\n",p.lc,p.segment,p.sc,err);
  }
  else {
    key_lc = pack_rec_key(key,p);
    err = kf_delete_rec(f,free_rec_ix,key,key_lc);
    if ( err!=no_err ) {
      f->error_code = free_dlt_err;
      if ( show_errors )
        printf("**Couldn't delete free_rec entry %d/%d, err=%d, free_rec_ix=%d\n",p.segment,p.sc,err,free_rec_ix);
    }
  }
}

/* not implemented */
/* recover_freespace copies records to a newly allocated block in an */
/*   attempt to recover freespace.  We find the first freespace      */
/*   entry of length<max_recovery_lc, then find the last user record */
/*   smaller than this      */
/*   freespace entry.  This ensures that we get past a previously    */
/*   compacted and undisturbed segment of the file.  We then copy    */
/*   records smaller than max_recovery_lc to the new block until it  */
/*   is full.  Note that the user position in the index should not   */
/*   be altered */

#define max_recovery_lc 2048
#define recovery_buf_lc 16*block_lc

static void recover_freespace(struct fcb *f)
{int i,err=no_err,key_lc,save_ix,lc=0,rec_lc,segment,sc; boolean done=false,have_space;
 unsigned char rec[recovery_buf_lc],key[maxkey_lc];
 struct leveln_pntr pn,save_position; struct level0_pntr p,px,first;

  save_position = f->position[user_ix]; save_ix = f->position_ix[user_ix];
  kf_set_bof(f,free_rec_ix);
  while ( err==no_err & lc<=max_recovery_lc ) {
    err = kf_next_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,(char *)&lc,&rec_lc,sizeof(int));
    unpack_rec_key(key,&first);
  }
  kf_set_bof(f,user_ix);
  err = kf_next_ptr(f,user_ix,key,&key_lc,maxkey_lc,&p);
  while ( err==no_err && p.segment<first.segment && p.sc<first.sc ) {
    err = kf_next_ptr(f,user_ix,key,&key_lc,maxkey_lc,&p);
  }
  err = kf_prev_ptr(f,user_ix,key,&key_lc,maxkey_lc,&p);
  have_space = extend_file(f,(long)recovery_buf_lc,&pn);
  px.segment = pn.segment; px.sc = pn.block<<block_shift; px.lc = recovery_buf_lc;
  lc = 0;
  do {
    err = kf_next_ptr(f,user_ix,key,&key_lc,maxkey_lc,&p);
    if ( err!=no_err ) done = true;
    else if ( p.lc>max_recovery_lc ) /* leave it where it is */;
    else if ( lc+p.lc>recovery_buf_lc ) done = true;
    else {
      copy_rec(f,&p,rec+lc,&rec_lc,recovery_buf_lc-lc);
      delete_freespace_entry(f,&p);
      lc = lc + p.lc;
    }
  }
  while ( !done );
  move_from_rec(f,rec,&px);       
  f->position[user_ix] = save_position; f->position_ix[user_ix] = save_ix;
  px.sc = px.sc + lc;
  px.lc = px.lc - lc;
  if ( px.lc>0 ) insert_freespace_entry(f,&px);
}

static boolean allocate_rec(struct fcb *f, long lc, struct level0_pntr *p)
{int i,err,key_lc,rec_allocate_lc,block_allocate_lc; boolean have_space=false;  struct level0_pntr p0,p1;
 unsigned char key[maxkey_lc];
 struct leveln_pntr pn;

  if ( f->trace_freespace ) printf("allocating rec lc=%d\n",lc);
  p->segment = 0; p->sc = 0; p->lc = lc;
  if ( lc<=long_lc ) have_space = true;
  else {
    rec_allocate_lc = rec_allocation_lc(lc);
    p0.segment = 0; p0.sc = 0; p0.lc = rec_allocate_lc;
    key_lc = pack_lc_key(key,p0);
    err = kf_get_ptr(f,free_lc_ix,key,key_lc,&p1);
    err = kf_next_ptr(f,free_lc_ix,key,&key_lc,maxkey_lc,&p1);
    if ( err!=ateof_err ) {
      if ( err!=no_err ) {
        f->error_code = alloc_rec_err;
        if ( show_errors ) printf("**Couldn't get free_lc entry, err=%d\n",err);
      }
      if ( key_lc!=10 && show_errors ) printf("**Uh Oh. free_lc_key is wrong size\n");
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
      block_allocate_lc = allocation_lc(lc,16*block_lc);
      have_space = extend_file(f,(long)block_allocate_lc,&pn);
      p->segment = pn.segment;
      p->sc = pn.block << block_shift;
      p0.segment = pn.segment;
      p0.sc = p->sc + rec_allocate_lc;
      p0.lc = block_allocate_lc - rec_allocate_lc;
      if ( f->trace_freespace ) printf("new space, block_lc=%d, residual=%d\n",
        block_allocate_lc,block_allocate_lc-rec_allocate_lc);
      if ( p0.lc>0 ) insert_freespace_entry(f,&p0);
    }
  }
  return(have_space);
}

/* deallocate_rec deallocates any space allocated with pointer p. */
/*   If p.lc<=long_lc then p doesn't point to a disk record, the  */
/*   data has been stored in p.sc.                                */


static void deallocate_rec(struct fcb *f, struct level0_pntr p)
{int err,key_lc,lc,rec_lc; struct level0_pntr p0,p1; char key[maxkey_lc];

  if ( f->trace_freespace ) printf("deallocating rec %d/%d/%d\n",p.segment,p.sc,p.lc);
  if ( p.lc > long_lc ) { /* a real disk record */
    p0 = p;
    p0.lc = rec_allocation_lc(p.lc);
    key_lc = pack_rec_key(key,p0);
    err = kf_get_rec(f,free_rec_ix,key,key_lc,(char *) &lc,&rec_lc,sizeof(int));
    if ( err==no_err ) {
      f->error_code = dealloc_rec_err;
      if ( show_errors ) printf("**Uh Oh. Trying to deallocate entry allready in free list %d/%d\n",p0.segment,p0.sc);
    }
    err = kf_prev_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,(char *) &lc,&rec_lc,sizeof(int));
    if ( err==atbof_err ) /* nothing to merge */ { if ( f->trace_freespace ) printf("prev is bof\n"); }
    else if ( err!=no_err ) {
      f->error_code = dealloc_rec_err;
      if ( show_errors ) printf("**couldn't get prev rec in deallocate_rec, err=%d\n",err);
    }
    else {
      key_lc = unpack_rec_key(key,&p1);
      p1.lc = lc;
      if ( f->trace_freespace ) printf("prev rec is %d/%d/%d",p1.segment,p1.sc,p1.lc);
      if ( contiguous(p1,p0) ) {
        delete_freespace_entry(f,&p1);
        p0.sc = p1.sc;
        p0.lc = p0.lc + p1.lc;
        if ( f->trace_freespace ) printf(" contiguous, merged entry is %d/%d/%d",p0.segment,p0.sc,p0.lc);
      }
      if ( f->trace_freespace ) printf("\n");
      err = kf_next_ptr(f,free_rec_ix,key,&key_lc,maxkey_lc,&p1 /*dummy*/);
    }
    err = kf_next_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,(char *) &lc,&rec_lc,sizeof(int));
    if ( err==ateof_err ) /* nothing to merge */ { if ( f->trace_freespace ) printf("next is eof\n"); }
    else if ( err!=no_err ) {
      f->error_code = dealloc_rec_err;
      if ( show_errors ) printf("**couldn't get next rec in deallocate_rec, err=%d\n",err);
    }
    else {
      key_lc = unpack_rec_key(key,&p1);
      p1.lc = lc;
      if ( f->trace_freespace ) printf("next rec is %d/%d/%d",p1.segment,p1.sc,p1.lc);
      if ( contiguous(p0,p1) ) {
        delete_freespace_entry(f,&p1);
        p0.lc = p0.lc + p1.lc;
        if ( f->trace_freespace ) printf(" contiguous, merged entry is %d/%d/%d",p0.segment,p0.sc,p0.lc);
      }
      if ( f->trace_freespace ) printf("\n");
    }
    insert_freespace_entry(f,&p0);
  }
}

static void deallocate_block(struct fcb *f, struct leveln_pntr p)
{struct level0_pntr p0;

  if ( f->trace_freespace ) printf("deallocating block %d/%d\n",p.segment,p.block);
  p0.segment = p.segment;
  p0.sc = p.block << block_shift;
  p0.lc = block_lc;
  insert_freespace_entry(f,&p0);
}

static void init_free_space(struct fcb *f)
{int bufix; struct leveln_pntr b;
  bufix = allocate_index_block(f,free_rec_ix,&b,level_zero);
  f->first_at_level[level_zero][free_rec_ix] = f->buffer[bufix].contents;
  kf_set_bof(f,free_rec_ix);
  bufix = allocate_index_block(f,free_lc_ix,&b,level_zero);
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

int open_key(struct fcb *f, char id[], int lc)
{char name[max_filename_lc];

  read_fib(f,id);
  if ( f->error_code!=no_err ) fatal_error(f,badopen_err);
  /*  build_segment_name(f,0,name);
      if ( strcmp(id,name)!=0 ) fatal_error(f,bad_name_err); */
  init_key(f,id,lc);
  kf_set_bof(f,user_ix);
  return(f->error_code);
}

int close_key(struct fcb *f)
{int i; long lc; FILE *temp;

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
      if (f->buffer[i].modified)
        write_page(f,f->buffer[i].contents,&(f->buffer[i].b) );
      if (f->buffer[i].locked) {
        f->error_code = bad_close_err;
        if ( show_errors ) printf("**Buffer %d locked at close\n",i);
      }
    }
    write_fib(f);
    for (i=0; i<f->open_file_cnt; i++) {
      lc = f->segment_length[f->open_segment[i]];
      if (f->trace) printf("  closing segment %d, length=%ld\n",
        f->open_segment[i],lc);
      temp = f->open_file[i];
      fseek(temp,lc,0); fputc(' ',temp);
      fclose(temp);
    }
    f->marker = 0;
  }
  return(f->error_code);
}

int create_key(struct fcb *f, char id[], int lc)
{int i,j,ix,bufix; struct leveln_pntr b; FILE *file;

  f->error_code = no_err; f->version = current_version;
  f->segment_cnt = 0;
  f->marker = keyf; f->file_ok = true;
  for (i=0; i<max_index; i++) {
    f->primary_level[i] = level_zero;
    for (j=0; j<max_level; j++) {
      f->first_at_level[j][i] = nulln_ptr;
      f->last_pntr[j][i] = nulln_ptr;
    }
  }
  for (i=0; i<max_segments; i++) f->segment_length[i] = 0;

  if ( f->error_code==no_err ) {
    init_key(f,id,lc);
    write_fib(f);
    f->segment_cnt = 1; f->segment_length[0] = allocation_lc(fib_lc,block_lc);
    bufix = allocate_index_block(f,user_ix,&b,level_zero);
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
{int ix,lc; struct leveln_pntr next;

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
  return( kf_prev_rec(f,user_ix,t,key_lc,max_key_lc,r,rlc,max_lc) );
}

int put_ptr(struct fcb *f, char t[], int key_lc, struct level0_pntr p)
{
  return( kf_put_ptr(f,user_ix,t,key_lc,p) );
}

int delete_ptr(struct fcb *f, char t[], int key_lc)
{
  return( kf_delete_ptr(f,user_ix,t,key_lc) );
}

int put_rec(struct fcb *f,char t[], int key_lc, char r[], int rlc)
{
  return( kf_put_rec(f,user_ix,t,key_lc,r,rlc) );
}

int delete_rec(struct fcb *f, unsigned char key[], int key_lc)
{
  return( kf_delete_rec(f,user_ix,key,key_lc) );
}
