conn = new Mongo();
  
db = conn.getDB("Disaster");

db.auth('','');

db.Site.renameCollection('SiteBk');

db.Site.insert(db.SiteBk.findOne());
db.Site.createIndex({geometry:"2dsphere"});
db.Site.remove({});

db.SiteBk.find().forEach(function(x){
        db.Site.insert(x);
})

db.SiteBk.drop();

