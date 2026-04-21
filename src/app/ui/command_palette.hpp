#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

namespace vk::app {

class CommandPalette : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
    explicit CommandPalette(QObject* parent = nullptr);

    [[nodiscard]] QVariantList results() const { return resultList; }
    [[nodiscard]] QString query() const { return searchText; }

public slots:
    void setQuery(const QString& text);
    void invoke(const QString& commandId);

signals:
    void resultsChanged();
    void queryChanged();

private:
    QVariantList resultList;
    QString searchText;

    void refresh();
};

}
