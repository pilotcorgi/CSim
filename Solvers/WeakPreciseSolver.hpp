//
//  WeakPreciseSolver.hpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef WeakPreciseSolver_hpp
#define WeakPreciseSolver_hpp

#include "Solver.hpp"

class WeakPreciseSolver : public Solver {
public:
    WeakPreciseSolver();
    virtual ~WeakPreciseSolver();
    
    virtual int UpdateValue(std::list<SimNodePtr> *node_list, std::set<SimNodePtr> &changed_list);
    // virtual int UpdateTransistor(SimNodePtr node, std::list<SimNodePtr> *changed_list);
};

#endif /* WeakPreciseSolver_hpp */
