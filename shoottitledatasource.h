#ifndef SHOOTTITLEDATASOURCE_H
#define SHOOTTITLEDATASOURCE_H

#include <QString>
#include <QVariant>
class ShootTitleDataSource
{
public:
    virtual int numberOfShootTitles() {return 0;}
    virtual QString shootTitleAtIndex(int) {return "";}
    virtual QString shootTitleIdAtIndex(int) {return "";}
    virtual QString shootTitleIdAtIndex( QString ) {return "";}
    virtual QString shootTitleForId(QString&) {return "";}
    virtual int shootTitleIndexForId(QString&) {return 0;}
    virtual QString userTokenId() {return 0;}
    virtual QString userId() {return 0;}

};

#endif // SHOOTTITLEDATASOURCE_H
