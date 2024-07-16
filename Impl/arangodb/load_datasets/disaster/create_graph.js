db._useDatabase("Disaster");

var graph_module = require("org/arangodb/general-graph");
var graph = graph_module._create("Road_Network");

graph._addVertexCollection("Roadnode");
var rel = graph_module._relation("Road", ["Roadnode"], ["Roadnode"]);
graph._extendEdgeDefinitions(rel);
graph = graph_module._graph("Road_Network");
