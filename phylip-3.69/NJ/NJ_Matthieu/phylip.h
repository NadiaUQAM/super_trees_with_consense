                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      */
  steptr oldnumsteps;            /* record previous steps                  */
  double sumsteps;               /* bookkeeps sum of steps                 */
  nucarray cumlengths;           /* bookkeeps cummulative minimum lengths  */
  discnucarray disccumlengths;   /* bookkeeps cummulative minimum lengths  */
  nucarray numreconst;           /* bookkeeps number of  reconstructions   */
  discnucarray discnumreconst;   /* bookkeeps number of  reconstructions   */
  vector d, w;                   /* for distance matrix programs           */
  double dist;                   /* dist used in fitch                     */
  bitptr stateone, statezero;    /* discrete char programs                 */
  long maxpos;                   /* maxpos used in Clique                  */
  Char state;                    /* state used in Dnamove, Dolmove & Move  */
  double* underflows;            /* used to record underflow               */
} node;

typedef node **pointarray;


/*** tree structure ***/

typedef struct tree {

  /* An array of pointers to nodes. Each tip node and ring of nodes has a
   * unique index starting from one. The nodep array contains pointers to each
   * one, starting from 0. In the case of internal nodes, the entries in nodep
   * point to the rootward node in the group. Since the trees are otherwise
   * entirely symmetrical, except at the root, this is the only way to resolve
   * parent, child, and sibling relationships.
   *
   * Indices in range [0, spp) point to tips, while indices [spp, nonodes)
   * point to fork nodes
   */
  pointarray nodep;

  /* A pointer to the first node. Typically, root is used when the tree is rooted,
   * and points to an internal node with no back link. */
  node *root;                    
  
  /* start is used when trees are unrooted. It points to an internal node whose
   * back link typically points to the outgroup leaf. */
  node *start;                    

  /* In maximum likelihood programs, the most recent evaluation is stored here */
  double likelihood;

  /* Branch transition matrices for restml */
  transptr trans;                 /* all transition matrices */
  long *freetrans;                /* an array of indexes of free matrices */
  long transindex;                /* index of last valid entry in freetrans[] */
} tree;

typedef void (*initptr)(node **, node **, node *, long, long,
                         long *, long *, initops, pointarray,
                         pointarray, Char *, Char *, FILE *);


#ifndef OLDC
/* function prototypes */
void   scan_eoln(FILE *);
boolean    eoff(FILE *);
boolean    eoln(FILE *);
int    filexists(char *);
const char*  get_command_name (const char *);
void   EOF_error(void);
void   getstryng(char *);
void   openfile(FILE **,const char *,const char *,const char *,const char *,
                char *);
void   cleerhome(void);
void   loopcount(long *, long);
double randum(longer);
void   randumize(longer, long *);
double normrand(longer);
long   readlong(const char *);

void   uppercase(Char *);
void   initseed(long *, long *, longer);
void   initjumble(long *, long *, longer, long *);
void   initoutgroup(long *, long);
void   initthreshold(double *);
void   initcatn(long *);
void   initcategs(long, double *);
void   initprobcat(long, double *, double *);
double logfac (long);
double halfroot(double (*func)(long , double), long, double, double);
double hermite(long, double);
void initlaguerrecat(long, double, double *, double *);
void   root_hermite(long, double *);
void   hermite_weight(long, double *, double *);
void   inithermitcat(long, double, double *, double *);
void   lgr(long, double, raterootarray);
double glaguerre(long, double, double);
void   initgammacat(long, double, double *, double *);
void   inithowmany(long *, long);
void   inithowoften(long *);

void   initlambda(double *);
void   initfreqs(double *, double *, double *, double *);
void   initratio(double *);
void   initpower(double *);
void   initdatasets(long *);
void   justweights(long *);
void   initterminal(boolean *, boolean *);
void   initnumlines(long *);
void   initbestrees(bestelm *, long, boolean);
void   newline(FILE *, long, long, long);

void   inputnumbers(long *, long *, long *, long);
void   inputnumbersold(long *, long *, long *, long);
void   inputnumbers2(long *, long *, long n);
void   inputnumbers3(long *, long *);
void   samenumsp(long *, long);
void   samenumsp2(long);
void   readoptions(long *, const char *);
void   matchoptions(Char *, const char *);
void   inputweights(long, steptr, boolean *);
void   inputweightsold(long, steptr, boolean *);
void   inputweights2(long, long, long *, steptr, boolean *, const char *);
void   printweights(FILE *, long, long, steptr, const char *);

void   inputcategs(long, long, steptr, long, const char *);
void   printcategs(FILE *, long, steptr, const char *);
void   inputfactors(long, Char *, boolean *);
void   inputfactorsnew(long, Char *, boolean *);
void   printfactors(FILE *, long, Char *, const char *);
void   headings(long, const char *, const char *);
void   initname(long);
void   findtree(boolean *,long *,long,long *,bestelm *);
void   addtree(long,long *,boolean,long *,bestelm *);
long   findunrearranged(bestelm *, long, boolean);
boolean torearrange(bestelm *, long);

void   reducebestrees(bestelm *, long *);
void   shellsort(double *, long *, long);
void   getch(Char *, long *, FILE *);
void   getch2(Char *, long *);
void   findch(Char, Char *, long);
void   findch2(Char, long *, long *, Char *);
void   findch3(Char, Char *, long, long);
void   processlength(double *,double *,Char *,boolean *,FILE *,long *);
void   writename(long, long, long *);
void   memerror(void);

void   odd_malloc(long);

void   gnu(node **, node **);
void   chuck(node **, node *);
void   zeronumnuc(node *, long);
void   zerodiscnumnuc(node *, long);
void   allocnontip(node *, long *, long);
void   allocdiscnontip(node *, long *, unsigned char *, long );
void   allocnode(node **, long *, long);
void   allocdiscnode(node **, long *, unsigned char *, long );
void   gnutreenode(node **, node **, long, long, long *);
void   gnudisctreenode(node **, node **, long , long, long *,
                unsigned char *);

void   setupnode(node *, long);
node * pnode(tree *t, node *p);
long   count_sibs (node *);
void   inittrav (node *);
void   commentskipper(FILE ***, long *);
long   countcomma(FILE **, long *);
long   countsemic(FILE **);
void   hookup(node *, node *);
void   unhookup(node *, node *);
void   link_trees(long, long , long, pointarray);
void   allocate_nodep(pointarray *, FILE **, long  *);
  
void   malloc_pheno(node *, long, long);
void   malloc_ppheno(node *, long, long);
long   take_name_from_tree (Char *, Char *, FILE *);
void   match_names_to_data (Char *, pointarray, node **, long);
void   addelement(node **, node *, Char *, long *, FILE *, pointarray,
                boolean *, boolean *, pointarray, long *, long *, boolean *,
                node **, initptr,boolean,long);
void   treeread (FILE *, node **, pointarray, boolean *, boolean *,
                pointarray, long *, boolean *, node **, initptr,boolean,long);
void   addelement2(node *, Char *, long *, FILE *, pointarray, boolean,
                double *, boolean *, long *, long *, long, boolean *,boolean,
                long);
void   treeread2 (FILE *, node **, pointarray, boolean, double *,
                boolean *, boolean *, long *,boolean,long);
void   exxit (int);
void countup(long *loopcount, long maxcount);
char gettc(FILE* file);
void unroot_r(node* p,node ** nodep, long nonodes);
void unroot(tree* t,long nonodes);
void unroot_here(node* root, node** nodep, long nonodes);
void clear_connections(tree *t, long nonodes);
void init(int argc, char** argv);
char **stringnames_new(void);
void stringnames_delete(char **names);
int fieldwidth_double(double val, unsigned int precision);
void output_matrix_d(FILE *fp, double **matrix,
    unsigned long rows, unsigned long cols,
    char **row_head, char **col_head, int flags);
void debugtree (tree *, FILE *);
void debugtree2 (pointarray, long, FILE *);
#endif /* OLDC */
#endif /* _PHYLIP_H_ */
