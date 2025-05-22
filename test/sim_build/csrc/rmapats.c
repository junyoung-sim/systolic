// file = 0; split type = patterns; threshold = 100000; total count = 0.
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "rmapats.h"

void  schedNewEvent (struct dummyq_struct * I1460, EBLK  * I1455, U  I627);
void  schedNewEvent (struct dummyq_struct * I1460, EBLK  * I1455, U  I627)
{
    U  I1744;
    U  I1745;
    U  I1746;
    struct futq * I1747;
    struct dummyq_struct * pQ = I1460;
    I1744 = ((U )vcs_clocks) + I627;
    I1746 = I1744 & ((1 << fHashTableSize) - 1);
    I1455->I673 = (EBLK  *)(-1);
    I1455->I674 = I1744;
    if (0 && rmaProfEvtProp) {
        vcs_simpSetEBlkEvtID(I1455);
    }
    if (I1744 < (U )vcs_clocks) {
        I1745 = ((U  *)&vcs_clocks)[1];
        sched_millenium(pQ, I1455, I1745 + 1, I1744);
    }
    else if ((peblkFutQ1Head != ((void *)0)) && (I627 == 1)) {
        I1455->I676 = (struct eblk *)peblkFutQ1Tail;
        peblkFutQ1Tail->I673 = I1455;
        peblkFutQ1Tail = I1455;
    }
    else if ((I1747 = pQ->I1361[I1746].I696)) {
        I1455->I676 = (struct eblk *)I1747->I694;
        I1747->I694->I673 = (RP )I1455;
        I1747->I694 = (RmaEblk  *)I1455;
    }
    else {
        sched_hsopt(pQ, I1455, I1744);
    }
}
#ifdef __cplusplus
extern "C" {
#endif
void SinitHsimPats(void);
#ifdef __cplusplus
}
#endif
