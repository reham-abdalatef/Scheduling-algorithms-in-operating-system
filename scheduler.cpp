#include "clkUtilities.h"
#include "queueUtilities.h"

using namespace std;

void tostring(char str[], int num);
queue<processData> job;   // processes wants to run in the future and its arrival time didn't come
priority_queue<processData> ready; // processes that arrival time of it came and it's ready to run
queue<processData>ready1 ;

struct processData curProcess;
ofstream fout("Sched.txt");

void createNewProcess(int remainingTime, int &curPid) {
 char str[10];
tostring(str,remainingTime);
    pid_t pid = fork();
    if(pid == 0) {
        // I'me the process
       execl("/home/ubuntu/Desktop/code/process", str, "-f", NULL);
    } else {
        curPid = pid;
    }
}


void RR(int q)
{ int t;
if (!ready1.empty()) {
	curProcess = ready1.front();
	curProcess.begin = getClk();
	if (ready1.front().state == 1) { // lw awl mra yd5ol
		cout << " ana f awl case" << endl;
		curProcess.remain = curProcess.runningTime;
		if (curProcess.runningTime <= q) // hy5ls mn awl mra
		{
			// create a new process and let it run
			cout << "ya rb awsl" << endl;
			createNewProcess(curProcess.runningTime, curProcess.pid);
			cout << "At​ ​ time ​" << curProcess.begin << " process " << curProcess.id << "started arr " << curProcess.arrivalTime << " total " << curProcess.runningTime;
			cout << " remain " << curProcess.runningTime << "wait 0 " << endl;
			while (getClk() < curProcess.begin + curProcess.runningTime);
			curProcess.remain = 0;

		}
		else
		{
			curProcess.begin = getClk();  // pause 
			cout << "At​ ​ time ​" << curProcess.begin << " process " << curProcess.id << "started arr " << curProcess.arrivalTime << " total " << curProcess.runningTime;
			cout << " remain " << curProcess.runningTime << "wait 0 " << endl;
			createNewProcess(curProcess.runningTime, curProcess.pid);

			while (getClk() < curProcess.begin + q);
			curProcess.begin = getClk();
			curProcess.remain -= q;
			curProcess.last = curProcess.begin;
			curProcess.state = 3;
			ready1.pop();
			ready1.push(curProcess);
			cout << "At​ ​ time ​" << curProcess.begin << " process " << curProcess.id << "stopped arr " << curProcess.arrivalTime << " total " << curProcess.runningTime;
			cout << " remain " << curProcess.remain << "wait 0 " << endl;
			kill(curProcess.pid, SIGSTOP);
		}

	}
	else if (ready1.front().state == 3) { // process is paused
	   // update cur process state to be paused

		kill(curProcess.pid, SIGCONT);
		if (curProcess.runningTime <= q)// will finish
		{
			curProcess.begin = getClk();
			cout << "At​ ​ time ​" << curProcess.begin << " process " << curProcess.id << "resumed arr " << curProcess.arrivalTime << " total " << curProcess.runningTime;
			cout << " remain " << curProcess.remain << "wait " << curProcess.begin - curProcess.last << endl;
			// create a new process and let it run
			while (getClk() < curProcess.begin + curProcess.runningTime);
			curProcess.remain = 0;

		}
		else
		{
			curProcess.begin = getClk();   //pause
			cout << "At​ ​ time ​" << curProcess.begin << " process " << curProcess.id << "resumed arr " << curProcess.arrivalTime << " total " << curProcess.runningTime;
			cout << " remain " << curProcess.remain << "wait " << curProcess.begin - curProcess.last << endl;

			while (getClk() < curProcess.begin + q);
			//curProcess.begin = getClk();
			curProcess.remain -= q;
			curProcess.last = getClk();
			curProcess.state = 3;
			ready1.pop();
			ready1.push(curProcess);
			cout << "At​ ​ time ​" << curProcess.last << " process " << curProcess.id << "stopped arr " << curProcess.arrivalTime << " total " << curProcess.runningTime;
			cout << " remain " << curProcess.remain << "wait 0 " << endl;
			kill(curProcess.pid, SIGSTOP);
		}


	}
	if (curProcess.id != -1 && curProcess.remain == 0) {
		fout << "Process " << curProcess.id << " is finished @time " << getClk() << "\n";
		curProcess.state = 4; //finished
		ready1.pop();
	}

}


}


int main(int argc, char *argv[]) {
    cout << "Scheduler: I went to Scheduler.\n";
    initClk();
    initQueue(false);
    fout.is_open();

    //TODO: implement the scheduler :)
    //===================================
    // Preimplemented Functions examples
    curProcess.id = curProcess.state = -1;
    ///// Toget time use the following function
    int x = getClk();
    printf("Scheduler: current time is %d\n", x);

    ////// To receive something from the generator, for example  id 2
    struct processData pD;
    while (true) {
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
            ready1.push(job.front());
            job.pop();
        }
        cout<<"ana lsa bra"<<endl;
        RR(2);
    }
    cout << job.size() << endl;
    while (!job.empty()) {
        x = getClk();

        if (job.front().arrivalTime <= x) {
            job.front().state = 1; // update process' state from created to ready
            ready1.push(job.front());
            job.pop();
        }
        RR(2);
    }
    while(!ready1.empty() || curProcess.state != 4) {
        RR(2);
    }
    fout.close();
    cout << "I've finished\n";
    return 0;
}

void tostring(char str[], int num)

{

    int i, rem, len = 0, n;
    n = num;

    while (n != 0)

    {

        len++;

        n /= 10;

    }

    for (i = 0; i < len; i++)

    {

        rem = num % 10;

        num = num / 10;

        str[len - (i + 1)] = rem + '0';

    }

    str[len] = '\0';

}
