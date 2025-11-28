#ifndef APPLISTMODEL_H
#define APPLISTMODEL_H

#include <QAbstractListModel>
#include "appmanager.h"

class AppListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum AppRoles {
        NameRole = Qt::UserRole + 1,
        PackageNameRole
    };

    explicit AppListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setApps(const QList<AppInfo> &apps);
    void clear();

private:
    QList<AppInfo> m_apps;
};

#endif // APPLISTMODEL_H
