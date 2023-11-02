#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../Zeta/CountingBinTree.h"
#include "../Zeta/OrdinaryCountingBinColoredTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/random.h"
#include "../Zeta/utils.h"
#include "SimpleVector.h"
#include "test_head.h"

int mkfifo(const char *pathname, mode_t mode);

int main() {
    int x;

    mkfifo("zetaexefifo_in", 0666);
    mkfifo("zetaexefifo_out", 0666);

    return 0;
}
