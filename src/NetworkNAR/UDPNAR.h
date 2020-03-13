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

#ifndef UDPNAR_H
#define UDPNAR_H

//////////////
//  UDPNAR  //
//////////////
//A networking NAR using UDP to receive Narsese

//References//
//----------//
#include "UDP.h"
#include "./../Shell.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 

static pthread_mutex_t nar_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t narQueue_mutex = PTHREAD_MUTEX_INITIALIZER;


//Methods//
//-------//
//Starts the UDPNAR with a reasoning speed given by timestep, example: 10000000L = 10ms
void UDPNAR_Start(char *ip, int port, long timestep, bool addOps);
//Stops the UDPNAR, cancelling its threads
void UDPNAR_Stop();

#endif
