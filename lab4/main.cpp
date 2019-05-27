#include <iostream>
#include "test.h"
#include "Optimal.h"
#include "FIFO.h"
#include "LRU.h"
#include "Clock.h"
#include "PBA.h"
int main()
{
    MemSchedule schedule= GeneVisitSeq(32);
//    Optimal(schedule);
//    Fifo(schedule) ;
//    LRU(schedule);
//    Clock(schedule);
    PBA(schedule);
    return 0;

}