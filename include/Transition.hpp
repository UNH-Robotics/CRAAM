#pragma once

#include "definitions.hpp"

#include <string>
#include <vector>

using namespace std;

namespace craam {

const prec_t tolerance = 1e-5;

/**
  Represents sparse transition probabilities and rewards from a single state.
  The class can be also used to represent a generic sparse distribution.

  The destination indexes are sorted increasingly (as added). This makes it simpler to
  aggregate multiple transition probabilities and should also make value iteration
  more cache friendly. However, transitions need to be added with increasing IDs to
  prevent excessive performance degradation.
 */
class Transition {
public:
    Transition(){};

    /**
    Creates a single transition from raw data.

    Because the transition indexes are stored increasingly sorted, this method
    must sort (and aggregate duplicate) the indices.

    \param indices The indexes of states to transition to
    \param probabilities The probabilities of transitions
    \param rewards The associated rewards with each transition
    */
    Transition(const indvec& indices, const numvec& probabilities, const numvec& rewards);

    /**
    Creates a single transition from raw data with uniformly zero rewards.

    Because the transition indexes are stored increasingly sorted, this method
    must sort (and aggregate duplicate) the indices.

    \param indices The indexes of states to transition to
    \param probabilities The probabilities of transitions
    */
    Transition(const indvec& indices, const numvec& probabilities);

    /**
    Creates a single transition from raw data with uniformly zero rewards,
    where destination states are indexed automatically starting with 0.

    \param probabilities The probabilities of transitions; indexes are implicit.
    */
    Transition(const numvec& probabilities);

    /**
    Adds a single transitions probability to the existing probabilities.

    If the transition to a state does not exist, then it is simply added to the
    list. If the transition to the desired state already exists, then the transition
    probability is added and the reward is updated as a weighted combination.
    Let \f$ p(s) \f$ and \f$ r(s) \f$ be the current transition probability
    and reward respectively. The updated transition probability and reward are:
        - Probability:
            \f[ p'(s) = p(s) + p \f]
        - Reward:
            \f[ r'(s) = \frac{p(s) \, r(s) + p \, r}{p'(s)} \f]
    Here, \f$ p \f$ is the argument probability and \f$ r \f$ is the argument
    reward.

    When the function is called multiple times with \f$ p_1 \ldots p_n \f$ and
    \f$  r_1 \ldots r_n \f$ for a single \f$ s \f$ then:
        - Probability:
            \f[ p'(s) = \sum_{i=1}^{n} p_i \f]
        - Reward:
            \f[ r'(s) = \frac{  \sum_{i=1}^{n} p_i \, r_i}{p'(s)} \f]


    Transition probabilities are not checked to sum to one.

    \param stateid ID of the target state
    \param probability Probability of transitioning to this state
    \param reward The reward associated with the transition
     */
    void add_sample(long stateid, prec_t probability, prec_t reward);

    prec_t sum_probabilities() const;

    /**
    Normalizes the transition probabilities to sum to 1. Exception is thrown if the
    distribution sums to 0.
    */
    void normalize();

    /** \returns Whether the transition probabilities sum to 1. */
    bool is_normalized() const;

    /**
    Computes value for the transition and a value function.

    When there are no target states, the function terminates with an error.

    \param valuefunction Value function, or an arbitrary vector of values
    \param discount Discount factor, optional (default value 1)
     */
    prec_t compute_value(numvec const& valuefunction, prec_t discount = 1.0) const;

    /** Computes the mean return from this transition */
    prec_t mean_reward() const;

    /** Returns the number of target states with non-zero transition probabilities.  */
    size_t size() const { return indices.size(); };

    /** Checks if the transition is empty. */
    bool empty() const { return indices.empty(); };

    /**
    Returns the maximal indexes involved in the transition.
    Returns -1 for and empty transition.
    */
    long max_index() const { return indices.empty() ? -1 : indices.back(); };

    /**
    Scales transition probabilities according to the provided parameter
    and adds them to the provided vector. This method ignores rewards.
    \param scale Multiplicative modification of transition probabilities
    \param transition Transition probabilities being added to. This value
                        is modified within the function.
    */
    void probabilities_addto(prec_t scale, numvec& transition) const;

    /**
    Scales transition probabilities and rewards according to the provided parameter
    and adds them to the provided vector.

    \param scale Multiplicative modification of transition probabilities
    \param transition Transition probabilities being added to. This value
                        is modified within the function.
    */
    void probabilities_addto(prec_t scale, Transition& transition) const;

    /**
    Constructs and returns a dense vector of probabilities, which
    includes 0 transition probabilities.
    \param size Size of the constructed vector
    */
    numvec probabilities_vector(size_t size) const;

    /**
    Constructs and returns a dense vector of rewards, which
    includes 0 transition probabilities. Rewards for indices with
    zero transition probability are zero.
    \param size Size of the constructed vector
    */
    numvec rewards_vector(size_t size) const;

    /**
    Indices with positive probabilities.
    */
    const indvec& get_indices() const { return indices; };
    /**
    Returns list of positive probabilities for indexes returned by
    get_indices. See also probabilities_vector.
    */
    const numvec& get_probabilities() const { return probabilities; };
    /**
    Rewards for indices with positive probabilities returned by
    get_indices. See also rewards_vector.
    */
    const numvec& get_rewards() const { return rewards; };

    /** Sets the reward for a transition to a particular state */
    void set_reward(long sampleid, prec_t reward) { rewards[sampleid] = reward; };
    /** Gets the reward for a transition to a particular state */
    prec_t get_reward(long sampleid) const { return rewards[sampleid]; };

    /** Returns a json representation of transition probabilities
    \param outcomeid Includes also outcome id*/
    string to_json(long outcomeid = -1) const;

protected:
    /// List of state indices
    indvec indices;
    /// List of probability distributions to states
    numvec probabilities;
    /// List of rewards associated with transitions
    numvec rewards;
};
}
