#include "uniprotxmlreader.h"

#include <QFile>
#include <QXmlStreamReader>


#include "common/exception.h"
#include <QtDebug>

namespace pepdb
{

UniprotXmlReader::UniprotXmlReader(const QString & fileName, QObject * parent) :
  QThread(parent)
{
  _file = new QFile(fileName);
  _dataList = new ProteinDataSet::ProteinDataList;
}

UniprotXmlReader::~UniprotXmlReader()
{

}

void UniprotXmlReader::run()
{
  readEntries();
}

void UniprotXmlReader::countEntries()
{
  _file->open(QFile::ReadOnly);
  _entryCount = 0;
  emit progressChanged(0);
  QXmlStreamReader reader(_file);
  while (reader.readNextStartElement())
  {
    if (reader.name() == "entry")
    {
      if (++_entryCount % 100 == 0)
        emit statusChanged(tr("Zähle Einträge: %1").arg(_entryCount));
      reader.skipCurrentElement();
    }
  }
  _file->close();
}

QVariant UniprotXmlReader::readAttribute(const QString & attName, AttributeType type )
{
    QXmlStreamAttributes atts = _reader->attributes();
    if (atts.hasAttribute(attName))
    {
        QString att = atts.value(attName).toString();
        switch (type)
        {
        case String:
            return att;
        case Int:
            return att.toInt();
        case Float:
            return att.toFloat();
        case Date:
            return QDate::fromString(att, Qt::ISODate);
        default:
            return att;
        }
    }
    return QVariant();
}

void UniprotXmlReader::readEntries()
{
    try
    {
        if (!_file->open(QFile::ReadOnly))
            throw new Exception(tr("Fehler beim Öffnen der Datei"));
        _reader = new QXmlStreamReader(_file);
        emit statusChanged(tr("Lese Einträge..."));
        if (! _reader->readNextStartElement() && _reader->name() != "uniprot" )
            throw new Exception(tr("Datei konnte nicht gelesen werden"));
        int i = 0;
        while (_reader->readNextStartElement() && _reader->name() == "entry" )
        {
            readEntry();
            emit statusChanged(tr("%1 Einträge gelesen...").arg(i));
            i++;
        }
    }
    catch (const Exception * e)
    {
        qCritical() << e->message();
    }
    emit finished();
}

void UniprotXmlReader::readEntry()
{
    _dataset = new ProteinDataSet;
    _dataset->uniprot.dataset = readAttribute("dataset").toString();
    _dataset->uniprot.created = readAttribute("created",Date).toDate();
    _dataset->uniprot.modified = readAttribute("modified",Date).toDate();
    _dataset->uniprot.version = readAttribute("version",Int).toInt();
    readAccessions();
    readNames();
    readProtein();

    while (_reader->readNextStartElement())
    {
        if (_reader->name() == "feature")
            readFeatureEntry(_dataset, _reader);
        else if (_reader->name() == "sequence")
            readSequenceEntry(_dataset, _reader);
        else
            _reader->skipCurrentElement();
    }
    _dataList->append(_dataset);
}

void UniprotXmlReader::readAccessions()
{
    int i=0;
    while (_reader->readNextStartElement() && _reader->name() == "accession")
    {
        _dataset->uniprot.accessionList.append(_reader->readElementText());
        i++;
    }
    if (i<1)
        throw new Exception(tr("Keine Uniprot-Accession."));
}

void UniprotXmlReader::readNames()
{
    int i=0;
    while (_reader->readNextStartElement() && _reader->name() == "name")
    {
        _dataset->uniprot.nameList.append(_reader->readElementText());
        i++;
    }
    if(i<1)
        throw new Exception(tr("Kein Uniprot-Name."));
}

void UniprotXmlReader::readProtein()
{
    if (_reader->readNextStartElement() && _reader->name() == "protein")
    {
        readProteinNames();
        while(_reader->readNextStartElement())
                readProteinNames(_reader->name().toString());
    }

}

    void UniprotXmlReader::readProteinNames(const QString & scope )
    {

        while (_reader->readNextStartElement())
        {
            ProteinDataSet::ProteinName * name = new ProteinDataSet::ProteinName;
            name->scope = scope;
            name->ref = readAttribute("ref").toString();
            name->type = _reader->name().toString();
            if(_reader->name == "recommandedName" ||
               _reader->name()== "alternativeName" ||
               _reader->name() == "submittedName")
            {
                ;
                if (reader->readNextStartElement() && reader->name() == "fullName")
                    name->fullName = readEvidencedString();
                while (reader->readNextStartElement() && reader->name() == "shortName")
                name->shortNameList.append(readEvidencedString());
            }
            else
                name->fullName = readEvidencedString();
            data->proteinNameList.append(name);

        }
    }

void UniprotXmlReader::readProteinName(QXmlStreamReader * reader,
    ProteinDataSet::ProteinName * protName)
{

  while ()
  {
    //qDebug() << reader->name();

        protName->length="full";
    else if (reader->name() == "shortName")
      protName->length="short";
    else
      reader->skipCurrentElement();
    protName->name = *readEvidencedString(reader);
  }
}

ProteinDataSet::EvidencedString * UniprotXmlReader::readEvidencedString(
    QXmlStreamReader * reader)
{
  ProteinDataSet::EvidencedString * s= new ProteinDataSet::EvidencedString();
  s->string = reader->readElementText();
  QXmlStreamAttributes att = reader->attributes();
  if (att.hasAttribute("evidence"))
    s->evidence = att.value("evidence").toString();
  if (att.hasAttribute("status"))
    s->status = att.value("status").toString();
  return s;
}

void UniprotXmlReader::readFeatureEntry(ProteinDataSet * data,
    QXmlStreamReader * reader)
{
  ProteinDataSet::Feature * feature =
      new ProteinDataSet::Feature();
  QXmlStreamAttributes att = reader->attributes();
  if (att.hasAttribute("status"))
    feature->status = att.value("status").toString();
  if (att.hasAttribute("id"))
    feature->id = att.value("id").toString();
  if (att.hasAttribute("description"))
    feature->description = att.value("description").toString();
  if (att.hasAttribute("evidence"))
    feature->evidence = att.value("evidence").toString();
  if (att.hasAttribute("ref"))
    feature->ref = att.value("ref").toString();
  if (att.hasAttribute("type"))
  {
    feature->type = att.value("type").toString();
  };
  while (reader->readNextStartElement())
  {
    //qDebug() << reader->name();
    if (reader->name() == "original")
      feature->original = reader->readElementText();
    else if (reader->name() == "variation")
      feature->variationList.append((reader->readElementText()));
    else if (reader->name() == "location")
    {
      QXmlStreamAttributes att = reader->attributes();
      if (att.hasAttribute("sequence"))
        feature->location.sequence = att.value("sequence").toString();
      while (reader->readNextStartElement())
      {
        //qDebug() << reader->name();
        if (reader->name() == "begin")
        {
          feature->location.begin.pos
              = reader->attributes().value("position").toString().toInt();
          feature->location.begin.status
              = reader->attributes().value("status").toString();
          reader->skipCurrentElement();
        }
        else if (reader->name() == "end")
        {
          feature->location.end.pos
              = reader->attributes().value("position").toString().toInt();
          feature->location.end.status = reader->attributes().value(
              "status").toString();
          reader->skipCurrentElement();
        }
        else if (reader->name() == "position")
        {
          int
              pos =
                  reader->attributes().value("position").toString().toInt();
          feature->location.begin.pos = pos;
          feature->location.end.pos = pos;
          feature->location.begin.status
              = reader->attributes().value("status").toString();
          reader->skipCurrentElement();
        }
        else
          reader->skipCurrentElement();
      }
    }
    else
      reader->skipCurrentElement();
  }
  data->featureList.append(feature);
}

void UniprotXmlReader::readSequenceEntry(ProteinDataSet * data,
    QXmlStreamReader * reader)
{
  QXmlStreamAttributes att = reader->attributes();
  if (att.hasAttribute("length"))
    data->sequence.length = att.value("length").toString().toInt();
  if (att.hasAttribute("mass"))
    data->sequence.mass = att.value("mass").toString().toInt();
  if (att.hasAttribute("checksum"))
    data->sequence.checksum = att.value("checksum").toString();
  if (att.hasAttribute("modified"))
    data->sequence.modified = QDate::fromString(att.value("modified").toString(),Qt::ISODate);
  if (att.hasAttribute("precursor"))
    data->sequence.precursor = toBool(att.value("precursor").toString());
  if (att.hasAttribute("version"))
    data->sequence.version = att.value("version").toString().toInt();
  if (att.hasAttribute("fragment"))
    data->sequence.fragment = att.value("fragment").toString();
  data->sequence.sequence
      = reader->readElementText().simplified().remove(' ');
}

ProteinDataSet::ProteinDataList * UniprotXmlReader::data()
{
  return _dataList;
}

bool UniprotXmlReader::toBool(const QString & s)
{
  if (s.simplified().toLower() == "true" || s== "1" )
    return true;
  return false;
}
}
