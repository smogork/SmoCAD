//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_UTILITES_H
#define SMOCAD_UTILITES_H

#define ASSIGN_NOTIFIER_FUNCTION(F)  std::bind(F, this)
#define ASSIGN_CONTEXT_FUNCTION(F)  std::bind(F, this, std::placeholders::_1)

#endif //SMOCAD_UTILITES_H
