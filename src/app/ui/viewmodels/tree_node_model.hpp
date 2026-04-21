#pragma once

#include <memory>

#include <QAbstractItemModel>

#include "fs/file_entry.hpp"

namespace vk::app {

class TreeNodeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        PathRole,
        TotalBytesRole,
        FileCountRole,
        DirectoryCountRole,
        RatioRole
    };

    explicit TreeNodeModel(QObject* parent = nullptr);

    void setRoot(std::shared_ptr<core::ScanNode> node);

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex& child) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:
    std::shared_ptr<core::ScanNode> root;
};

}
