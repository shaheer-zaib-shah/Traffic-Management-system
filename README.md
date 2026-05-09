# 🚦 Traffic Management System

A C++ backend application that models a city road network as a weighted graph and computes optimal routes using **Dijkstra's Shortest Path algorithm**. It exposes a REST API via the **Crow framework** and is paired with a multi-page HTML/CSS frontend.

---

## Features

- **Shortest Path Finder** — Dijkstra's algorithm across Car, Bike, Train, and Plane routes
- **Vehicle-Aware Routing** — Each transport mode has its own speed and cost-per-km
- **Dynamic Graph Management** — Add cities and routes at runtime via API
- **Road Closure & Reopening** — Close/reopen individual roads and query all closed roads
- **REST API** — Clean JSON endpoints served via the Crow C++ HTTP framework
- **Multi-page Frontend** — Eight HTML/CSS option pages with background video/image support
- **File-based Graph Loading** — Road network loaded from `routes.txt` on startup

---

## Tech Stack

| Layer | Technology |
|---|---|
| Backend | C++17 (GCC/MinGW) |
| HTTP Framework | [Crow](https://crowcpp.org/) (single-header `crow_all.h`) |
| Build System | CMake |
| IDE | CLion (Visual Studio / MSVC not supported) |
| Algorithm | Dijkstra's Shortest Path (min-heap priority queue) |
| Frontend | HTML5, CSS3 (vanilla) |

---

## Project Structure

```
TrafficManagementSystem/
├── SPF.cpp              # Main backend — graph logic + Crow REST API
├── crow_all.h           # Crow single-header HTTP library
├── CMakeLists.txt       # CMake build configuration
├── routes.txt           # Initial road network data
├── index.html           # Landing page
├── opt1.html – opt8.html  # Feature pages
├── opt1.css  – opt8.css   # Page-specific stylesheets
├── style.css            # Shared styles
├── bgpicture.jpg        # Background image asset
└── bgvideo.mp4          # Background video asset
```

---

## Getting Started

### Prerequisites

- **CLion** (recommended IDE — Visual Studio is not supported due to Crow library incompatibility with MSVC)
- GCC/MinGW compiler (bundled with CLion's default toolchain on Windows)
- CMake 3.16+
- Boost libraries (required by Crow)

> ⚠️ **Note:** This project does **not** build with Visual Studio / MSVC. The Crow HTTP library relies on POSIX-style headers and GCC/Clang extensions that are incompatible with the MSVC compiler. Use CLion with a MinGW or GCC toolchain.

### Build & Run (CLion)

1. Open the project folder in **CLion**
2. CLion will automatically detect `CMakeLists.txt` and configure the project
3. Click **Build** (Ctrl+F9) then **Run** (Shift+F10)

The server starts at `http://127.0.0.1:18080`.  
Open `http://127.0.0.1:18080/` in your browser to load the frontend.

---

## API Reference

| Method | Endpoint | Description |
|---|---|---|
| GET | `/shortest/{start}/{end}/{vehicle}` | Find shortest path (vehicle: 0=Car, 1=Bike, 2=Train, 3=Plane) |
| GET | `/cities` | List all cities in the graph |
| GET | `/routes` | List all roads with vehicle type and open/closed status |
| GET | `/add_city/{name}` | Add a new city to the graph |
| GET | `/add_route/{city1}/{city2}/{dist}/{vehicle}` | Add a road between two cities |
| POST | `/closeRoad` | Close a road (`{"city1": "...", "city2": "..."}`) |
| POST | `/reopenRoad` | Reopen a closed road |
| GET | `/closedRoads` | List all currently closed roads |

### Example

```bash
# Find shortest car route from Islamabad to Lahore
curl http://127.0.0.1:18080/shortest/islamabad/lahore/0
```

```json
{
  "found": true,
  "path": ["islamabad", "gt_road", "gujranwala", "lahore"],
  "totalDistance": 240,
  "totalTime": 3.0,
  "totalCost": 36.0
}
```

---

## Road Network (`routes.txt`)

The graph is loaded from `routes.txt` at startup. Each line follows the format:

```
CityA CityB Distance VehicleType
```

Supported vehicle types: `car`, `bike`, `train`, `plane`

Comments start with `#`. Example:

```
# CAR ROUTES
Islamabad GT_Road 10 car
GT_Road Gujranwala 150 car
Gujranwala Lahore 80 car

# TRAIN ROUTES
Islamabad Rawalpindi_Junction 20 train
```

---

## Vehicle Profiles

| Vehicle | Speed (km/h) | Cost (per km) |
|---|---|---|
| Car | 80 | Rs. 0.15 |
| Bike | 60 | Rs. 0.05 |
| Train | 100 | Rs. 0.10 |
| Plane | 800 | Rs. 0.50 |

---

## Algorithm

The core routing uses **Dijkstra's algorithm** with a min-heap priority queue (`std::priority_queue`). Edge weights are distances in km. Only roads matching the selected vehicle type and with `isOpen = true` are traversed. After finding the shortest distance, the path is reconstructed via a parent-pointer map.

---

## Notes

- City names are normalized to lowercase with spaces replaced by underscores internally.
- The `crow_all.h` single header is included directly — no separate Crow installation needed.
- Crow is incompatible with MSVC; always build using GCC/MinGW via CLion.
- CORS headers (`Access-Control-Allow-Origin: *`) are set on all responses for local development.
