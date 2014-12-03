#ifndef XMLFILEIO_H
#define XMLFILEIO_H

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include "gplace.h"
#include "gedge.h"


class XmlFileIO
{
public:
    XmlFileIO();
    void saveToFile();
    void LoadFromFile();
    bool graphChanged();

private:
    QString filename;
    bool reload_editor;
};

#endif // XMLFILEIO_H
