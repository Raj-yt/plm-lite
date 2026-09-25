# PLM-Lite: A Mini Product Lifecycle & BOM Management Engine

A C++ engine that models the core problem real PLM software (like Siemens
Teamcenter) solves: managing multi-level **Bills of Materials (BOMs)**,
detecting invalid product structures, tracking part revisions, and
answering impact-analysis queries like "everywhere this part is used."

Built as a systems-level project, not a toy — every algorithm here maps to
a real, named problem in CAD/PLM tooling.

## Why this project

Siemens PLM builds software for managing the entire lifecycle of a
product's data — parts, assemblies, revisions, and how they all relate.
This project is a small, self-contained model of that same problem:

| Real PLM concept              | Implemented here as                              |
|--------------------------------|---------------------------------------------------|
| Part / Assembly hierarchy       | `Part` base class, `Assembly` (Composite pattern) |
| Bill of Materials (BOM)         | Tree of `Part*` built at runtime                  |
| Invalid circular BOM structure  | Graph DFS cycle detection before linking          |
| Cost/weight rollup              | Recursive traversal over the BOM tree             |
| "Where used" / impact analysis  | Reverse-index hash map                            |
| Part revisions / ECOs           | Per-part version history with snapshots           |
| Fast part lookup at scale       | O(1) hash map registry (`unordered_map`)          |

## Architecture

```
Part (base class)
 ├─ id, name, material, cost, weight
 ├─ version history (vector<VersionSnapshot>)
 └─ virtual getCost(), display(), serialize()

Assembly : public Part          <-- inheritance
 ├─ vector<pair<Part*, int>> components   <-- composition (Composite pattern)
 └─ getCost() override: base cost + Σ(child.getCost() * qty), recursively

BOMManager                      <-- owns everything, exposes operations
 ├─ unordered_map<string, Part*> registry        (O(1) lookup)
 ├─ unordered_map<string, vector<string>> whereUsed  (reverse index)
 ├─ hasCycleDFS(): 3-color DFS cycle detection
 ├─ addComponentToAssembly(): links + rejects if it would create a cycle
 └─ saveToFile(): flat-file persistence
```

## Key algorithm: circular dependency detection

Adding a component to an assembly is a graph-edge insertion. Before
committing it, the manager runs a DFS with a **visited set** and a
**recursion-stack set** (classic white/gray/black coloring). If the DFS
ever revisits a node currently on the recursion stack, that's a back-edge
→ a cycle → the link is rejected and rolled back.

This is the same class of bug that causes real CAD/PLM corruption: e.g. an
assembly accidentally referencing an ancestor of itself. Catching it at
insert time, not at query time, is the whole point.

## Build & run

```bash
g++ -std=c++17 -Wall -Iinclude src/*.cpp -o plm_lite
./plm_lite
```

(A `CMakeLists.txt` is also included if you prefer `cmake -B build && cmake --build build`.)

## Demo data

`main.cpp` builds a small bicycle BOM (Frame, 2x Wheel Assembly, Drivetrain)
and deliberately attempts one illegal circular link, which gets rejected,
so you can see the cycle detection fire on first run.

## What I'd extend next

- `loadFromFile()` to make persistence round-trip (currently save-only)
- Multi-user check-out/check-in locking (real PLM systems block concurrent edits)
- Diff view between two versions of a part, not just a flat history log
- Swap the flat-file format for JSON so it's a real interchange format

