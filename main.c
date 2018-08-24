#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

typedef    struct send_data_infomation {
    int            _wc;
    int            _wc_strt;
    int            _wc_min;
    int            _wc_max;
    struct timeval    _1st_enq_tv;
    struct timeval    _wait_tv;
    sem_t        _sem;
    pthread_t    _pthread;
} SNDDATINF;

#define    SEND_WT_SEC     0
#define    SEND_WT_USEC    0
#define    SEND_WT_INTVL_USEC    0

SNDDATINF send_data = {
    ._wc=0,
    ._wait_tv.tv_sec=SEND_WT_USEC,
    ._wait_tv.tv_usec=SEND_WT_INTVL_USEC
};

void *
sched_func(void *vp)
{
    int                   rtn, pthpolicy;
    struct sched_param    pthprm;
    int                   pmax, pmin, prio;
    int                   preerrno;
    
    rtn = pthread_getschedparam(send_data._pthread, &pthpolicy, &pthprm);
    if (rtn == 0)
    {
        if (pthpolicy != SCHED_FIFO)
        {
            pthpolicy = SCHED_FIFO;
            rtn = sched_get_priority_max(pthpolicy); 
            if (0 <= rtn)
            {
                pmax = rtn;
                rtn = sched_get_priority_min(pthpolicy); 
                if (0 <= rtn)
                {
                    preerrno = 0;
                    pmin = rtn;
                    for (prio=pmax; pmin<=prio; --prio)
                    {
                        pthprm.sched_priority = prio;
                        rtn = pthread_setschedparam(send_data._pthread, pthpolicy, &pthprm);
                        if (rtn == 0)
                        {
                            printf("Set FIFO %d\n", pthprm.sched_priority);
                            break;
                        }
                        if (rtn != 0)
                        {
                            if (preerrno != errno)
                            {
                                preerrno = errno;
                                fprintf(stderr, "%s pthread_setschedparam err %d:%s\n", __func__, errno,strerror(errno));
                            }
                        }
                    }
                }
            }
        }
        else
        {
            printf("Already FIFO %d\n", pthprm.sched_priority);
        }
    }
    printf("End of %s\n", __func__);
    while (1)    sleep (1);
}

int
main(int ac, char *av[])
{
    int                rtn;
    pthread_attr_t    attr;
    
    rtn = pthread_attr_init(&attr);
    if (!rtn)
    {
        rtn = SCHED_FIFO;
        rtn = pthread_attr_setschedpolicy(&attr, rtn);
        if (!rtn)
        {
            struct sched_param    param;
            param.sched_priority = 88;
            rtn = pthread_attr_setschedparam(&attr, &param);
            if (!rtn)
            {
                rtn = pthread_create(&send_data._pthread, &attr, sched_func, NULL);
                if (rtn)
                    fprintf(stderr, "pthread_create error %d:%s\n",errno, strerror(errno));
                else
                    printf("pthread_create with SCHED_FIFO OK\n");
            }
        }
    }
    if (rtn)
    {
        rtn = pthread_create(&send_data._pthread, NULL, sched_func, NULL);
        if (rtn)
            fprintf(stderr, "pthread_create error %d:%s\n",errno, strerror(errno));
        else
            printf("pthread_create OK\n");
    }
    
    printf("Please ^C\n");
    while (1)    sleep (1);
}
