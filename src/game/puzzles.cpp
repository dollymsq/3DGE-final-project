#include "puzzles.h"


Puzzles::Puzzles()
{
    collisioncount = 0;
    solvedPuzzlesNumber = 0;
    QObject::connect(this,SIGNAL(puzzlesSolved(QString)),
                     this, SLOT(OnePuzzleSolved(QString)));
    infoToPrint = " ";
}


void Puzzles::OnePuzzleSolved(QString info)
{
    solvedPuzzlesNumber++;
    if(solvedPuzzlesNumber == 5)
    {
        infoToPrint = info + "\nYou have won the game.";
        qDebug() << infoToPrint;
    }
    else
    {
        infoToPrint = info;
        qDebug() << infoToPrint;
    }
}






