#include "Samples.hpp"
#include "modeltools.hpp"

#include <string>
#include <utility>
#include <vector>

namespace craam {
namespace msen {

using namespace std;
using namespace util::lang;

SampledMDP::SampledMDP() : mdp(make_shared<MDP>()) {}

void SampledMDP::add_samples(const DiscreteSamples& samples) {
    // copy the state and action counts to be
    auto old_state_action_weights = state_action_weights;

    // add transition samples
    for (size_t si : indices(samples)) {
        DiscreteSample s = samples.get_sample(si);

        // -----------------
        // Computes sample weights:
        // the idea is to normalize new samples by the same
        // value as the existing samples and then re-normalize
        // this is linear complexity
        // -----------------

        // weight used to normalize old data
        prec_t weight = 1.0; // this needs to be initialized to 1.0
        // whether the sample weight has been initialized
        bool weight_initialized = false;

        // resize transition counts
        // the actual values are updated later
        if ((size_t)s.state_from() >= state_action_weights.size()) {
            state_action_weights.resize(s.state_from() + 1);

            // we know that the value will not be found in old data
            weight_initialized = true;
        }

        // check if we have something for the action
        numvec& actioncount = state_action_weights[s.state_from()];
        if ((size_t)s.action() >= actioncount.size()) {
            actioncount.resize(s.action() + 1);

            // we know that the value will not be found in old data
            weight_initialized = true;
        }

        // update the new count
        assert(size_t(s.state_from()) < state_action_weights.size());
        assert(size_t(s.action()) < state_action_weights[s.state_from()].size());

        state_action_weights[s.state_from()][s.action()] += s.weight();

        // get number of existing transitions
        // this is only run when we do not know if we have any prior
        // sample
        if (!weight_initialized && (size_t(s.state_from()) < old_state_action_weights.size()) &&
                (size_t(s.action()) < old_state_action_weights[s.state_from()].size())) {
            size_t cnt = old_state_action_weights[s.state_from()][s.action()];

            // adjust the weight of the new sample to be consistent
            // with the previous normalization (use 1.0 if no previous action)
            weight = 1.0 / prec_t(cnt);
        }
        // ---------------------

        // adds a transition
        add_transition(*mdp, s.state_from(), s.action(), s.state_to(), weight * s.weight(), s.reward());
    }

    // make sure to set action validity based on whether there have been
    // samples observed for the action
    for (size_t si : indices(*mdp)) {
        auto& state = mdp->get_state(si);

        for (size_t ai : indices(state)) {
            auto& action = state[ai];

            // assert(si < state_action_weights.size());
            // assert(ai < state_action_weights[si].size());

            // valid only if there are some samples for the action
            action.set_validity(state_action_weights[si][ai] > 0);
        }
    }

    //  Normalize the transition probabilities and rewards
    mdp->normalize();

    // set initial distribution
    for (long state : samples.get_initial()) {
        initial.add_sample(state, 1.0, 0.0);
    }
    initial.normalize();
}
}
}
