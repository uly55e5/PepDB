PREPARE insert_uniprot FROM "INSERT INTO pepdb.uniprot (dataset,created,modified,version,proteinexistence) VALUES (?,?,?,?,?)";
PREPARE insert_sequence FROM "INSERT INTO pepdb.sequences ( uniprotId, sequence, length, mass, checksum, modified, version,precursor,fragment ) VALUES (?,?,?,?,?,?,?,?,?)";
PREPARE insert_uniprotaccession FROM "INSERT INTO pepdb.uniprotAccessions (uniprotId, name) VALUES (?,?)";
PREPARE insert_uniprotname FROM "INSERT INTO pepdb.uniprotNames( uniprotId, accession) VALUES (?,?)";