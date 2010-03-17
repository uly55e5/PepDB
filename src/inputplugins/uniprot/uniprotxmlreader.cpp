#include "uniprotxmlreader.h"

#include <QFile>
#include <QXmlStreamReader>

#include "common/exception.h"
#include <QtDebug>
#include <QMessageBox>

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
    delete _file;

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

ProteinDataSet::EvidencedString UniprotXmlReader::readEvidencedString()
{
    ProteinDataSet::EvidencedString s;
    s.string = _reader->readElementText();
    s.evidence = readAttribute("evidence").toString();
    s.status = readAttribute("status").toString();
    return s;
}

void UniprotXmlReader::readEntries()
{
    try
    {
        if (!_file->open(QFile::ReadOnly))
            throw new Exception(tr("Fehler beim Öffnen der Datei"));
        _reader = new QXmlStreamReader(_file);
        emit statusChanged(tr("Lese Einträge..."));
        if (!_reader->readNextStartElement() && _reader->name() != "uniprot")
            throw new Exception(tr("Datei konnte nicht gelesen werden"));
        int i = 0;
        while (_reader->readNextStartElement() && _reader->name() == "entry")
        {
            readEntry();
            emit statusChanged(tr("%1 Einträge gelesen...").arg(i));
            i++;
        }
    } catch (const Exception * e)
    {
        qDebug() << "Exception: " << e->message();
    }
    emit finished();
}

void UniprotXmlReader::readEntry()
{
    _currentDataset = new ProteinDataSet;
    _currentDataset->uniprot.dataset = readAttribute("dataset").toString();
    _currentDataset->uniprot.created = readAttribute("created", Date).toDate();
    _currentDataset->uniprot.modified
            = readAttribute("modified", Date).toDate();
    _currentDataset->uniprot.version = readAttribute("version", Int).toInt();

    if (!_reader->readNextStartElement())
        throw new Exception(tr("Leerer Eintrag."));

    while (_reader->name() == "accession")
    {
        readAccession();
        _reader->readNextStartElement();
    }
    if (_currentDataset->uniprot.accessionList.isEmpty())
        throw new Exception(tr("Keine Accessionnummer vorhanden."));

    while (_reader->name() == "name")
    {
        readName();
        _reader->readNextStartElement();
    }
    if (_currentDataset->uniprot.nameList.isEmpty())
        throw new Exception(tr("Kein Uniprotnamen vohanden."));

    if (_reader->name() == "protein")
    {
        readProtein();
        _reader->readNextStartElement();

    }
    else
        throw new Exception(tr("Keine Proteinnamen vorhanden."));

    while (_reader->name() == "gene")
    {
        readGene();
        _reader->readNextStartElement();

    }

    if (_reader->name() == "organism")
    {
        _currentDataset->organism = *readOrganism();
        _reader->readNextStartElement();

    }
    else
        throw new Exception(tr("Kein Organismus vorhanden."));

    while (_reader->name() == "organismHost")
    {
        readOrganismHost();
        _reader->readNextStartElement();
    }

    while (_reader->name() == "geneLocation")
    {
        readGeneLocation();
        _reader->readNextStartElement();
        qDebug() << _reader->name();
    }

    while (_reader->name() == "reference")
    {
        readReference();
        _reader->readNextStartElement();
    }
    //    if (_currentDataset->referenceList.isEmpty())
    //        throw new Exception(tr("Keine Referenzen vorhanden."));

    while (_reader->name() == "comment")
    {
        readComment();
        _reader->readNextStartElement();
    }
    while (_reader->name() == "dbReference")
    {
        _currentDataset->dbReferenceList.append(readDbReference());
        _reader->readNextStartElement();
    }

    if (_reader->name() == "proteinExistence")
    {
        readProteinExistence();
        _reader->readNextStartElement();
    }
    else
        throw new Exception(tr("Keine Proteinexistence vorhanden."));

    while (_reader->name() == "keyword")
    {
        readKeyword();
        _reader->readNextStartElement();
    }

    while (_reader->name() == "feature")
    {
        readFeature();
        _reader->readNextStartElement();

    }

    while (_reader->name() == "evidence")
    {
        readEvidence();
        _reader->readNextStartElement();
    }

    if (_reader->name() == "sequence")
    {
        readSequence();
        _reader->readNextStartElement();
    }
    else
        throw new Exception(tr("Keine Sequenz vorhanden."));

    _dataList->append(_currentDataset);
}

void UniprotXmlReader::readAccession()
{
    _currentDataset->uniprot.accessionList.append(_reader->readElementText());
}

void UniprotXmlReader::readName()
{
    _currentDataset->uniprot.nameList.append(_reader->readElementText());
}

void UniprotXmlReader::readProtein(const QString & scope)
{

    while (_reader->readNextStartElement())
    {
        if (_reader->name() == "component" || _reader->name() == "domain")
            readProtein(_reader->name().toString());
        else
        {

            ProteinDataSet::ProteinName * name =
                    new ProteinDataSet::ProteinName;
            name->scope = scope;
            name->type = _reader->name().toString();

            if (_reader->name() == "recommendedName" || _reader->name()
                    == "alternativeName" || _reader->name() == "submittedName")
            {
                ;
                if (_reader->readNextStartElement() && _reader->name()
                        == "fullName")
                    name->fullName = readEvidencedString();
                while (_reader->readNextStartElement() && _reader->name()
                        == "shortName")
                    name->shortNameList.append(
                            new ProteinDataSet::EvidencedString(
                                    readEvidencedString()));
            }
            else
                name->fullName = readEvidencedString();

            _currentDataset->proteinNameList.append(name);
        }
    }
}

void UniprotXmlReader::readFeature()
{

    ProteinDataSet::Feature * feature = new ProteinDataSet::Feature();
    feature->status = readAttribute("status").toString();
    feature->id = readAttribute("id").toString();
    feature->description = readAttribute("description").toString();
    feature->evidence = readAttribute("evidence").toString();
    feature->ref = readAttribute("ref").toString();
    feature->type = readAttribute("type").toString();
    _reader->readNextStartElement();
    if (_reader->name() == "original")
    {
        feature->original = _reader->readElementText();
        _reader->readNextStartElement();
    }

    while (_reader->name() == "variation")
    {
        feature->variationList.append((_reader->readElementText()));
        _reader->readNextStartElement();
    }

    if (_reader->name() == "location")
    {
        feature->location = readLocation();
        _reader->readNextStartElement();
    }
    else
    {

        throw new Exception(tr(
                "Keine Lokalisierung von einem Feature angegeben."));
    }
    _reader->skipCurrentElement();
    _currentDataset->featureList.append(feature);
}

ProteinDataSet::Location UniprotXmlReader::readLocation()
{
    ProteinDataSet::Location location;
    location.sequence = readAttribute("sequence").toString();
    if (_reader->readNextStartElement() && _reader->name() == "begin")
    {
        location.begin = readPosition();

        if (_reader->readNextStartElement() && _reader->name() == "end")
            location.end = readPosition();
        else
        {

            throw new Exception(tr("Location begin ohne end"));
        }
    }
    else if (_reader->name() == "position")
    {
        location.begin = readPosition();
        location.end = location.begin;
    }
    else
        throw new Exception(tr("Keine Position für Feature"));
    return location;
}

ProteinDataSet::Position UniprotXmlReader::readPosition()
{

    ProteinDataSet::Position pos;
    pos.evidence = readAttribute("evidence").toString();
    pos.pos = readAttribute("position", Int).toInt();
    pos.status = readAttribute("status").toString();
    _reader->skipCurrentElement();
    return pos;
}

void UniprotXmlReader::readSequence()
{
    _currentDataset->sequence.length = readAttribute("length", Int).toInt();
    _currentDataset->sequence.mass = readAttribute("mass", Int).toInt();
    _currentDataset->sequence.checksum = readAttribute("checksum").toString();
    _currentDataset->sequence.modified
            = readAttribute("modified", Date).toDate();
    _currentDataset->sequence.precursor
            = readAttribute("precursor", Bool).toBool();
    _currentDataset->sequence.version = readAttribute("version", Int).toInt();
    _currentDataset->sequence.fragment = readAttribute("fragment").toString();
    _currentDataset->sequence.sequence
            = _reader->readElementText().simplified().remove(' ');
}

ProteinDataSet::ProteinDataList * UniprotXmlReader::data()
{
    return _dataList;
}

void UniprotXmlReader::readGene()
{
    ProteinDataSet::Gene * gene = new ProteinDataSet::Gene();
    while (_reader->readNextStartElement() && _reader->name() == "name")
    {
        ProteinDataSet::GeneName * name = new ProteinDataSet::GeneName();
        name->evidence = readAttribute("evidence").toString();
        name->type = readAttribute("type").toString();
        name->name = _reader->readElementText();
        gene->geneNameList.append(name);
    }
    _currentDataset->geneList.append(gene);

}

ProteinDataSet::Organism * UniprotXmlReader::readOrganism()
{
    ProteinDataSet::Organism * organism = new ProteinDataSet::Organism();
    organism->evidence = readAttribute("evidence").toString();
    organism->key = readAttribute("key").toString();
    while (_reader->readNextStartElement() && _reader->name() == "name")
    {
        ProteinDataSet::OrganismName * name =
                new ProteinDataSet::OrganismName();
        name->type = readAttribute("type").toString();
        name->name = _reader->readElementText();
        organism->nameList.append(name);
    }
    while (_reader->name() == "dbReference")
    {
        organism->dbReferenceList.append(readDbReference());
        _reader->readNextStartElement();
    }
    if (_reader->name() == "lineage")
    {
        while (_reader->readNextStartElement() && _reader->name() == "taxon")
            organism->lineage.append(_reader->readElementText());
        _reader->readNextStartElement();
    }

    return organism;
}

void UniprotXmlReader::readOrganismHost()
{
    //TODO
    _reader->skipCurrentElement();
}

void UniprotXmlReader::readGeneLocation()
{
    ProteinDataSet::GeneLocation * loc = new ProteinDataSet::GeneLocation();
    loc->evidence = readAttribute("evidence").toString();
    loc->type = readAttribute("type").toString();
    while (_reader->readNextStartElement() && _reader->name() == "name")
    {
        ProteinDataSet::Status * status = new ProteinDataSet::Status();
        status->status = readAttribute("status").toString();
        status->name = _reader->readElementText();
        loc->nameList.append(status);
    }
    qDebug() << "genlocation end: " << _reader->name();
    _currentDataset->geneLocationList.append(loc);
}

void UniprotXmlReader::readReference()
{
    ProteinDataSet::Reference * ref = new ProteinDataSet::Reference();
    ref->evidence = readAttribute("evidence").toString();
    ref->key = readAttribute("key").toString();
    if (_reader->readNextStartElement() && _reader->name() == "citation")
    {

    }
    else
        throw new Exception(tr("Kein Zitat in Referenz"));

    while (_reader->readNextStartElement() && _reader->name() == "scope")
    {

    }

}

ProteinDataSet::Citation UniprotXmlReader::readCitation()
{
    ProteinDataSet::Citation citation;
    citation.type = readAttribute("type").toString();
    citation.date = readAttribute("date").toString();
    citation.name = readAttribute("name").toString();
    citation.volume = readAttribute("volume").toString();
    citation.first = readAttribute("first").toString();
    citation.last = readAttribute("last").toString();
    citation.publisher = readAttribute("publisher").toString();
    citation.city = readAttribute("city").toString();
    citation.db = readAttribute("db").toString();
    citation.country = readAttribute("country").toString();
    citation.number = readAttribute("number").toString();
    citation.institute = readAttribute("institute").toString();
    _reader->readNextStartElement();
    if (_reader->name() == "title")
    {
        citation.title = _reader->readElementText();
        _reader->readNextStartElement();
    }

    if (_reader->name() == "editorlist")
    {
        citation.editorList = readNameList();
        _reader->readNextStartElement();
    }

    if (_reader->name() == "authorList")
    {
        citation.authorList = readNameList();
        _reader->readNextStartElement();

    }

    if (_reader->name() == "locator")
    {
        citation.locator = _reader->readElementText();
        _reader->readNextStartElement();

    }

    while (_reader->name() == "dbReference")
    {
        citation.dbReferenceList.append(readDbReference());
        _reader->readNextStartElement();

    }

    if (_reader->name() == "citingCitation")
    {
        citation.citing = new ProteinDataSet::Citation(readCitation());
        _reader->readNextStartElement();

    }

    return citation;
}

ProteinDataSet::NameList UniprotXmlReader::readNameList()
{
    ProteinDataSet::NameList names;
    while(_reader->readNextStartElement())
    {
    if (_reader->name() == "person")
        names.personList.append(readAttribute("name").toString());
    else if (_reader->name() == "consortium")
        names.consortiumList.append(readAttribute("name").toString());
    else
        throw new Exception(tr("Weder Person noch Consortium in Namensliste"));
    }
    return names;
}

void UniprotXmlReader::readComment()
{
    //TODO
    _reader->skipCurrentElement();
}

ProteinDataSet::DbReference * UniprotXmlReader::readDbReference()
{

    ProteinDataSet::DbReference * ref = new ProteinDataSet::DbReference();
    ref->evidence = readAttribute("evidence").toString();
    ref->key = readAttribute("key").toString();
    ref->id = readAttribute("id").toString();
    ref->type = readAttribute("type").toString();
    while (_reader->readNextStartElement() && _reader->name() == "property")
    {
        ProteinDataSet::Property * prop = new ProteinDataSet::Property();
        prop->type = readAttribute("type").toString();
        prop->value = readAttribute("value").toString();
        ref->propertyList.append(prop);
        _reader->skipCurrentElement();
    }
    return ref;
}

void UniprotXmlReader::readProteinExistence()
{
    _currentDataset->proteinExistence = readAttribute("type").toString();
    _reader->skipCurrentElement();
}

void UniprotXmlReader::readKeyword()
{
    ProteinDataSet::Keyword * keyword = new ProteinDataSet::Keyword();
    keyword->evidence = readAttribute("evidence").toString();
    keyword->id = readAttribute("id").toString();
    keyword->keyword = _reader->readElementText();
    _currentDataset->keywordList.append(keyword);
}

void UniprotXmlReader::readEvidence()
{
    ProteinDataSet::Evidence * evidence = new ProteinDataSet::Evidence();
    evidence->attribute = readAttribute("attribute").toString();
    evidence->category = readAttribute("category").toString();
    evidence->date = readAttribute("date", Date).toDate();
    evidence->key = readAttribute("key").toString();
    evidence->type = readAttribute("type").toString();
    _reader->skipCurrentElement();
}

bool UniprotXmlReader::toBool(const QString & s)
{
    if (s.simplified().toLower() == "true" || s == "1")
        return true;
    return false;
}
}
