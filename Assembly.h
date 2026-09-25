#pragma once
#include "Part.h"
#include <vector>
#include <utility>

// An Assembly IS-A Part (inheritance), but also CONTAINS other Parts
// (composition) — the classic Composite design pattern. This is exactly
// how real BOMs work: an "Engine" assembly contains "Piston" parts,
// which could themselves be sub-assemblies.
class Assembly : public Part {
private:
    // child part id + quantity used in this assembly
    std::vector<std::pair<Part*, int>> components;

public:
    Assembly(std::string id, std::string name, std::string material = "Mixed");

    void addComponent(Part* part, int quantity);
    const std::vector<std::pair<Part*, int>>& getComponents() const { return components; }

    // Overridden: cost of an assembly = its own base cost + recursive
    // sum of (child cost * quantity) for every child, however deep.
    double getCost() const override;
    double getWeight() const override;
    void display(int indent = 0) const override;
    bool isAssembly() const override { return true; }

    std::string serialize() const override;
};
