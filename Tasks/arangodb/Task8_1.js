db._useDatabase("Healthcare")

db.drug_temp.drop()
db.target_temp.drop()
db.has_bond.drop()

db._create("drug_temp")
db._create("target_temp")
db._createEdgeCollection("has_bond")

