// cache.c
#include "cache.h"

const int cache_block_size[] = {100, 1024, 5120, 10240, 25600, 102400};
const int cache_cnt[] = {40, 20, 20, 10, 12, 5};

// mutex
sem_t mutex;

int64_t currentTimeMillis();

void init_cache()
{
    Sem_init(&mutex, 0, 1);
    int i = 0;
    for (; i < TYPES; i++)
    {
        caches[i].size = cache_cnt[i];
        caches[i].cacheobjs = (cache_block *)malloc(cache_cnt[i] * sizeof(cache_block));
        cache_block *j = caches[i].cacheobjs;
        int k;
        for (k = 0; k < cache_cnt[i]; j++, k++)
        {
            j->time = 0;
            j->datasize = 0;
            j->url = malloc(sizeof(char) * MAXLINE);
            strcpy(j->url, "");
            j->data = malloc(sizeof(char) * cache_block_size[i]);
            memset(j->data, 0, cache_block_size[i]);
            pthread_rwlock_init(&j->rwlock, NULL);
        }
    }
}

void free_cache()
{
    int i = 0;
    for (; i < TYPES; i++)
    {
        cache_block *j = caches[i].cacheobjs;
        int k;
        for (k = 0; k < cache_cnt[i]; j++, k++)
        {
            free(j->url);
            free(j->data);
            pthread_rwlock_destroy(&j->rwlock);
        }
        free(caches[i].cacheobjs);
    }
}

int read_cache(char *url, int fd)
{

    int tar = 0, i = 0;
    cache_type cur;
    cache_block *p;
    printf("read cache %s \n", url);
    for (; tar < TYPES; tar++)
    {
        cur = caches[tar];
        p = cur.cacheobjs;
        for (i = 0; i < cur.size; i++, p++)
        {
            if (p->time != 0 && strcmp(url, p->url) == 0)
                break;
        }
        if (i < cur.size)
            break;
    }

    if (i == cur.size)
    {
        printf("read cache fail\n");
        return 0;
    }
    pthread_rwlock_rdlock(&p->rwlock);
    if (strcmp(url, p->url) != 0)
    {
        pthread_rwlock_unlock(&p->rwlock);
        printf("read cache fail\n");
        return 0;
    }
    /*为了保证线程安全，读cache不更新时间戳*/
    // pthread_rwlock_unlock(&p->rwlock);
    // if (!pthread_rwlock_trywrlock(&p->rwlock))
    // {
    //     p->time = currentTimeMillis();
    //     pthread_rwlock_unlock(&p->rwlock);
    // }
    // pthread_rwlock_rdlock(&p->rwlock);
    Rio_writen(fd, p->data, p->datasize);
    pthread_rwlock_unlock(&p->rwlock);
    printf("read cache successful\n");
    return 1;
}

void write_cache(char *url, char *data, int len)
{
    int tar = 0;
    for (; tar < TYPES && len > cache_block_size[tar]; tar++)
        ;

    printf("write cache %s in No.%d\n", url, tar);
    /* find empty block */
    cache_type cur = caches[tar];
    cache_block *p = cur.cacheobjs, *pt;
    P(&mutex);
    int i;
    for (i = 0; i < cur.size; i++, p++)
    {
        if (p->time == 0)
        {
            break;
        }
    }
    /* find last visited */
    int64_t min = currentTimeMillis();
    //淘汰算法
    if (i == cur.size)
    {
        for (i = 0, pt = cur.cacheobjs; i < cur.size; i++, pt++)
        {
            if (pt->time <= min)
            {
                min = pt->time;
                p = pt;
            }
        }
    }
    pthread_rwlock_wrlock(&p->rwlock);
    V(&mutex);
    p->time = currentTimeMillis();
    p->datasize = len;
    memcpy(p->url, url, MAXLINE);
    memcpy(p->data, data, len);
    pthread_rwlock_unlock(&p->rwlock);
    printf("write Cache\n");
}

int64_t currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t s1 = (int64_t)(time.tv_sec) * 1000;
    int64_t s2 = (time.tv_usec / 1000);
    return s1 + s2;
}