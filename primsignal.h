#ifndef PRIMSIGNAL_H
#define PRIMSIGNAL_H

#include <QObject>
#include "interface.h"

class PrimSignal : public QObject
{
    Q_OBJECT
public:
    explicit PrimSignal(QObject *parent = 0);
    void qSigEvent(int signal, unsigned m_u, unsigned m_v);

    unsigned getU();
    unsigned getV();

signals:
    void sig(int signal);

private:
    unsigned u;
    unsigned v;
};

#endif // PRIMSIGNAL_H
