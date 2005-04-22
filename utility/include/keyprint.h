#if !defined(KEYPRINT_H)
  #define KEYPRINT_H

void get_page(struct fcb *f, struct leveln_pntr blk, block_type_t *buf);
void print_key_struct(FILE *list, struct key *k, char caption[]);
void print_freespace(FILE *list, struct fcb *f);
int  print_hash_chain(FILE *list,struct fcb *f, int ix);
void print_buffer_summary(FILE *list, struct fcb *f);
void print_index_block(FILE *list, struct ix_block *b);
void print_fib(FILE *list, struct fcb *f);
void print_fcb(FILE *list, struct fcb *f);

#endif
