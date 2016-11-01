#include "UCB.h"

static double getRand(){
    return ((double) rand() / (RAND_MAX));
}

void applyUCB1(){
    cout<<"UCB1"<<endl;

    ofstream outfile;
    outfile.open("UCB_output.dat");

    int state[4], actionCount[2], numLevel=100, numState=2;
    double payOff[2];

    for(int i=0;i<4;i++){
        //cout<<"i: "<<i<<endl;
        state[i] = 1;
    }

    //Play each action once to init the mean payoffs
    for(int i=0;i<numState;i++){
        double p = getRand();
        if(p<0.5) payOff[i] = 1;
        else payOff[i] = 0;

        actionCount[i] = 1;
    }

    for(int i=1;i<numLevel;i++){
        double maxActionVal = -1;
        int maxAction = 0;

        //find max action
        for(int j=0;j<numState;j++){
            double val = payOff[j] + sqrt( (2 * log2(i)) / actionCount[j] );
            if(maxActionVal<val){
                maxActionVal = val;
                maxAction = j;
            }
        }

        double r = getRand();
        if(maxAction==0){
            double mean = ((double)state[0]) / ((double)state[0]+ (double)state[1]);
            if(r<mean){
                state[0]++;
                payOff[0] = (payOff[0]*actionCount[0]+1)/(actionCount[0]+1);
                outfile<<"pull arm: 1, reward: 1, payOff: "<<payOff[0]<<", mean: "<<mean<<endl;
            } else{
                state[1]++;
                payOff[0] = (payOff[0]*actionCount[0]+0)/(actionCount[0]+1);
                outfile<<"pull arm: 1, reward: 0, payOff: "<<payOff[0]<<", mean: "<<mean<<endl;
            }
            actionCount[0]++;
        } else{
            double mean = ((double)state[2]) / ((double)state[2]+ (double)state[3]);
            if(r<mean){
                state[2]++;
                payOff[1] = (payOff[1]*actionCount[1]+1)/(actionCount[1]+1);
                outfile<<"pull arm: 2, reward: 1, payOff: "<<payOff[1]<<", mean: "<<mean<<endl;
            } else{
                state[3]++;
                payOff[1] = (payOff[1]*actionCount[1]+0)/(actionCount[1]+1);
                outfile<<"pull arm: 2, reward: 0, payOff: "<<payOff[1]<<", mean: "<<mean<<endl;
            }
            actionCount[1]++;
        }
    }
}

