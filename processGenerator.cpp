#include "clkUtilities.h"
#include "queueUtilities.h"
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;
void ClearResources(int);
int integer(char *ptch);
void readData();
void fillProcessData(string line, processData &pD);
string intToString(int num);

int algoNum = 0; // type of the algorithm

int main() {
    //TODO:
    // 1-Ask the user about the chosen scheduling Algorithm and its parameters if exists. **DONE**
    // 2-Initiate and create Scheduler and Clock processes. **DONE**
    cout << "Please, Enter 1 for HPF, 2 for Shortest time and 3 for Round Robin: ";
    cin >> algoNum;
    cout << endl << endl;

    // creating Clock Process
    pid_t pid_clk = fork();
    if (pid_clk == 0) {
        // I'm children to processGenerator
        cout << "processGenerator: I'm going to Clock\n";
        execl("/home/mohamed/Downloads/code/clk", "clk", "-f", NULL);
    }
    sleep(1);
    // 3-use this function AFTER creating clock process to initialize clock, and initialize MsgQueue **DONE**
    initClk();
    initQueue(true);

    // creating Schedule Process
    pid_t pid_sched = fork();
    if (pid_sched == 0) {
        // I'm children to processGenerator
        cout << "processGenerator: I'm going to scheduler\n";
        execl("/home/mohamed/Downloads/code/scheduler", intToString(algoNum).c_str(), "-f", NULL);
    }

    //TODO:  Generation Main Loop
    //4-Creating a data structure for process  and  provide it with its parameters
    //5-Send & Notify the information to  the scheduler at the appropriate time
    //(only when a process arrives) so that it will be put it in its turn.

    //===================================
    //Preimplemented Functions examples

//    /////Toget time use the following function
    int x = getClk();
    printf("processGenerator: current time is %d\n", x);

    ////// reading the data from file then send it to the scheduler
    readData();
    //no more processes, send end of transmission message
    lastSend();
     cout << "mstny s3adtk " << pid_sched <<endl;
    int status;
    while(waitpid(pid_sched,&status,WNOHANG) == 0); // wait until the childern terminates 
    //////////To clear all resources
    ClearResources(0);
    //======================================
    return 0;
}

void ClearResources(int) {
    msgctl(qid, IPC_RMID, (struct msqid_ds *) 0);
    destroyClk(true);
    exit(0);
}


void readData() {
    processData pD;
    ifstream myfile("processes.txt");
    if (myfile.is_open()) {
        string line;
        while (getline(myfile, line)) {
            if (line[0] == '#') continue;
            else {
                 fillProcessData(line, pD);
                 cout << "I'm sending process: " << pD.id << " to scheduler - using algo " << pD.algo << "\n";
                 Sendmsg(pD); // send the received process to the scheduler
            }
        }
        myfile.close();
    }
}

int integer(char *ptch) {
    stringstream ss;
    int num = -1;
    ss << ptch;
    ss >> num;
    return num;
}

void fillProcessData(string line, processData &pD) {

    pD.algo = algoNum;
    char *hey = new char[line.length() + 1];
    strcpy(hey, line.c_str());
    char *ptch = strtok(hey, "\t");
    int status = 0;
    while (ptch != NULL) {
        pD.state = 0;
        switch (status) {
            case 0:
                pD.id = integer(ptch);
                status++;
                break;
            case 1:
                pD.arrivalTime = integer(ptch);
                status++;
                break;
            case 2:
                pD.runningTime = integer(ptch);
		pD.remainTime = pD.runningTime;
                status++;
                break;
            default:
                pD.priority = integer(ptch);
                break;
        }
        ptch = strtok(NULL, "\t");
    }
}

string intToString(int num) {
    string s;
    stringstream ss;
    ss << num;
    ss >> s;
    return s;
}
