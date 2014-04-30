#include "lparser.h"
//#include "math/command.h"
#include "math/common.h"
#include <QPair>
#include <QVector>
#include <QHash>
#include <QString>
#include <iostream>

LParser::LParser()
{
}

QPair<Command,float> LParser::parseString(QString L, int &pointer) {
    QChar c = L.at(pointer);
//    std::cout << "curr char: " << c.toLatin1() << std::endl;
    QPair<Command,int> toReturn;
    if(c == '[')  toReturn.first = PUSH;
    else if(c == ']') toReturn.first = POP;
    else if(c == 'e') toReturn.first = STOP;
    else {
//        toReturn.second = L[pointer+2] - '0';
        if(c == 'f') toReturn.first = f;
        else if(c == 'F') toReturn.first = F;
        else if(c == '+') toReturn.first = YAW;
        else if(c == '-') toReturn.first = NEGYAW;
        else if(c == '&') toReturn.first = PITCH;
        else if(c == '^') toReturn.first = NEGPITCH;
        else if(c == '\\') toReturn.first = ROLL;
        else if(c == '/') toReturn.first = NEGROLL;
        else if(c == '!') toReturn.first = LINEWIDTH;
        else {toReturn.first = IGNORE;}
        //pointer += 2;
        QString num;
        pointer++;
        while(L.at(++pointer) != ')')  {
            num.append(L.at(pointer));
        }
        toReturn.second = num.toFloat();
//        if(toReturn.first == LINEWIDTH) std::cout << "linewidth: " << toReturn.second << std::endl;
    }
    pointer++;
    return toReturn;
}

QString LParser::rewrite(QHash<QChar,QVector<QPair<QString,float> > > *rules, QString input, int steps)  {
//    std::cout << "steps: " << steps << ", input: " << input.toStdString() << std::endl;
    if(steps == 0) return input;
    QString q;
    for (int i = 0; i < input.size(); i++)  {
        QChar flag = input.at(i);
//        std::cout << "flag: " << flag.toLatin1() << std::endl;
        if (!rules->contains(flag)) {
            q.append(flag);
            if(flag == 'e') break;
            if(flag == ']' || flag == '[') continue;
            while(input.at(i) != ')')  {
                q.append(input.at(++i));
            }
            continue;
        }
        QVector<QPair<QString,float> > rule = rules->value(flag);
        QPair<QString,float> ruleToUse;

        QString paramString;
        ++i;
        while(input.at(++i) != ')') {
            paramString.append(input.at(i));
        }

        float param = paramString.toFloat();

        float randNum = ((float) rand() / (RAND_MAX));
//        std::cout << "randomNum: " << randNum << std::endl;
        float totalProb = 0;
        for(int j = 0; j < rule.length(); j++)  {
            totalProb += rule.at(j).second;
//            std::cout << "randomNum: " << randNum << ", totalProb: " << totalProb << std::endl;
            if(randNum <= totalProb)  {
                ruleToUse = rule.at(j);
                break;
            }
        }
        QString replaceString = ruleToUse.first;
//        if(flag == 'A') std::cout << "replaced by: " << replaceString.toStdString() << std::endl;
        QString newReplaceString;
        for(int j = 0; j < replaceString.length(); j++) {
            newReplaceString.append(replaceString.at(j));
            if(replaceString.at(j) == '(' && replaceString.at(j+1) == 'x')  {
                j = j+2;
                Operation o;
                if(replaceString.at(j) == '+') o = ADD;
                else if(replaceString.at(j) == '-') o = SUBTRACT;
                else if(replaceString.at(j) == '*') o = MULTIPLY;
                else if(replaceString.at(j) == '/') o = DIVIDE;
                else o = EXPONENT;
                QString numToBe;
                while(replaceString.at(++j) != ')')  {
                    numToBe.append(replaceString.at(j));
                }
                float numOperator = numToBe.toFloat();
                switch(o)  {
                    case ADD:
                        param += numOperator;
                        break;
                    case SUBTRACT:
                        param -= numOperator;
                        break;
                    case MULTIPLY:
                        param *= numOperator;
                        break;
                    case DIVIDE:
                        param /= numOperator;
                        break;
                    case EXPONENT:
                        param = std::pow(param,numOperator);
                        break;
                    default:
                        break;
                }

                QString str;
                str.setNum(param,'g',4);
                for(int k = 0; k < str.length(); k++)  {
                    newReplaceString.append(str.at(k));
                }
                newReplaceString.append(replaceString.at(j));
            }
        }
        q.append(newReplaceString);
    }
    return LParser::rewrite(rules,q,steps-1);
}

QString LParser::testTree()  {
    int steps = 10;
    QString input = "!(1)F(28)A()e";
    QHash<QChar,QVector<QPair<QString,float> > > *rules = new QHash<QChar,QVector<QPair<QString,float> > >();
    QVector<QPair<QString, float> > firstInsert;
    QPair<QString,float> firstInsertPair;
    firstInsertPair.first = "!(x+2)";
    firstInsertPair.second = 1.0f;
    firstInsert.append(firstInsertPair);
    rules->insert('!',firstInsert);

    QVector<QPair<QString, float> > secondInsert;
    QPair<QString,float> secondInsertPairOne, secondInsertPairTwo;
    secondInsertPairOne.second = .5;
    secondInsertPairOne.first = "/(137)[+(20)!(1)F(6)A()]-(20)!(1)F(6)A()";
    secondInsertPairTwo.second = .5;
    secondInsertPairTwo.first = "/(137)-(20)!(1)F(6)A()";
    secondInsert.append(secondInsertPairOne);
    secondInsert.append(secondInsertPairTwo);

    rules->insert('A',secondInsert);

    QString rewritten = LParser::rewrite(rules,input,steps);

    delete rules;

//    std::cout << rewritten.toStdString() << std::endl;
//    std::cout << std::endl;

    return rewritten;

}

QString LParser::testTreeEnd()  {
    int steps = 7;
    QString input = "!(1)F(48)A()e";
    QHash<QChar,QVector<QPair<QString,float> > > *rules = new QHash<QChar,QVector<QPair<QString,float> > >();
    QVector<QPair<QString, float> > firstInsert;
    QPair<QString,float> firstInsertPair;
    firstInsertPair.first = "!(x+2)";
    firstInsertPair.second = 1.0f;
    firstInsert.append(firstInsertPair);
    rules->insert('!',firstInsert);

    QVector<QPair<QString, float> > secondInsert;
    QPair<QString,float> secondInsertPairOne, secondInsertPairTwo;
    secondInsertPairOne.second = 1;
    secondInsertPairOne.first = "/(137)[+(20)!(1)F(16)A()]-(20)!(1)F(16)A()";
//    secondInsertPairTwo.second = .5;
//    secondInsertPairTwo.first = "/(137)-(20)!(1)F(6)A()";
    secondInsert.append(secondInsertPairOne);
//    secondInsert.append(secondInsertPairTwo);

    rules->insert('A',secondInsert);

    QString rewritten = LParser::rewrite(rules,input,steps);

    delete rules;

//    std::cout << rewritten.toStdString() << std::endl;
//    std::cout << std::endl;

    return rewritten;

}

QString LParser::smallTest()  {
    QString test = "[f(2)!(5)/(45)&(90)F(3)]!(10)F(2)e";
    return test;
}


