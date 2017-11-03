//
//  AccurateSolver.cpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "AccurateSolver.hpp"

#include <iostream>

using namespace std;

AccurateSolver::AccurateSolver() {
}

AccurateSolver::~AccurateSolver() {
}

int AccurateSolver::UpdateValue(std::list<SimNodePtr> *node_list, std::list<SimNodePtr> *changed_list) {
    int changed_node_cnt = 0;
    NodeState new_value = ComputeNewValue(node_list);
    
    for (auto node_temp : *node_list) {
        if (IS_SOURCE(node_temp)) {
            continue;
        }
        
        node_temp->new_state_ = new_value;
        if (node_temp->new_state_ != node_temp->current_state_) {
            node_temp->current_state_ = node_temp->new_state_;
            changed_list->push_back(node_temp);
            changed_node_cnt++;
        }
    }
    
    node_list->clear();
    delete node_list;
    
    return changed_node_cnt;
}
