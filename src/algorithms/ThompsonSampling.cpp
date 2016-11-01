#include "algorithms/ThompsonSampling.hpp"

static double getRand(){
    return ((double) rand() / (RAND_MAX));
}

double sampleFromBetaDist(double alpha, double beta){
    double randFromUnif = getRand();
    beta_distribution<> dist(alpha, beta);
    double randFromDist = quantile(dist, randFromUnif);
    //cout<<"alpha: "<<alpha<<", beta: "<<beta<<", randFromUnif: "<<randFromUnif<<", randFromDist: "<<randFromDist<<endl;
    return randFromDist;
}

void applyThompsonSampling(){
    cout<<"ThompsonSampling"<<endl;

    ofstream outfile;
    outfile.open("ThompsonSampling_output.dat");

    int state[4];
    for(int i=0;i<4;i++){
        //cout<<"i: "<<i<<endl;
        state[i] = 1;
    }

    int numLevel=100;
    for(int i=0;i<numLevel;i++){
        //outfile<<"I: "<<i<<" --------------- "<<endl;
        double betaSample1 = sampleFromBetaDist(state[0],state[1]);
        double betaSample2 = sampleFromBetaDist(state[2],state[3]);
        double r = getRand();

        if(betaSample1>betaSample2){
            //pull arm 1

            double mean = ( (double)state[0]/( (double)state[0] + (double)state[1]));
            //outfile<<"r: "<<r<<", mean: "<<mean<<endl;

            if(r<mean){
                //got a reward of 1 by pulling arm 1
                state[0]++;
                outfile<<"pull arm: 1, reward 1, mean: "<<mean<<endl;
            } else{
                //got a reward of 0 by pulling arm 1
                state[1]++;
                outfile<<"pull arm: 1, reward 0, mean: "<<mean<<endl;
            }
        } else{
            double mean = ( (double)state[2]/( (double)state[2] + (double)state[3]));
            //outfile<<"r: "<<r<<", mean: "<<mean<<endl;

            if(r<mean){
                //got a reward of 1 by pulling arm 2
                state[2]++;
                outfile<<"pull arm: 2, reward 1, mean: "<<mean<<endl;
            } else{
                //got a reward of 0 by pulling arm 2
                state[3]++;
                outfile<<"pull arm: 2, reward 0, mean: "<<mean<<endl;
            }
        }
    }
}

