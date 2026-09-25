#include "BOMManager.h"
#include <iostream>
#include <fstream>

BOMManager::~BOMManager() {
    for (auto& [id, part] : registry) delete part;
}

Part* BOMManager::createPart(const std::string& id, const std::string& name,
                              const std::string& material, double cost, double weight) {
    Part* p = new Part(id, name, material, cost, weight);
    registry[id] = p;
    return p;
}

Assembly* BOMManager::createAssembly(const std::string& id, const std::string& name,
                                      const std::string& material) {
    Assembly* a = new Assembly(id, name, material);
    registry[id] = a;
    return a;
}

Part* BOMManager::find(const std::string& id) const {
    auto it = registry.find(id);
    return it == registry.end() ? nullptr : it->second;
}

// Classic "3-color" DFS cycle detection: white (unvisited), gray (in
// recursion stack = currently being explored), black (fully explored).
// If we ever reach a gray node again, we've found a cycle.
bool BOMManager::hasCycleDFS(const std::string& nodeId,
                              std::unordered_set<std::string>& visited,
                              std::unordered_set<std::string>& recursionStack) const {
    visited.insert(nodeId);
    recursionStack.insert(nodeId);

    Part* node = find(nodeId);
    if (node && node->isAssembly()) {
        auto* asmNode = static_cast<Assembly*>(node);
        for (const auto& [child, qty] : asmNode->getComponents()) {
            const std::string& childId = child->getId();
            if (recursionStack.count(childId)) return true;            // back-edge -> cycle
            if (!visited.count(childId) && hasCycleDFS(childId, visited, recursionStack))
                return true;
        }
    }
    recursionStack.erase(nodeId);
    return false;
}

bool BOMManager::addComponentToAssembly(const std::string& assemblyId,
                                         const std::string& childId, int quantity) {
    Part* parentPart = find(assemblyId);
    Part* childPart = find(childId);
    if (!parentPart || !childPart || !parentPart->isAssembly()) {
        std::cout << "  [!] Invalid assembly or part ID.\n";
        return false;
    }
    auto* parent = static_cast<Assembly*>(parentPart);

    // Tentatively link, then check for a cycle. If one exists, roll back.
    parent->addComponent(childPart, quantity);

    std::unordered_set<std::string> visited, recStack;
    if (hasCycleDFS(assemblyId, visited, recStack)) {
        // Roll back — rebuild components list without the last addition.
        std::vector<std::pair<Part*, int>> restored;
        for (const auto& c : parent->getComponents())
            if (c.first != childPart) restored.push_back(c);
        // Simplest rollback: rebuild via a fresh addComponent loop.
        Assembly fixed(parent->getId(), parent->getName());
        for (auto& c : restored) fixed.addComponent(c.first, c.second);
        *parent = fixed;

        std::cout << "  [!] REJECTED: adding '" << childId << "' to '" << assemblyId
                  << "' would create a circular dependency.\n";
        return false;
    }

    whereUsed[childId].push_back(assemblyId);
    std::cout << "  [OK] Linked " << childId << " x" << quantity << " into " << assemblyId << "\n";
    return true;
}

std::vector<std::string> BOMManager::whereUsedQuery(const std::string& partId) const {
    auto it = whereUsed.find(partId);
    if (it == whereUsed.end()) return {};
    return it->second;
}

void BOMManager::printBOMTree(const std::string& rootId) const {
    Part* root = find(rootId);
    if (!root) { std::cout << "Not found: " << rootId << "\n"; return; }
    root->display(0);
}

void BOMManager::printAllParts() const {
    for (const auto& [id, part] : registry) part->display(0);
}

bool BOMManager::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) return false;
    for (const auto& [id, part] : registry) out << part->serialize() << "\n";
    out.close();
    return true;
}
