#include "applistmodel.h"

AppListModel::AppListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int AppListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_apps.count();
}

QVariant AppListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_apps.count())
        return QVariant();

    const AppInfo &app = m_apps[index.row()];

    switch (role) {
    case NameRole:
        return app.name;
    case PackageNameRole:
        return app.packageName;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> AppListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PackageNameRole] = "packageName";
    return roles;
}

void AppListModel::setApps(const QList<AppInfo> &apps)
{
    beginResetModel();
    m_apps = apps;
    endResetModel();
}

void AppListModel::clear()
{
    beginResetModel();
    m_apps.clear();
    endResetModel();
}
