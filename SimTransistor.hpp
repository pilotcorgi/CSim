//
//  SimTransistor.hpp
//  CADisCMOS
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef SimTransistor_hpp
#define SimTransistor_hpp

#include <memory>

#define CMOS_TRANSISTOR_PORT_CNT    3

typedef enum tagTransistorType {
    pmos = 0,
    nmos
} TransistorType;

typedef enum tagTransistorState {
    UNKNOWN = 0,
    STRONG_ON, STRONG_OFF, WEAK_ON, RATIOED_STATE, INVALID_STATE
} TransistorState;

class SimNode;

class SimTransistor {
public:
    SimTransistor(unsigned int u_id, TransistorType type);
    ~SimTransistor();
    
    std::shared_ptr<SimNode> GetOtherNode(std::shared_ptr<SimNode> node);
    
    int drain_level_;
    int source_level_;
    int gate_level_;
    
    std::shared_ptr<SimNode> drain_;
    std::shared_ptr<SimNode> source_;
    TransistorState state_, new_state_;
    TransistorType type_;
    
private:
    const unsigned int u_id_;
};

typedef std::shared_ptr<SimTransistor> SimTransistorPtr;

#endif /* SimTransistor_hpp */
