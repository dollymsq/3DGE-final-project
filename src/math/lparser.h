#ifndef LPARSER_H
#define LPARSER_H

#include "math/common.h"
#include "math/lcommand.h"
#include <QPair>
#include <QHash>
#include <QVector>
#include <cmath>

class LParser
{
public:
    LParser();
    static QPair<Command,float> parseString(QString L,int &pointer);
    static QString rewrite(QHash<QChar,QVector<QPair<QString,float> > > *rules, QString input, int steps);
    static QString testTree();
    static QString smallTest();
};

#endif // LPARSER_H
