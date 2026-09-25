#pragma once
#include "Part.h"
#include "Assembly.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

// The central engine. Owns every Part/Assembly, and provides the
// operations a real PLM system needs on top of a raw BOM tree:
//   - O(1) lookup by ID              (hash map)
//   - "where used" reverse lookup    (hash map of adjacency lists)
//   - circular dependency detection  (graph DFS, visited + recursion stack)
//   - cost/weight rollup             (recursive traversal via Assembly::getCost)
//   - simple flat-file persistence
class BOMManager {
private:
    std::unordered_map<std::string, Part*> registry;                 // id -> Part*
    std::unordered_map<std::string, std::vector<std::string>> whereUsed; // childId -> [parentIds]

    // DFS helper for cycle detection over the "assembly contains part" graph.
    bool hasCycleDFS(const std::string& nodeId,
                      std::unordered_set<std::string>& visited,
                      std::unordered_set<std::string>& recursionStack) const;

public:
    ~BOMManager();

    Part* createPart(const std::string& id, const std::string& name,
                      const std::string& material, double cost, double weight);
    Assembly* createAssembly(const std::string& id, const std::string& name,
                              const std::string& material = "Mixed");

    // Returns false (and refuses to add) if linking child under parent
    // would introduce a circular dependency, e.g. Engine contains
    // Piston contains ... contains Engine.
    bool addComponentToAssembly(const std::string& assemblyId,
                                 const std::string& childId, int quantity);

    Part* find(const std::string& id) const;
    std::vector<std::string> whereUsedQuery(const std::string& partId) const;

    void printBOMTree(const std::string& rootId) const;
    void printAllParts() const;

    bool saveToFile(const std::string& filename) const;
    // (loadFromFile omitted for brevity in v1 — see README "Extensions")
};
