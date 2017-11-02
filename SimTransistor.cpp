//
//  SimTransistor.cpp
//  CADisCMOS
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "SimTransistor.hpp"

#include "SimNode.hpp"

SimTransistor::SimTransistor(unsigned int u_id, TransistorType type) :
u_id_(u_id), type_(type) {
    drain_level_ = -1;
    source_level_ = -1;
    gate_level_ = -1;
    
}

SimTransistor::~SimTransistor() {
    
}

SimNodePtr SimTransistor::GetOtherNode(SimNodePtr node) {
    return ((drain_ == node) ? source_ : drain_);
}
