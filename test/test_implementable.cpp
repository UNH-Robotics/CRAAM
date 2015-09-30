#include "RMDP.hpp"
#include "ImMDP.hpp"
#include "Transition.hpp"

#include <iostream>

using namespace std;
using namespace craam;
using namespace craam::impl;


#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MAIN

//#define BOOST_TEST_MODULE MainModule
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( simple_construct_imdp ) {

    shared_ptr<RMDP> mdp(new RMDP());
    vector<long> observations({0,0});
    Transition initial(vector<long>{0,1},vector<prec_t>{0.5,0.5},vector<prec_t>{0,0});

    mdp->add_transition_d(0,0,1,1.0,1.0);
    mdp->add_transition_d(1,0,0,1.0,1.0);

    IMDP im(const_pointer_cast<const RMDP>(mdp), observations, initial);

}