
/*                                                               */
/* Copyright 1984,1985,1986,1988,1989,1990,2003 by Howard Turtle      */
/*                                                               */

#define max_long 2147483647
#define keyf 32472                   /* marker for fcb */
#define current_version  4           /* version of keyed file software */
#define max_key_lc 256                /* maximum key length */
#define key_ptrs_per_block 506       /* number of key_ptrs in index block */
#define random_split_lc 880          /* chars to be freed on random split */
#define level_zero 0                 /* level of index leaves */
#define level_one 1                  /* level immediately above leaves */
#define long_lc   sizeof(long)       /* lc of a long int */
#define min_buffer_cnt 3             /* default number of buffers allocated */
#define max_buffer_cnt 1024          /* max buffers allowed */
#define buf_hash_load_factor 3       /* hash table is>=this times buffers alloc,*/
#define max_level 32                 /* number of index block levels */
#define max_char '\377'
#define fib_lc min_fcb_lc-(min_buffer_cnt*buffer_lc) /* length of fib */
#define max_segment_lc max_long      /* max length of a file segment */
#define max_segments 1024            /* max number of file segments */
#define max_files 10                 /* max number of open files */
/*POSIX VALUES HERE?!? was 82, 40*/
#define max_filename_lc 245 /* max length of a file name -10 for init_filename HACK! */
#define max_extension_lc 40 /* max length of file name extension unused?!? */
enum file_access {random,seq};
enum comparison {less,equal,greater};

struct key {
  unsigned char
    text[max_key_lc];
  short
    lc;
};

struct leveln_pntr{
  int
    segment;
  long
    block;
};
#define leveln_lc sizeof(struct leveln_pntr)

struct level0_pntr {
  int
    segment;
  long
    sc,
    lc;
};
#define level0_lc sizeof(struct level0_pntr)

struct key_ptr_t {
  short
    sc,                         /* start character of key */
    lc;                         /* lc of key, does not include pointer */
};
#define key_ptr_lc sizeof(struct key_ptr_t)
#define keyspace_lc (key_ptr_lc*key_ptrs_per_block)

struct block {                  /* block is the disk resident image of */
  short                         /*   an index block */
    keys_in_block,
    chars_in_use,               /* chars in key/pointer pool, does not */
                                /*   include length of key_ptr_t entries */
    level;
  struct leveln_pntr
    next,prev;
  struct key_ptr_t              /* key_ptrs are inserted from 0, keys and */
    keys[key_ptrs_per_block];   /*  file pointers overlay the top end */
};
#define block_lc sizeof(struct block)

/* Buffer handling.  Buffers contain the disk image of an index block */
/*   together with additional information.  A hashing technique is    */
/*   used to find a buffer that holds a given block.  A hash table is */
/*   allocated as the last buffers in the fcb of roughly three times  */
/*   the number of buffers allocated.  buf_hash_table[k] contains the */
/*   index of the first buffer containing a block whose hash value is */
/*   k.  If there are multiple buffers containing blocks with hash    */
/*   value k then they are linked using hash_next.                    */

struct buffer_type {            /* buffer is the memory resident image of */
  boolean                       /*   an index block */
    locked,
    modified;
  int
    older,                      /* index to prev element in LRU list */
    younger,                    /* index to next element in LRU list */
    hash_next;
  struct leveln_pntr
    contents;                   /* block in buffer, nulln_ptr if empty */
  struct block
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
    primary_level;              /* level of primary index block */
  boolean
    file_ok;
  long
    marker;
  struct leveln_pntr
    first_at_level[max_level];  /* block containing lowest key at level */
  long
    segment_length[max_segments];/* length in bytes of each segment     */
  char
    file_name[max_filename_lc],
    file_extension[max_extension_lc];

    /* start of temporary information */

  enum file_access
    insert_mode;                /* random or sequential inserts */
  int
    open_file_cnt,              /* number of files actually open */
    open_segment[max_files],    /* segment to which each file is open */
    file_age[max_files],        /* age of each open file  */
    oldest_buffer,              /* first buffer in LRU buffer list */
    youngest_buffer;            /* last buffer in LRU buffer list */
  FILE
    *open_file[max_files];      /* pointers to  open files */

  int
    segment_ix[max_segments],   /* index into open_file[] if segment open */
    position_ix,                /* posn. in level0 blk of last retrieval */
    current_age,                /* age of file pool (0..maxint)*/
    buffers_allocated,          /* number of buffers actually allocated */
    buffers_in_use,             /* buffers actually used */
    *buf_hash_table,                /* pointer to base of buffer hash table */
    buf_hash_entries;           /* size of buf_hash_table              */
  struct leveln_pntr
    mru_at_level[max_level],    /* most recently used block at each level*/
    position;                   /* level0 block of last retrieval */
  struct buffer_type
    buffer[min_buffer_cnt];     /* should be at end of fcb so we can extend */
};
#define min_fcb_lc sizeof(struct fcb)
