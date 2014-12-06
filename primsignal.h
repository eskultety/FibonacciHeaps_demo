#ifndef PRIMSIGNAL_H
#define PRIMSIGNAL_H

#include <QObject>
#include "interface.h"

class PrimSignal : public QObject
{
    Q_OBJECT
public:
    explicit PrimSignal(QObject *parent = 0);
    void qSigEvent(unsigned event);

signals:
    void sig(unsigned event);
};

#endif // PRIMSIGNAL_H
