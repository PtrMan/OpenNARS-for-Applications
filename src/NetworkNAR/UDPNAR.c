/* 
 * The MIT License
 *
 * Copyright 2020 The OpenNARS authors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "UDPNAR.h"

volatile bool Stopped = false;
pthread_cond_t start_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;

static char narseseQueue[100][NARSESE_LEN_MAX];
static int narseseQueueLen;

void* Reasoner_Thread_Run(void* timestep_address)
{
    pthread_mutex_lock(&start_mutex);
    pthread_cond_signal(&start_cond);
    pthread_mutex_unlock(&start_mutex);
    long timestep = *((long*) timestep_address);
    assert(timestep >= 0, "Nonsensical timestep for UDPNAR!");
    while(!Stopped)
    {
        char narseseQueue2[100][NARSESE_LEN_MAX];
        int narseseQueueLen2;

        // take queued messages and stuff them into NAR
        pthread_mutex_lock(&narQueue_mutex);
        narseseQueueLen2 = narseseQueueLen;
        for(int idx=0;idx<narseseQueueLen;idx++) {
            memcpy(narseseQueue2[idx], narseseQueue[idx], NARSESE_LEN_MAX);
        }
        narseseQueueLen = 0;
        pthread_mutex_unlock(&narQueue_mutex);

        // ...stuff them into NAR
        puts("[d] input to NAR...");
        fflush(stdout);

        if (narseseQueueLen2 > 0) {
            for(int idx=0;idx<narseseQueueLen2;idx++) {
                if(Shell_ProcessInput(narseseQueue2[idx])) //reset?
                {
                    Shell_NARInit(true);
                }
            }
        }
        puts("[d] ...done");
        fflush(stdout);


        int cycleBatch = 2;
        pthread_mutex_lock(&nar_mutex);
        puts("[d] cycles");
        fflush(stdout);
        NAR_Cycles(cycleBatch);
        puts("[d] ...done");
        fflush(stdout);
        pthread_mutex_unlock(&nar_mutex);


        if(timestep >= 0)
        {
            nanosleep((struct timespec[]){{0, timestep}}, NULL); //POSIX sleep for timestep nanoseconds
        }
    }
    return NULL;
}

void* Receive_Thread_Run(void *sockfd_address)
{
    pthread_mutex_lock(&start_mutex);
    pthread_cond_signal(&start_cond);
    pthread_mutex_unlock(&start_mutex);
    int sockfd = *((int*) sockfd_address);
    for(;;)
    {
        char buffer[NARSESE_LEN_MAX];
        UDP_ReceiveData(sockfd, buffer, NARSESE_LEN_MAX);
        if(Stopped) //avoids problematic buffer states due to socket shutdown, most portable solution!
        {
            break;
        }
        //puts("[d] UDP received, lock");
        //fflush(stdout);

        // put on queue
        pthread_mutex_lock(&narQueue_mutex);
        memcpy(narseseQueue[narseseQueueLen++], buffer, NARSESE_LEN_MAX);
        narseseQueueLen = MIN(narseseQueueLen, 100-1);
        pthread_mutex_unlock(&narQueue_mutex);
        
        /*
        pthread_mutex_lock(&nar_mutex);
        if(Shell_ProcessInput(buffer)) //reset?
        {
            Shell_NARInit(true);
        }
        pthread_mutex_unlock(&nar_mutex);
        */

        //puts("[d] ...done");
        //fflush(stdout);

    }
    return NULL;
}

pthread_t thread_reasoner, thread_receiver;
bool Started = false;
int receiver_sockfd; 
void UDPNAR_Start(char *ip, int port, long timestep, bool addOps)
{
    narseseQueueLen = 0;

    assert(!Stopped, "UDPNAR was already started!");
    Shell_NARInit(addOps);
    receiver_sockfd = UDP_INIT_Receiver(ip, port);
    //Create reasoner thread and wait for its creation
    pthread_mutex_lock(&start_mutex);
    pthread_create(&thread_reasoner, NULL, Reasoner_Thread_Run, &timestep);
    pthread_cond_wait(&start_cond, &start_mutex);
    pthread_mutex_unlock(&start_mutex);
    //Create receive thread and wait for its creation
    pthread_mutex_lock(&start_mutex);
    pthread_create(&thread_receiver, NULL, Receive_Thread_Run, &receiver_sockfd);
    pthread_cond_wait(&start_cond, &start_mutex);
    pthread_mutex_unlock(&start_mutex);
    puts("//UDPNAR started!");
    fflush(stdout);
    Started = true;
}

void UDPNAR_Stop()
{
    assert(Started, "UDPNAR not started, call UDPNAR_Start first!");
    Stopped = true;
    shutdown(receiver_sockfd, SHUT_RDWR);
    pthread_join(thread_reasoner, NULL);
    pthread_join(thread_receiver, NULL);
    Stats_Print(currentTime);
}
