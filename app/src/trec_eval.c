#ifdef RCSID
static char rcsid[] = "$Header$";
#endif

/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/

/********************   PROCEDURE DESCRIPTION   ************************
 *0 Take TREC results text file, TREC qrels file, and evaluate
 *1 local.convert.obj.trec_eval
 *2 trec_eval [-q] [-a] trec_rel_file trec_top_file

 *7 Read text tuples from trec_top_file of the form
 *7     030  Q0  ZF08-175-870  0   4238   prise1
 *7     qid iter   docno      rank  sim   run_id
 *7 giving TREC document numbers (a string) retrieved by query qid 
 *7 (an integer) with similarity sim (a float).  The other fields are ignored.
 *7 Input is asssumed to be sorted numerically by qid.
 *7 Sim is assumed to be higher for the docs to be retrieved first.
 *7 Relevance for each docno to qid is determined from text_qrels_file, which
 *7 consists of text tuples of the form
 *7    qid  iter  docno  rel
 *7 giving TREC document numbers (a string) and their relevance to query qid
 *7 (an integer). Tuples are asssumed to be sorted numerically by qid.
 *7 The text tuples with relevence judgements are converted to TR_VEC form
 *7 and then submitted to the evaluation routines.
 *7
 *7 -q: In addition to summary evaluation, give evaluation for each query
 *7 -a: Print all evaluation measures calculated, instead of just the
 *7      official measures for TREC 2.

 *8 Procedure is to read all the docs retrieved for a query, and all the
 *8 relevant docs for that query,
 *8 sort and rank the retrieved docs by sim/docno, 
 *8 and look up docno in the relevant docs to determine relevance.
 *8 The qid,did,rank,sim,rel fields of of TR_VEC are filled in; 
 *8 action,iter fields are set to 0.
 *8 Queries for which there are no relevant docs are ignored completely.

***********************************************************************/

#include "common.h"
#include "sysfunc.h"
#include "smart_error.h"
#include "tr_vec.h"
#include "textline.h"
#include "trec_eval.h"
#include "buf.h"

#define TR_INCR 250

void print_error();
int init_tr_trec_eval(), tr_trec_eval(), close_tr_trec_eval();
void print_trec_eval_list();
int text_textline();

static int init_get_trec_top(), get_trec_top();
static int init_get_trec_qrels(), get_trec_qrels();
static int comp_tr_tup_did(), comp_tr_docno(), comp_qrels_docno(),
    comp_sim_docno();
static int form_and_eval_trvec();

typedef struct {
    char *docno;
    float sim;
    long rank;
} TEXT_TR;

typedef struct {
    char *docno;
} TEXT_QRELS;

typedef struct {
    long qid;
    long num_text_tr;
    long max_num_text_tr;
    TEXT_TR *text_tr;
} TREC_TOP;

typedef struct {
    long qid;
    long num_text_qrels;
    long max_num_text_qrels;
    TEXT_QRELS *text_qrels;
} TREC_QRELS;


static char *usage = "Usage: trec_eval [-q] [-a] trec_rel_file trec_top_file\n";


/* WARNING: Global variables! */
long query_flag = 0;              /* If set, evaluation output will be
                                     printed for each query, in addition
                                     to summary at end. */
long all_flag = 0;                /* If set, all evaluation measures will
                                     be printed instead of just the
                                     final TREC 2 measures. */

void main (argc, argv)
int argc;
char *argv[];
{
    char *trec_rel_file, *trec_top_file;
    TREC_TOP trec_top;
    TREC_QRELS trec_qrels;
    TREC_EVAL eval_accum;

    if (argc < 3) {
        (void) fputs (usage,stderr);
        exit (1);
    }

    /* Should use getopts, but some people may not have it. */
    while (argc > 3 && argv[1][0] == '-') {
        if (argv[1][1] == 'q')
            query_flag++;
        else if (argv[1][1] == 'a')
            all_flag++;
        else {
            (void) fputs (usage,stderr);
            exit (1);
        }
        argc--; argv++;
    }

    if (argc != 3) {
        (void) fputs (usage,stderr);
        exit (1);
    }

    trec_rel_file = argv[1];
    trec_top_file = argv[2];

    /* Open text tr input file */
    /* Open text qrels input file */
    /* Initialize evaluation computation */
    if (UNDEF == init_get_trec_top (trec_top_file) ||
        UNDEF == init_get_trec_qrels (trec_rel_file) ||
        UNDEF == init_tr_trec_eval (&eval_accum)) {
        print_error ("trec_eval: initialization", "Quit");
        exit (2);
    }

    /* Get first trec_top and trec_qrels */
    trec_top.max_num_text_tr = 0;
    trec_qrels.max_num_text_qrels = 0;
    if (1 != get_trec_top (&trec_top) ||
        1 != get_trec_qrels (&trec_qrels)) {
        print_error ("trec_eval: Empty file", "Quit");
        exit (2);
    }

    while (trec_top.qid != UNDEF && trec_qrels.qid != UNDEF) {
        if (trec_top.qid == trec_qrels.qid) {
            if (UNDEF == form_and_eval_trvec (&eval_accum,
                                              &trec_top,
                                              &trec_qrels)) {
                print_error ("trec_eval: evaluation", "Quit");
                exit (3);
            }
            if (UNDEF == get_trec_top (&trec_top) ||
                UNDEF == get_trec_qrels (&trec_qrels)) {
                print_error ("trec_eval: input format", "Quit");
                exit (3);
            }
        }
        else if (trec_top.qid < trec_qrels.qid) {
            if (UNDEF == get_trec_top (&trec_top)) {
                print_error ("trec_eval: input format", "Quit");
                exit (3);
            }
        }
        else {
            if (UNDEF == get_trec_qrels (&trec_qrels)) {
                print_error ("trec_eval: input format", "Quit");
                exit (3);
            }
        }
    }

    /* Finish evaluation computation */
    if (UNDEF == close_tr_trec_eval (&eval_accum)) {
        print_error ("trec_eval: evaluation end", "Quit");
        exit (3);
    }

    /* Output evaluation */
    print_trec_eval_list (&eval_accum, 1, (SM_BUF *) NULL);

    exit (1);
}


/* Space reserved for output TR_TUP tuples */
static TR_TUP *start_tr_tup;
static long max_tr_tup;


static int
form_and_eval_trvec (eval_accum, trec_top, trec_qrels)
TREC_EVAL *eval_accum;
TREC_TOP *trec_top;
TREC_QRELS *trec_qrels;
{
    TR_TUP *tr_tup;
    TEXT_QRELS *qrels_ptr, *end_qrels;
    long i;

    TR_VEC tr_vec;

    /* Reserve space for output tr_tups, if needed */
    if (trec_top->num_text_tr > max_tr_tup) {
        if (max_tr_tup > 0) 
            (void) free ((char *) start_tr_tup);
        max_tr_tup += trec_top->num_text_tr;
        if (NULL == (start_tr_tup = (TR_TUP *)
                     malloc ((unsigned) max_tr_tup * sizeof (TR_TUP))))
            return (UNDEF);
    }

    /* Sort trec_top by sim, breaking ties lexicographically using docno */
    qsort ((char *) trec_top->text_tr,
           (int) trec_top->num_text_tr,
           sizeof (TEXT_TR),
           comp_sim_docno);

    /* Add ranks to trec_top (starting at 1) */
    for (i = 0; i < trec_top->num_text_tr; i++) {
        trec_top->text_tr[i].rank = i+1;
    }

    /* Sort trec_top lexicographically */
    qsort ((char *) trec_top->text_tr,
           (int) trec_top->num_text_tr,
           sizeof (TEXT_TR),
           comp_tr_docno);

    /* Sort trec_qrels lexicographically */
    qsort ((char *) trec_qrels->text_qrels,
           (int) trec_qrels->num_text_qrels,
           sizeof (TEXT_QRELS),
           comp_qrels_docno);

    /* Go through trec_top, trec_qrels in parallel to determine which
       docno's are in both (ie, which trec_top are relevant).  Once relevance
       is known, convert trec_top tuple into TR_TUP. */
    tr_tup = start_tr_tup;
    qrels_ptr = trec_qrels->text_qrels;
    end_qrels = &trec_qrels->text_qrels[trec_qrels->num_text_qrels];
    for (i = 0; i < trec_top->num_text_tr; i++) {
        while (qrels_ptr < end_qrels &&
               strcmp (qrels_ptr->docno, trec_top->text_tr[i].docno) < 0)
            qrels_ptr++;
        if (qrels_ptr >= end_qrels ||
            strcmp (qrels_ptr->docno, trec_top->text_tr[i].docno) > 0)
            /* Doc is non-relevant */
            tr_tup->rel = 0;
        else 
            /* Doc is relevant */
            tr_tup->rel = 1;
        
        tr_tup->did = i;
        tr_tup->rank = trec_top->text_tr[i].rank;
        tr_tup->sim = trec_top->text_tr[i].sim;
        tr_tup->action = 0;
        tr_tup->iter = 0;
        tr_tup++;
    }
    /* Form and output the full TR_VEC object for this qid */
    /* Sort trec_top tr_tups by did */
    qsort ((char *) start_tr_tup,
           (int) trec_top->num_text_tr,
           sizeof (TR_TUP),
           comp_tr_tup_did);
    tr_vec.qid = trec_top->qid;
    tr_vec.num_tr = trec_top->num_text_tr;
    tr_vec.tr = start_tr_tup;

    /* Accumulate this tr_vec's eval figures in eval_accum */
    if (UNDEF == tr_trec_eval (&tr_vec, eval_accum,trec_qrels->num_text_qrels))
        return (UNDEF);

    return (1);
}

static int 
comp_sim_docno (ptr1, ptr2)
TEXT_TR *ptr1;
TEXT_TR *ptr2;
{
    if (ptr1->sim > ptr2->sim)
        return (-1);
    if (ptr1->sim < ptr2->sim)
        return (1);
    return (strcmp (ptr2->docno, ptr1->docno));
}

static int 
comp_tr_docno (ptr1, ptr2)
TEXT_TR *ptr1;
TEXT_TR *ptr2;
{
    return (strcmp (ptr1->docno, ptr2->docno));
}

static int 
comp_qrels_docno (ptr1, ptr2)
TEXT_QRELS *ptr1;
TEXT_QRELS *ptr2;
{
    return (strcmp (ptr1->docno, ptr2->docno));
}

static int 
comp_tr_tup_did (ptr1, ptr2)
TR_TUP *ptr1;
TR_TUP *ptr2;
{
    return (ptr1->did - ptr2->did);
}

static char *trec_top_buf;           /* Buffer for entire trec_top file */
static char *trec_top_ptr;           /* Current position in trec_top_buf */

/* Current trec_top input line, broken up into tokens */
static SM_TEXTLINE trec_top_line;
static char *trec_top_array[6];

static int 
init_get_trec_top (filename)
char *filename;
{
    int fd;
    int size;

    /* Read entire file into memory */
    if (-1 == (fd = open (filename, 0)) ||
        -1 == (size = lseek (fd, 0L, 2)) ||
        NULL == (trec_top_buf = malloc ((unsigned) size+1)) ||
        -1 == lseek (fd, 0L, 0) ||
        size != read (fd, trec_top_buf, size) ||
        -1 == close (fd))
        return (UNDEF);
    trec_top_buf[size] = '\0';

    trec_top_line.max_num_tokens = 6;
    trec_top_line.tokens = trec_top_array;

    trec_top_ptr = trec_top_buf;
    while (*trec_top_ptr && *trec_top_ptr != '\n')
        trec_top_ptr++;
    if (! *trec_top_ptr) {
        /* No lines in tr_file */
        set_error (SM_ILLPA_ERR, "Illegal tr input line", "trec_top");
        return (UNDEF);
    }
    /* Replace newline by \0, and break into tokens */
    *trec_top_ptr++ = '\0';
    if (UNDEF == text_textline (trec_top_buf, &trec_top_line) ||
        trec_top_line.num_tokens != 6) {
        set_error (SM_ILLPA_ERR, "Illegal tr input line", "trec_top");
        return (UNDEF);
    }
    return (1);
}


static char *trec_qrels_buf;           /* Buffer for entire trec_qrels file */
static char *trec_qrels_ptr;           /* Next line in trec_qrels_buf */

/* Current trec_qrels input line, broken up into tokens */
static SM_TEXTLINE trec_qrels_line;
static char *trec_qrels_array[6];

static int 
init_get_trec_qrels (filename)
char *filename;
{
    int fd;
    int size = 0;

    /* Read entire file into memory */
    if (-1 == (fd = open (filename, 0)) ||
        -1 == (size = lseek (fd, 0L, 2)) ||
        NULL == (trec_qrels_buf = malloc ((unsigned) size+1)) ||
        -1 == lseek (fd, 0L, 0) ||
        size != read (fd, trec_qrels_buf, size) ||
        -1 == close (fd))
        return (UNDEF);
    trec_qrels_buf[size] = '\0';

    trec_qrels_line.max_num_tokens = 4;
    trec_qrels_line.tokens = trec_qrels_array;

    trec_qrels_ptr = trec_qrels_buf;
    while (*trec_qrels_ptr && *trec_qrels_ptr != '\n')
        trec_qrels_ptr++;
    if (! *trec_qrels_ptr) {
        /* No lines in tr_file */
        set_error (SM_ILLPA_ERR, "Illegal qrels input line", "trec_eval");
        return (UNDEF);
    }
    /* Replace newline by \0, and break into tokens */
    *trec_qrels_ptr++ = '\0';
    if (UNDEF == text_textline (trec_qrels_buf, &trec_qrels_line) ||
        trec_qrels_line.num_tokens != 4) {
        set_error (SM_ILLPA_ERR, "Illegal qrels input line", "trec_eval");
        return (UNDEF);
    }
    return (1);
}


/* Get entire trec_top vector for next query */
static int
get_trec_top (trec_top)
TREC_TOP *trec_top;
{
    long qid;
    char *start_trec_top_ptr;     /* Start of current line */

    qid = trec_top->qid = atol (trec_top_line.tokens[0]);
    if (qid == -1)
        return (0);

    trec_top->num_text_tr = 0;
    while (qid == trec_top->qid) {
        /* Make sure enough space is reserved for next tuple */
        if (trec_top->num_text_tr >= trec_top->max_num_text_tr) {
            if (trec_top->max_num_text_tr == 0) {
                if (NULL == (trec_top->text_tr = (TEXT_TR *) 
                             malloc ((unsigned) TR_INCR * sizeof (TEXT_TR))))
                    return (UNDEF);
            }
            else if (NULL == (trec_top->text_tr = (TEXT_TR *)
                           realloc ((char *) trec_top->text_tr,
                                    (unsigned) (trec_top->max_num_text_tr +
                                                TR_INCR) * sizeof (TEXT_TR))))
                return (UNDEF);
            trec_top->max_num_text_tr += TR_INCR;
        }
        /* Add current textline tuple to trec_top */
        trec_top->text_tr[trec_top->num_text_tr].docno = trec_top_line.tokens[2];
        trec_top->text_tr[trec_top->num_text_tr].sim = atof (trec_top_line.tokens[4]);
        trec_top->num_text_tr++;

        /* Get next textline tuple */
        trec_top_line.num_tokens = 0;
        while (trec_top_line.num_tokens == 0) {
            start_trec_top_ptr = trec_top_ptr;
            start_trec_top_ptr = trec_top_ptr;
            while (*trec_top_ptr && *trec_top_ptr != '\n')
                trec_top_ptr++;
            if (! *trec_top_ptr) {
                /* No more lines in tr_file */
                trec_top_line.tokens[0] = "-1";
                break;
            }
            /* Replace newline by \0, and break into tokens */
            *trec_top_ptr++ = '\0';
            if (UNDEF == text_textline (start_trec_top_ptr, &trec_top_line) ||
                (trec_top_line.num_tokens != 6 &&
                 trec_top_line.num_tokens != 0)) {
                set_error (SM_ILLPA_ERR, "Illegal tr input line", "trec_top");
                return (UNDEF);
            }
        }
        qid = atol (trec_top_line.tokens[0]);
    }

    return (1);
}


/* Get entire trec_qrels vector for next query */
static int
get_trec_qrels (trec_qrels)
TREC_QRELS *trec_qrels;
{
    long qid;
    char *start_trec_qrels_ptr;     /* Start of current line */

    qid = trec_qrels->qid = atol (trec_qrels_line.tokens[0]);
    if (qid == -1)
        return (0);
    trec_qrels->num_text_qrels = 0;
    while (qid == trec_qrels->qid) {
        /* Make sure enough space is reserved for next tuple */
        if (trec_qrels->num_text_qrels >= trec_qrels->max_num_text_qrels) {
            if (trec_qrels->max_num_text_qrels == 0) {
                if (NULL == (trec_qrels->text_qrels = (TEXT_QRELS *) 
                            malloc ((unsigned) TR_INCR * sizeof (TEXT_QRELS))))
                    return (UNDEF);
            }
            else if (NULL == (trec_qrels->text_qrels = (TEXT_QRELS *)
                        realloc ((char *) trec_qrels->text_qrels,
                                 (unsigned) (trec_qrels->max_num_text_qrels +
                                             TR_INCR) * sizeof (TEXT_QRELS))))
                return (UNDEF);
            trec_qrels->max_num_text_qrels += TR_INCR;
        }
        /* Add current textline tuple to trec_qrels as long as relevant */
        trec_qrels->text_qrels[trec_qrels->num_text_qrels].docno =
            trec_qrels_line.tokens[2];
        if (atol (trec_qrels_line.tokens[3]))
            trec_qrels->num_text_qrels++;

        /* Get next textline tuple */
        trec_qrels_line.num_tokens = 0;
        while (trec_qrels_line.num_tokens == 0) {
            start_trec_qrels_ptr = trec_qrels_ptr;
            while (*trec_qrels_ptr && *trec_qrels_ptr != '\n')
                trec_qrels_ptr++;
            if (! *trec_qrels_ptr) {
                /* No more lines in tr_file */
                trec_qrels_line.tokens[0] = "-1";
                break;
            }
            /* Replace newline by \0, and break into tokens */
            *trec_qrels_ptr++ = '\0';
            if (UNDEF == text_textline (start_trec_qrels_ptr,
                                        &trec_qrels_line) ||
                (trec_qrels_line.num_tokens != 4 &&
                 trec_qrels_line.num_tokens != 0)) {
                set_error (SM_ILLPA_ERR, "Illegal qrels input line", "trec_qrels");
                return (UNDEF);
            }
        }
        qid = atol (trec_qrels_line.tokens[0]);
    }

    return (1);
}

