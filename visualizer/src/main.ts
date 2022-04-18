import Graph from "graphology";
import Sigma from "sigma";
import { NodeDisplayData } from "sigma/types";

import GUI from "lil-gui";

import "./style.css";

import RoadNodeCoords from "../../data/cal-road-node-coords.txt?raw";
import RoadNodeEdges from "../../data/cal-road-edges.txt?raw";

// fetch("http://localhost:8888/graph-type")
//   .then((response) => response.json())
//   .then((data) => console.log(data));

// fetch("http://localhost:8888/solve")
//   .then((response) => response.json())
//   .then((data) => console.log(data));

const graph = new Graph();
const graphContainer = document.getElementById(
  "graph-container"
) as HTMLElement;

let renderer: Sigma;

const graphOptions = {
  showEdges: true,
  nodeSize: 1,
  edgeThickness: 1,
  nodeColor: "#16a34a",
  edgeColor: "#ef4444",
  searchStart: 0,
  searchEnd: 0,
  searchDjikstra: () => console.log("Test"),
};

const graphOptionsGUI = new GUI();

graphOptionsGUI
  .add(graphOptions, "showEdges")
  .onChange(() => renderer.refresh());
graphOptionsGUI
  .add(graphOptions, "nodeSize", 1, 10, 0.1)
  .onFinishChange((val: number) => {
    graph.updateEachNodeAttributes((_, attr) => ({
      ...attr,
      size: val,
    }));
    renderer.refresh();
  });
graphOptionsGUI
  .add(graphOptions, "edgeThickness", 1, 10, 0.1)
  .onFinishChange((val: number) => {
    graph.updateEachEdgeAttributes((_, attr) => ({
      ...attr,
      size: val,
    }));
    renderer.refresh();
  });
graphOptionsGUI.addColor(graphOptions, "nodeColor");
graphOptionsGUI.addColor(graphOptions, "edgeColor");
graphOptionsGUI.add(graphOptions, "searchStart", 0, 21407, 1);
graphOptionsGUI.add(graphOptions, "searchEnd", 0, 21407, 1);
graphOptionsGUI.add(graphOptions, "searchDjikstra").name("Search - Djikstra");

function processCaliRoads() {
  // Process cali road lat/lng data
  let roadCoords = RoadNodeCoords.split("\n").map((e) =>
    e.split(" ").map((e) => Number(e))
  );

  // Find max/min lat/lng for coordinate scaling
  let [minLng, maxLng] = [roadCoords[0][1], roadCoords[0][1]];
  let [minLat, maxLat] = [roadCoords[0][2], roadCoords[0][2]];

  for (const point of roadCoords) {
    minLng = Math.min(minLng, point[1]);
    maxLng = Math.max(maxLng, point[1]);

    minLat = Math.min(minLat, point[2]);
    maxLat = Math.max(maxLat, point[2]);
  }

  const scaleTargetUpper = 10;
  const scaleTargetLower = 0;

  const scaleFactor = (scaleTargetUpper - scaleTargetLower) / (maxLat - minLat);

  // Scale cooordinates for canvas drawing
  roadCoords = roadCoords.map((point) => {
    return [
      point[0],
      (maxLng + point[1]) * scaleFactor,
      (maxLat + point[2]) * scaleFactor,
    ];
  });

  for (const point of roadCoords) {
    graph.addNode(point[0], {
      x: point[1],
      y: point[2],
      size: graphOptions.nodeSize,
      label: point[0],
      color: graphOptions.nodeColor,
    });
  }

  // Add edges
  const roadEdges = RoadNodeEdges.split("\n").map((e) =>
    e.split(" ").map((e) => Number(e))
  );

  for (const edge of roadEdges) {
    graph.addEdge(edge[1], edge[2], {
      weight: edge[3],
      color: graphOptions.edgeColor,
      size: graphOptions.edgeThickness,
    });
  }
}

processCaliRoads();

renderer = new Sigma(graph, graphContainer);
renderer.setSetting("nodeReducer", (node, data) => {
  const res: Partial<NodeDisplayData> = { ...data };

  return res;
});

renderer.setSetting("edgeReducer", (edge, data) => {
  const res: Partial<NodeDisplayData> = { ...data };

  if (!graphOptions.showEdges) {
    res.hidden = true;
  } else {
    res.hidden = false;
  }

  return res;
});
