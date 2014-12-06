#include "primsignal.h"

PrimSignal::PrimSignal(QObject *parent) :
    QObject(parent)
{
}

void PrimSignal::qSigEvent(unsigned event)
{
    emit sig(event);
}
