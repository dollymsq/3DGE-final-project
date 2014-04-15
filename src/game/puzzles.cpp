#include "puzzles.h"


Puzzles::Puzzles()
{
    collisioncount = 0;
    solvedPuzzlesNumber = 0;
}

void Puzzles::storeSubtitles(QString info)
{
    infoToPrint = info;
    qDebug() << infoToPrint;
    solvedPuzzlesNumber++;

//    if(solvedPuzzlesNumber == 5)
}
