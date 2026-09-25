#pragma once
#include <string>
#include <vector>
#include <ctime>

// A single point-in-time snapshot of a Part's state.
// This is what gives us "version control" for engineering parts,
// similar to how Teamcenter tracks revisions of a part/document.
struct VersionSnapshot {
    int versionNumber;
    std::string changeDescription;
    double costAtSnapshot;
    std::time_t timestamp;
};

// Base class for anything that can appear in a Bill of Materials.
// Assembly (a container of Parts) will inherit from this, which lets
// a BOM tree hold either leaf Parts or nested Assemblies polymorphically.
class Part {
protected:
    std::string id;
    std::string name;
    std::string material;
    double cost;       // unit cost
    double weight;      // in kg
    std::vector<VersionSnapshot> history;
    int currentVersion;

public:
    Part(std::string id, std::string name, std::string material,
         double cost, double weight);
    virtual ~Part() = default;

    // Polymorphic hooks — Assembly overrides these to roll up children.
    virtual double getCost() const;         // unit cost (overridden for assemblies)
    virtual double getWeight() const;
    virtual void display(int indent = 0) const;
    virtual bool isAssembly() const { return false; }

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getMaterial() const { return material; }

    // Version control
    void reviseVersion(const std::string& changeDescription, double newCost);
    void printHistory() const;
    int getVersion() const { return currentVersion; }

    // Simple serialization for save/load
    virtual std::string serialize() const;
};
