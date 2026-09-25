#include "BOMManager.h"
#include <iostream>
#include <iomanip>

// Builds a small demo product structure representing a bicycle,
// modeled the way a real BOM would look in a PLM system:
//
//   Bicycle (assembly)
//    +-- Frame (part)
//    +-- Wheel Assembly (assembly) x2
//         +-- Rim (part)
//         +-- Tire (part)
//         +-- Spoke (part) x32
//    +-- Drivetrain (assembly)
//         +-- Chain (part)
//         +-- Gear Set (part)
void buildDemoBOM(BOMManager& mgr) {
    std::cout << "=== Building demo BOM: Bicycle ===\n";

    mgr.createPart("P-FRAME", "Aluminum Frame", "Aluminum", 45.00, 2.1);
    mgr.createPart("P-RIM", "Wheel Rim", "Alloy", 12.00, 0.6);
    mgr.createPart("P-TIRE", "Tire", "Rubber", 8.00, 0.4);
    mgr.createPart("P-SPOKE", "Spoke", "Steel", 0.20, 0.01);
    mgr.createPart("P-CHAIN", "Chain", "Steel", 6.50, 0.3);
    mgr.createPart("P-GEARSET", "Gear Set", "Steel", 22.00, 0.5);

    mgr.createAssembly("A-WHEEL", "Wheel Assembly", "Mixed");
    mgr.createAssembly("A-DRIVETRAIN", "Drivetrain", "Mixed");
    mgr.createAssembly("A-BICYCLE", "Bicycle", "Mixed");

    mgr.addComponentToAssembly("A-WHEEL", "P-RIM", 1);
    mgr.addComponentToAssembly("A-WHEEL", "P-TIRE", 1);
    mgr.addComponentToAssembly("A-WHEEL", "P-SPOKE", 32);

    mgr.addComponentToAssembly("A-DRIVETRAIN", "P-CHAIN", 1);
    mgr.addComponentToAssembly("A-DRIVETRAIN", "P-GEARSET", 1);

    mgr.addComponentToAssembly("A-BICYCLE", "P-FRAME", 1);
    mgr.addComponentToAssembly("A-BICYCLE", "A-WHEEL", 2);
    mgr.addComponentToAssembly("A-BICYCLE", "A-DRIVETRAIN", 1);

    std::cout << "\n--- Attempting an illegal circular link (A-WHEEL contains A-BICYCLE) ---\n";
    mgr.addComponentToAssembly("A-WHEEL", "A-BICYCLE", 1); // should be rejected
}

void printMenu() {
    std::cout << "\n========= PLM-Lite: BOM Management Console =========\n"
              << "1. Show full BOM tree for Bicycle\n"
              << "2. Show cost rollup for Bicycle\n"
              << "3. Where-used query for a part\n"
              << "4. Show version history for Frame\n"
              << "5. Revise Frame (simulate an engineering change)\n"
              << "6. Save BOM to file (bom_export.txt)\n"
              << "0. Exit\n"
              << "Choice: ";
}

int main() {
    BOMManager mgr;
    buildDemoBOM(mgr);

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) break;

        switch (choice) {
            case 1:
                std::cout << "\n";
                mgr.printBOMTree("A-BICYCLE");
                break;
            case 2: {
                Part* bike = mgr.find("A-BICYCLE");
                std::cout << "\nTotal rollup cost of Bicycle: $"
                          << std::fixed << std::setprecision(2) << bike->getCost() << "\n";
                std::cout << "Total rollup weight: " << bike->getWeight() << " kg\n";
                break;
            }
            case 3: {
                std::cout << "Enter part ID (e.g. P-SPOKE): ";
                std::string id; std::cin >> id;
                auto users = mgr.whereUsedQuery(id);
                if (users.empty()) std::cout << "Not used anywhere (or unknown ID).\n";
                else {
                    std::cout << id << " is used in:\n";
                    for (auto& u : users) std::cout << "  - " << u << "\n";
                }
                break;
            }
            case 4:
                mgr.find("P-FRAME")->printHistory();
                break;
            case 5:
                mgr.find("P-FRAME")->reviseVersion("Switched to thicker-gauge tubing", 52.50);
                std::cout << "Frame revised. New cost reflected in future rollups.\n";
                break;
            case 6:
                mgr.saveToFile("bom_export.txt")
                    ? std::cout << "Saved to bom_export.txt\n"
                    : std::cout << "Save failed.\n";
                break;
            case 0:
                std::cout << "Exiting.\n";
                break;
            default:
                std::cout << "Unknown option.\n";
        }
    }
    return 0;
}
