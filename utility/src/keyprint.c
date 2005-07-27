/*                                                                    */
/* Copyright 1984,1985,1986,1988,1989,1990,2003,2004,2005             */
/*    by Howard Turtle                                                */
/*                                                                    */

#define boolean int
#define true 1
#define false 0

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "keydef.h"
#include "keyerr.h"
#include "keyprint.h"

void get_max_key();
void get_nth_key();
int get_nth_key_and_pntr();
boolean eq_key();
enum comparison compare_key();
int unpack0_ptr();
int unpackn_ptr();
void read_page();
int unpack_lc_key();
int unpack_rec_key();
void kf_set_bof();
int kf_next_ptr();
int kf_next_rec();

static struct leveln_pntr
  nulln_ptr = {max_segments,0};

#define eq_pntr(p1,p2) ((p1.block==p2.block) && (p1.segment==p2.segment))
#define null_pntr(p) ((p.segment==max_segments) && (p.block==0))
#define mvc(t1,sc1,t2,sc2,lc) memmove((unsigned char *)t2+sc2,(unsigned char *)t1+sc1,(size_t)lc);

void toggle_trace(struct fcb *f)
{
  f->trace = ! f->trace;
}

static void print_leveln_pntr(FILE *list, char caption[], struct leveln_pntr *pn)
{
  fprintf(list,"%s%u/",caption,pn->segment);
  fprintf(list,UINT64_format,pn->block);
  fprintf(list," ");
}

static void print_level0_pntr(FILE *list, char caption[], struct level0_pntr *p0)
{
  fprintf(list,"%s%u/",caption,p0->segment);
  fprintf(list,UINT64_format,p0->sc);
  fprintf(list,"/%u ",p0->lc);
}

static boolean is_primary(struct fcb *f, int bufix)
{int index;

  index = f->buffer[bufix].b.ix.index_type;
  if ( f->buffer[bufix].b.ix.level==f->primary_level[index] ) return(true);
  else return(false);
}

void get_page(struct fcb *f, struct leveln_pntr blk, block_type_t *buf)
{int i; boolean found; struct leveln_pntr p;

  found = false;
  for (i=0; i<f->buffers_in_use; i++) {
    p = f->buffer[i].contents;
    if ( p.segment==blk.segment && p.block==blk.block ) { *buf = f->buffer[i].b; found = true; }
  }
  if ( ! found ) read_page(f,blk,buf);
}

/*static void print_lc_key(FILE *list,struct key *k, char caption[])
{struct level0_pntr p;

  unpack_lc_key(k->text,&p);
  fprintf(list,"%s free_lc=%ld, %d/%ld",caption,p.lc,p.segment,p.sc);
}

static void print_rec_key(FILE *list, struct key *k, char caption[])
{struct level0_pntr p;

  unpack_rec_key(k->text,&p);
  fprintf(list,"%s free_rec=%d/%ld",caption,p.segment,p.sc);
}

static void print_user_key(FILE *list, struct key *k, char caption[])
{int i; boolean is_text;

  is_text = true;
  for (i=0; i<k->lc; i++) if ( !isprint(k->text[i]) ) is_text = false;
  fprintf(list,"%s",caption);
  if ( k->lc==0 ) fprintf(list,"null");
  else if ( is_text ) for (i=0; i<k->lc; i++) fprintf(list,"%c",k->text[i]);
  else {
    fprintf(list,"0x");
    for (i=0; i<k->lc; i++) fprintf(list,"%2x",k->text[i]);
  }
}

static void print_any_key(FILE *list, int index, struct key *k, char caption[])
{
  if ( index==user_ix ) print_user_key(list,k,caption);
  else if ( index==free_rec_ix ) print_rec_key(list,k,caption);
  else if ( index==free_lc_ix )  print_lc_key(list,k,caption);
}*/

void print_key_struct(FILE *list, struct key *k, char caption[])
{int i; boolean is_text;

  is_text = true;
  for (i=0; i<k->lc; i++) if ( !isprint(k->text[i]) ) is_text = false;
  fprintf(list,"%s",caption);
  if ( k->lc==0 ) fprintf(list,"null");
  else if ( is_text ) for (i=0; i<k->lc; i++) fprintf(list,"%c",k->text[i]);
  else {
    fprintf(list,"0x");
    for (i=0; i<k->lc; i++) fprintf(list,"%2x",k->text[i]);
  }
}

/** fcb print procedures **/


static void print_freespace_chain(FILE *list, struct fcb *f)
{int err,lc,key_lc,rec_lc; long lc_entries=0,rec_entries=0,total_lc=0,total_rec=0;struct level0_pntr p0;
 unsigned char key[maxkey_lc];

  kf_set_bof(f,free_lc_ix);
  err = kf_next_ptr(f,free_lc_ix,key,&key_lc,maxkey_lc,&p0);
  if ( err==ateof_err ) fprintf(list," freespace chain is empty\n");
  else {
    fprintf(list,"freespace lc list\n");
    while ( err==no_err ) {
      lc_entries++;
      unpack_lc_key(key,&p0);
      fprintf(list,"%5ld  lc=%10u, %4u/",lc_entries,p0.lc,p0.segment);
      fprintf(list,UINT64_format,p0.sc);
      fprintf(list,"\n");
      total_lc = total_lc + p0.lc;
      err = kf_next_ptr(f,free_lc_ix,key,&key_lc,maxkey_lc,&p0);
    }
    fprintf(list,"freespace rec list\n");
    kf_set_bof(f,free_rec_ix);
    err = kf_next_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,&lc,&rec_lc,sizeof(int));
    while ( err==no_err ) {
      rec_entries++;
      unpack_rec_key(key,&p0);
      fprintf(list,"%5ld  ",rec_entries);
      print_level0_pntr(list,"",&p0);
      fprintf(list,"\n");
      total_rec = total_rec + lc;
      err = kf_next_rec(f,free_rec_ix,key,&key_lc,maxkey_lc,&lc,&rec_lc,sizeof(int));
    }
    if ( rec_entries!=lc_entries )
      fprintf(list,"**list mismatch, %ld entries in lc, %ld entries in rec\n",lc_entries,rec_entries);
    if ( total_lc!=total_rec )
      fprintf(list,"**length mismatch, lc list has %ld bytes, rec list has%ld\n",total_lc,total_rec);
    fprintf(list,"  entries=%ld, total_lc=%ld, mean_entry=%5.1f\n",rec_entries,total_rec,(double)total_rec/rec_entries);
  }
}

/* static void print_empty_block_chain(FILE *list, struct fcb *f, int index_type, int level)
{struct leveln_pntr next; block_type_t buf;

  printf(" empty block chain[%d][%d] =",level,index_type);
  next = f->first_free_block[level][index_type];
  if ( null_pntr(next) ) printf(" empty\n");
  else {
    while ( !null_pntr(next) ) {
      printf(" %d/%lu",next.segment,next.block);
      get_page(f,next,&buf);
      next = buf.ix.next;
    }
    printf("\n");
  }
}*/


static boolean block_is_sorted(struct ix_block *b)
{int i,r,lc,lc1,min; boolean sorted=true;

  for (i=0; i<b->keys_in_block-1; i++) {
    lc = b->keys[i].lc;  lc1 = b->keys[i+1].lc;
    if ( lc<lc1 ) min = lc; else min = lc1;
    r = memcmp( (char *)b->keys+b->keys[i].sc, (char *)b->keys+b->keys[i+1].sc,(size_t)min);
    if ( r>0 ) sorted = false;
    else if ( r==0 && lc>=lc1 ) sorted = false;
  }
  return(sorted);
}

static int key_length(struct ix_block *b)
{int i,lc=0;

  for (i=0; i<b->keys_in_block; i++) {
    lc = lc + b->keys[i].lc;
  }
  return(lc);
}

static int uncompressed_key_length(struct ix_block *b)
{int i,lc=0;

  for (i=0; i<b->keys_in_block; i++) {
    lc = lc + b->keys[i].lc + b->prefix_lc;
  }
  return(lc);
}

static void check_index_chain(FILE *list, struct fcb *f, struct leveln_pntr head, struct leveln_pntr parent_head)
{int block_cnt=0,key_cnt=0,min_cnt=32767,max_cnt=0,lc=0,load,key_lc=0,uncompressed_key_lc=0,empty_block_cnt=0,
  singleton_block_cnt=0,parent_ix=0,index_type;
 unsigned level;
struct leveln_pntr p,parent; levelx_pntr px; block_type_t b,parent_block; struct key max,last_max,min,parent_key;

  last_max.lc = 0; last_max.text[0] = '\0';
  p = head;
  parent = parent_head;
  if ( !null_pntr(parent) ) {
    get_page(f,parent,&parent_block);
    /*    printf("got parent page %d/%d\n",parent.segment,parent.block);*/
  }
  while ( !null_pntr(p) ) {
    get_page(f,p,&b);
    block_cnt++;
    if ( b.ix.keys_in_block==0 ) empty_block_cnt++;
    if ( b.ix.keys_in_block==1 ) singleton_block_cnt++;
    if ( b.ix.keys_in_block>0 ) {
      get_nth_key(&(b.ix),&min,0);
      if ( compare_key(&last_max,&(b.ix),0)==greater ) {
        print_key_struct(list,&last_max,"  block sequence problem, last_max=");
        print_key_struct(list,&min,", current min=");
        fprintf(list,"\n");
      }
      else get_nth_key(&(b.ix),&last_max,b.ix.keys_in_block-1);
    }
    key_cnt = key_cnt + b.ix.keys_in_block;
    lc = lc + b.ix.chars_in_use + (b.ix.keys_in_block * key_ptr_lc);
    key_lc = key_lc + key_length(&(b.ix));
    uncompressed_key_lc = uncompressed_key_lc + uncompressed_key_length(&(b.ix));
    if ( b.ix.keys_in_block>max_cnt ) max_cnt = b.ix.keys_in_block;
    if ( b.ix.keys_in_block<min_cnt ) min_cnt = b.ix.keys_in_block;
    if ( !block_is_sorted(&(b.ix)) ) {
      print_leveln_pntr(list,"  Block ",&p);
      fprintf(list,"is not sorted\n");
      print_index_block(list,&(b.ix));
    }
    if ( null_pntr(parent)  ) { /* we're checking the primary or at end of chain*/
      level = b.ix.level;
      index_type = b.ix.index_type;
      if ( level==f->primary_level[index_type] ) /* ok */;
      else if ( null_pntr(b.ix.next) && eq_pntr(f->last_pntr[level+1][index_type],p) ) /* ok */;
      else {
        print_leveln_pntr(list,"  End of parent chain, last_pntr=",&(f->last_pntr[level+1][index_type]));
        print_leveln_pntr(list,"doesn't match current block=",&p);
        fprintf(list,"\n");
      }
    }
    else {
      get_nth_key_and_pntr(&(parent_block.ix),&parent_key,parent_ix,&px);
      parent_ix++;
      get_max_key(&(b.ix),&max);
      /*      print_key_struct(list,&parent_key,"    parent key=");
      print_key_struct(list,&max," max_key=");
      fprintf(list,"\n");*/
      if ( !eq_key(&parent_key,&max) ) {
        print_leveln_pntr(list,"  Parent key from ",&parent);
        print_leveln_pntr(list,"doesn't match max_key in ",&p);
        fprintf(list,"\n");
        print_key_struct(list,&parent_key,"    parent_key=");
        print_key_struct(list,&max," max_key=");
        fprintf(list,"\n");
      }
      if ( !eq_pntr(px.pn,p) ) {
        print_leveln_pntr(list,"  Parent pointer=",&(px.pn));
        print_leveln_pntr(list,"doesn't match current block=",&p);
        fprintf(list,"\n");
      }
      if ( parent_ix>=parent_block.ix.keys_in_block ) {
        parent = parent_block.ix.next;
        if ( !null_pntr(parent) ) get_page(f,parent,&parent_block);
        parent_ix = 0;
      }
    }
    p = b.ix.next;
  }
  lc = lc / block_cnt;
  load = (lc*100) / (keyspace_lc);
  fprintf(list,"  chain contains %d blocks, %d keys, min=%d, max=%d, mean keys=%d, mean lc=%d, mean load=%d%%\n",
    block_cnt,key_cnt,min_cnt,max_cnt,key_cnt/block_cnt,lc,load);
  fprintf(list,"    key length=%d(mean=%d), uncompressed_key_length=%d(mean=%d)\n",
    key_lc,key_lc/block_cnt,uncompressed_key_lc,uncompressed_key_lc/block_cnt);
  if ( empty_block_cnt>0 || singleton_block_cnt>0 )
    fprintf(list,"    empty_block_cnt=%d, singleton_block_cnt=%d\n",empty_block_cnt,singleton_block_cnt);
}

void check_index(FILE *list,struct fcb *f, int index)
{int i; struct leveln_pntr head,parent;

  fprintf(list,"Index type=%d\n",index);
  parent = nulln_ptr;
  for (i=f->primary_level[index]; i>=0; i--) {
    head = f->first_at_level[i][index];
    fprintf(list,"Beginning sort check of level %d\n",i);
    check_index_chain(list,f,head,parent);
    parent = head;
  }
}

static void print_freespace_summary(FILE *list, struct fcb *f)
{
  fprintf(list,"Freespace rec summary:");
  check_index(list,f,free_rec_ix);
  fprintf(list,"Freespace lc summary:");
  check_index(list,f,free_lc_ix);
}

void print_freespace(FILE *list, struct fcb *f)
{
  print_freespace_summary(list,f);
  print_freespace_chain(list,f);
}

int print_hash_chain(FILE *list,struct fcb *f, int ix)
{int next,cnt=0;

  next = f->buf_hash_table[ix];
  while ( next>=0 ) {
    if ( is_primary(f,next) ) fprintf(list,"**");
    fprintf(list," %d(",next);
    print_leveln_pntr(list,"",&(f->buffer[next].contents));
    fprintf(list,")(%d)",f->buffer[next].b.ix.level);
    if ( is_primary(f,next) ) fprintf(list,"**");
    cnt++;
    next = f->buffer[next].hash_next;
  }
  fprintf(list,"\n");
  return(cnt); 
}

void print_buffer_caption(FILE *list, struct fcb *f, int ix)
{int index_type,level;

  index_type = f->buffer[ix].b.ix.index_type;
  level = f->buffer[ix].b.ix.level;
  if (index_type==user_ix) {
    if ( is_primary(f,ix) ) fprintf(list," %4d(X%2d",ix,level);
    else fprintf(list," %4d(x%2d",ix,level);
  }
  else if (index_type==free_lc_ix) {
    if ( is_primary(f,ix) ) fprintf(list," %4d(L%2d",ix,level);
    else fprintf(list," %4d(l%2d",ix,level);
  }
  else if (index_type==free_rec_ix) {
    if ( is_primary(f,ix) ) fprintf(list," %4d(R%2d",ix,level);
    else fprintf(list," %4d(r%2d",ix,level);
  }
  if ( f->buffer[ix].modified ) fprintf(list,"*)");
  else fprintf(list," )");
}

static void print_buffer_MRU_chain(FILE *list,struct fcb *f)
{int ix,cnt=0;

  ix = f->youngest_buffer;
  while ( ix>=0 ) {
    if ( (cnt%10)==0) fprintf(list,"\n    ");
    cnt++;
    print_buffer_caption(list,f,ix);
    ix = f->buffer[ix].older;
  }
  fprintf(list,"\n");
}

void print_buffer_summary(FILE *list, struct fcb *f)
{int i=0,j,empties=0,entries=0,min,max=0,allocated;

  fprintf(list,"Buffer summary, buffers_allocated=%d, buffers_in_use=%d, hash table size=%d\n",
    f->buffers_allocated,f->buffers_in_use,f->buf_hash_entries);
  fprintf(list,"  MRU chain is ( bufix(type level modified) x=user l=free_lc r=free_rec *=modified)");
  print_buffer_MRU_chain(list,f);
  fprintf(list,"  Hash table\n");
  allocated = f->buf_hash_entries;
  while ( i<allocated ) {
    min = i;
    while ( i<allocated && f->buf_hash_table[i]<0 ) {
      i++; empties++; 
    }
    if ( i==min+1 )  fprintf(list,"    chain  from %d is empty\n",min);
    else if ( i>min) fprintf(list,"    chains from %d to %d are empty\n",min,i-1);
    if ( i<allocated ) {
      fprintf(list,"    chain  from %d ",i);
      j =  print_hash_chain(list,f,i);
      if ( j>max ) max = j;
      entries = entries + j;
      i++;
    }
  }
  printf("  %d chains are empty, total entries=%d, longest=%d\n",
    empties,entries,max);
  printf("   mean chain length=%.2f\n",(float) entries / (f->buf_hash_entries - empties));
}
    

static void print_segments(FILE *list, struct fcb *f)
{unsigned i;

  fprintf(list,"Segment directory, segment_cnt=%u, max_segments=%d, ",
    f->segment_cnt,max_segments);
  fprintf(list,UINT64_format,f->max_file_lc);
  fprintf(list,"\n");
  if ( f->segment_cnt>0 ) {
    fprintf(list,
      "    segment    length  open on    segment    length  open on    segment    length  open on\n");
    for (i=0; i<f->segment_cnt; i++) {
      fprintf(list,"     %6d ",i);
      fprintf(list,UINT64_format,f->segment_length[i]);
      if ( f->segment_ix[i]>=max_files ) fprintf(list,"  closed");
      else fprintf(list,"   %3d  ",f->segment_ix[i]);
      if ( i%3==2 ) fprintf(list,"\n");
    }
    fprintf(list,"\n");
  }
  else fprintf(list,"  **no segments open\n");
}

static void print_files(FILE *list, struct fcb *f)
{int i; unsigned j;

  fprintf(list,"File directory, files open=%d, max_files=%d\n",
    f->open_file_cnt,max_files);
  if ( f->open_file_cnt>0 ) {
    for (i=0; i<f->open_file_cnt; i++) {
      fprintf(list,"  file %3d open to segment",i);
      for (j=0; j<f->segment_cnt; j++)
        if (f->segment_ix[j]==i) fprintf(list," %d",j);
      fprintf(list,", age=%d\n",f->current_age-f->file_age[i]);
    }
  }
  else fprintf(list,"  **no files open\n");
}

static void print_ix_block_entry(FILE *list, struct ix_block *b, int i)
{int lc,temp; struct key k; struct level0_pntr p0; struct leveln_pntr pn;

  lc = b->keys[i].lc;
  fprintf(list,"%4d%3d ",b->keys[i].sc,lc);
  k.lc = b->keys[i].lc;
  mvc(b->keys,b->keys[i].sc,k.text,0,k.lc);
  print_key_struct(list,&k,"");
  if ( b->level>0 ) {
    temp = unpackn_ptr(b,i,&pn);
    print_leveln_pntr(list," - ",&pn);
    fprintf(list,"(lc=%d)\n",temp);
  }
  else {
    temp = unpack0_ptr(b,i,&p0);
    print_level0_pntr(list," - ",&p0);
    fprintf(list,"(lc=%d)\n",temp);
  }
}

void print_index_block(FILE *list, struct ix_block *b)
{int i,lc,load; unsigned char t[maxkey_lc];

  fprintf(list," keys=%d",b->keys_in_block);
  lc = b->chars_in_use + (b->keys_in_block * key_ptr_lc);
  load = (lc*100) / (keyspace_lc);
  fprintf(list,", chars=%d(%d) %d%% loaded",b->chars_in_use,lc,load);
  fprintf(list,", level=%d",b->level);
  fprintf(list,", ix_type=%d",b->index_type);
  fprintf(list,", prefix_lc=%d",b->prefix_lc);
  if ( b->prefix_lc>0 ) {
    fprintf(list,", prefix=");
    mvc(b->keys,keyspace_lc-b->prefix_lc,t,0,b->prefix_lc);
    for (i=0; i<b->prefix_lc; i++) 
      if ( b->index_type==user_ix ) fprintf(list,"%c",t[i]);
      else fprintf(list,"%2x ",t[i]);
    fprintf(list,"\n");
  }
  print_leveln_pntr(list,"   next=",&(b->next));
  print_leveln_pntr(list,"prev=",&(b->prev));
  fprintf(list,"\n");
  if ( b->keys_in_block<50 ) {
    for (i=0; i<b->keys_in_block; i++) print_ix_block_entry(list,b,i);
  }
  else {
    for (i=0; i<20; i++) print_ix_block_entry(list,b,i);
    fprintf(list,"    ...\n");
    for (i=b->keys_in_block-20; i<b->keys_in_block; i++) print_ix_block_entry(list,b,i);
  }

}

static void print_index_block1(FILE *list, struct fcb *f, int bufix)
{
  print_index_block(list,&(f->buffer[bufix].b.ix));
}

void print_index(FILE *list, struct fcb *f, int index_type)
{int i; struct leveln_pntr next; block_type_t buf;

  for (i=f->primary_level[index_type]; i>=0; i--) {
    fprintf(list,"**Start of level %d\n",i);
    next = f->first_at_level[i][index_type];
    while ( !null_pntr(next) ) {
      print_leveln_pntr(list,"block number ",&next);
      fprintf(list,"\n");
      get_page(f,next,&buf);
      print_index_block(list,&buf.ix);
      next = buf.ix.next;
    }
  }
}
static void print_level_line(FILE *list, struct leveln_pntr mru, struct leveln_pntr first,
  struct leveln_pntr last, struct leveln_pntr first_free)
{

  print_leveln_pntr(list," ",&mru);
  print_leveln_pntr(list," ",&first);
  print_leveln_pntr(list," ",&last);
  print_leveln_pntr(list," ",&first_free);
}

static void print_levels(FILE *list, struct fcb *f)
{unsigned i;

  fprintf(list,"Level pointers\n");
  fprintf(list," lvl user MRU     user first   user last    free_block\n");
  for (i=0; i<=f->primary_level[user_ix]; i++) {
    fprintf(list,"  %2d",i);
    print_level_line(list,
      f->mru_at_level[i][user_ix],
      f->first_at_level[i][user_ix],
      f->last_pntr[i][user_ix],
      f->first_free_block[i][user_ix]);
  }
  fprintf(list," lvl frec MRU     frec first   frec last    free_block\n");
  for (i=0; i<=f->primary_level[free_rec_ix]; i++) {
    fprintf(list,"  %2d",i);
    print_level_line(list,
      f->mru_at_level[i][free_rec_ix],
      f->first_at_level[i][free_rec_ix],
      f->last_pntr[i][free_rec_ix],
      f->first_free_block[i][free_rec_ix]);
  }
  fprintf(list," lvl f_lc MRU     f_lc first   f_lc last    free_block\n");
  for (i=0; i<=f->primary_level[free_lc_ix]; i++) {
    fprintf(list,"  %2d",i);
    print_level_line(list,
      f->mru_at_level[i][free_lc_ix],
      f->first_at_level[i][free_lc_ix],
      f->last_pntr[i][free_lc_ix],
      f->first_free_block[i][free_lc_ix]);
  }
  fprintf(list,"\n");
}

void print_fib(FILE *list, struct fcb *f)
{
  fprintf(list,"FCB Print for file=%s",f->file_name);
  if ( f->file_extension[0]=='\0' ) fprintf(list,"\n");
  else fprintf(list,"%s\n",f->file_extension);
  fprintf(list,"  status is ");
  if ( f->file_ok ) fprintf(list,"ok");
    else fprintf(list,"damaged");
  fprintf(list,", error_code=%u",f->error_code);
  fprintf(list,", version=%u",f->version);
  fprintf(list,", marker=%u",f->marker);
  fprintf(list,", current_age=%d, block_lc=%d\n",f->current_age,(int)block_lc);
  fprintf(list,"  block_shift=%d, swapping_required=%d, read_only=%d\n",
    f->block_shift,f->byte_swapping_required,f->read_only);
  fprintf(list,"  primary level[user    ]=%u,",f->primary_level[user_ix]);
  print_leveln_pntr(list," position[user    ]=",&(f->position[user_ix]));
  fprintf(list,"%d\n",f->position_ix[user_ix]);
  fprintf(list,"  primary level[free_rec]=%u,",f->primary_level[free_rec_ix]);
  print_leveln_pntr(list," position[free_rec]=",&(f->position[free_rec_ix]));
  fprintf(list,"%d\n",f->position_ix[free_rec_ix]);
  fprintf(list,"  primary level[free_lc ]=%u,",f->primary_level[free_lc_ix]);
  print_leveln_pntr(list," position[free_lc ]=",&(f->position[free_lc_ix]));
  fprintf(list,"%d\n",f->position_ix[free_lc_ix]);
  fprintf(list,"  buf_hash_entries=%d, hash_entries_per_buf=%d\n",
    f->buf_hash_entries,(int)hash_entries_per_buf);
  print_segments(list,f);
  print_files(list,f);
  print_levels(list,f);
  print_freespace_summary(list,f);
  fprintf(list,"Buffers_allocated=%d, in use=%d, age list (young to old) is (x=user l=free_lc r=free_rec  *=modified)",
    f->buffers_allocated,f->buffers_in_use);
  print_buffer_MRU_chain(list,f);
}

void print_fcb(FILE *list, struct fcb *f)
{int i;
  print_fib(list,f);
  fprintf(list,"**buffer directory\n");
  fprintf(list,"       lock?   mod?   next   type\n");
  for (i=0; i<f->buffers_in_use; i++) {
    fprintf(list,"%5d   %3d  ",i,f->buffer[i].lock_cnt);
    if ( f->buffer[i].modified    ) fprintf(list,"  true");
      else fprintf(list," false");
    fprintf(list," %6d ",f->buffer[i].hash_next);
    if ( f->buffer[i].b.ix.index_type==user_ix ) fprintf(list," user ");
    else if ( f->buffer[i].b.ix.index_type==free_rec_ix ) fprintf(list," frec ");
    else if ( f->buffer[i].b.ix.index_type==free_lc_ix ) fprintf(list," f_lc ");
    else fprintf(list," undef");
    fprintf(list,", level=%d ",f->buffer[i].b.ix.level);
    print_leveln_pntr(list,"",&(f->buffer[i].contents));
    fprintf(list,"\n");
  }
  for (i=0; i<f->buffers_in_use; i++) {
    fprintf(list,"**buffer %d ",i);
    print_leveln_pntr(list,"contains index block ",&(f->buffer[i].contents));
    fprintf(list,"\n");
    print_index_block1(list,f,i);
  }
}
