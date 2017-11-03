//
//  AccurateSolver.hpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef AccurateSolver_hpp
#define AccurateSolver_hpp

#include "Solver.hpp"

class AccurateSolver : public Solver {
public:
    AccurateSolver();
    virtual ~AccurateSolver();
    
    virtual int UpdateValue(std::list<SimNodePtr> *node_list, std::list<SimNodePtr> *changed_list);
    // virtual int UpdateTransistor(SimNodePtr node, std::list<SimNodePtr> *changed_list);
};

#endif /* AccurateSolver_hpp */
