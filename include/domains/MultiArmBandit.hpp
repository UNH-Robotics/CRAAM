#pragma once

#include <random>
#include <tuple>
#include <utility>

using namespace std;

namespace craam {
namespace bandits{

/**
Models the evolution of the belief space, the reward probabilities depend on the state.
This is *NOT* the simulator to be used for evaluation. The probability of a success for each arm depends 
on the state.

State:
( Arm0 positive count, Arm0 negative count, Arm1 positive count, Arm1 negative count )

Action: {0,1} = arm to pull

Careful with parallelization; shared random 
*/
class MultiArmedBandit_BeliefSpace {
public:
    typedef tuple<long,long,long,long> State;
    typedef int Action; // TODO: change it to enum class?

    /** Constructor */
    MultiArmedBandit(random_device::result_type seed = random_device{}): generator(seed) {};

    /** The initial state in simulations */
    State init_state(){return make_tuple(1,1,1,1)};

    /** Probabilistic transition function */
    pair<double,State> transition(State s, Action a){
        assert(a == 0 || a == 1);

        if(a == 0){
            bernoulli_distribution dst(s[0]/(s[0]+s[1]));
            return dst(generator) ? make_pair(1.0, make_tuple(s[0]+1,s[1],s[2],s[3])) :
                                    make_pair(0.0, make_tuple(s[0],s[1]+1,s[2],s[3]));
        }else if(a == 1){ // a == 1
            bernoulli_distribution dst(s[2]/(s[2]+s[3]));
            return dst(generator) ? make_pair(1.0, make_tuple(s[0],s[1],s[2]+1,s[3])) :
                                    make_pair(0.0, make_tuple(s[0],s[1],s[2],s[3]+1));
        }else{
            throw InvalidOperationException("Unknown action");
        }
    }

protected:
    default_random_engine generator(seed);
};


/**
Models the *real* multi-armed bandit. The probability of a success for each arm is
independent from the state! It is given when the simulator is initialized. 

State:
( Arm0 positive count, Arm0 negative count, Arm1 positive count, Arm1 negative count )

Action: {0,1} = arm to pull

Careful with parallelization; shared random 
*/
class MultiArmedBandit {
public:
    typedef tuple<long,long,long,long> State;
    typedef int Action; // TODO: change it to enum class?

    /** Constructor
    \param p0 Probability of success of arm 0
    \param p1 Probability of success of arm 1 
    */
    MultiArmedBandit(double p0, double p1, random_device::result_type seed = random_device{}): 
        generator(seed), p0(p0), p1(p1)
    {
        assert(p0 >= 0 && p0 <=1);
        assert(p1 >= 0 && p1 <=1);
    };

    /** The initial state in simulations */
    State init_state(){return make_tuple(1,1,1,1)};

    /** Probabilistic transition function */
    pair<double,State> transition(State s, Action a){
        assert(a == 0 || a == 1);

        if(a == 0){
            bernoulli_distribution dst(p0);
            return dst(generator) ? make_pair(1.0, make_tuple(s[0]+1,s[1],s[2],s[3])) :
                                    make_pair(0.0, make_tuple(s[0],s[1]+1,s[2],s[3]));
        }else if(a == 1){ // a == 1
            bernoulli_distribution dst(p1);
            return dst(generator) ? make_pair(1.0, make_tuple(s[0],s[1],s[2]+1,s[3])) :
                                    make_pair(0.0, make_tuple(s[0],s[1],s[2],s[3]+1));
        }else{
            throw InvalidOperationException("Unknown action");
        }
    }

protected:
    default_random_engine generator(seed);
    double p0, p1;
};


}}
