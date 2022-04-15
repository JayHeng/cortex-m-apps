/*
 * vim: ai ts=4 sts=4 sw=4 cinoptions=>4 expandtab
 */
#define _GNU_SOURCE

#include "mbw.h"

/* how many runs to average by default */
#define DEFAULT_NR_LOOPS 10

/* we have 3 tests at the moment */
#define MAX_TESTS 3

/* default block size for test 2, in bytes */
#define DEFAULT_BLOCK_SIZE 262144

/* test types */
#define TEST_MEMCPY 0
#define TEST_DUMB 1
#define TEST_MCBLOCK 2

/* version number */
#define VERSION "1.4"

/*
 * MBW memory bandwidth benchmark
 *
 * 2006, 2012 Andras.Horvath@gmail.com
 * 2013 j.m.slocum@gmail.com 
 * (Special thanks to Stephen Pasich)
 *
 * http://github.com/raas/mbw
 *
 * compile with:
 *			gcc -O -o mbw mbw.c
 *
 * run with eg.:
 *
 *			./mbw 300
 *
 * or './mbw -h' for help
 *
 * watch out for swap usage (or turn off swap)
 */

void usage()
{
    PRINTF("mbw memory benchmark v%s, https://github.com/raas/mbw\n", VERSION);
    PRINTF("Usage: mbw [options] array_size_in_MiB\n");
    PRINTF("Options:\n");
    PRINTF("	-n: number of runs per test\n");
    PRINTF("	-a: Don't display average\n");
    PRINTF("	-t%d: memcpy test\n", TEST_MEMCPY);
    PRINTF("	-t%d: dumb (b[i]=a[i] style) test\n", TEST_DUMB);
    PRINTF("	-t%d: memcpy test with fixed block size\n", TEST_MCBLOCK);
    PRINTF("	-b <size>: block size in bytes for -t2 (default: %d)\n", DEFAULT_BLOCK_SIZE);
    PRINTF("	-q: quiet (print statistics only)\n");
    PRINTF("(will then use two arrays, watch out for swapping)\n");
    PRINTF("'Bandwidth' is amount of data copied over the time this operation took.\n");
    PRINTF("\nThe default is to run all tests available.\n");
}

/* ------------------------------------------------------ */

/* allocate a test array and fill it with data
 * so as to force Linux to _really_ allocate it */
long *make_array(unsigned long long asize)
{
    unsigned long long t;
    unsigned int long_size=sizeof(long);
    long *a;

    a=my_calloc(asize, long_size);

    if(NULL==a) {
        PRINTF("Error allocating memory");
        exit(1);
    }

    /* make sure both arrays are allocated, fill with pattern */
    for(t=0; t<asize; t++) {
        a[t]=0xaa;
    }
    return a;
}

/* actual benchmark */
/* asize: number of type 'long' elements in test arrays
 * long_size: sizeof(long) cached
 * type: 0=use memcpy, 1=use dumb copy loop (whatever GCC thinks best)
 *
 * return value: elapsed time in seconds
 */
double worker(unsigned long long asize, long *a, long *b, int type, unsigned long long block_size)
{
    unsigned long long t;
    timeval_t starttime, endtime;
    double te;
    unsigned int long_size=sizeof(long);
    /* array size in bytes */
    unsigned long long array_bytes=asize*long_size;

    if(type==TEST_MEMCPY) { /* memcpy test */
        /* timer starts */
        gettimeofday(&starttime, NULL);
        memcpy(b, a, array_bytes);
        /* timer stops */
        gettimeofday(&endtime, NULL);
    } else if(type==TEST_MCBLOCK) { /* memcpy block test */
        char* aa = (char*)a;
        char* bb = (char*)b;
        gettimeofday(&starttime, NULL);
        for (t=array_bytes; t >= block_size; t-=block_size, aa+=block_size){
            bb=mempcpy(bb, aa, block_size);
        }
        if(t) {
            bb=mempcpy(bb, aa, t);
        }
        gettimeofday(&endtime, NULL);
    } else if(type==TEST_DUMB) { /* dumb test */
        gettimeofday(&starttime, NULL);
        for(t=0; t<asize; t++) {
            b[t]=a[t];
        }
        gettimeofday(&endtime, NULL);
    }

    te=((double)(endtime.tv_sec*1000000-starttime.tv_sec*1000000+endtime.tv_usec-starttime.tv_usec))/1000000;

    return te;
}

/* ------------------------------------------------------ */

/* pretty print worker's output in human-readable terms */
/* te: elapsed time in seconds
 * kt: amount of transferred data in KiB
 * type: see 'worker' above
 *
 * return value: -
 */
void printout(double te, double kt, int type)
{
    switch(type) {
        case TEST_MEMCPY:
            PRINTF("Method: MEMCPY\t");
            break;
        case TEST_DUMB:
            PRINTF("Method: DUMB\t");
            break;
        case TEST_MCBLOCK:
            PRINTF("Method: MCBLOCK\t");
            break;
    }
    PRINTF("Elapsed: %.5f\t", te);
    PRINTF("MiB: %.5f\t", kt/1024);
    PRINTF("Copy: %.3f MiB/s\n", kt/1024/te);
    return;
}

/* ------------------------------------------------------ */
/*******************************************************************************
* Input parameters:
* ---- testno       : Test set type: 1, 2, 3
                      1: memcpy test
                      2: dumb (b[i]=a[i] style) test
                      3: memcpy test with fixed block size
* ---- showavg      : Display average
* ---- nr_loops     : number of runs per test
* ---- block_size   : block size in bytes for - testno = 2
* ---- mem_start    : array start address
* ---- mem_size     : array_size_in_Byte
*/
int mbw_main(uint32_t testno, uint32_t showavg, uint32_t nr_loops, uint64_t block_size, uint32_t mem_start, uint32_t mem_size) 
//int main(int argc, char **argv)
{
    unsigned int long_size=0;
    double te, te_sum; /* time elapsed */
    unsigned long long asize=0; /* array size (elements in array) */
    int i;
    long *a, *b; /* the two arrays to be copied from/to */
    //int o; /* getopt options */
    //unsigned long testno;

    /* options */

    /* how many runs to average? */
    //int nr_loops=DEFAULT_NR_LOOPS;
    /* fixed memcpy block size for -t2 */
    //unsigned long long block_size=DEFAULT_BLOCK_SIZE;
    /* show average, -a */
    //int showavg=1;
    /* what tests to run (-t x) */
    int tests[MAX_TESTS];
    //double mt=0; /* MiBytes transferred == array size in MiB */
    double kt=0; /* MiBytes transferred == array size in KiB */
    kt = mem_size / 1024.0;
    int quiet=0; /* suppress extra messages */

    tests[0]=0;
    tests[1]=0;
    tests[2]=0;
    
    g_myMem.memStart = mem_start;
    g_myMem.memSize = mem_size;
    for (uint32_t i = 0; i < MAX_MEM_REGIONS; i++)
    {
        g_myMem.regionStart[i] = 0;
        g_myMem.regionSize[i] = 0;
    }

#if 0
    while((o=getopt(argc, argv, "haqn:t:b:")) != EOF) {
        switch(o) {
            case 'h':
                usage();
                exit(1);
                break;
            case 'a': /* suppress printing average */
                showavg=0;
                break;
            case 'n': /* no. loops */
                nr_loops=strtoul(optarg, (char **)NULL, 10);
                break;
            case 't': /* test to run */
                testno=strtoul(optarg, (char **)NULL, 10);
                if(0>testno) {
                    PRINTF("Error: test number must be between 0 and %d\n", MAX_TESTS);
                    exit(1);
                }
                tests[testno]=1;
                break;
            case 'b': /* block size in bytes*/
                block_size=strtoull(optarg, (char **)NULL, 10);
                if(0>=block_size) {
                    PRINTF("Error: what block size do you mean?\n");
                    exit(1);
                }
                break;
            case 'q': /* quiet */
                quiet=1;
                break;
            default:
                break;
        }
    }
#else
    if (!nr_loops)
    {
        nr_loops=DEFAULT_NR_LOOPS;
    }

    if ((testno > 0) || (testno <= MAX_TESTS))
    {
        tests[testno-1]=1;
    }

    if (!block_size)
    {
        block_size=DEFAULT_BLOCK_SIZE;
    }
#endif

    /* default is to run all tests if no specific tests were requested */
    if( (tests[0]+tests[1]+tests[2]) == 0) {
        tests[0]=1;
        tests[1]=1;
        tests[2]=1;
    }

#if 0
    if(optind<argc) {
        kt=strtoul(argv[optind++], (char **)NULL, 10);
    } else {
        PRINTF("Error: no array size given!\n");
        exit(1);
    }
#endif

    if(0>=kt) {
        PRINTF("Error: array size wrong!\n");
        exit(1);
    }

    /* ------------------------------------------------------ */

    long_size=sizeof(long); /* the size of long on this platform */
    //asize=(1024*1024/long_size*mt); /* how many longs then in one array? */
    asize=(unsigned long long)(1024/long_size*kt); /* how many longs then in one array? */

    if(asize*long_size < block_size) {
        PRINTF("Error: array size larger than block size (%llu bytes)!\n", block_size);
        exit(1);
    }

    if(!quiet) {
        PRINTF("Long uses %d bytes. ", long_size);
        PRINTF("Allocating 2*%lld elements = %lld bytes of memory.\n", asize, 2*asize*long_size);
        if(tests[2]) {
            PRINTF("Using %lld bytes as blocks for memcpy block copy test.\n", block_size);
        }
    }

    a=make_array(asize/2);
    b=make_array(asize/2);

    /* ------------------------------------------------------ */
    if(!quiet) {
        PRINTF("Getting down to business... Doing %d runs per test.\n", nr_loops);
    }

    /* run all tests requested, the proper number of times */
    for(testno=0; testno<MAX_TESTS; testno++) {
        te_sum=0;
        if(tests[testno]) {
            for (i=0; i<nr_loops; i++) {
                te=worker(asize, a, b, testno, block_size);
                te_sum+=te;
                PRINTF("%d\t", i);
                printout(te, kt, testno);
            }
            if(showavg) {
                PRINTF("AVG\t");
                printout(te_sum/nr_loops, kt, testno);
            }
        }
    }

    my_free(a);
    my_free(b);
    return 0;
}

