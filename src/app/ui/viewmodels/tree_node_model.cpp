#include "ui/viewmodels/tree_node_model.hpp"

#include "util/string_intern.hpp"

namespace vk::app {

namespace {

const core::ScanNode* nodePtr(const QModelIndex& index) {
    return static_cast<const core::ScanNode*>(index.internalPointer());
}

}

TreeNodeModel::TreeNodeModel(QObject* parent) : QAbstractItemModel(parent) {}

void TreeNodeModel::setRoot(std::shared_ptr<core::ScanNode> node) {
    beginResetModel();
    root = std::move(node);
    endResetModel();
}

QModelIndex TreeNodeModel::index(int row, int column, const QModelIndex& parent) const {
    if (root == nullptr) { return {}; }
    const auto* base = parent.isValid() ? nodePtr(parent) : root.get();
    if (base == nullptr) { return {}; }
    if (row < 0 || static_cast<std::size_t>(row) >= base->children.size()) { return {}; }
    return createIndex(row, column, const_cast<core::ScanNode*>(&base->children[row]));
}

QModelIndex TreeNodeModel::parent(const QModelIndex&) const {
    return {};
}

int TreeNodeModel::rowCount(const QModelIndex& parent) const {
    if (root == nullptr) { return 0; }
    const auto* base = parent.isValid() ? nodePtr(parent) : root.get();
    return base ? static_cast<int>(base->children.size()) : 0;
}

int TreeNodeModel::columnCount(const QModelIndex&) const { return 1; }

QVariant TreeNodeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) { return {}; }
    const auto* node = nodePtr(index);
    if (node == nullptr) { return {}; }

    switch (role) {
        case NameRole:
            return QString::fromStdString(std::string(core::StringInterner::pathInterner().view(node->name)));
        case TotalBytesRole: return QVariant::fromValue<qlonglong>(static_cast<qlonglong>(node->totalLogicalBytes));
        case FileCountRole: return QVariant::fromValue<qlonglong>(static_cast<qlonglong>(node->totalFiles));
        case DirectoryCountRole: return QVariant::fromValue<qlonglong>(static_cast<qlonglong>(node->totalDirectories));
        case RatioRole: {
            if (root == nullptr || root->totalLogicalBytes == 0) { return 0.0; }
            return static_cast<double>(node->totalLogicalBytes) / static_cast<double>(root->totalLogicalBytes);
        }
        default: return {};
    }
}

QHash<int, QByteArray> TreeNodeModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[NameRole] = "name";
    names[PathRole] = "path";
    names[TotalBytesRole] = "totalBytes";
    names[FileCountRole] = "fileCount";
    names[DirectoryCountRole] = "directoryCount";
    names[RatioRole] = "ratio";
    return names;
}

}
