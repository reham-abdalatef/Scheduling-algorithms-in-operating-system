#include "clkUtilities.h"
#include <bits/stdc++.h>
using namespace std;

/* Modify this file as needed*/
int remainingTime = 0;

int stringToInt(string s) {
    int num;
    stringstream ss;
    ss << s;
    ss >> num;
    return num;
}

int main(int argc, char* argv[]) {

    //if you need to use the emulated clock uncomment the following line
    initClk();

    //TODO: it needs to get the remaining time from somewhere
    remainingTime = stringToInt(argv[0]);

    while (remainingTime > 0) {
        sleep(1);
        remainingTime--;
    }
    //if you need to use the emulated clock uncomment the following line
    destroyClk(false);
    return 0;
}
