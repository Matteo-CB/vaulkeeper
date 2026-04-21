#include "ui/viewmodels/rules_view_model.hpp"

#include <QVariantMap>

#include "rules/rule_engine.hpp"
#include "rules/rule_parser.hpp"
#include "ui/viewmodels/application_view_model.hpp"

namespace vk::app {

RulesViewModel::RulesViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent)
    : QObject(parent), application(std::move(app)) {
    reload();
}

void RulesViewModel::reload() {
    const auto path = application->paths().rulesDir / "rules.yaml";
    auto parsed = core::parseRulesFromYaml(path);
    if (!parsed) {
        ruleList.clear();
        emit rulesChanged();
        return;
    }
    QVariantList list;
    for (const auto& r : *parsed) {
        QVariantMap row;
        row["id"] = QString::fromStdString(r.id);
        row["description"] = QString::fromStdString(r.description);
        row["enabled"] = r.enabled;
        row["dryRun"] = r.dryRun;
        list.push_back(row);
    }
    ruleList = std::move(list);
    emit rulesChanged();
}

void RulesViewModel::executeRule(const QString& ruleId, bool dryRun) {
    core::RuleEngine engine(application->quarantine(), application->journal());
    core::Rule stub;
    stub.id = ruleId.toStdString();
    stub.dryRun = dryRun;
    auto report = engine.execute(stub, dryRun);
    if (!report) {
        application->notify(QString::fromStdString(report.error().message));
        return;
    }
    application->notify(QString::fromStdString("Rule " + stub.id + " executed"));
}

}
