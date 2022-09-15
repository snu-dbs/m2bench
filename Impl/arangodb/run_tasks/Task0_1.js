/* 0. Create TEMP_A, TEMP_C, LR_w, and LR_w_new */
db.TEMP_A.drop();
db.TEMP_C.drop();
db.LR_w.drop();
db.LR_w_new.drop();

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

