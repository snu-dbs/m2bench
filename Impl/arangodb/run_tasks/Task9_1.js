db._useDatabase("Healthcare")

if (db._collection("drug_matrix") != null) db.drug_matrix.drop();
if (db._collection("similarity1") != null) db.similarity1.drop();
if (db._collection("inv_norm") != null) db.inv_norm.drop();
if (db._collection("similarity2") != null) db.similarity2.drop();
if (db._collection("drug_similarity") != null) db.drug_similarity.drop();

db._create("drug_matrix")
db._create("similarity1")
db._create("inv_norm")
db._create("similarity2")
db._create("drug_similarity")

db.drug_matrix.ensureIndex({type:"persistent", fields:["drug"]})
db.drug_matrix.ensureIndex({type:"persistent", fields:["adverse_effect"]})
db.similarity1.ensureIndex({type:"persistent", fields:["drug1"]})
db.similarity1.ensureIndex({type:"persistent", fields:["drug2"]})
db.inv_norm.ensureIndex({type:"persistent", fields:["drug1"]})
db.inv_norm.ensureIndex({type:"persistent", fields:["drug2"]})
db.similarity2.ensureIndex({type:"persistent", fields:["drug1"]})
db.similarity2.ensureIndex({type:"persistent", fields:["drug2"]})
db.drug_similarity.ensureIndex({type:"persistent", fields:["drug1"]})
db.drug_similarity.ensureIndex({type:"persistent", fields:["drug2"]})
