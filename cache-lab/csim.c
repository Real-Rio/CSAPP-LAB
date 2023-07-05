#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

typedef struct cacheline
{
    int valid;
    int tag;
    int index;
    int offset;
    int LRU;
} cacheline;

int miss_count = 0, hit_count = 0, eviction_count = 0;

// expecially in 'M',M is a combination of 'L' and 'S','S' must hit
void loadandhit(int set_index, int tag, cacheline **cache, int E_val)
{
    for (int i = 0; i < E_val; i++)
    {
        // hit
        if (cache[set_index][i].valid == 1 && cache[set_index][i].tag == tag)
        {
            printf("hit ");
            hit_count++;
            cache[set_index][i].LRU = -1;
        }
        cache[set_index][i].LRU++;
    }
}

void access_cache(int set_index, int tag, cacheline **cache, int E_val)
{
    int flag = 0,replace_index=0,LRU_max=0;
    for (int i = 0; i < E_val; i++)
    {
        // hit
        if (cache[set_index][i].valid == 1 && cache[set_index][i].tag == tag)
        {
            printf("hit ");
            hit_count++;
            cache[set_index][i].LRU = -1;
            flag = 1;
            break;
        }
    }
    // miss
    if (!flag)
    {
        printf("miss ");
        miss_count++;
    }
    // update LRU flag and insert new line
    for (int i = 0; i < E_val; i++)
    {
        // find the line with max LRU
        if(cache[set_index][i].LRU>LRU_max){
            LRU_max=cache[set_index][i].LRU;
            replace_index=i;
        }
        // if there is a empty line, insert new line
        if (!flag && cache[set_index][i].valid == 0)
        {
            cache[set_index][i].valid = 1;
            cache[set_index][i].tag = tag;
            cache[set_index][i].LRU = -1;
            flag = 1;
        }

        cache[set_index][i].LRU++;
    }
    // after iteration, if there is no empty line, replace the line with max LRU
    if(!flag){
        eviction_count++;
        printf("eviction ");
        cache[set_index][replace_index].tag = tag;
        cache[set_index][replace_index].LRU = 0;
        cache[set_index][replace_index].valid = 1;
    }
}

int main(int argc, char *argv[])
{
    int cmd_opt = 0;
    int E_val, s_val, b_val;
    FILE *tracefile;
    cacheline **cache;

    // process argument
    while (1)
    {
        cmd_opt = getopt(argc, argv, "b:t:E:s:h::v::");
        /* End condition always first */
        if (cmd_opt == -1)
        {
            break;
        }

        /* Print option when it is valid */
        // if (cmd_opt != '?')
        // {
        //     fprintf(stderr, "option:-%c\n", cmd_opt);
        // }

        /* Lets parse */
        switch (cmd_opt)
        {
        /* No args */
        case 'h':
        case 'v':
            break;

        /* Single arg */
        case 's':
            s_val = pow(2, atoi(optarg));
            break;
        case 'E':
            E_val = atoi(optarg);
            break;
        case 'b':
            b_val = pow(2, atoi(optarg));
            break;
        case 't':
            if ((tracefile = fopen(optarg, "r")) == NULL)
            {

                printf("open file failed\n");
                return 0;
            }
            break;

        /* Error handle: Mainly missing arg or illegal option */
        case '?':
            fprintf(stderr, "Illegal option:-%c\n", isprint(optopt) ? optopt : '#');
            break;
        default:
            fprintf(stderr, "Not supported option\n");
            break;
        }
    }

    // initialize cache structure
    // malloc (2^s * 2^E）cache lines
    // cache can be interpreted as cache[2^s][2^E]
    cache = (cacheline **)calloc(s_val, sizeof(cacheline *));
    if (cache == NULL)
    {
        printf("malloc failed\n");
        return 0;
    }
    for (int i = 0; i < s_val; i++)
    {
        cache[i] = (cacheline *)calloc(E_val, sizeof(cacheline));
        if (cache[i] == NULL)
        {
            printf("malloc failed\n");
            return 0;
        }
    }

    // deal with trace file
    char identifier, identifier2, rubbish;
    unsigned long address;
    int size;
    // Reading lines like " M 20,1" or "L 19,3"

    while (fscanf(tracefile, "%c%c %lx,%d%c", &identifier2, &identifier, &address, &size, &rubbish) > 0)
    {
        // Do stuff
        if (identifier2 == 'I')
        {
            continue;
        }
        if (identifier == 'L' || identifier == 'S')
        {
            printf("%c %lx,%d ", identifier, address, size);
            int set_index = address / b_val % s_val;
            int tag = address / (b_val * s_val);
            access_cache(set_index, tag, cache, E_val);
            printf("line:%d,tag:%d\n", set_index, tag);
        }
        else if (identifier == 'M')
        {
            printf("%c %lx,%d ", identifier, address, size);
            int set_index = address / b_val % s_val;
            int tag = address / (b_val * s_val);
            access_cache(set_index, tag, cache, E_val);
            loadandhit(set_index, tag, cache, E_val);
            printf("line:%d,tag:%d\n", set_index, tag);
        }
        else
        {
            fprintf(stderr, "illegal identifier:%c.\n", identifier);
            return 0;
        }
    }

    fclose(tracefile); // remember to close file when done

    // free cache
    for (int i = 0; i < E_val; i++)
    {
        free(cache[i]);
    }
    free(cache);

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
