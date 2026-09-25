#include "Assembly.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Assembly::Assembly(std::string id, std::string name, std::string material)
    : Part(std::move(id), std::move(name), std::move(material), 0.0, 0.0) {}

void Assembly::addComponent(Part* part, int quantity) {
    components.emplace_back(part, quantity);
}

double Assembly::getCost() const {
    double total = cost; // base assembly cost (labor/overhead)
    for (const auto& [part, qty] : components) {
        total += part->getCost() * qty; // recursive: works for nested assemblies too
    }
    return total;
}

double Assembly::getWeight() const {
    double total = weight;
    for (const auto& [part, qty] : components) {
        total += part->getWeight() * qty;
    }
    return total;
}

void Assembly::display(int indent) const {
    std::cout << std::string(indent, ' ') << "+ [" << id << "] " << name
              << " (v" << currentVersion << ", ASSEMBLY, rollup cost: $"
              << std::fixed << std::setprecision(2) << getCost() << ")\n";
    for (const auto& [part, qty] : components) {
        std::cout << std::string(indent + 2, ' ') << "x" << qty << " -> ";
        part->display(0);
    }
}

std::string Assembly::serialize() const {
    std::ostringstream oss;
    oss << "ASSEMBLY|" << id << "|" << name << "|" << material << "|"
        << currentVersion << "|children:";
    for (const auto& [part, qty] : components) {
        oss << part->getId() << ":" << qty << ",";
    }
    return oss.str();
}
