/* Copyright 1984,1985,1986,1988,1989,1990,2003 by Howard Turtle      */
#define min_fcb_lc 15248
#define buffer_lc 2076

enum file_access {random_acc,seq};

int keyrec_lc (void *pointer);
int get_ptr   (char *fcb, char *key, void *pointer);
int put_ptr   (char *fcb, char *key, void *pointer);
int delete_ptr(char *fcb, char *key);
int next_ptr  (char *fcb, char *key, void *pointer);
int prev_ptr  (char *fcb, char *key, void *pointer);
int get_rec   (char *fcb, char *key, void *rec, int *lc, int max_lc);
int put_rec   (char *fcb, char *key, void *rec, int lc);
int delete_rec(char *fcb, char *key);
int next_rec  (char *fcb, char *key, void *rec, int *lc, int max_lc);
int prev_rec  (char *fcb, char *key, void *rec, int *lc, int max_lc);
int copy_rec  (char *fcb, void *pointer, void *rec, int *lc, int max_lc);
int open_key  (char *fcb, char *id, int fcb_lc, int mode);
int create_key(char *fcb, char *id, int fcb_lc, int mode);
int close_key (char *fcb);
