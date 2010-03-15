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
    emit
    progressChanged(0);
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

QVariant UniprotXmlReader::readAttribute(const QString & attName,
        AttributeType type)
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
        case Bool:
            return toBool(att);
        default:
            return att;
        }
    }
    return QVariant();
}
ProteinDataSet::EvidencedString * UniprotXmlReader::readEvidencedString()
{
    ProteinDataSet::EvidencedString * s = new ProteinDataSet::EvidencedString;
    s->string = _reader->readElementText();
    s->evidence = readAttribute("evidence").toString();
    s->status = readAttribute("status").toString();
    return s;
}

void UniprotXmlReader::readEntries()
{
    try
    {
        if (!_file->open(QFile::ReadOnly))
            throw new Exception(tr("Fehler beim Öffnen der Datei"));
        _reader = new QXmlStreamReader(_file);
        emit
        statusChanged(tr("Lese Einträge..."));
        if (!_reader->readNextStartElement() && _reader->name() != "uniprot")
            throw new Exception(tr("Datei konnte nicht gelesen werden"));
        int i = 0;
        while (_reader->readNextStartElement() && _reader->name() == "entry")
        {
            readEntry();
            emit
            statusChanged(tr("%1 Einträge gelesen...").arg(i));
            i++;
        }
    } catch (const Exception * e)
    {
        qCritical() << e->message();
    }
    emit finished();
}

void UniprotXmlReader::readEntry()
{
    _dataset = new ProteinDataSet;
    _dataset->uniprot.dataset = readAttribute("dataset").toString();
    _dataset->uniprot.created = readAttribute("created", Date).toDate();
    _dataset->uniprot.modified = readAttribute("modified", Date).toDate();
    _dataset->uniprot.version = readAttribute("version", Int).toInt();
    _reader->readNextStartElement();
    readAccessions();
    readNames();
    readProtein();
    readGenes();
    readOrganism();
    readOrganismHosts();
    readGeneLocations();
    readReferences();
    readComments();
    readDbReferences();
    readProteinExistence();
    readKeywords();
    readFeatures();
    readEvidences();
    readSequence();

    _dataList->append(_dataset);
}

void UniprotXmlReader::readAccessions()
{
    int i = 0;
    while ( _reader->name() == "accession")
    {
        _dataset->uniprot.accessionList.append(_reader->readElementText());
        i++;
        _reader->readNextStartElement();
    }
    if (i < 1)
        throw new Exception(tr("Keine Uniprot-Accession."));
}

void UniprotXmlReader::readNames()
{
    int i = 0;
    while ( _reader->name() == "name")
    {
        _dataset->uniprot.nameList.append(_reader->readElementText());
        i++;
        _reader->readNextStartElement();
    }
    if (i < 1)
        throw new Exception(tr("Kein Uniprot-Name."));
}

void UniprotXmlReader::readProtein()
{
    if ( _reader->name() == "protein")
    {
        readProteinNames();
        _reader->readNextStartElement();
    }
    else
        throw Exception(tr("Keine Proteinnamen"));
}

void UniprotXmlReader::readProteinNames(const QString & scope)
{

    while (_reader->readNextStartElement())
    {
        if (_reader->name() == "component" || _reader->name() == "domain")
            readProteinNames(_reader->name().toString());
        ProteinDataSet::ProteinName * name = new ProteinDataSet::ProteinName;
        name->scope = scope;
        name->ref = readAttribute("ref").toString();
        name->type = _reader->name().toString();
        if (_reader->name() == "recommandedName" || _reader->name()
                == "alternativeName" || _reader->name() == "submittedName")
        {
            ;
            if (_reader->readNextStartElement() && _reader->name()
                    == "fullName")
                name->fullName = readEvidencedString();
            while (_reader->readNextStartElement() && _reader->name()
                    == "shortName")
                name->shortNameList.append(readEvidencedString());
        }
        else
            name->fullName = readEvidencedString();
        _dataset->proteinNameList.append(name);

    }
}

void UniprotXmlReader::readFeatures()
{
    while (_reader->readNextStartElement() && _reader->name() == "feature")
    {
        ProteinDataSet::Feature * feature = new ProteinDataSet::Feature();
        feature->status = readAttribute("status").toString();
        feature->id = readAttribute("id").toString();
        feature->description = readAttribute("description").toString();
        feature->evidence = readAttribute("evidence").toString();
        feature->ref = readAttribute("ref").toString();
        feature->type = readAttribute("type").toString();
        if (_reader->readNextStartElement() && _reader->name() == "original")
            feature->original = _reader->readElementText();
        while (_reader->readNextStartElement() && _reader->name()
                == "variation")
            feature->variationList.append((_reader->readElementText()));
        if (_reader->readNextStartElement() && _reader->name() == "location")
            feature->location = readLocation();
        else
            throw Exception(tr(
                    "Keine Lokalisierung von einem Feature angegben."));
        _dataset->featureList.append(feature);
    }
}

ProteinDataSet::Location * UniprotXmlReader::readLocation()
{
    ProteinDataSet::Location * location = new ProteinDataSet::Location();
    location->sequence = readAttribute("sequence").toString();
    if (_reader->readNextStartElement() && _reader->name() == "begin")
    {
        location->begin = readPosition();
        if (_reader->readNextStartElement() && _reader->name() == "end")
            location->end = readPosition();
    }
    else if (_reader->name() == "position")
    {
        location->begin = readPosition();
        location->end = 0;
    }
    else
        throw new Exception(tr("Keine Position für Feature"));
    return location;
}

ProteinDataSet::Position * UniprotXmlReader::readPosition()
{

    ProteinDataSet::Position * pos = new ProteinDataSet::Position();
    pos->evidence = readAttribute("evidence").toString();
    pos->pos = readAttribute("position",Int).toInt();
    pos->status =readAttribute("status").toString();
    return pos;
}

void UniprotXmlReader::readSequence()
{
    if (_reader->readNextStartElement() && _reader->name() == "protein")
    {
        _dataset->sequence.length = readAttribute("length", Int).toInt();
        _dataset->sequence.mass = readAttribute("mass", Int).toInt();
        _dataset->sequence.checksum = readAttribute("checksum").toString();
        _dataset->sequence.modified = readAttribute("modified", Date).toDate();
        _dataset->sequence.precursor
                = readAttribute("precursor", Bool).toBool();
        _dataset->sequence.version = readAttribute("version", Int).toInt();
        _dataset->sequence.fragment = readAttribute("fragment").toString();
        _dataset->sequence.sequence
                = _reader->readElementText().simplified().remove(' ');
    }
    else
        throw new Exception(tr("Keine Sequenz."));
}

ProteinDataSet::ProteinDataList * UniprotXmlReader::data()
{
    return _dataList;
}

void UniprotXmlReader::readGenes()
{
    while (_reader->name() == "gene")
    {
        _reader->skipCurrentElement();
        _reader->readNextStartElement();
    }
}
void UniprotXmlReader::readOrganism()
{
    while (_reader->name() == "organism")
        {
            _reader->skipCurrentElement();
            _reader->readNextStartElement();
        }
}
void UniprotXmlReader::readOrganismHosts()
{
    while (_reader->name() == "organismHost")
        {
            _reader->skipCurrentElement();
            _reader->readNextStartElement();
        }
}
void UniprotXmlReader::readGeneLocations()
{
    while (_reader->name() == "geneLocation")
        {
            _reader->skipCurrentElement();
            _reader->readNextStartElement();
        }
}
void UniprotXmlReader::readReferences()
{
    while (_reader->name() == "reference")
        {
            _reader->skipCurrentElement();
            _reader->readNextStartElement();
        }
}
void UniprotXmlReader::readComments()
{
    while (_reader->name() == "comment")
        {
            _reader->skipCurrentElement();
            _reader->readNextStartElement();
        }
}

void UniprotXmlReader::readDbReferences()
{
    while (_reader->name() == "dbReference")
        {
            _reader->skipCurrentElement();
            _reader->readNextStartElement();
        }
}
void UniprotXmlReader::readProteinExistence()
{
if (_reader->name() == "proteinExistence")
{
    _reader->skipCurrentElement();
    _reader->readNextStartElement();
}
}

void UniprotXmlReader::readKeywords()
{

while (_reader->name() == "keyword")
{
    _reader->skipCurrentElement();
    _reader->readNextStartElement();
}
}

void UniprotXmlReader::readEvidences()
{

while (_reader->name() == "evidence")
{
    _reader->skipCurrentElement();
    _reader->readNextStartElement();
}
}


bool UniprotXmlReader::toBool(const QString & s)
{
    if (s.simplified().toLower() == "true" || s == "1")
        return true;
    return false;
}
}
