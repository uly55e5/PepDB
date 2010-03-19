PREPARE insert_uniprot FROM "INSERT INTO pepdb.uniprot (dataset,created,modified,version,proteinexistence) VALUES (?,?,?,?,?)";
PREPARE insert_sequence FROM "INSERT INTO pepdb.sequences ( uniprotId, sequence, length, mass, checksum, modified, version,precursor,fragment ) VALUES (?,?,?,?,?,?,?,?,?)";
PREPARE insert_uniprotaccession FROM "INSERT INTO pepdb.uniprotAccessions (uniprotId, accession) VALUES (?,?)";
PREPARE insert_uniprotname FROM "INSERT INTO pepdb.uniprotNames( uniprotId, name) VALUES (?,?)";
PREPARE insert_proteinnames FROM "INSERT INTO pepdb.proteinNames( uniprotId, scope, type, ref, fullNameId) VALUES (?,?,?,?,?)";
PREPARE insert_evidenced FROM "INSERT INTO pepdb.evidencedStrings (name, evidence, status) VALUES (?,?,?)";
PREPARE insert_shortName FROM "INSERT INTO pepdb.shortNames (protNameId, shortNameId) VALUES (?,?)";

PREPARE insert_gene FROM "INSERT INTO pepdb.genes (uniprotID) VALUES (?)";
PREPARE insert_genename FROM "INSERT INTO pepdb.geneNames(geneId, name, evidence, type) VALUES(?,?,?,?)";

PREPARE insert_feature FROM "INSERT INTO pepdb.features(uniprotId, original, locationId, type, status, id, description, evidence, ref) VALUES (?,?,?,?,?,?,?,?,?)";
PREPARE insert_location FROM "INSERT INTO pepdb.locations(beginId, endId, sequence) VALUES(?,?,?)";
PREPARE insert_position FROM "INSERT INTO pepdb.positions(position,status,evidence) VALUES (?,?,?)";
PREPARE insert_variation FROM "INSERT INTO pepdb.featureVariations(featureId, variation) VALUES (?,?)";
