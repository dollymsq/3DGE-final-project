#ifndef PUZZLES_H
#define PUZZLES_H
#include <QObject>
#include <QDebug>
#include <QString>

class Puzzles : public QObject
{
    Q_OBJECT

public:
    Puzzles();

    int collisioncount;
    int solvedPuzzlesNumber;
    QString infoToPrint;

public slots:
    void storeSubtitles(QString info);
    void OnePuzzleSolved();

signals:
    void collisionReachedValue(QString);
    void puzzlesReachedValue(QString);
};

#endif // PUZZLES_H
