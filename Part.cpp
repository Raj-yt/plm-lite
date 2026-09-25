#include "Part.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Part::Part(std::string id, std::string name, std::string material,
           double cost, double weight)
    : id(std::move(id)), name(std::move(name)), material(std::move(material)),
      cost(cost), weight(weight), currentVersion(1) {
    history.push_back({currentVersion, "Initial release", cost, std::time(nullptr)});
}

double Part::getCost() const { return cost; }
double Part::getWeight() const { return weight; }

void Part::display(int indent) const {
    std::cout << std::string(indent, ' ') << "- [" << id << "] " << name
              << " (v" << currentVersion << ", " << material
              << ", $" << std::fixed << std::setprecision(2) << getCost() << ")\n";
}

void Part::reviseVersion(const std::string& changeDescription, double newCost) {
    currentVersion++;
    cost = newCost;
    history.push_back({currentVersion, changeDescription, newCost, std::time(nullptr)});
}

void Part::printHistory() const {
    std::cout << "Version history for " << name << " [" << id << "]:\n";
    for (const auto& snap : history) {
        std::cout << "  v" << snap.versionNumber << " - " << snap.changeDescription
                  << " ($" << std::fixed << std::setprecision(2) << snap.costAtSnapshot << ")\n";
    }
}

std::string Part::serialize() const {
    std::ostringstream oss;
    oss << "PART|" << id << "|" << name << "|" << material << "|"
        << cost << "|" << weight << "|" << currentVersion;
    return oss.str();
}
