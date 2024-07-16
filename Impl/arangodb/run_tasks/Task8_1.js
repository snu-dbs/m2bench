db._useDatabase("Healthcare")

if (db._collection("drug_temp") != null) db.drug_temp.drop();
if (db._collection("target_temp") != null) db.target_temp.drop();
if (db._collection("has_bond") != null) db.has_bond.drop();

db._create("drug_temp")
db._create("target_temp")
db._createEdgeCollection("has_bond")

