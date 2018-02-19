#include "clkUtilities.h"
#include "queueUtilities.h"

//type of the algo
#define HPF_algo '1'
#define SRTF_algo '2'
#define RR_algo '3'

using namespace std;


queue<processData> job;   // processes wants to run in the future and its arrival time didn't come
priority_queue<processData> ready; // processes that arrival time of it came and it's ready to run
struct processData curProcess;
ofstream fout("Sched.txt");

string intToString(int num) {
    string s;
    stringstream ss;
    ss << num;
    ss >> s;
    return s;
}

int stringToInt(string s) {
    int num;
    stringstream ss;
    ss << s;
    ss >> num;
    return num;
}

void createNewProcess(int remainingTime, int &curPid) {
    pid_t pid = fork();
    if(pid == 0) {
        // I'me the process
        execl("/home/mohamed/Downloads/code/process", intToString(remainingTime).c_str(), "-f", NULL);
    } else {
        curPid = pid;
    }
}

void update_HPF(int flag) { // for HPF = 1 , SRTN = 2
    // TODO: case of same priority
    if(ready.empty() && curProcess.id != -1 && curProcess.state != 4) {
        // don't update the current process
        if(getClk() - curProcess.begin >= curProcess.remainTime) {
            curProcess.remainTime = 0;
        }
    }
    if (!ready.empty()) {
	bool  cond = 0;
	if(flag == 1)
		cond = ready.top().priority >= curProcess.priority;
	else {
		cond = ready.top().remainTime >= curProcess.remainTime - (curProcess.begin - getClk());
	}
        if(cond && curProcess.state != 4) {
            // don't update the current process
            if(getClk() - curProcess.begin >= curProcess.remainTime) {
                curProcess.remainTime = 0;
            }
        } else {
            if (ready.top().state == 1) {
                if (curProcess.id != -1 && curProcess.state != 4) {
                    // update cur process state from running to paused
                    curProcess.state = 3;
                    curProcess.remainTime -= (getClk() - curProcess.begin);
                    kill(curProcess.pid, SIGSTOP);
                    if(curProcess.remainTime > 0)
                        ready.push(curProcess);
                    fout << "Process " << curProcess.id << " will be paused now @time " << getClk() << "\n";
                }

                // update new process state from ready to running and save its begin time
                curProcess = ready.top();
                ready.pop();
                curProcess.state = 2; // process is running
		//curProcess.remainTime--; //by hamada
                curProcess.begin = getClk();
                fout << "Process " << curProcess.id << " will run now @time " << getClk() << "\n";
                // create a new process and let it run
                createNewProcess(curProcess.remainTime, curProcess.pid);

            } else if (ready.top().state == 3) { // process is paused and wants to come back
                if(curProcess.state != 4) {
                    // update cur process state to be paused
                    curProcess.state = 3;
                    curProcess.remainTime -= (getClk() - curProcess.begin);
                    fout << "Process " << curProcess.id << " will be paused now @time " << getClk() << "\n";
                    //TODO: send a SIGSTOP to pause this process
                    kill(curProcess.pid, SIGSTOP);
                    if (curProcess.remainTime > 0)
                        ready.push(curProcess);
                }

                // update its state with running
                curProcess = ready.top();
                ready.pop();
                curProcess.state = 2;
                curProcess.begin = getClk(); // update begin time for the process
                fout << "Process " << curProcess.id << " will resume its job now @time " << getClk() << "\n";
                //TODO: send a SIGCONT to resume the process
                kill(curProcess.pid, SIGCONT);
            }
        }
    }

    if (curProcess.id != -1 && curProcess.remainTime == 0) {
        fout << "Process " << curProcess.id << " is finished @time " << getClk() << "\n";
        curProcess.state = 4; //finished
    }
}

int main(int argc, char *argv[]) {

    cout << "Scheduler: I went to Scheduler.\n";
  
    initClk();
    initQueue(false);
    fout.is_open();

    //TODO: implement the scheduler :)

    int algo = stringToInt(argv[0]);
    cout << "algo" << algo << endl;
    int lastTime = -1;
    //===================================
    // Preimplemented Functions examples
    curProcess.id = curProcess.state = -1;
    curProcess.priority = curProcess.remainTime = 10000;

    ///// Toget time use the following function
    int x = getClk();
    printf("Scheduler: current time is %d\n", x);

    ////// To receive something from the generator, for example  id 2
    struct processData pD;
    while (true) {

	/*
	if(lastTime == -1)lastTime = getClk();
        else if(curProcess.id != -1) {
	   if(getClk() == lastTime+1){
		curProcess.remainTime--;
		lastTime = getClk();
	    }
	}
	*/
        //returns -1 on failure; 1 on the end of processes, 0 no processes available yet
        int status = Recmsg(pD);
        if (status == -1) {
            cout << "Scheduler: There's no processes available or there's a failure\n";
            goto skipProcess;
        } else if (status == 1) {
            cout << "Scheduler: End of processes\n";
            break;
        }
//        printf("Scheduler: current received data %d\n", pD.id);
        job.push(pD); // Add the received process to the schedule queue
        skipProcess:
        // Avoid that job queue might be empty
        if (job.empty()) continue;

        x = getClk();
        if (job.front().arrivalTime <= x) {
            job.front().state = 1; // update process' state from created to ready
            ready.push(job.front());
            job.pop();
        }
        update_HPF(algo);
    }

    cout << job.size() << endl;
    while (!job.empty()) {
        x = getClk();

        if (job.front().arrivalTime <= x) {
            job.front().state = 1; // update process' state from created to ready
            ready.push(job.front());
            job.pop();
	    cout << job.size() << endl;
        }
        update_HPF(algo);
    }
    while(!ready.empty() || curProcess.state != 4) {
        update_HPF(algo);
    }

    fout.close();
    cout << "I've finished\n";
    return 0;
}
