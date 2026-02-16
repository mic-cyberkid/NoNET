#include <QApplication>
#include <QString>
#include "gui/mainwindow.h"
#include "firewall/firewall.h"
#include "logging/logger.h"
#include <iostream>
#include <string>
#include <vector>

void printHelp(const char* progName) {
    std::cout << "FirewallManager v1.0.0\n"
              << "Usage: " << progName << " [options]\n\n"
              << "Options:\n"
              << "  --help                Show this help message\n"
              << "  --block-all           Set default outbound action to block\n"
              << "  --allow <path> <name> Add an allow rule for an application\n"
              << "  --list                List all firewall rules\n"
              << "  (no arguments)        Launch GUI mode\n";
}

int main(int argc, char* argv[]) {
    Logger::init();

    if (argc > 1) {
        std::string cmd = argv[1];
        Firewall firewall;

        if (cmd == "--help") {
            printHelp(argv[0]);
            return 0;
        } else if (cmd == "--block-all") {
            auto err = firewall.setDefaultOutboundBlock();
            if (err) {
                Logger::error("Failed to set default block: {}", err.message());
                return 1;
            }
            Logger::info("Default outbound connections blocked.");
            return 0;
        } else if (cmd == "--allow" && argc >= 4) {
            std::string path = argv[2];
            std::string name = argv[3];
            std::wstring wpath = QString::fromStdString(path).toStdWString();
            std::wstring wname = QString::fromStdString(name).toStdWString();
            auto err = firewall.addAllowRule(wpath, wname);
            if (err) {
                Logger::error("Failed to add rule: {}", err.message());
                return 1;
            }
            Logger::info("Added allow rule for: {}", path);
            return 0;
        } else if (cmd == "--list") {
            auto rules = firewall.listRules();
            for (const auto& rule : rules) {
                std::wcout << L"Rule: " << rule.name << L" (App: " << rule.applicationPath << L")" << std::endl;
            }
            return 0;
        } else {
            std::cerr << "Unknown option: " << cmd << "\n";
            printHelp(argv[0]);
            return 1;
        }
    }

    // GUI Mode
    QApplication app(argc, argv);
    app.setApplicationName("FirewallManager");
    app.setOrganizationName("FirewallManagerProject");

    MainWindow w;
    w.show();

    return app.exec();
}
