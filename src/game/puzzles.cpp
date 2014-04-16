#include "puzzles.h"


Puzzles::Puzzles()
{
    collisioncount = 0;
    solvedPuzzlesNumber = 0;
    QObject::connect(this,SIGNAL(puzzlesReachedValue(QString)),
                     this, SLOT(storeSubtitles(QString)));
}

void Puzzles::storeSubtitles(QString info)
{
    infoToPrint = info;
    qDebug() << infoToPrint;
}


void Puzzles::OnePuzzleSolved()
{

    solvedPuzzlesNumber++;
    if(solvedPuzzlesNumber == 5)
        emit puzzlesReachedValue("You have found the hidden box");
}


