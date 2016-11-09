#include "algorithms/ThompsonSampling.hpp"
#include "domains/MultiArmBandit.hpp"

namespace craam{
namespace bandits{

static double getRand(){
    return ((double) rand() / (RAND_MAX));
}

/** Samples a point from a beta distribution; the parameters are alpha and beta */
double sampleFromBetaDist(double alpha, double beta){
    double randFromUnif = getRand();
    beta_distribution<> dist(alpha, beta);
    double randFromDist = quantile(dist, randFromUnif);
    //cout<<"alpha: "<<alpha<<", beta: "<<beta<<", randFromUnif: "<<randFromUnif<<", randFromDist: "<<randFromDist<<endl;
    return randFromDist;
}

/** Thompson sampling policy */
int thompson_sampling_policy(MultiArmedBandit::State state){
    double betaSample0 = sampleFromBetaDist(state[0],state[1]);
    double betaSample1 = sampleFromBetaDist(state[2],state[3]);

    return betaSample0 > betaSample1 ? 0 : 1;
}

// Run as:  simulate(MultiArmBandit(p0,p1), thompson_sampling_policy, horizon ...) see Simulation.hpp
}}
