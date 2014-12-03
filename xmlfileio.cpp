#include "xmlfileio.h"


XmlFileIO::XmlFileIO()
{
    reload_editor = false;
}

void XmlFileIO::saveToFile()
{
    if (glob_places.empty())
    {
        QMessageBox::information(0, "Editor",
                                 "Editor is empty, nothing to save.");
        return;
    }


    filename = QFileDialog::getSaveFileName(0, "Save gpraph", "C://",
                                            "XML files (*.xml)");

    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("graphml");
    xmlWriter.writeAttribute("xmlns", "http://graphml.graphdrawing.org/xmlns");
    xmlWriter.writeAttribute("xmlns:xsi",
                             "http://www.w3.org/2001/XMLSchema-instance");
    xmlWriter.writeAttribute("xsi:schemaLocation",
                             "http://graphml.graphdrawing.org/xmlns "
                             "http://graphml.graphdrawing.org/xmlns/1.0/"
                             "graphml.xsd");

    // <key id="d0" for="node" attr.name="x" attr.type="double"/>
    xmlWriter.writeStartElement("key");
    xmlWriter.writeAttribute("id", "d0");
    xmlWriter.writeAttribute("for", "node");
    xmlWriter.writeAttribute("attr.name", "x");
    xmlWriter.writeAttribute("attr.type", "double");
    xmlWriter.writeEndElement();

    // <key id="d1" for="node" attr.name="y" attr.type="double"/>
    xmlWriter.writeStartElement("key");
    xmlWriter.writeAttribute("id", "d1");
    xmlWriter.writeAttribute("for", "node");
    xmlWriter.writeAttribute("attr.name", "y");
    xmlWriter.writeAttribute("attr.type", "double");
    xmlWriter.writeEndElement();

    // <key id="d2" for="edge" attr.name="weight" attr.type="int"/>
    xmlWriter.writeStartElement("key");
    xmlWriter.writeAttribute("id", "d2");
    xmlWriter.writeAttribute("for", "edge");
    xmlWriter.writeAttribute("attr.name", "weight");
    xmlWriter.writeAttribute("attr.type", "int");
    xmlWriter.writeEndElement();

    // <graph id="G" edgedefault="directed">
    xmlWriter.writeStartElement("graph");
    xmlWriter.writeAttribute("id", "G");
    xmlWriter.writeAttribute("edgedefault", "undirected");


    //PLACES
    /*
    <node id="n0">
        <data key="d0">170.5</data>
        <data key="d1">125.0</data>
    </node>
    */
    QString node_num;
    foreach (gPlace *p, glob_places)
    {
        xmlWriter.writeStartElement("node");
        node_num = "n" + QString::number(p->id());
        xmlWriter.writeAttribute("id", node_num);
        xmlWriter.writeStartElement("data");
        xmlWriter.writeAttribute("key", "d0");
        xmlWriter.writeCharacters(QString::number(p->x()));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("data");
        xmlWriter.writeAttribute("key", "d1");
        xmlWriter.writeCharacters(QString::number(p->y()));
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
    }

    // EDGES
    /*
    <edge id="e1" source="n0" target="n1">
      <data key="d2">1</data>
    </edge>
    <edge id="e2" source="n1" target="n3">
      <data key="d2">6</data>
    </edge>
    */
    QString edge_num, place_from, place_to;
    unsigned i = 0;
    foreach (gEdge *e, glob_edges)
    {
        xmlWriter.writeStartElement("edge");
        edge_num = "e" + QString::number(i++);
        xmlWriter.writeAttribute("id", edge_num);
        place_from = "n" + QString::number(e->getFrom()->id());
        place_to = "n" + QString::number(e->getTo()->id());
        xmlWriter.writeAttribute("source", place_from);
        xmlWriter.writeAttribute("target", place_to);
        xmlWriter.writeStartElement("data");
        xmlWriter.writeAttribute("key", "d2");
        xmlWriter.writeCharacters(QString::number(e->w()));
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
    }


    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();
}

void XmlFileIO::LoadFromFile()
{
    filename = QFileDialog::getOpenFileName(0, "Load graph", "C://",
                                            "XML files (*.xml)");

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(0, "Editor", file.errorString());
        reload_editor = false;
        return;
    }

    if (!glob_places.empty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(0, "Editor",
                                      "This will overwrite the current graph, "
                                      "proceed?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            reload_editor = false;
            return;
        }
    }

    reload_editor = true;
    glob_places.clear();
    glob_edges.clear();

    QXmlStreamReader xmlReader(&file);
    xmlReader.readNext();
    xmlReader.readNext();

    gPlace *p;
    gEdge *e;
    QString str;
    QXmlStreamAttributes attr;
    unsigned id_from, id_to;
    while (!xmlReader.atEnd())
    {
        if (xmlReader.name() == "node")
        {
            if (xmlReader.isEndElement())
            {
                xmlReader.readNext();
                continue;
            }

            p = new gPlace();

            // id
            attr = xmlReader.attributes();
            str = attr.value("id").toString();
            if (str.isEmpty())
            {
                delete p;
                xmlReader.raiseError("Expected <node> attribute 'id'.");
                break;
            }
            p->setID(str.split("n")[1].toUInt());
            xmlReader.readNext();

            // x
            xmlReader.readNext();
            if (xmlReader.name() != "data")
            {
                delete p;
                xmlReader.raiseError("Expected <data> element.");
                break;
            }
            str = xmlReader.readElementText();
            if (str.isEmpty())
            {
                delete p;
                xmlReader.raiseError("Missing text inside <data> element.");
                break;
            }
            p->setX(str.toDouble());
            xmlReader.readNext();

            // y
            xmlReader.readNext();
            if (xmlReader.name() != "data")
            {
                delete p;
                xmlReader.raiseError("Expected <data> element.");
                break;
            }
            str = xmlReader.readElementText();
            if (str.isEmpty())
            {
                delete p;
                xmlReader.raiseError("Missing text inside <data> element.");
                break;
            }
            p->setY(str.toDouble());
            xmlReader.readNext();

            xmlReader.readNext();
            glob_places.push_back(p);
            continue;
        }

        if (xmlReader.name() == "edge")
        {
            if (xmlReader.isEndElement())
            {
                xmlReader.readNext();
                continue;
            }

            if (glob_places.empty())
            {
                xmlReader.readNext();
                continue;
            }

            e = new gEdge();

            attr = xmlReader.attributes();
            str = attr.value("source").toString();
            if (str.isEmpty())
            {
                delete e;
                xmlReader.raiseError("Expected <edge> attribute 'source'.");
                break;
            }
            id_from = str.split("n")[1].toUInt();
            str = attr.value("target").toString();
            if (str.isEmpty())
            {
                delete e;
                xmlReader.raiseError("Expected <edge> attribute 'target'.");
                break;
            }
            id_to = str.split("n")[1].toUInt();
            // set edge starting and ending place
            bool fplace = false;
            bool tplace = false;
            foreach (gPlace *it, glob_places)
            {
                if (it->id() == id_from)
                {
                    e->setFrom(it);
                    fplace = true;
                    continue;
                }
                if (it->id() == id_to)
                {
                    e->setTo(it);
                    tplace = true;
                    continue;
                }
                if (fplace && tplace)
                    break;
            }
            if (!(fplace && tplace))
            {
                delete e;
                xmlReader.raiseError("Edge contains non-existing place.");
                break;
            }
            xmlReader.readNext();

            // edge weight
            xmlReader.readNext();
            if (xmlReader.name() != "data")
            {
                delete e;
                xmlReader.raiseError("Expected <data> element.");
                break;
            }
            str = xmlReader.readElementText();
            if (str.isEmpty())
            {
                delete e;
                xmlReader.raiseError("Missing text inside <data> element.");
                break;
            }
            e->setW(str.toUInt());
            xmlReader.readNext();

            xmlReader.readNext();
            glob_edges.push_back(e);
            continue;
        }

        xmlReader.readNext();
    }

    file.close();

    if (xmlReader.hasError())
    {
        QMessageBox::warning(0, "Editor",
                             ("Error in parsing GraphML file:\n"
                              + xmlReader.errorString()));
        glob_places.clear();
        glob_edges.clear();
        return;
    }
}

bool XmlFileIO::graphChanged()
{
    return reload_editor;
}
