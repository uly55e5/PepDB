/*
 * proteindataset.h
 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#ifndef PROTEINDATASET_H_
#define PROTEINDATASET_H_

#include <QDate>
#include <QMap>
#include <QString>
#include <QStringList>

namespace pepdb
{
struct ProteinDataSet
{

public:

  /*!
   * @brief Uniprot entry data.
   */
  struct Uniprot
  {
    QString dataset;
    QDate created;
    QDate modified;
    int version;
    QStringList accessionList;
    QStringList nameList;
  };


  struct Sequence
  {
    QString sequence;
    int length;
    int mass;
    QString checksum;
    QDate modified;
    int version;
    bool precursor;
    QString fragment;
  };

  struct Evidence
  {
    QString category;
    QString attribute;
    QString type;
    QDate date;
    QString key;
  };

  typedef QList<Evidence *> EvidenceList;

  struct Position
  {
    int pos;
    QString status;
    QString evidence;
  };

  struct Location
  {
    Position begin;
    Position end;
    QString sequence;
  };

  typedef QList<Location *> LocationList;

  struct Feature
  {
    QString original;
    QStringList variationList;
    Location location;
    QString type;
    QString status;
    QString id;
    QString description;
    QString evidence;
    QString ref;
  };

  typedef QList<Feature *> FeatureList;

  struct Keyword
  {
    QString keyword;
    QString id;
    QString evidence;
  };

  typedef QList<Keyword *> KeywordList;

  struct Property
  {
    QString type;
    QString value;
  };

  typedef QList<Property *> PropertyList;

  struct DbReference
  {
    PropertyList propertyList;
    QString type;
    QString id;
    QString evidence;
    QString key;

  };

  typedef QList<DbReference *> DbReferenceList;


  struct EvidencedString
  {
    QString string;
    QString evidence;
    QString status;
  };

  typedef QList<EvidencedString *> EvidencedStringList;

  struct BpcComment
  {
    EvidencedString absMax;
    EvidencedString absText;
    EvidencedStringList kinVMaxList;
    EvidencedStringList kinKmList;
    EvidencedString kinText;
    EvidencedString phDependence;
    EvidencedString redoxPotential;
    EvidencedString tempDependence;
  };

  struct SubCellularLocation
  {
    EvidencedStringList locationList;
    EvidencedStringList topologyList;
    EvidencedStringList orientationList;
  };

  typedef QList <SubCellularLocation *>SubCellularLocationList;

  struct SubCellularLocationComment
  {
    QString molecule;
    SubCellularLocationList subCellularLocationList;

  };

  struct ConflictComment
  {
    QString seqResource;
    QString seqId;
    int seqVersion;
    QString type;
    QString ref;
  };

  struct LinkComment
  {
    QStringList uriList;
  };

  struct IsoformName
  {
    QString name;
    QString evidence;
  };

  typedef QList <IsoformName *> IsoformNameList;

  struct Isoform
  {
    IsoformNameList nameList;
    QStringList idList;
    QString seqType;
    QString seqRef;
    QString note;
    QString noteEvidence;
  };

  typedef QList<Isoform *> IsoformList;

  struct EventComment
  {
    QStringList eventList;
    IsoformList isoformList;

  };

  struct Interactant
  {
   QString id;
   QString label;
   QString intactId;
  };

  struct InteractionComment
  {
    Interactant interactant1;
    Interactant interactant2;
    bool organismsDiffer;
    int experiments;
  };

  struct Comment
  {
    BpcComment * bpcComment;
    SubCellularLocationComment * subcellularLocationComment;
    ConflictComment * conflictComment;
    LinkComment * linkComment;
    InteractionComment * interactionComment;
    EventComment * eventComment;

    LocationList locationList;
    EvidencedString text;
    QString name;
    double mass;
    QString error;
    QString method;
    QString type;
    QString locationType;
    QString evidence;
  };

  typedef QList<Comment *> CommentList;

  struct NameList
  {
    QStringList personList;
    QStringList consortiumList;
  };

  struct Citation
  {
    QString title;
    NameList editorList;
    NameList authorList;
    QString locator;
    DbReferenceList dbReferenceList;
    QString type;
    QString date;
    Citation * citing;
    QString name;
    QString volume;
    QString first;
    QString last;
    QString publisher;
    QString city;
    QString db;
    QString country;
    QString number;
    QString institute;
  };

  struct Source
  {
    QString type;
    QString ref;
    QString evidence;
  };

  typedef QList<Source *> SourceList;


  struct Reference
  {
    Citation citation;
    QString key;
    QString evidence;
    QStringList scopeList;
    SourceList source;

  };

  typedef QList<Reference *> ReferenceList;

  struct Status
  {
    QString name;
    QString status;
  };

  typedef QList<Status *> StatusList;

  struct GeneLocation
  {
    StatusList nameList;
    QString type;
    QString evidence;
  };

  typedef QList<GeneLocation *> GeneLocationList;

  struct OrganismNameList
  {
    QString name;
    QString type;
  };


  struct Organism
  {
    OrganismNameList nameList;
    DbReferenceList dbReferenceList;
    QStringList lineage;
    QString key;
    QString evidence;
  };

  typedef QList<Organism *> OrganismList;

  struct GeneName
  {
    QString name;
    QString evidence;
    QString type;
  };

  typedef QList<GeneName *> GeneNameList;

  struct Gene
  {
    GeneNameList geneNameList;
  };

  typedef QList<Gene * > GeneList;

  struct ProteinName
  {
    EvidencedString fullName;
    EvidencedStringList shortNameList;
    QString scope;
    QString type;
    QString ref;
  };

  typedef QList<ProteinName *> ProteinNameList;

  Uniprot uniprot; // in Uniprotreader
  ProteinNameList proteinNameList;

  Sequence sequence;
  EvidenceList evidenceList;
  FeatureList featureList;
  KeywordList keywordList;
  QString proteinExistence;
  DbReferenceList dbReferenceList;
  ReferenceList referenceList;
  GeneLocationList geneLocationList;
  OrganismList organismHostList;
  Organism organism;
  GeneList geneList;

  CommentList commentList;

  typedef QList<ProteinDataSet *> ProteinDataList;



};
}

#endif /* PROTEINDATASET_H_ */
