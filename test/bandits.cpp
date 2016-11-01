#include "RMDP.hpp"
#include "modeltools.hpp"

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;
using namespace craam;

int main(void) {
    vector<long> vec;
    int numLevel = 6;
    for (int i = 4; i <= numLevel; i++) {
        cout << i << " --------------- " << endl;
        for (int j = 1; j <= i - 3; j++) {
            for (int k = 1; k <= i - j - 2; k++) {
                for (int m = 1; m <= i - j - k - 1; m++) {
                    // cout<<(i-j-k-m)<<" "<<j<<" "<<k<<" "<< m <<endl;
                    long num = (i - j - k - m) * 1000 + j * 100 + k * 10 + m;
                    cout << num << endl;
                    vec.push_back(num);
                }
            }
        }
    }

    cout << "vecSize: " << vec.size() << endl;

    MDP mdp(15);

    for (int i = 0; i < vec.size(); i++) {
        for (int j = i + 1; j < vec.size(); j++) {
            int num1[4], num2[4];
            long x = vec[i], y = vec[j];
            int mult = 1000, diff = 0, ind = 0, reward = 0;

            for (int k = 0; k < 4; k++) {
                num1[k] = x / mult;
                x %= mult;
                num2[k] = y / mult;
                y %= mult;
                diff += abs(num1[k] - num2[k]);
                if (num2[k] - num1[k] == 1) {
                    ind = k;
                    if (k == 0 || k == 2)
                        reward = 1;
                    else
                        reward = 0;
                }
                mult /= 10;
            }

            if (diff == 1) {
                double trp = 0;
                int action = 0;
                if (ind == 0)
                    trp = ((double)num1[0] / ((double)num1[0] + (double)num1[1]));
                if (ind == 1)
                    trp = ((double)num1[1] / ((double)num1[0] + (double)num1[1]));
                if (ind == 2)
                    trp = ((double)num1[2] / ((double)num1[2] + (double)num1[3]));
                if (ind == 3)
                    trp = ((double)num1[3] / ((double)num1[2] + (double)num1[3]));

                if (ind < 2)
                    action = 1;
                else
                    action = 2;

                cout << "Link: " << vec[i] << " and " << vec[j] << ", action: " << action << ", trp: " << trp
                     << ", reward: " << reward << endl;
                add_transition(mdp, vec[i], action, vec[j], trp, reward);
            }
        }
    }

    cout << "Linking done." << endl;
    auto&& re = mdp.vi_gs(Uncertainty::Robust, 0.9);

    for (auto v : re.valuefunction) {
        cout << v << " ";
    }

    /*MDP mdp(3);

    // transitions for action 0
    add_transition(mdp,0,0,0,1,0);
    add_transition(mdp,1,0,0,1,1);
    add_transition(mdp,2,0,1,1,1);

    // transitions for action 1
    add_transition(mdp,0,1,1,1,0);
    add_transition(mdp,1,1,2,1,0);
    add_transition(mdp,2,1,2,1,1.1);

    // solve using Jacobi value iteration
    //auto&& re = mdp.mpi_jac(Uncertainty::Average,0.9);
    auto&& re = mdp.vi_gs(Uncertainty::Average,0.9);

    for(auto v : re.valuefunction){
        cout << v << " ";
    }*/

    return 0;
}