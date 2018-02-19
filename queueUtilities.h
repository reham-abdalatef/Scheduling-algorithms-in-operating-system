
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <bits/stdc++.h>

#define QUEUEKEY 777
#define ENDTYPE 19L

//type of the algo
#define HPF 1
#define SRTF 2
#define RR 3

///==============================
//don't mess with this variable//
int qid;
//===============================

struct processData {
    //TODO: add the process data details
    int priority;
    int runningTime;
    int arrivalTime;
    int pid;
    int state; // state:0 is just created, state:1 is ready, state:2 is running, state:3 is paused, state:4 is finished
    int begin; // the time at which the process begin to run
    int lstPaused; // the last clk at which the process was paused
    int remainTime; // the process will finish after remainTime be zero
    int algo; // 1 for HPF __ 2 for SRTF __ 3 for RR
    int id;

    bool operator<(const processData &rhs) const {
       if(this->algo == HPF){
          // HPF needs this method for sorting the processes by priorities
          return priority > rhs.priority;
	}else if(this->algo == SRTF){
	  // in case of tie --> sorting by the arrival time
	  if(this->remainTime == rhs.remainTime)return this->arrivalTime > rhs.arrivalTime;
	  // SRTN needs this method for sorting the processes by remaining Time
          return this->remainTime > rhs.remainTime;	
	}else{
	      // TODO: Round Roben
               return priority > rhs.priority;
	}
	
    }
};

struct messagebuffer {
    long mtype;
    struct processData data;
};

void initQueue(bool flag) {
    if (flag) {
        qid = msgget(QUEUEKEY, IPC_CREAT | 0644);
        if (qid == -1) {
            perror("queue not created");
        }
    } else {
        qid = msgget(QUEUEKEY, 0444);
        if (qid == -1) {
            //Make sure that the Clock exists
            printf("Error, queue not initialized yet\n");
            kill(getpgrp(), SIGINT);
        }
    }
}

int Sendmsg(struct processData pData) {
    // send the process from processGenerator to the Scheduler
    struct messagebuffer msg;
    msg.data = pData;
    msg.mtype = 1L;
    return msgsnd(qid, &msg, sizeof(msg) - sizeof(long), !IPC_NOWAIT);
}

int Recmsg(processData &pData) {
    // receive the process that sended by the processGenerator
    struct messagebuffer msg;
    msg.mtype = 1L;
    int ret = msgrcv(qid, &msg, sizeof(msg) - sizeof(long), 0, IPC_NOWAIT);
    pData = msg.data;
    if (ret == -1)
        return -1;
    if (msg.mtype == ENDTYPE)
        return 1;
    return 0;
}

void lastSend() {
    // send a msg with ENDTYPE to stop trasmitting between scheduler and processGenerator
    struct messagebuffer msg;
    msg.mtype = ENDTYPE;
    msgsnd(qid, &msg, sizeof(msg) - sizeof(long), !IPC_NOWAIT);
}

void destroyQueueAndExit(int) {
    msgctl(qid, IPC_RMID, (struct msqid_ds *) 0);
    exit(0);
}


