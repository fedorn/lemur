/*                                                               */
/* Copyright 1984,1985,1986,1988,1989,1990,2003 by Howard Turtle      */
/*                                                               */

#include <stdio.h>
#include "ht.h"
#include "keydef.h"

void unpack_ptr();
void mvc();

boolean null_pntr(struct leveln_pntr p)
{
  if (p.segment==-1 && p.block==0) return(true);
  else return(false);
}

void get_page(struct fcb *f, struct leveln_pntr blk, struct block buf)
{int i; boolean found; struct leveln_pntr p;

  found = false;
  for (i=0; i<f->buffers_in_use; i++) {
    p = f->buffer[i].contents;
    if ( p.segment==blk.segment && p.block==blk.block ) { buf = f->buffer[i].b; found = true; }
  }
  if ( ! found ) read_page(f,blk,buf);
}

/** fcb print procedures **/

int print_hash_chain(FILE *list,struct fcb *f, int ix)
{int next,cnt=0;

  next = f->buf_hash_table[ix];
  while ( next>=0 ) {
    if ( f->buffer[next].b.level==f->primary_level ) fprintf(list,"**");
    fprintf(list," %d(%d/%d)(%d)",next,f->buffer[next].contents.segment,
      f->buffer[next].contents.block,f->buffer[next].b.level);
    if ( f->buffer[next].b.level==f->primary_level ) fprintf(list,"**");
    cnt++;
    next = f->buffer[next].hash_next;
  }
  fprintf(list,"\n");
  return(cnt); 
}


void print_buffer_MRU_chain(FILE *list,struct fcb *f)
{int ix,cnt=0;

  ix = f->youngest_buffer;
  while ( ix>=0 ) {
    if ( (cnt%10)==0) fprintf(list,"\n    ");
    cnt++;
    fprintf(list," %4d(%2d)",ix,f->buffer[ix].b.level);
    ix = f->buffer[ix].older;
  }
  fprintf(list,"\n");
}

void print_buffer_summary(FILE *list, struct fcb *f)
{int i=0,j,empties=0,entries=0,min,max=0,allocated;

  fprintf(list,"Buffer summary, buffers_allocated=%d, buffers_in_use=%d, hash table size=%d\n",
    f->buffers_allocated,f->buffers_in_use,f->buf_hash_entries);
  fprintf(list,"  MRU chain is ( bufix(level) )");
  print_buffer_MRU_chain(list,f);
  fprintf(list,"  Hash table\n");
  allocated = f->buf_hash_entries;
  while ( i<allocated ) {
    min = i;
    while ( i<allocated && f->buf_hash_table[i]<0 ) {
      i++; empties++; 
    }
    if ( i==min+1 )  fprintf(list,"hash chain  from %d is empty\n",min);
    else if ( i>min) fprintf(list,"hash chains from %d to %d are empty\n",min,i-1);
    if ( i<allocated ) {
      fprintf(list,"hash chain  from %d ",i);
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
    

void print_segments(FILE *list, struct fcb *f)
{int i;

  fprintf(list,"Segment directory, segment_cnt=%d, max_segments=%d, max_segment_lc=%ld\n",
    f->segment_cnt,max_segments,max_segment_lc);
  if ( f->segment_cnt>0 ) {
    fprintf(list,
      "    segment    length  open on    segment    length  open on    segment    length  open on\n");
    for (i=0; i<f->segment_cnt; i++) {
      fprintf(list,"     %6d %10d",i,f->segment_length[i]);
      if ( f->segment_ix[i]>=max_files ) fprintf(list,"  closed");
      else fprintf(list,"   %3d  ",f->segment_ix[i]);
      if ( i%3==2 ) fprintf(list,"\n");
    }
    fprintf(list,"\n");
  }
  else fprintf(list,"  **no segments open\n");
}

void print_files(FILE *list, struct fcb *f)
{int i,j;

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

void print_block(FILE *list, struct block *b)
{int i,j,lc,load; struct level0_pntr ptr; struct leveln_pntr pn; char t[max_key_lc];

  fprintf(list," keys=%d",b->keys_in_block);
  lc = b->chars_in_use + (b->keys_in_block * key_ptr_lc);
  load = (lc*100) / (keyspace_lc);
  fprintf(list,", chars=%d(%d) %d%% loaded",b->chars_in_use,lc,load);
  fprintf(list,", level=%d",b->level);
  fprintf(list,", next=%d,%ld, prev=%d,%ld\n",b->next.segment,
    b->next.block,b->prev.segment,b->prev.block);
  for (i=0; i<b->keys_in_block; i++) {
    fprintf(list,"%4d%3d ",b->keys[i].sc,b->keys[i].lc);
    mvc(b->keys,b->keys[i].sc,t,0,b->keys[i].lc);
    for (j=0; j<b->keys[i].lc; j++) fprintf(list,"%c",t[j]);
    if ( b->level>0 ) {
      unpackn_ptr(b,i,&pn);
      fprintf(list," %d %ld\n",pn.segment,pn.block);
    }
    else {
      unpack0_ptr(b,i,&ptr);
      fprintf(list," %d %ld %ld\n",ptr.segment,ptr.sc,ptr.lc);
    }
  }
}

void print_index(FILE *list, struct fcb *f)
{int i; struct leveln_pntr next; struct block buf;

  for (i=f->primary_level; i>=0; i--) {
    fprintf(list,"**Start of level %d\n",i);
    next = f->first_at_level[i];
    while ( !null_pntr(next) ) {
      fprintf(list,"block number %d/%ld\n",next.segment,next.block);
      get_page(f,next,buf);
      print_block(list,&buf);
      next =buf.next;
    }
  }
}

void print_levels(FILE *list, struct fcb *f)
{int i;

  fprintf(list,"Level pointers\n");
  for (i=0; i<2; i++) fprintf(list,"      lvl    first          MRU       ");
  for (i=0; i<=f->primary_level; i++) {
    if (i % 2==0) fprintf(list,"\n");
    fprintf(list,"       %2d%4d/%-9ld %4d/%-9ld",i,
      f->first_at_level[i].segment,f->first_at_level[i].block,
      f->mru_at_level[i].segment,f->mru_at_level[i].block);
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
  fprintf(list,",    error_code=%d",f->error_code);
  fprintf(list,",  version=%d",f->version);
  fprintf(list,",     marker=%ld",f->marker);
  fprintf(list,", insert mode=");
  if ( f->insert_mode==seq ) fprintf(list,"seq\n");
    else fprintf(list,"random\n");
  fprintf(list,"  primary level=%d,",f->primary_level);
  fprintf(list," current_age=%d, block_lc=%d,",f->current_age,block_lc);
  fprintf(list," position=%d/%ld %d\n",f->position.segment,
    f->position.block,f->position_ix);
  fprintf(list,"  buffers_allocated=%d, buffers_in_use=%d\n",f->buffers_allocated,f->buffers_in_use);
  fprintf(list,"  buf_hash_entries=%d, hash_entries_per_buf=%d\n",f->buf_hash_entries,hash_entries_per_buf);
  print_segments(list,f);
  print_files(list,f);
  print_levels(list,f);
}

void print_fcb(FILE *list, struct fcb *f)
{int i;
  print_fib(list,f);
  fprintf(list,"**buffer directory, buffers_allocated=%d, age list (young to old) is",f->buffers_allocated);
  print_buffer_MRU_chain(list,f);
  fprintf(list,"         contnt      level lock?  modified?    next\n");
  for (i=0; i<f->buffers_in_use; i++) {
    fprintf(list,"%5d %4d/%10ld %5d",i,f->buffer[i].contents.segment,f->buffer[i].contents.block,f->buffer[i].b.level);
    if (f->buffer[i].locked) fprintf(list,"  true  ");
      else fprintf(list,"  false ");
    if ( f->buffer[i].modified    ) fprintf(list,"   true");
      else fprintf(list,"  false");
    fprintf(list," %6d\n",f->buffer[i].hash_next);
  }
  for (i=0; i<f->buffers_in_use; i++) {
    fprintf(list,"**buffer %d\n",i);
    print_block(list,&(f->buffer[i].b) );
  }
}
