/*                                                               */
/* Copyright 1984,1985,1986,1988,1989,1990,2003 by Howard Turtle */
/*                                                               */

#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include "keyerr.h"
#include "ht.h"
#include "keydef.h"

#define size_t unsigned
#define maxint 32767

/*extern boolean
  trace;*/
static boolean
  trace=false;

/*int
  get_buffer_cnt=0,
  replace_buffer_cnt=0,
  get_index_cnt=0,
  hash_chain_cnt=0;*/

static struct level0_pntr
  null0_ptr;
static struct leveln_pntr
  nulln_ptr;

void update_interior();
static void split_block();
static FILE *file_index();

/* miscellaneous procedures */

static void fatal_error(f,err)
struct fcb *f; int err;
{
   f->error_code = err; f->file_ok = false;
}

/* init_file_name separates the file name and any extension */
/*   and saves the two parts in the fcb                     */

static void init_file_name(struct fcb *f, char id[])
{int i=0;

  if ( (strlen(id)+10)>max_filename_lc) fatal_error(f,bad_name_err);
  else strcpy(f->file_name,id);
  while ( id[i]!='.' && id[i]!='\0' ) i++;
  if ( id[i]=='.' ) {
    f->file_name[i] = '\0';
    strcpy(f->file_extension,id+i);
  }
  else f->file_extension[0] = '\0';
}

static int min(int i, int j)
{
  if ( i<j ) return(i);
  else return(j);
}

void mvc(t1,sc1,t2,sc2,lc)
char t1[],t2[]; int sc1,sc2,lc;
{int i,p1,p2;

/*  printf("in mvc, t2=%ld, t2+sc2=%ld\n",t2,t2+sc2); */
  if ( t1+sc1 > t2+sc2 )
    for (i=0; i<lc; i++) t2[sc2++] = t1[sc1++];
  else {
    p1 = sc1 + lc; p2 = sc2 + lc;
    for (i=0; i<lc; i++) t2[--p2] = t1[--p1];
  }
}

enum comparison compare_key(struct key *k, struct block *b, int ix)
{int r,lc1,lc2;

  lc1 = k->lc;
  lc2 = b->keys[ix].lc;
  r = memcmp(k->text,(char *) b->keys+b->keys[ix].sc,min(lc1,lc2) );
  if (r<0) return(less);
  else if (r>0) return(greater);
  else {
    if ( lc1==lc2 ) return(equal);
    else if ( lc1<lc2 ) return(less);
    else return(greater);
  }
}

static enum comparison cmpstr(t1,lc1,t2,sc2,lc2)
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
}

static boolean read_fib(struct fcb *f,char id[])
{size_t size; FILE *file;

  file = fopen(id,"rb+");
  if ( file==NULL ) f->error_code = badopen_err;
  else if ( fseek(file,(long) 0,0)!=0 ) f->error_code = badopen_err;
  else {
    size = fread(f,(size_t) 1, (size_t) fib_lc,file);
    if ( size!=fib_lc ) f->error_code = notkeyfil_err;
    fclose(file);
  }
  return(f->error_code==no_err);
}

static void write_fib(struct fcb *f)
{size_t size=0; FILE *file;

  file = file_index(f,0);
  if ( f->error_code==no_err ) {
    fseek(file,(long) 0,0);
    size = fwrite(f,(size_t) 1, (size_t) fib_lc,file);
  }
  if ( size!=fib_lc ) printf("**write_fib failed\n");
}

void read_page(struct fcb *f, struct leveln_pntr p, void *buf)
{size_t size=0; FILE *file;

  if ( trace ) printf("reading page %d/%ld\n",p.segment,p.block);
  file = file_index(f,p.segment);
  if ( file==NULL ) f->error_code = read_err;
  else if ( fseek(file,p.block,0)!=0 ) f->error_code = seek_err;
  else size = fread(buf,(size_t) 1,(size_t) block_lc,file);
  if ( size!=block_lc )
   printf("**fread failed, block=%d/%ld, requested lc=%d (%ud)\n",p.segment,p.block,block_lc,size);
}

static void write_page(struct fcb *f, struct leveln_pntr p, void *buf)
{size_t size=0; FILE *file;

  if ( trace ) printf("writing page %d/%ld\n",p.segment,p.block);
  file = file_index(f,p.segment);
  if ( file==NULL ) f->error_code = write_err;
  else if ( fseek(file,p.block,0)!=0 ) f->error_code = seek_err;
  else size = fwrite(buf,(size_t) 1,(size_t) block_lc,file);
  if ( size!=block_lc )
   printf("**fwrite failed, block=%d/%ld, requested lc=%d (%ud)\n",p.segment,p.block,block_lc,size);
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
  if ( trace)  printf("Opening segment %s on file index %d\n",name,ix);
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
  if ( trace ) printf("  segment %d open on file index %d\n",segment,ix);
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
    case  4: /* dltnokey_err */ break;
    case  5: /* getnokey_err */ break;
    case  6: /* notkeyfil_err */    ok = false; break;
    case  7: /* filenotok_err */    ok = false; break;
    case  8: /* badkey_err */ break;
    case  9: /* maxlevel_err */     ok = false; break;
    case 10: /* ateof_err */ break;
    case 11: /* nospace_err */      ok = false; break;
    case 12: /* longrec_err */ break;
    case 13: /* atbof_err */  break;
    case 14: /* version_err */      ok = false; break;
    case 15: /* seek_err */         ok = false; break;
    case 16: /* read_err */         ok = false; break;
    case 17: /* write_err */        ok = false; break;
    case 18: /* segment_open_err */ ok = false; break;
    case 19: /* segment_create_err */ok = false; break;
    case 20: /* bad_name_err */     ok = false; break;
    default: /* illegal_err code */ ok = false; break;
  }
  if ( ok ) f->error_code = no_err;
  return(ok);
}

static boolean set_up(struct fcb *f,char t[],struct key *k)
{
  if ( !check_fcb(f) ) return(false);
  k->lc = strlen(t);
  if ( k->lc>0 && k->lc<max_key_lc ) {
    strcpy(k->text,t); return(true);
  }
  else {
    f->error_code = badkey_err; k->lc = 0;
    return(false);
  }
}

static void set_position(struct fcb *f,struct leveln_pntr b,int ix)
{
  f->position = b; f->position_ix = ix;
}

static void set_bof(struct fcb *f)
{
  f->position = f->first_at_level[0];
  f->position_ix = 0;
}

static void copy_ptr(struct leveln_pntr p1, struct level0_pntr *p2)
{
  p2->segment = p1.segment; p2->sc = p1.block; p2->lc = 0;
}

static boolean gt_pntr(struct leveln_pntr p1, struct leveln_pntr p2)
{
  if ( p1.segment<p2.segment ) return(false);
  else if ( p1.segment>p2.segment ) return(true);
  else return( p1.block>p2.block );
}

/* static boolean eq_pntr(struct leveln_pntr p1, struct leveln_pntr p2)
{
  return( (p1.segment==p2.segment) && (p1.block==p2.block) );
}
*/

#define eq_pntr(p1,p2) ((p1.block==p2.block) && (p1.segment==p2.segment))

/* uncompress_int uncompresses an integer compressed in array p */
/*   and returns the number of bytes consumed to decompress     */
/*   the int.  */


static int uncompress_int(int *i, unsigned char p[])
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
  /*  do {
    t[j] = i & 127;
    i = i>>7;
    if ( i>0 ) j++;
    else done = true;
  } while ( !done );
  lc = j + 1;
  for ( j=0; j<lc-1; j++) { p[j] = t[lc-j-1] | 128; }
  p[lc-1] = t[0];
  return(lc); */
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

  lc = uncompress_int(&(p->segment),cp);
  memcpy(&(p->sc),cp+lc,sizeof(int));
  lc1 = uncompress_int((int *)&(p->lc),cp+lc+sizeof(int));
  return( lc+lc1+sizeof(int) );
}

/* compressn_pntr compresses a leveln pointer p into a byte */
/*   string.  cp points to the last byte in the compressed  */
/*   string and compression will proceed from right to left */
/*   Note that segment is compressed but sc is not  */
/*   since it would usually expand under compression.       */

static int compressn_pntr(struct leveln_pntr *p, unsigned char *cp)
{int lc;

  memcpy((char *)cp-sizeof(int)+1,&(p->block),sizeof(int));
  lc = compress_int(p->segment,cp-sizeof(int));
  return( lc+sizeof(int) );
}

/* uncompressn_pntr uncompresses a leveln pointer.  Note   */
/*   cp points to the first character in the compressed    */
/*   string (unlike compressn_pntr).                       */

static int uncompressn_pntr(struct leveln_pntr *p, unsigned char *cp)
{int lc;

  lc = uncompress_int(&(p->segment),cp);
  memcpy(&(p->block),cp+lc,sizeof(int));
  return( lc+sizeof(int) );
}

static int packn_ptr(struct block *b, struct leveln_pntr p)
{int lc;

  lc = compressn_pntr(&p,(unsigned char *) b->keys+(keyspace_lc-b->chars_in_use-1));
  return(lc);
}

int unpackn_ptr(struct block *b, int ix, struct leveln_pntr *p)
{int lc;

  lc = uncompressn_pntr(p,(char *)b->keys+(b->keys[ix].sc+b->keys[ix].lc));
  return(lc);
}

static int pack0_ptr(struct block *b, struct level0_pntr p)
{int lc;

  lc = compress0_pntr(&p,(unsigned char *) b->keys+(keyspace_lc-b->chars_in_use-1));
  return(lc);
}

int unpack0_ptr(struct block *b, int ix, struct level0_pntr *p)
{int lc;

  lc = uncompress0_pntr(p,(char *)b->keys+(b->keys[ix].sc+b->keys[ix].lc));
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

#define hash_value(b,limit)  (((b.block / block_lc) + b.segment) % limit)


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
  if ( trace ) {
    printf("  inserted buffer %d (%d/%d) into hash chain %d\n",bufix,block.segment,block.block,k);
    print_hash_chain(stdout,f,k);
  }
}

/* remove the buffer with index bufix from its hash chain */

static void hash_chain_remove(struct fcb *f, int bufix)
{int k,next,last; struct leveln_pntr block;

  block = f->buffer[bufix].contents;
  k = hash_value(block,f->buf_hash_entries);
  if ( trace ) {
    printf("Removing buffer %d from hash chain %d\n",bufix,k);
    printf("  old "); print_hash_chain(stdout,f,k);
  }
  next = f->buf_hash_table[k];
  if ( next==bufix ) f->buf_hash_table[k] = f->buffer[bufix].hash_next;
  else {
    while ( (next>=0) && !eq_pntr(block,f->buffer[next].contents) ) {
      last = next; next = f->buffer[next].hash_next;
    }
    if ( next<0 ) printf("**Tried to remove nonexistent buffer (%d) from hash chain %d\n",bufix,k);
    else f->buffer[last].hash_next = f->buffer[next].hash_next;
  }
  if ( trace ) {
    printf("  new "); print_hash_chain(stdout,f,k);
  }
  f->buffer[bufix].hash_next = -1;
}

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
  f->buffer[i].b.keys_in_block = 0;
  f->buffer[i].b.chars_in_use = 0;
  f->buffer[i].b.level = 0;
  f->buffer[i].b.next = nulln_ptr;
  f->buffer[i].b.prev = nulln_ptr;
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
        done = true; printf("**couldn't allocate a buffer\n");
      }
      else if ( f->buffer[oldest].locked ) done = false;
      else done = true;
    }
    while ( !done );
    if ( f->buffer[oldest].modified )
      write_page(f,f->buffer[oldest].contents,&(f->buffer[oldest].b));
    hash_chain_remove(f,oldest);
  }
  if ( trace ) printf("  just vacated oldest buffer, bufix=%d\n",oldest);
  return(oldest);
}

static int get_buffer(struct fcb *f, struct leveln_pntr block, boolean *not_found)
{int bufix;

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
  /*  else if (f->buffer[bufix].b.level>0 ) make_buffer_youngest(f,bufix); */
  else make_buffer_youngest(f,bufix);
  /*  printf("  just got buf=%d, chain=",bufix); print_buffer_MRU_chain(stdout,f);printf("\n"); */
  return(bufix);
}

static int get_index(struct fcb *f, struct leveln_pntr b)
{boolean not_found; int bufix;

/*  get_index_cnt++; */
  bufix = get_buffer(f,b,&not_found);
  /*  printf("just got %d/%d, not_found=%d\n",b.segment,b.block,not_found); */
  if ( not_found ) read_page(f,b,&(f->buffer[bufix].b));
  /*  print_block(stdout,&(f->buffer[bufix].b));*/
  f->mru_at_level[f->buffer[bufix].b.level] = b;
  return(bufix);
}

int kf_wire_index(struct fcb *f, struct leveln_pntr b)
{
  int bufix;

/*  get_index_cnt++; */
  bufix = get_index(f,b);
  f->buffer[bufix].locked = true;
  return(bufix);
}

static int get_for_read(struct fcb *f, struct leveln_pntr b)
{boolean not_found; int bufix;

  bufix = get_buffer(f,b,&not_found);
  if ( not_found ) read_page(f,b,&(f->buffer[bufix].b));
}

/* index searching */

/*
 * search_block searches the block for the first entry<=k returns result
 *   as less or equal and ix as index to entry
 * if k is greater than all keys in block then result=greater
 *   ix=keys_in_block
 */

static int search_block(struct block *b, struct key *k, enum comparison *result)
{int mid,high,ix=0;

  if ( trace ) printf("  searching block for %s, ",k->text);
  high = b->keys_in_block-1;
  while ( ix<=high ) {
    mid = (ix + high) / 2;
    *result = compare_key(k,b,mid);
    /*    *result = cmpstr(k->text,k->lc,b->keys,b->keys[mid].sc,b->keys[mid].lc); */
    switch ( *result ) {
      case greater: ix = mid + 1; break;
      case equal:   ix = mid; high = -1; break;
      case less:    high = mid - 1; break;
    }
  }
  /* now ix points to first entry>=k or keys_in_block */
  if ( trace )
    if (ix>=b->keys_in_block) printf("its larger that any in block\n");
    else if (*result==equal) printf("found it, ix=%d\n",ix);
    else printf("not found, ix=%d\n",ix);
  if ( ix >= b->keys_in_block) *result = greater;
  return(ix);
}

/*
 * search_index searches index blocks down to stop_lvl and returns
 *   a pointer to the block in which the key lies
 */

static struct leveln_pntr search_index(struct fcb *f, int stop_lvl, struct key *k)
{struct leveln_pntr child; int ix,bufix; enum comparison result; boolean done=false;

  child = f->first_at_level[f->primary_level];
  if ( stop_lvl<=f->primary_level )
    do {
      bufix = get_index(f,child);
      ix = search_block(&(f->buffer[bufix].b),k,&result);
      if ( ix>=(f->buffer[bufix].b.keys_in_block) ) {
        done = true; f->error_code = max_key_err; child = nulln_ptr;
        printf("Uh.Oh. %s is larger than any key in block\n",k->text);
      }
      else done = f->buffer[bufix].b.level<=stop_lvl;
      unpackn_ptr(&(f->buffer[bufix].b),ix,&child);
    }
    while ( !done );
  return(child);
}

/* file initialization */

/* init_key initializes the temporary part of the fcb and a few static  */
/*   variables.  It assumes that the fib has been initialized.          */

static void init_key(struct fcb *f, int lc, enum file_access insert_mode)
{int i,hash_target,hash_blocks;;

  null0_ptr.segment = max_segments; null0_ptr.sc = 0; null0_ptr.lc = 0;
  nulln_ptr.segment = max_segments; nulln_ptr.block = 0;
  if ( !check_fcb(f) ) return;
  if ( f->version!=current_version) { f->error_code = version_err; return; }
  if ( lc<min_fcb_lc ) { f->error_code = smallfcb_err; return; }
  f->insert_mode = insert_mode;
  f->open_file_cnt = 0;
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
/*  for (i=0; i<f->buffers_allocated; i++) init_buffer(f,i);
  f->oldest_buffer = 0;
  f->youngest_buffer = f->buffers_allocated - 1;
  f->buffer[f->youngest_buffer].younger = -1; */
  for (i=0; i<max_level; i++) f->mru_at_level[i] = nulln_ptr;
  /*  printf("MRU chain="); print_buffer_MRU_chain(stdout,f);printf("\n"); */
}

/* record moving */

int copy_rec(struct fcb *f,struct level0_pntr *p, unsigned char r[], int *rlc ,int max_lc)
{size_t size; FILE *file;

  if ( check_fcb(f) ) {
    *rlc = p->lc;
    if ( p->lc>max_lc ) f->error_code = longrec_err;
    else if ( p->lc<=long_lc ) memcpy(r,&p->sc,(size_t) p->lc);
    else {
      file = file_index(f,p->segment);
      if ( f->error_code!=no_err ) return;
      if ( fseek(file,p->sc,0)!=0 ) {
        f->error_code = seek_err; return;
      }
      size = fread(r,(size_t) 1,(size_t) p->lc,file);
      if ( size!=p->lc ) f->error_code = read_err;
    }
  }
  return(f->error_code);
}

/* user callable entries */

int open_key(struct fcb *f, char id[], int lc, enum file_access insert_mode)
{char name[max_filename_lc];

  read_fib(f,id);
  if ( f->error_code!=no_err ) fatal_error(f,badopen_err);
  build_segment_name(f,0,name);
  if ( strcmp(id,name)!=0 ) fatal_error(f,bad_name_err);
  init_key(f,lc,insert_mode);
  set_bof(f);
  return(f->error_code);
}

int close_key(struct fcb *f)
{int i; long lc; FILE *temp;

/* printf("  get_buffer_cnt=%d, replace_buffer_cnt=%d\n",get_buffer_cnt,replace_buffer_cnt);*/
/* printf("  get_index_cnt=%d\n",get_index_cnt); */

  if ( f->marker!=keyf ) f->error_code = notkeyfil_err;
  else {
    f->error_code = no_err;
    for (i=0; i<f->buffers_in_use; i++)
      if (f->buffer[i].modified)
        write_page(f,f->buffer[i].contents,&(f->buffer[i].b) );
    write_fib(f);
    for (i=0; i<f->open_file_cnt; i++) {
      lc = f->segment_length[f->open_segment[i]];
      if (trace) printf("  closing segment %d, length=%ld\n",
        f->open_segment[i],lc);
      temp = f->open_file[i];
      fseek(temp,lc,0); fputc(' ',temp);
      fclose(temp);
    }
    f->marker = 0;
  }
  return(f->error_code);
}

static void extract_next(struct fcb *f, int ix, char t[], struct level0_pntr *p)
{int lc;

  lc = f->buffer[ix].b.keys[f->position_ix].lc;
  unpack0_ptr(&(f->buffer[ix].b),f->position_ix,p);
  mvc(f->buffer[ix].b.keys,f->buffer[ix].b.keys[f->position_ix].sc,t,0,lc);
  t[lc] = '\0';
  if ( t[0]==max_char ) {
    f->error_code = ateof_err; t[0] = '\0'; *p = null0_ptr;
  }
  else if ( f->position_ix>=f->buffer[ix].b.keys_in_block-1 && !eq_pntr(f->buffer[ix].b.next,nulln_ptr) )
    set_position(f,f->buffer[ix].b.next,0);
  else f->position_ix++;
}

int next_ptr(struct fcb *f, char t[], struct level0_pntr *p)
{int ix; struct leveln_pntr next;

  if ( check_fcb(f) ) {
    ix = get_index(f,f->position);
    next = f->buffer[ix].b.next;
    while ( f->buffer[ix].b.keys_in_block==0 ) {
      /*      printf("  skipping block %d/%d\n",f->buffer[ix].contents.segment,f->buffer[ix].contents.block);*/
      ix = get_index(f,next);
      set_position(f,next,0);
      next = f->buffer[ix].b.next;
    }
    extract_next(f,ix,t,p);
  }
  return(f->error_code);
}

int prev_ptr(struct fcb *f, char t[], struct level0_pntr *p)
{int bufix,lc; boolean done=false; struct leveln_pntr prev;

  if ( check_fcb(f) ) {
    bufix = get_index(f,f->position);
    if ( f->position_ix==0 ) {
      prev = f->buffer[bufix].b.prev;
      do {
        if ( eq_pntr(prev,nulln_ptr) ) {
          done = true; f->error_code = atbof_err; t[0] = '\0'; *p = null0_ptr;
        }
        else {
          bufix = get_index(f,prev);
          set_position(f,prev,f->buffer[bufix].b.keys_in_block);
          prev = f->buffer[bufix].b.prev;
          done = f->buffer[bufix].b.keys_in_block > 0;
        }
      } while ( !done );
    }
    if ( f->error_code==no_err ) {
      f->position_ix--;
      lc = f->buffer[bufix].b.keys[f->position_ix].lc;
      unpack0_ptr(&(f->buffer[bufix].b),f->position_ix,p);
      mvc(f->buffer[bufix].b.keys,f->buffer[bufix].b.keys[f->position_ix].sc,
        t,0,lc);
      t[lc] = '\0';
    }
  }
  return(f->error_code);
}

int keyrec_lc(struct level0_pntr *p)
{
  if ( p->segment >= max_segments ) return(-1);
  else return(p->lc);
}


int get_ptr(struct fcb *f, char t[], struct level0_pntr *p)
{struct leveln_pntr b; int sc,bufix; struct key k; enum comparison result;
char t1[max_key_lc]; 

  set_up(f,t,&k);
  if ( f->error_code==no_err ) {
    b = search_index(f,level_one,&k);
    bufix = get_index(f,b);
    sc = search_block(&(f->buffer[bufix].b),&k,&result);
    set_position(f,b,sc);
    if ( result==equal ) extract_next(f,bufix,t1,p);
    else {
      f->error_code = getnokey_err; p->lc = 0;
    }
  }
  return(f->error_code);
}


int get_rec(struct fcb *f,char t[],char r[],int *rlc,int max_lc)
{struct level0_pntr p;

  get_ptr(f,t,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

int next_rec(struct fcb *f,char t[],char r[],int *rlc,int max_lc)
{struct level0_pntr p;

  next_ptr(f,t,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

int prev_rec(struct fcb *f,char t[],char r[],int *rlc,int max_lc)
{struct level0_pntr p;

  prev_ptr(f,t,&p);
  if ( f->error_code==no_err ) copy_rec(f,&p,r,rlc,max_lc);
  else *rlc = 0;
  return(f->error_code);
}

/*key manipulation*/

void get_nth_key(struct block *b, struct key *k, int n)
{
  if ( n<0 || n>=b->keys_in_block ) { k->lc = 0; k->text[0] = '\0'; }
  else {
    k->lc = b->keys[n].lc;
    mvc(b->keys,b->keys[n].sc,k->text,0,k->lc);
    k->text[k->lc] = '\0';
  }
}

static void get_max_key(b,k)
struct block *b; struct key *k;
{
  if ( b->keys_in_block<1 ) { k->lc = 0; k->text[0] = '\0'; }
  else {
    k->lc = b->keys[b->keys_in_block-1].lc;
    mvc(b->keys,b->keys[b->keys_in_block-1].sc,k->text,0,k->lc);
    k->text[k->lc] = '\0';
  }
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

static void mark_modified(f,bufix)
struct fcb *f; int bufix;
{
  f->buffer[bufix].modified = true;
}

static int get_index_update(struct fcb *f, struct leveln_pntr b)
{int bufix;

  bufix = get_index(f,b); mark_modified(f,bufix); return(bufix);
}

static void get_for_update(f,b,bufix)
struct fcb *f; struct leveln_pntr b; int *bufix;
{
  *bufix = get_for_read(f,b); mark_modified(f,bufix);
}

/* space allocation - note that if the record will fit in the    */
/*   space used for a block pointer, it will go in the pointer   */
/*   and not be written to the file.  Otherwise the rec is       */
/*   appended to the current segment.  If it will not fit a new  */
/*   segment is created.  allocate_rec and allocate_block manage */
/*   segment_cnt.  Note that nospace_err is set when we allocate */
/*   space in the last possible segment even though the          */
/*   allocation succeeds.  It simplifies error handling.         */


static boolean allocate_rec(struct fcb *f,long lc,struct level0_pntr *p)
{int current,ix;

  current = f->segment_cnt - 1;
  if ( (max_segment_lc - f->segment_length[current]) < lc ) { /* segment full */
    if ( f->segment_cnt>=(max_segments-1) ) {
      f->error_code = nospace_err;
      if ( trace ) printf("  allocate_rec failed, segment=%d\n",current);
    }
    current++;
    ix = file_ix(f,current); /* opens current segment */
    f->segment_cnt++;
  }
  p->segment =current; p->sc = f->segment_length[current]; p->lc = lc;
  if ( lc>long_lc ) f->segment_length[current] = f->segment_length[current] + lc;
  if ( trace )
    printf("  just allocated rec, seg=%d, sc=%ld, lc=%ld\n",p->segment,p->sc,p->lc);
  return(f->error_code!=nospace_err);
}

static int allocate_block(f)
struct fcb *f;
{int bufix,current,ix; long new_blk;

  current = f->segment_cnt - 1;
  if ( (max_segment_lc - f->segment_length[current]) < block_lc ) {
    if ( f->segment_cnt>=(max_segments-1) ) {
      f->error_code = nospace_err;
      if ( trace ) printf("  allocate_block failed, segment=%d\n",current);
    }
    current++;
    ix = file_ix(f,current); /* opens current segment */
    f->segment_cnt++;
  }
  new_blk = f->segment_length[current];
  f->segment_length[current] = f->segment_length[current] + block_lc;
  bufix = vacate_oldest_buffer(f);
  f->buffer[bufix].contents.segment = current;
  f->buffer[bufix].contents.block = new_blk;
  f->buffer[bufix].modified = true;
  hash_chain_insert(f,bufix);
  if ( trace )
    printf("  just allocated block in buf=%d, seg=%d, block=%ld, seg_cnt=%d\n",
      bufix,current,new_blk,f->segment_cnt);
  return(bufix);
}

static int allocate_index_block(struct fcb *f, struct leveln_pntr *b, int lvl)
{int bufix;
/* printf("  allocating index block at level %d\n",lvl); */
  bufix = allocate_block(f);
  f->buffer[bufix].b.keys_in_block = 0;
  f->buffer[bufix].b.chars_in_use = 0;
  f->buffer[bufix].b.level = lvl;
  f->buffer[bufix].b.next = nulln_ptr;
  f->buffer[bufix].b.prev = nulln_ptr;
  f->mru_at_level[lvl] = f->buffer[bufix].contents;
  *b = f->buffer[bufix].contents;
  return(bufix);
}

static void deallocate_rec(f,p)
struct fcb *f; struct level0_pntr p;
{
}

static void deallocate_block(f,p)
struct fcb *f; struct leveln_pntr p;
{
}

static void init_free_space(f)
struct fcb *f;
{
}

/*** index deletion ***/

/* simple_delete deletes the ix^th key from the given block.
 */

static void simple_delete(struct block *blok, int ix)
{int i,key_sc,deleted_lc,deleted_sc,plc; struct level0_pntr p0; struct leveln_pntr pn;

  key_sc = keyspace_lc - blok->chars_in_use;
  if ( blok->level>0 ) plc = unpackn_ptr(blok,ix,&pn);
  else plc = unpack0_ptr(blok,ix,&p0);
  deleted_lc = blok->keys[ix].lc + plc;
  deleted_sc = blok->keys[ix].sc;
  mvc(blok->keys,key_sc,blok->keys,key_sc+deleted_lc,deleted_sc-key_sc);
  blok->chars_in_use = blok->chars_in_use - deleted_lc;
  blok->keys_in_block--;
  for (i=ix; i<blok->keys_in_block; i++) blok->keys[i] = blok->keys[i+1];
  for (i=0; i<blok->keys_in_block; i++)
    if (blok->keys[i].sc<deleted_sc )
      blok->keys[i].sc = blok->keys[i].sc + deleted_lc;
}

static void remove_index(f,b)
struct fcb *f; struct leveln_pntr b;
{struct leveln_pntr parent,save_next,save_prev; int ix=0,bufix; boolean done=false;
 struct leveln_pntr p;

  bufix = get_index(f,b); f->buffer[bufix].modified = false;
  parent = f->mru_at_level[f->buffer[bufix].b.level+1];
  /*printf("  removing index block %d/%d, level=%d, parent=%d/%d\n",
    b.segment,b.block,f->buffer[bufix].b.level,parent.segment,parent.block); */
  save_next = f->buffer[bufix].b.next;
  save_prev = f->buffer[bufix].b.prev;
  if ( eq_pntr(save_prev,nulln_ptr) )
    f->first_at_level[f->buffer[bufix].b.level] = save_next;
  else {
    bufix = get_index_update(f,save_prev);
    f->buffer[bufix].b.next = save_next;
  }
  if ( !eq_pntr(save_next,nulln_ptr) ) {
    bufix = get_index_update(f,save_next);
    f->buffer[bufix].b.prev = save_prev;
  }
  deallocate_block(f,b);
  /*now remove reference from parent*/
  bufix = get_index_update(f,parent);
  do {
    unpackn_ptr(&(f->buffer[bufix].b),ix,&p);
    if ( eq_pntr(p,b) ) done = true;
    else if ( ix>=f->buffer[bufix].b.keys_in_block ) {
      done = true; f->error_code = bad_dlt_err;
      printf("**Delete failed - couldn't find pointer in parent\n");
    }
    else ix++;
  }
  while ( !done );
  simple_delete(&(f->buffer[bufix].b),ix);
  if (f->buffer[bufix].b.keys_in_block<=0 ) {
    /* printf("  recursive index removal\n"); */
    remove_index(f,parent);
  }
}

/* index_delete removes key k from level0 block b.  The current version
     does not reclaim empty level0 blocks in order to avoid problems
     of propagating max_key values up the tree */

static void index_delete(f,k,b,p)
struct fcb *f; struct key k; struct leveln_pntr b; struct level0_pntr *p;
{int bufix,ix; enum comparison result;

  *p = null0_ptr;
  bufix = get_index(f,b);
  ix = search_block(&(f->buffer[bufix].b),&k,&result);
  if ( result!=equal ) f->error_code = dltnokey_err;
  else {
    unpack0_ptr(&(f->buffer[bufix].b),ix,p);
    simple_delete(&(f->buffer[bufix].b),ix);
    mark_modified(f,bufix);
    /*    if (f->buffer[bufix].b.keys_in_block>0 ) mark_modified(f,bufix);
	  else remove_index(f,b);*/
  }
}

/*** index insertion ***/

static void simple_insertn(struct block *b, int ix, struct key *k, struct leveln_pntr p)
{int i,plc;

  plc = packn_ptr(b,p);
  b->keys_in_block++;
  b->chars_in_use = b->chars_in_use + k->lc + plc;
  for (i=b->keys_in_block-1; i>ix; i--) b->keys[i] = b->keys[i-1];
  b->keys[ix].lc = k->lc;
  b->keys[ix].sc = keyspace_lc - b->chars_in_use;
  mvc(k->text,0,b->keys,b->keys[ix].sc,k->lc);
}

static void simple_insert0(struct block *b, int ix, struct key *k, struct level0_pntr p)
{int i,plc;

  plc = pack0_ptr(b,p);
  b->keys_in_block++;
  b->chars_in_use = b->chars_in_use + k->lc + plc;
  for (i=b->keys_in_block-1; i>ix; i--) b->keys[i] = b->keys[i-1];
  b->keys[ix].lc = k->lc;
  b->keys[ix].sc = keyspace_lc - b->chars_in_use;
  mvc(k->text,0,b->keys,b->keys[ix].sc,k->lc);
}

static boolean will_fit(struct key *k, struct block *b)
{int needed,have;

  needed = k->lc + pntr_lc(b->level) + key_ptr_lc;
  have = keyspace_lc - (b->chars_in_use
    + (b->keys_in_block * key_ptr_lc));
  return( needed<=have );
}

static boolean replace_max_key_pntr(struct fcb *f, struct leveln_pntr parent, struct leveln_pntr old_ptr,
  struct leveln_pntr new_ptr)
{int ix=0,bufix; boolean found=false,split=false; struct level0_pntr p0; struct leveln_pntr p; struct key k;

  bufix = get_index_update(f,parent);
  while ( ix<f->buffer[bufix].b.keys_in_block ) {
    unpackn_ptr(&(f->buffer[bufix].b),ix,&p);
    if ( eq_pntr(old_ptr,p) ) {
      found = true;
      get_nth_key(&(f->buffer[bufix].b),&k,ix);

      simple_delete(&(f->buffer[bufix].b),ix);
      if ( will_fit(&k,&(f->buffer[bufix].b)) ) {
        if ( trace ) printf("simple insert\n");
        simple_insertn(&(f->buffer[bufix].b),ix,&k,new_ptr);
      }
      else {
        if ( trace ) printf("replace_max_key forced block split, k=%s, keys_in_block=%d, chars_in_use=%d\n",
          k.text,f->buffer[bufix].b.keys_in_block,f->buffer[bufix].b.chars_in_use);
        split = true;
        copy_ptr(new_ptr,&p0);
        split_block(f,&k,parent,p0,bufix,ix);
      }

      ix = f->buffer[bufix].b.keys_in_block;
    }
    else ix++;
  }
  if ( !found ) printf("**Couldn't replace max_key pntr for %d/%d in %d/%d\n",
    old_ptr.segment,old_ptr.block,parent.segment,parent.block);
  return(split);
}

/* move_keys_to_new_block moves keys from the block in buffer[ix] */
/*   to the block in buffer[new_ix] then inserts key k and        */
/*   pointer p into the appropriate block.                        */

static void move_keys_to_new_block(struct fcb *f, struct key *k,
  struct level0_pntr p,int index, int bufix, int new_ix)
{int freed=0,needed,entry_lc,level,plc; struct key max; struct level0_pntr p0;
 struct leveln_pntr pn;

  level = f->buffer[bufix].b.level;
  entry_lc = k->lc + pntr_lc(level) + key_ptr_lc;
  if ( f->insert_mode==seq ) needed = entry_lc;
  else if ( entry_lc>(keyspace_lc/3) ) needed = entry_lc;
  else needed = keyspace_lc / 3;
  do {
    get_max_key(&(f->buffer[bufix].b),&max);
    if ( level==0 ) {
      plc = unpack0_ptr(&(f->buffer[bufix].b),f->buffer[bufix].b.keys_in_block-1,&p0);
      simple_insert0(&(f->buffer[new_ix].b),0,&max,p0); /* always min key */
    }
    else {
      plc = unpackn_ptr(&(f->buffer[bufix].b),f->buffer[bufix].b.keys_in_block-1,&pn);
      simple_insertn(&(f->buffer[new_ix].b),0,&max,pn);
    }
    freed = freed + f->buffer[bufix].b.keys[f->buffer[bufix].b.keys_in_block-1].lc +
      plc + key_ptr_lc;
    simple_delete(&(f->buffer[bufix].b),f->buffer[bufix].b.keys_in_block-1);
  } while ( freed<needed );
  if ( trace ) {
    printf("  moved %d keys to new block, insert is to ",f->buffer[new_ix].b.keys_in_block);
  }
  if ( index<f->buffer[bufix].b.keys_in_block ) {
    if ( trace ) printf("old block\n");
    if ( level==0 ) simple_insert0(&(f->buffer[bufix].b),index,k,p);
    else {
      pn.segment = p.segment; pn.block = p.sc;
      simple_insertn(&(f->buffer[bufix].b),index,k,pn);
    }
  }
  else {
    if ( trace ) printf("new block\n");
    if ( level==0 )simple_insert0(&(f->buffer[new_ix].b),index-f->buffer[bufix].b.keys_in_block,k,p);
    else {
      pn.segment = p.segment; pn.block = p.sc;
      simple_insertn(&(f->buffer[new_ix].b),index-f->buffer[bufix].b.keys_in_block,k,pn);
    }
  }
  if ( trace ) {
    get_max_key(&(f->buffer[bufix].b),&max);
    pn = f->buffer[bufix].contents;
    printf("  old(%d/%d): %d keys, max is %s\n",pn.segment,pn.block,
      f->buffer[bufix].b.keys_in_block,max.text);
    get_max_key(&(f->buffer[new_ix].b),&max);
    pn = f->buffer[new_ix].contents;
    printf("  new(%d/%d): %d keys, max is %s\n",pn.segment,pn.block,
      f->buffer[new_ix].b.keys_in_block,max.text);
  }
}

/* create_new_primary creates a new primary block containing the keys max and  */
/*   new_max  */

static void create_new_primary(struct fcb *f, struct leveln_pntr b, struct key *max,
  struct leveln_pntr newb, struct key *new_max)
{int ix;

  if ( f->primary_level>=max_level-1 ) fatal_error(f,maxlevel_err);
  else {
    f->primary_level++;
    ix = allocate_index_block(f,&(f->first_at_level[f->primary_level]),
      f->primary_level);
    simple_insertn(&(f->buffer[ix].b),0,max,b);
    simple_insertn(&(f->buffer[ix].b),1,new_max,newb);
  }
}

/* split_block splits the block in buffer[bufix]. On entry key k    */
/*   belongs in entry index and must be <= the */
/*   max_key in the parent block that points to this block.  It     */
/*   creates a new block, moves keys to it, and adjusts prev and    */
/*   next pointers. It then updates the parent block with a new     */
/*   pointer for the right block and a new max_key for the left or  */
/*   creates a new primary, as appropriate.  Note that the max_key  */
/*   value in the parent may not match the child (child could have  */
/*   been emptied).  But it is still a valid max_key.  The parent   */
/*   entry is changed to point to the newly allocated block.        */

static void split_block(struct fcb *f, struct key *k, struct leveln_pntr b,
  struct level0_pntr p, int bufix, int index)
{int new_ix,parent_level,i; boolean split; struct leveln_pntr parent,newb,save_next;
 struct key max,new_max;

  lock_buffer(f,bufix);
  parent_level = f->buffer[bufix].b.level + 1;
  parent = f->mru_at_level[parent_level];
  new_ix = allocate_index_block(f,&newb,f->buffer[bufix].b.level);
  /*  printf("  splitting block %d/%d, new block is %d/%d\n",
      b.segment,b.block,newb.segment,newb.block);*/
  if ( trace ) printf("  splitting block %d/%d, new block is %d/%d\n",
    b.segment,b.block,newb.segment,newb.block);
  f->buffer[new_ix].b.next = f->buffer[bufix].b.next;
  f->buffer[new_ix].b.prev = b;
  save_next = f->buffer[new_ix].b.next;
  f->buffer[bufix].b.next = newb;
  move_keys_to_new_block(f,k,p,index,bufix,new_ix);
  get_max_key(&(f->buffer[bufix].b),&max);
  get_max_key(&(f->buffer[new_ix].b),&new_max);
  unlock_buffer(f,bufix);
  if ( !eq_pntr(save_next,nulln_ptr) ) {
    i = get_index_update(f,save_next); f->buffer[i].b.prev = newb;
  }
  /*now propogate upward*/
  if ( parent_level>f->primary_level )
    create_new_primary(f,b,&max,newb,&new_max);
  else { /*update parent*/
    split = replace_max_key_pntr(f,parent,b,newb);
    if ( split ) {
      /* replace_max_key_pntr split the parent block so we */
      /* have to search for the right one */
      parent = search_index(f,parent_level+1,&max);
      if ( trace ) printf("  parent after replace_max split is %d/%d\n",parent.segment,parent.block);
    }
    update_interior(f,&max,parent,b);
  }
}

/* update_interior inserts key k and pointer p into interior block b. */

void update_interior(struct fcb *f, struct key *k, struct leveln_pntr b, struct leveln_pntr p)
{int bufix,ix; enum comparison result; struct level0_pntr p0;

  bufix = get_index_update(f,b);
  ix = search_block(&(f->buffer[bufix].b),k,&result);
  if ( trace ) printf("  updating interior node %ld/%ld with key %s, ",b.segment,b.block,k->text);
  if (result==equal ) {
    /*    if ( trace ) printf(" pointer replacement\n");
	  packn_ptr(&(f->buffer[bufix].b),ix,p);*/
    simple_delete(&(f->buffer[bufix].b),ix);
  }
  if ( will_fit(k,&(f->buffer[bufix].b)) ) {
    if ( trace ) printf("simple insert\n");
    simple_insertn(&(f->buffer[bufix].b),ix,k,p);
  }
  else {
    if ( trace ) printf("block split, k=%s, keys_in_block=%d, chars_in_use=%d\n",
      k->text,f->buffer[bufix].b.keys_in_block,f->buffer[bufix].b.chars_in_use);
    copy_ptr(p,&p0);
    split_block(f,k,b,p0,bufix,ix);
  }
}

/* update_leaf inserts key k and pointer p into leaf block b.       */

void update_leaf(struct fcb *f, struct key *k, struct leveln_pntr b, struct level0_pntr p)
{int bufix,ix; enum comparison result;

  bufix = get_index_update(f,b);
  ix = search_block(&(f->buffer[bufix].b),k,&result);
  if ( trace ) printf("  updating leaf %ld/%ld with key %s, ",b.segment,b.block,k->text);
  if (result==equal ) {
    /*  if ( trace ) printf(" pointer replacement\n");
	pack0_ptr(&(f->buffer[bufix].b),p); */
    simple_delete(&(f->buffer[bufix].b),ix);
  }
  if ( will_fit(k,&(f->buffer[bufix].b)) ) {
    if ( trace ) printf("simple insert\n");
    simple_insert0(&(f->buffer[bufix].b),ix,k,p);
  }
  else {
    if ( trace ) printf("leaf split\n    k=%s, keys_in_block=%d, chars_in_use=%d\n",
      k->text,f->buffer[bufix].b.keys_in_block,f->buffer[bufix].b.chars_in_use);
    split_block(f,k,b,p,bufix,ix);
  }
}

/* record moving -- if the record is so short that it fit in a long */
/*   then it is kept in the sc field of the pointer, otherwise the  */
/*   record is appended to the file     */

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
    if ( size!=p->lc ) printf("**move_from_rec failed\n");
  }
}

/* user callable entries */

int create_key(struct fcb *f, char id[], int lc, enum file_access insert_mode)
{int i,bufix; struct leveln_pntr b; struct key max_key; 

  f->error_code = no_err; f->version = current_version;
  f->segment_cnt = 0;
  f->primary_level = level_zero;
  f->marker = keyf; f->file_ok = true;
  for (i=0; i<max_level; i++) f->first_at_level[i]= nulln_ptr;
  for (i=0; i<max_segments; i++) f->segment_length[i] = 0;
  init_file_name(f,id);

  if ( f->error_code==no_err ) {
    init_key(f,lc,insert_mode);
    init_free_space(f);
    write_fib(f);
    f->segment_cnt = 1; f->segment_length[0] = fib_lc;
    bufix = allocate_index_block(f,&b,level_zero);
    f->first_at_level[level_zero] = f->buffer[bufix].contents;
    max_key.lc = 1;
    max_key.text[0] = max_char; max_key.text[1] = '\0';
    simple_insert0(&(f->buffer[bufix].b),0,&max_key,null0_ptr);
    set_bof(f);
  }
  return(f->error_code);
}

int put_ptr(struct fcb *f, char t[], struct level0_pntr p)
{struct leveln_pntr b; struct key k;

  set_up(f,t,&k);
  if ( f->error_code==no_err ) {
    b = search_index(f,level_one,&k);
    update_leaf(f,&k,b,p);
    set_bof(f);
    }
  return(f->error_code);
}

int delete_ptr(struct fcb *f, char t[])
{struct leveln_pntr b; struct key k; struct level0_pntr p;

  set_up(f,t,&k);
  if (f->error_code==no_err ) {
    b = search_index(f,level_one,&k);
    index_delete(f,k,b,&p);
    set_bof(f);
    }
  return(f->error_code);
}

int put_rec(struct fcb *f,char t[],char r[], int rlc)
{struct level0_pntr p; boolean have_space=false;

  get_ptr(f,t,&p);
  if (f->error_code==no_err ) {
    if ( p.lc>=rlc ) { have_space = true; p.lc = rlc; }
    else deallocate_rec(f,p);
  }
  if (f->error_code==getnokey_err ) f->error_code =no_err;
  if ( ! have_space ) have_space = allocate_rec(f,(long)rlc,&p);
  if  ( have_space ) {
    move_from_rec(f,r,&p); 
    if ( f->error_code==no_err ) put_ptr(f,t,p);
  }
  return(f->error_code);
}

int delete_rec(struct fcb *f, char t[])
{struct level0_pntr p; struct key k; struct leveln_pntr b;

   set_up(f,t,&k);
  if (f->error_code==no_err ) {
    b = search_index(f,level_one,&k);
    index_delete(f,k,b,&p);
    if (f->error_code==no_err ) deallocate_rec(f,p);
    set_bof(f);
  }
  return(f->error_code);
}
