//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_TYPENUMBERING_H
#define SMOCAD_TYPENUMBERING_H

//https://gpfault.net/posts/mapping-types-to-values.txt.html
#include <atomic>

extern std::atomic_int TypeIdCounter;

template <typename T>
int getTypeId() {
    static int id = ++TypeIdCounter;
    return id;
}


#endif //SMOCAD_TYPENUMBERING_H
