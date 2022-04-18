import Graph from "graphology";
import Sigma from "sigma";
import { NodeDisplayData } from "sigma/types";

import GUI from "lil-gui";

import "./style.css";

import RoadNodeCoords from "../../data/cal-road-node-coords.txt?raw";
import RoadNodeEdges from "../../data/cal-road-edges.txt?raw";

const graph = new Graph();
const graphContainer = document.getElementById(
  "graph-container"
) as HTMLElement;

let currentPath: number[] = [];

let renderer: Sigma;

const graphOptions = {
  showEdges: true,
  nodeSize: 1,
  edgeThickness: 1,
  nodeColor: "#ef4444",
  nodeFoundColor: "#c026d3",
  edgeColor: "#64748b",
  searchStart: 0,
  searchEnd: 0,
  searchDjikstra: () => dispatchSearch("dijkstra"),
  searchBellmanFord: () => dispatchSearch("bellmanford"),
  distance: "No distance",
};

const graphOptionsGUI = new GUI();

graphOptionsGUI
  .add(graphOptions, "showEdges")
  .onChange(() => renderer.refresh());

const folderVisualTweaks = graphOptionsGUI.addFolder("Visuals");

folderVisualTweaks
  .add(graphOptions, "nodeSize", 1, 10, 0.1)
  .onFinishChange((val: number) => {
    graph.updateEachNodeAttributes((_, attr) => ({
      ...attr,
      size: val,
    }));
    renderer.refresh();
  });
folderVisualTweaks
  .add(graphOptions, "edgeThickness", 1, 10, 0.1)
  .onFinishChange((val: number) => {
    graph.updateEachEdgeAttributes((_, attr) => ({
      ...attr,
      size: val,
    }));
    renderer.refresh();
  });
folderVisualTweaks.addColor(graphOptions, "nodeColor");
folderVisualTweaks.addColor(graphOptions, "nodeFoundColor");
folderVisualTweaks.addColor(graphOptions, "edgeColor");

graphOptionsGUI.add(graphOptions, "searchStart", 0, 21407, 1);
graphOptionsGUI.add(graphOptions, "searchEnd", 0, 21407, 1);
graphOptionsGUI.add(graphOptions, "searchDjikstra").name("Search - Djikstra");
graphOptionsGUI
  .add(graphOptions, "searchBellmanFord")
  .name("Search - Bellman Ford");

const graphOptionsControllerDistance = graphOptionsGUI
  .add(graphOptions, "distance")
  .disable();

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

  if (currentPath.length != 0) {
    if (currentPath.includes(Number(node))) {
      res.color = graphOptions.nodeFoundColor;
      res.size = graphOptions.nodeSize * 2;
    } else {
      res.color = graphOptions.nodeColor;
      res.size = graphOptions.nodeSize;
    }
  } else {
    res.color = graphOptions.nodeColor;
  }

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

function dispatchSearch(type: "dijkstra" | "bellmanford") {
  const url = new URL("http://localhost:8888/search");
  url.searchParams.set("from", graphOptions.searchStart.toString());
  url.searchParams.set("to", graphOptions.searchEnd.toString());
  url.searchParams.set("type", type);

  fetch(url.toString())
    .then((response) => response.json())
    .then((data) => {
      const pathProcessed = data.path.split(",").map((e) => Number(e));
      pathProcessed.pop();

      currentPath = pathProcessed;

      graphOptionsControllerDistance.setValue(
        `${((data.distance / 1_000_000) * 69).toFixed(3)} mi`
      );
      renderer.refresh();
    });
}
