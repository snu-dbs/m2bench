/* 0. Create TEMP_A, TEMP_C, LR_w, and LR_w_new */
if (db._collection("TEMP_A") != null) db.TEMP_A.drop();
if (db._collection("TEMP_C") != null) db.TEMP_C.drop();
if (db._collection("LR_w") != null) db.LR_w.drop();
if (db._collection("LR_w_new") != null) db.LR_w_new.drop();

db._create("TEMP_A");
db._create("TEMP_C");
db._create("LR_w");
db._create("LR_w_new");

/* 2. Create indexes (tag_id), (person_id), and (person_id, tag_id) for TEMP_A,
 *     (person_id) for TEMP_C, and
 *         (i) for LR_w  */
db.TEMP_A.ensureIndex({'type': 'persistent', 'fields': ['tag_id']});
db.TEMP_A.ensureIndex({'type': 'persistent', 'fields': ['person_id']});
db.TEMP_A.ensureIndex({'type': 'persistent', 'fields': ['person_id', 'tag_id']});

db.TEMP_C.ensureIndex({'type': 'persistent', 'fields': ['person_id']});

db.LR_w.ensureIndex({'type': 'persistent', 'fields': ['i']});

