#pragma once

#include "RMDP.hpp"
#include "Transition.hpp"

#include <memory>
#include <random>
#include <vector>

using namespace std;

namespace craam {

/// A namespace with tools for implementable, interpretable, and aggregated MDPs
namespace impl {

/**
Represents an MDP with implementability constraints.

Consists of an MDP and a set of observations.
*/
class MDPI {
public:
    /**
    Constructs the MDP with implementability constraints. This constructor makes it
    possible to share the MDP with other data structures.

    Important: when the underlying MDP changes externally, the object becomes invalid
    and may result in unpredictable behavior.

    \param mdp A non-robust base MDP model.
    \param state2observ Maps each state to the index of the corresponding observation.
                    A valid policy will take the same action in all states
                    with a single observation. The index is 0-based.
    \param initial A representation of the initial distribution. The rewards
                    in this transition are ignored (and should be 0).
    */
    MDPI(const shared_ptr<const MDP>& mdp, const indvec& state2observ, const Transition& initial);

    /**
    Constructs the MDP with implementability constraints. The MDP model is
    copied (using the copy constructor) and stored internally.
    \param mdp A non-robust base MDP model. It cannot be shared to prevent
                direct modification.
    \param state2observ Maps each state to the index of the corresponding observation.
                    A valid policy will take the same action in all states
                    with a single observation. The index is 0-based.
    \param initial A representation of the initial distribution. The rewards
                    in this transition are ignored (and should be 0).
    */
    MDPI(const MDP& mdp, const indvec& state2observ, const Transition& initial);

    size_t obs_count() const { return obscount; };
    size_t state_count() const { return mdp->state_count(); };
    long state2obs(long state) { return state2observ[state]; };
    size_t action_count(long obsid) { return action_counts[obsid]; };

    /**
    Converts a policy defined in terms of observations to a policy defined in
    terms of states.
    \param obspol Policy that maps observations to actions to take
    \return Observation policy
    */
    indvec obspol2statepol(const indvec& obspol) const;
    /**
    Converts a policy defined in terms of observations to a policy defined in
    terms of states.
    \param obspol Policy that maps observations to actions to take
    \param statepol State policy target
    */
    void obspol2statepol(const indvec& obspol, indvec& statepol) const;

    /**
    Converts a transition from states to observations, adding probabilities
    of individual states. Rewards are a convex combination of the original
    values.
    */
    Transition transition2obs(const Transition& tran);

    /** Internal MDP representation */
    shared_ptr<const MDP> get_mdp() { return mdp; };

    /** Initial distribution of MDP */
    Transition get_initial() const { return initial; };

    /** Constructs a random observation policy */
    indvec random_policy(random_device::result_type seed = random_device{}());

    /**
    Computes a return of an observation policy.

    \param obspol Policy in terms of observations
    \param discount Discount factor
    \return Discounted return of the policy
    */
    prec_t total_return(const indvec& obspol, prec_t discount, prec_t precision = SOLPREC) const;

    // save and load description.
    /**
    Saves the MDPI to a set of 3 csv files, for transitions,
    observations, and the initial distribution

    \param output_mdp Transition probabilities and rewards
    \param output_state2obs Mapping states to observations
    \param output_initial Initial distribution
    */
    void to_csv(ostream& output_mdp, ostream& output_state2obs, ostream& output_initial, bool headers = true) const;

    /**
    Saves the MDPI to a set of 3 csv files, for transitions, observations,
    and the initial distribution

    \param output_mdp File name for transition probabilities and rewards
    \param output_state2obs File name for mapping states to observations
    \param output_initial File name for initial distribution
    */
    void to_csv_file(const string& output_mdp,
            const string& output_state2obs,
            const string& output_initial,
            bool headers = true) const;

    /**
    Loads an MDPI from a set of 3 csv files, for transitions, observations,
    and the initial distribution

    The MDP size is defined by the transitions file.

    \param input_mdp File name for transition probabilities and rewards
    \param input_state2obs File name for mapping states to observations
    \param input_initial File name for initial distribution
     */
    template <typename T = MDPI>
    static unique_ptr<T> from_csv(istream& input_mdp,
            istream& input_state2obs,
            istream& input_initial,
            bool headers = true);
    template <typename T = MDPI>
    static unique_ptr<T> from_csv_file(const string& input_mdp,
            const string& input_state2obs,
            const string& input_initial,
            bool headers = true);

protected:
    /** the underlying MDP */
    shared_ptr<const MDP> mdp;
    /** maps index of a state to the index of the observation */
    indvec state2observ;
    /** initial distribution */
    Transition initial;
    /** number of observations */
    long obscount;
    /** number of actions for each observation */
    indvec action_counts;

    /**
     Checks whether the parameters are correct. Throws an exception if the parameters
     are wrong.
     */
    static void check_parameters(const MDP& mdp, const indvec& state2observ, const Transition& initial);
};

/**
An MDP with implementability constraints. The class contains solution
methods that rely on robust MDP reformulation of the problem.

Uses L1 version of the robust MDP
 */
class MDPI_R : public MDPI {
public:
    /**
    Calls the base constructor and also constructs the corresponding
    robust MDP
     */
    MDPI_R(const shared_ptr<const MDP>& mdp, const indvec& state2observ, const Transition& initial);

    /**
    Calls the base constructor and also constructs the corresponding
    robust MDP.
    */
    MDPI_R(const MDP& mdp, const indvec& state2observ, const Transition& initial);

    const RMDP_L1& get_robust_mdp() const {
        /** Returns the internal robust MDP representation  */
        return robust_mdp;
    };

    /**
    Updates the weights on outcomes in the robust MDP based on the state
    weights provided.

    This method modifies the stored robust MDP.
     */
    void update_importance_weights(const numvec& weights);

    /**
    Uses a simple iterative algorithm to solve the MDPI.

    The algorithm starts with a policy composed of actions all 0, and
    then updates the distribution of robust outcomes (corresponding to MDP states),
    and computes the optimal solution for thus weighted RMDP.

    This method modifies the stored robust MDP.

    \param iterations Maximal number of iterations; terminates when the policy no longer changes
    \param discount Discount factor
    \param initobspol Initial observation policy (optional). When omitted or has length 0
        a policy that takes the first action (action 0) is used.
    \returns Policy for observations (an index of each action for each observation)
    */
    indvec solve_reweighted(long iterations, prec_t discount, const indvec& initpol = indvec(0));

    /**
    Uses a robust MDP formulation to solve the MDPI. States in the observation are treated
    as outcomes. The baseline distribution is inferred from the provided policy.

    The uncertainty is bounded by using an L1 norm deviation and the provided
    threshold.

    The method can run for several iterations, like solve_reweighted.

    \param iterations Maximal number of iterations; terminates when the policy no longer changes
    \param threshold Upper bound on the L1 deviation from the baseline distribution.
    \param discount Discount factor
    \param initobspol Initial observation policy (optional). When omitted or has length 0
        a policy that takes the first action (action 0) is used.
    \returns Policy for observations (an index of each action for each observation)
    */
    indvec solve_robust(long iterations, prec_t threshold, prec_t discount, const indvec& initpol = indvec(0));

    static unique_ptr<MDPI_R> from_csv(istream& input_mdp,
            istream& input_state2obs,
            istream& input_initial,
            bool headers = true) {
        return MDPI::from_csv<MDPI_R>(input_mdp, input_state2obs, input_initial, headers);
    };

    /** Loads the class from an set of CSV files. See also from_csv. */
    static unique_ptr<MDPI_R> from_csv_file(const string& input_mdp,
            const string& input_state2obs,
            const string& input_initial,
            bool headers = true) {
        return MDPI::from_csv_file<MDPI_R>(input_mdp, input_state2obs, input_initial, headers);
    };

protected:
    /** Robust representation of the MDPI */
    RMDP_L1 robust_mdp;
    /** Maps the index of the mdp state to the index of the observation
    within the state corresponding to the observation (multiple states per observation) */
    indvec state2outcome;
    /** Constructs a robust version of the implementable MDP.*/
    void initialize_robustmdp();
};
}
}
