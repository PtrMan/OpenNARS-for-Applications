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

bool USE_Pong_Left_executed = false;
void USE_Pong_Left()
{
    USE_Pong_Left_executed = true;
}
bool USE_Pong_Right_executed = false;
void USE_Pong_Right()
{
    USE_Pong_Right_executed = true;
}

// use this with SymVision which is running in a java environment
// communicates with symvision bidirectional
// * send commands as text to SymVision
// * receive narsese like UDPNAR
void USE_Pong()
{
    puts(">>USE Pong: start UDPNAR");
    UDPNAR_Start("127.0.0.1", 50000, 50000, false);
    puts(">>USE Pong: start");
    NAR_AddOperation(Narsese_AtomicTerm("^left"), USE_Pong_Left); 
    NAR_AddOperation(Narsese_AtomicTerm("^right"), USE_Pong_Right);

    MOTOR_BABBLING_CHANCE = MOTOR_BABBLING_CHANCE_INITIAL;
    PRINT_DERIVATIONS = false;


    int sockfd = UDP_INIT_Sender();
    char *destIp = "127.0.0.1";

    while(1)
    {
        //pthread_mutex_lock(&nar_mutex);
        //NAR_AddInputGoal(Narsese_AtomicTerm("good_nar"));
        //pthread_mutex_unlock(&nar_mutex);
        
        if(USE_Pong_Left_executed)
        {
            USE_Pong_Left_executed = false;
            puts("Exec: op_left");
            
            // send to SymVision
            UDP_SendData(sockfd, destIp, 50001, "op_left!", 9);
        }
        if(USE_Pong_Right_executed)
        {
            USE_Pong_Right_executed = false;
            puts("Exec: op_right");
            
            // send to SymVision
            UDP_SendData(sockfd, destIp, 50001, "op_right!", 10);
        }
    }
}
