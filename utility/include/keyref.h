
#define min_fcb_lc 44620
#define buffer_lc 4120

int keyrec_lc (void *pointer);
int set_bof   (char *fcb);
int set_eof   (char *fcb);
int get_ptr   (char *fcb, char *key, int key_lc, void *pointer);
int put_ptr   (char *fcb, char *key, int key_lc, void *pointer);
int delete_ptr(char *fcb, char *key, int key_lc);
int next_ptr  (char *fcb, char *key, int *key_lc, int max_key_lc, void *pointer);
int prev_ptr  (char *fcb, char *key, int *key_lc, int max_key_lc, void *pointer);
int get_rec   (char *fcb, char *key, int key_lc, void *rec, int *lc, int max_rec_lc);
int put_rec   (char *fcb, char *key, int key_lc, void *rec, int lc);
int delete_rec(char *fcb, char *key, int key_lc);
int next_rec  (char *fcb, char *key, int *key_lc, int max_key_lc,
                          void *rec, int *rec_lc, int max_rec_lc);
int prev_rec  (char *fcb, char *key, int *key_lc, int max_key_lc,
                          void *rec, int *rec_lc, int max_rec_lc);
int copy_rec  (char *fcb, void *pointer, void *rec, int *lc, int max_lc);
int open_key  (char *fcb, char *id, int fcb_lc, int read_only);
int create_key(char *fcb, char *id, int fcb_lc);
int close_key (char *fcb);
