let nodes = [];
let edges = [];

const svg = document.querySelector("#map");
const startSelect = document.querySelector("#startSelect");
const goalSelect = document.querySelector("#goalSelect");
const distanceValue = document.querySelector("#distanceValue");
const routeNodes = document.querySelector("#routeNodes");
const resetButton = document.querySelector("#resetButton");
const algorithmButtons = document.querySelectorAll("[data-algorithm]");
const exportButtons = document.querySelectorAll("[data-export]");

let nodeById = new Map();
let adjacency = new Map();
let startId = 1;
let goalId = 24;
let selectionTarget = "start";
let algorithm = "astar";
let currentPath = { found: false, distance: 0, nodes: [] };

function parseCsv(text) {
  const rows = text.trim().split(/\r?\n/);
  const headers = rows.shift().split(",").map((header) => header.trim());

  return rows
    .filter((row) => row.trim().length > 0)
    .map((row) => {
      const values = row.split(",").map((value) => value.trim());
      return Object.fromEntries(headers.map((header, index) => [header, values[index]]));
    });
}

async function loadCsv(path) {
  const response = await fetch(path);
  if (!response.ok) {
    throw new Error(`Cannot load ${path}`);
  }
  return parseCsv(await response.text());
}

async function loadMapData() {
  const [nodeRows, edgeRows] = await Promise.all([
    loadCsv("../resources/nodes.csv"),
    loadCsv("../resources/edges.csv")
  ]);

  nodes = nodeRows.map((row) => ({
    id: Number(row.id),
    x: Number(row.x),
    y: Number(row.y)
  }));

  edges = edgeRows.map((row) => ({
    from: Number(row.from),
    to: Number(row.to),
    distance: Number(row.distance)
  }));
}

function buildGraph() {
  nodeById = new Map(nodes.map((node) => [node.id, node]));
  adjacency = new Map(nodes.map((node) => [node.id, []]));

  for (const edge of edges) {
    adjacency.get(edge.from).push({ id: edge.to, distance: edge.distance });
    adjacency.get(edge.to).push({ id: edge.from, distance: edge.distance });
  }
}

function svgElement(tag, attributes = {}) {
  const element = document.createElementNS("http://www.w3.org/2000/svg", tag);
  for (const [key, value] of Object.entries(attributes)) {
    element.setAttribute(key, value);
  }
  return element;
}

function roadWidth(distance) {
  if (distance >= 170) return 8;
  if (distance >= 120) return 6;
  return 4;
}

function distanceBetween(a, b) {
  const dx = a.x - b.x;
  const dy = a.y - b.y;
  return Math.sqrt(dx * dx + dy * dy);
}

function heuristic(fromId, toId) {
  return distanceBetween(nodeById.get(fromId), nodeById.get(toId));
}

function shortestPath(start, goal, useHeuristic) {
  const distances = new Map(nodes.map((node) => [node.id, Infinity]));
  const previous = new Map();
  const frontier = [{ id: start, priority: 0 }];
  distances.set(start, 0);

  while (frontier.length > 0) {
    frontier.sort((a, b) => a.priority - b.priority);
    const current = frontier.shift();

    if (current.id === goal) {
      break;
    }

    for (const neighbor of adjacency.get(current.id)) {
      const candidate = distances.get(current.id) + neighbor.distance;
      if (candidate < distances.get(neighbor.id)) {
        distances.set(neighbor.id, candidate);
        previous.set(neighbor.id, current.id);
        const priority = candidate + (useHeuristic ? heuristic(neighbor.id, goal) : 0);
        frontier.push({ id: neighbor.id, priority });
      }
    }
  }

  if (!Number.isFinite(distances.get(goal))) {
    return { found: false, distance: 0, nodes: [] };
  }

  const path = [goal];
  let current = goal;
  while (current !== start) {
    current = previous.get(current);
    path.push(current);
  }

  path.reverse();
  return { found: true, distance: distances.get(goal), nodes: path };
}

function edgeKey(from, to) {
  return `${from}-${to}`;
}

function selectedEdgeKeys(path) {
  const keys = new Set();
  for (let i = 1; i < path.nodes.length; i += 1) {
    keys.add(edgeKey(path.nodes[i - 1], path.nodes[i]));
    keys.add(edgeKey(path.nodes[i], path.nodes[i - 1]));
  }
  return keys;
}

function populateSelects() {
  for (const select of [startSelect, goalSelect]) {
    select.innerHTML = "";
    for (const node of nodes) {
      const option = document.createElement("option");
      option.value = String(node.id);
      option.textContent = `Node ${node.id}`;
      select.appendChild(option);
    }
  }
}

function drawDistricts() {
  const districts = [
    { x: 32, y: 44, width: 210, height: 118, fill: "#d9ead7" },
    { x: 474, y: 72, width: 148, height: 124, fill: "#dbeafe" },
    { x: 96, y: 362, width: 180, height: 82, fill: "#fef3c7" },
    { x: 430, y: 352, width: 132, height: 88, fill: "#dcfce7" }
  ];

  for (const district of districts) {
    svg.appendChild(svgElement("rect", {
      ...district,
      rx: 14,
      class: "district"
    }));
  }

  const labels = [
    { x: 72, y: 122, text: "Park" },
    { x: 512, y: 126, text: "Lake" },
    { x: 128, y: 412, text: "Market" },
    { x: 462, y: 410, text: "Campus" }
  ];

  for (const label of labels) {
    const text = svgElement("text", { x: label.x, y: label.y, class: "map-label" });
    text.textContent = label.text;
    svg.appendChild(text);
  }
}

function drawRoads(path) {
  const pathEdges = selectedEdgeKeys(path);

  for (const edge of edges) {
    const from = nodeById.get(edge.from);
    const to = nodeById.get(edge.to);
    const highlighted = pathEdges.has(edgeKey(edge.from, edge.to));
    const casing = svgElement("line", {
      x1: from.x,
      y1: from.y,
      x2: to.x,
      y2: to.y,
      class: highlighted ? "route-casing" : "road-casing",
      "stroke-width": highlighted ? 13 : roadWidth(edge.distance) + 4
    });
    const road = svgElement("line", {
      x1: from.x,
      y1: from.y,
      x2: to.x,
      y2: to.y,
      class: highlighted ? "route" : "road",
      "stroke-width": highlighted ? 8 : roadWidth(edge.distance)
    });

    svg.appendChild(casing);
    svg.appendChild(road);
  }
}

function drawNodes() {
  for (const node of nodes) {
    const isStart = node.id === startId;
    const isGoal = node.id === goalId;

    if (isStart || isGoal) {
      const marker = svgElement("circle", {
        cx: node.x,
        cy: node.y,
        r: 16,
        fill: isStart ? "var(--start)" : "var(--goal)",
        stroke: "#ffffff",
        "stroke-width": 4,
        class: "marker"
      });
      const label = svgElement("text", {
        x: node.x,
        y: node.y + 1,
        class: "marker-label"
      });
      label.textContent = isStart ? "S" : "E";
      svg.appendChild(marker);
      svg.appendChild(label);
    } else {
      svg.appendChild(svgElement("circle", {
        cx: node.x,
        cy: node.y,
        r: 4,
        class: "node-dot"
      }));
    }

    const hitArea = svgElement("circle", {
      cx: node.x,
      cy: node.y,
      r: 18,
      class: "node-hit",
      "data-node-id": node.id
    });
    svg.appendChild(hitArea);
  }
}

function updateRoutePanel(path) {
  distanceValue.textContent = path.found ? String(Math.round(path.distance)) : "No route";
  routeNodes.innerHTML = "";

  for (const id of path.nodes) {
    const item = document.createElement("li");
    item.textContent = `Node ${id}`;
    routeNodes.appendChild(item);
  }
}

function render() {
  startSelect.value = String(startId);
  goalSelect.value = String(goalId);
  currentPath = shortestPath(startId, goalId, algorithm === "astar");

  svg.innerHTML = "";
  drawDistricts();
  drawRoads(currentPath);
  drawNodes();
  updateRoutePanel(currentPath);
}

function chooseNode(id) {
  if (selectionTarget === "start") {
    startId = id;
    selectionTarget = "goal";
  } else {
    goalId = id;
    selectionTarget = "start";
  }
  render();
}

function routeRows() {
  return currentPath.nodes.map((nodeId, index) => ({
    order: index + 1,
    node_id: nodeId
  }));
}

function downloadText(filename, text, type) {
  const blob = new Blob([text], { type });
  const url = URL.createObjectURL(blob);
  const link = document.createElement("a");
  link.href = url;
  link.download = filename;
  document.body.appendChild(link);
  link.click();
  link.remove();
  URL.revokeObjectURL(url);
}

function exportCsv() {
  const rows = routeRows();
  const csv = ["order,node_id", ...rows.map((row) => `${row.order},${row.node_id}`)].join("\n");
  downloadText("route_result.csv", `${csv}\n`, "text/csv");
}

function exportJson() {
  const payload = {
    algorithm: algorithm === "astar" ? "A*" : "Dijkstra",
    start: startId,
    destination: goalId,
    found: currentPath.found,
    distance: currentPath.distance,
    path: currentPath.nodes
  };
  downloadText("route_result.json", `${JSON.stringify(payload, null, 2)}\n`, "application/json");
}

function exportSvg() {
  const clone = svg.cloneNode(true);
  clone.setAttribute("xmlns", "http://www.w3.org/2000/svg");
  const serialized = new XMLSerializer().serializeToString(clone);
  downloadText("route_map.svg", serialized, "image/svg+xml");
}

function bindEvents() {
  startSelect.addEventListener("change", () => {
    startId = Number(startSelect.value);
    selectionTarget = "goal";
    render();
  });

  goalSelect.addEventListener("change", () => {
    goalId = Number(goalSelect.value);
    selectionTarget = "start";
    render();
  });

  resetButton.addEventListener("click", () => {
    startId = 1;
    goalId = 24;
    selectionTarget = "start";
    render();
  });

  for (const button of algorithmButtons) {
    button.addEventListener("click", () => {
      algorithm = button.dataset.algorithm;
      for (const candidate of algorithmButtons) {
        candidate.classList.toggle("is-active", candidate === button);
      }
      render();
    });
  }

  for (const button of exportButtons) {
    button.addEventListener("click", () => {
      const format = button.dataset.export;
      if (format === "csv") exportCsv();
      if (format === "json") exportJson();
      if (format === "svg") exportSvg();
    });
  }

  svg.addEventListener("click", (event) => {
    const hit = event.target.closest("[data-node-id]");
    if (!hit) {
      return;
    }
    chooseNode(Number(hit.dataset.nodeId));
  });
}

async function boot() {
  try {
    await loadMapData();
    buildGraph();
    populateSelects();
    bindEvents();
    render();
  } catch (error) {
    distanceValue.textContent = "Load error";
    routeNodes.innerHTML = "<li>Start a local server from the project folder to load CSV data.</li>";
    console.error(error);
  }
}

boot();
