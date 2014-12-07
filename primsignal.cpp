#include "primsignal.h"

PrimSignal::PrimSignal(QObject *parent) :
    QObject(parent)
{
}

void PrimSignal::qSigEvent(int signal, unsigned m_u, unsigned m_v)
{
    u = m_u;
    v = m_v;
    emit sig(signal);
}

unsigned PrimSignal::getU()
{
    return u;
}

unsigned PrimSignal::getV()
{
    return v;
}
