//
// Created by elarielcl on 26/09/18.
//

#ifndef BLOCKTREE_PBTCISA_H
#define BLOCKTREE_PBTCISA_H

#include <compressed/DAPCBlockTree.h>


class PBTCISA {
public:

    static const int PAPER = 0;
    static const int PAPER_NO_CLEAN = 1;
    static const int PRUNIG_C = 2;
    static const int HEURISTIC = 3;
    static const int LIBERAL = 4;
    static const int CONSERVATIVE = 5;

    DAPCBlockTree* disa_;

    PBTCISA(std::string&, int = PAPER, int = 2, int = 4, int = 2);
    ~PBTCISA();


    int access(int);
    int saccess(int);
    int caccess(int);
    int csaccess(int);
    int mapped_access(int);
    int mapped_saccess(int);

    int mapped_caccess(int);
    int mapped_csaccess(int);
    void print_statistics();
};

#endif //BLOCKTREE_PBTCISA_H
