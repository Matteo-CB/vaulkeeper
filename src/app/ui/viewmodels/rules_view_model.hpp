#pragma once

#include <memory>

#include <QObject>
#include <QVariantList>

namespace vk::app {

class ApplicationViewModel;

class RulesViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList rules READ rules NOTIFY rulesChanged)

public:
    explicit RulesViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] QVariantList rules() const { return ruleList; }

public slots:
    void reload();
    void executeRule(const QString& ruleId, bool dryRun);

signals:
    void rulesChanged();

private:
    std::shared_ptr<ApplicationViewModel> application;
    QVariantList ruleList;
};

}
