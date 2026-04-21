#include "ui/command_palette.hpp"

#include <array>

#include <QVariantMap>

namespace vk::app {

namespace {

struct CommandEntry {
    const char* id;
    const char* label;
    const char* category;
};

constexpr std::array<CommandEntry, 10> AllCommands {{
    { "scan.all", "Scan all volumes", "Scan" },
    { "scan.home", "Scan user profile", "Scan" },
    { "duplicates.run", "Find duplicates in downloads", "Duplicates" },
    { "rules.reload", "Reload rules", "Rules" },
    { "rules.run", "Run rule by id", "Rules" },
    { "quarantine.purge", "Purge expired quarantine", "Quarantine" },
    { "uninstall.refresh", "Refresh installed applications", "Uninstall" },
    { "health.refresh", "Refresh system health", "Health" },
    { "theme.toggle", "Toggle light and dark theme", "View" },
    { "about.show", "About Vaulkeeper", "View" }
}};

}

CommandPalette::CommandPalette(QObject* parent) : QObject(parent) {
    refresh();
}

void CommandPalette::setQuery(const QString& text) {
    if (searchText == text) { return; }
    searchText = text;
    emit queryChanged();
    refresh();
}

void CommandPalette::invoke(const QString&) {}

void CommandPalette::refresh() {
    QVariantList out;
    for (const auto& cmd : AllCommands) {
        const QString label = QString::fromUtf8(cmd.label);
        if (!searchText.isEmpty() && !label.contains(searchText, Qt::CaseInsensitive)) { continue; }
        QVariantMap row;
        row["id"] = QString::fromUtf8(cmd.id);
        row["label"] = label;
        row["category"] = QString::fromUtf8(cmd.category);
        out.push_back(row);
    }
    resultList = std::move(out);
    emit resultsChanged();
}

}
