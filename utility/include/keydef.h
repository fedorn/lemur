#if !defined(KEYDEF_H)
#define KEYDEF_H

/*                                                               */
/* Copyright 1984,1985,1986,1988,1989,1990,2003,2004,2005 by Howard Turtle      */
/*                                                               */

#define max_long 2147483647
#define keyf 32472                   /* marker for fcb */
#define current_version  5           /* version of keyed file software */
#define maxkey_lc 512                /* maximum key length */
#define max_prefix_lc 255
#define key_ptrs_per_block 1018 /*506*/       /* number of key_ptrs in index block */
#define level_zero 0                 /* level of index leaves */
#define level_one 1                  /* level immediately above leaves */
#define long_lc   sizeof(long)       /* lc of a long int */
#define min_disk_rec_lc 5            /* records this lc or longer are on disk */
#define min_buffer_cnt 8             /* default number of buffers allocated */
#define max_buffer_cnt 16384         /* max buffers allowed */
#define buf_hash_load_factor 3       /* hash table is>=this times buffers alloc,*/
#define max_level 32                 /* number of index block levels */
/*#define max_segment_lc 4194304 */      /* 1024 blocks of 4096 each */
#define max_segment_lc max_long      /* max length of a file segment */
#define max_segments 1024            /* max number of file segments */
#define max_files 10                 /* max number of open files */
#define max_filename_lc 128          /* max length of a file name */
#define max_extension_lc 40          /* max length of file name extension */
#define rec_allocation_unit 8        /* user rec allocation unit */
#define block_allocation_unit 16     /* # blocks to allocate at a time */
#define user_ix 0
#define free_rec_ix 1
#define free_lc_ix 2
#define max_index 3

enum comparison {less,equal,greater};

struct key {
  unsigned char
    text[maxkey_lc];
  short
    lc;
};

struct leveln_pntr{
  int
    segment;
  unsigned long
    block;
};
#define leveln_lc sizeof(struct leveln_pntr)

struct level0_pntr {
  int
    segment;
  unsigned long
    sc,
    lc;
};
#define level0_lc sizeof(struct level0_pntr)

typedef struct level0_pntr keyfile_pointer; /* external name for Chiliad use */

struct key_ptr_t {
  short
    sc,                         /* start character of key */
    lc;                         /* lc of key, does not include pointer */
};
#define key_ptr_lc sizeof(struct key_ptr_t)
#define keyspace_lc (key_ptr_lc*key_ptrs_per_block)

struct ix_block {                  /* block is the disk resident image of */
  short                         /*   an index block */
    keys_in_block,
    chars_in_use;               /* chars in key/pointer pool, does not */
                                /*   include length of key_ptr_t entries */
  unsigned char
    index_type,                 /* user, free_lc, or free_rec */
    prefix_lc,                  /* lc of prefix removed from all keys in block */
    free_block_cnt,             /* number of free blocks if on free_block_chain */
    level;                      /* level of block */
  struct leveln_pntr
    next,prev;
  struct key_ptr_t              /* key_ptrs are inserted from 0, keys and */
    keys[key_ptrs_per_block];   /*  file pointers overlay the top end */
};
#define ix_block_lc sizeof(struct ix_block)

/* Free space management.  Available space is recorded in two separate */
/*   indexes.  The first (free_rec_ix) records each space in address   */
/*   order using a binary key of segment/sc and lc as the record,  The */
/*   second (free_lc_ix) records each space in length order using a    */
/*   key of lc/segment/sc.  To allocate a record the lc list is        */
/*   searched with a key of lc/0/0 then next_rec is used to find a     */
/*   space of lc or longer (if it exists).  To deallocate, the rec     */
/*   list searched and any contiguous entries are combined.            */

typedef union ix_or_freespace_block {
  struct ix_block        ix;
  /*  struct freespace_block free;*/
} block_type_t;
#define block_lc sizeof(block_type_t)

typedef union level0orn_pntr {
  struct level0_pntr     p0;
  struct leveln_pntr     pn;
} levelx_pntr;

/*typedef union int_or_byte {
  int           as_int;
  unsigned char as_byte[sizeof(int)];
} int_alias;

typedef union short_or_byte {
  short         as_short;
  unsigned char as_byte[sizeof(short)];
} short_alias;
*/

/* Buffer handling.  Buffers contain the disk image of an index or    */
/*   freespace block */
/*   together with additional information.  A hashing technique is    */
/*   used to find a buffer that holds a given block.  A hash table is */
/*   allocated as the last buffers in the fcb of roughly three times  */
/*   the number of buffers allocated.  buf_hash_table[k] contains the */
/*   index of the first buffer containing a block whose hash value is */
/*   k.  If there are multiple buffers containing blocks with hash    */
/*   value k then they are linked using hash_next.                    */

struct buffer_type {            /* buffer is the memory resident image of */
                                /* a disk block */
  unsigned char
    lock_cnt,
    modified,
    notused;
  int
    older,                      /* index to prev element in LRU list */
    younger,                    /* index to next element in LRU list */
    hash_next;
  struct leveln_pntr
    contents;                   /* block in buffer, nulln_ptr if empty */
  block_type_t
    b;
};
#define buffer_lc sizeof(struct buffer_type)
#define hash_entries_per_buf (buffer_lc / sizeof(int))

/* Segment handling.  A keyed file consist of one or more component files */
/*    called segments.  Segment 0 contains the file information block and */
/*    is alway present.  Additional segments are created as needed with   */
/*    a suffix of "$n" appended to the base file name where n is the      */
/*    segment number.  The file information block contains a segment_cnt  */
/*    and a list of each segment_length.  After open the fcb contains a   */
/*    list of the file number on which each segment is open (max_files    */
/*    implies not open) in segment_ix                                     */

/* File handling.  Up to max_files files may be open at one time.         */
/*   open_file_cnt is the number of files actually open, open_file[] is   */
/*   a list of file_indexes in use, file_age[] is the age of each open    */
/*   file, open_segment[] is the segment to which the file is open        */

struct fcb {

  int
    error_code,
    version,                    /* version of keyed file manager */
    segment_cnt,                /* number of segments in use     */
    primary_level[max_index],              /* level of primary index block */
    marker;
  boolean
    file_ok;
  struct leveln_pntr
    first_free_block[max_level][max_index],/* points to start of empty block chain */
    first_at_level[max_level][max_index],  /* block containing lowest key at level */
    last_pntr[max_level][max_index];       /* last pointer at each level */
  long
    segment_length[max_segments];/* length in bytes of each segment     */

    /* start of temporary information */

  char
    file_name[max_filename_lc],
    file_extension[max_extension_lc];
  unsigned char
    byte_swapping_required,     /* true means swap bytes on I/O */
    trace,                      /* true means trace execution */
    trace_freespace,            /* true means trace space management */
    read_only;                  /* true means file is read only */
  int
    open_file_cnt,              /* number of files actually open */
    open_segment[max_files],    /* segment to which each file is open */
    file_age[max_files],        /* age of each open file  */
    oldest_buffer,              /* first buffer in LRU buffer list */
    youngest_buffer,            /* last buffer in LRU buffer list */
    block_shift;                /* log2(block_lc) */
  FILE
    *open_file[max_files];      /* pointers to open files */

  int
    segment_ix[max_segments],   /* index into open_file[] if segment open */
    position_ix[max_index],                /* posn. in level0 blk of last retrieval */
    current_age,                /* age of file pool (0..maxint)*/
    buffers_allocated,          /* number of buffers actually allocated */
    buffers_in_use,             /* buffers actually used */
    *buf_hash_table,            /* pointer to base of buffer hash table */
    buf_hash_entries;           /* size of buf_hash_table              */
  struct leveln_pntr
    mru_at_level[max_level][max_index],    /* most recently used block at each level*/
    position[max_index];                   /* level0 block of last retrieval */
  struct buffer_type
    buffer[min_buffer_cnt];     /* should be at end of fcb so we can extend */
};
#define min_fcb_lc sizeof(struct fcb)

#define fib_lc ((int) &((struct fcb *) 0)->file_name)
#define fib_blocks ((fib_lc-1)/block_lc+1)


#endif
