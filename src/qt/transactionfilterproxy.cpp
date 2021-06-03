// Copyright (c) 2011-2013 The Bitcoin developers
// Copyright (c) 2017 The PIVX developers
// Copyright (c) 2017-2019 The nint developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transactionfilterproxy.h"

#include "transactionrecord.h"
#include "transactiontablemodel.h"

#include <cstdlib>

#include <QDateTime>

// Earliest date that can be represented (far in the past)
const QDateTime TransactionFilterProxy::MIN_DATE = QDateTime::fromTime_t(0);
// Last date that can be represented (far in the future)
const QDateTime TransactionFilterProxy::MAX_DATE = QDateTime::fromTime_t(0xFFFFFFFF);

TransactionFilterProxy::TransactionFilterProxy(QObject* parent) : QSortFilterProxyModel(parent),
                                                                  dateFrom(MIN_DATE),
                                                                  dateTo(MAX_DATE),
                                                                  addrPrefix(),
                                                                  typeFilter(COMMON_TYPES),
                                                                  watchOnlyFilter(WatchOnlyFilter_All),
                                                                  minAmount(0),
                                                                  limitRows(-1),
                                                                  showInactive(true),
                                                                  fHideOrphans(false)
{
}

bool TransactionFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    int type = index.data(TransactionTableModel::TypeRole).toInt();
    QDateTime datetime = index.data(TransactionTableModel::DateRole).toDateTime();
    bool involvesWatchAddress = index.data(TransactionTableModel::WatchonlyRole).toBool();
    QString address = index.data(TransactionTableModel::AddressRole).toString();
    QString label = index.data(TransactionTableModel::LabelRole).toString();
    qint64 amount = llabs(index.data(TransactionTableModel::AmountRole).toLongLong());
    int status = index.data(TransactionTableModel::StatusRole).toInt();

    if (!showInactive && status == TransactionStatus::Conflicted)
        return false;
    if (fHideOrphans && isOrphan(status, type))
        return false;
    if (!(TYPE(type) & typeFilter))
        return false;
    if (involvesWatchAddress && watchOnlyFilter == WatchOnlyFilter_No)
        return false;
    if (!involvesWatchAddress && watchOnlyFilter == WatchOnlyFilter_Yes)
        return false;
    if (datetime < dateFrom || datetime > dateTo)
        return false;
    if (!address.contains(addrPrefix, Qt::CaseInsensitive) && !label.contains(addrPrefix, Qt::CaseInsensitive))
        return false;
    if (amount < minAmount)
        return false;

    return true;
}

void TransactionFilterProxy::setDateRange(const QDateTime& from, const QDateTime& to)
{
    this->dateFrom = from;
    this->dateTo = to;
    invalidateFilter();
}

void TransactionFilterProxy::setAddressPrefix(const QString& addrPrefix)
{
    this->addrPrefix = addrPrefix;
    invalidateFilter();
}

void TransactionFilterProxy::setTypeFilter(quint32 modes)
{
    this->typeFilter = modes;
    invalidateFilter();
}

void TransactionFilterProxy::setMinAmount(const CAmount& minimum)
{
    this->minAmount = minimum;
    invalidateFilter();
}

void TransactionFilterProxy::setWatchOnlyFilter(WatchOnlyFilter filter)
{
    this->watchOnlyFilter = filter;
    invalidateFilter();
}

void TransactionFilterProxy::setLimit(int limit)
{
    this->limitRows = limit;
}

void TransactionFilterProxy::setShowInactive(bool showInactive)
{
    this->showInactive = showInactive;
    invalidateFilter();
}

void TransactionFilterProxy::setHideOrphans(bool fHide)
{
    this->fHideOrphans = fHide;
    invalidateFilter();
}

int TransactionFilterProxy::rowCount(const QModelIndex& parent) const
{
    if (limitRows != -1) {
        return std::min(QSortFilterProxyModel::rowCount(parent), limitRows);
    } else {
        return QSortFilterProxyModel::rowCount(parent);
    }
}

bool TransactionFilterProxy::isOrphan(const int status, const int type)
{
    return ( (type == TransactionRecord::Generated || type == TransactionRecord::StakeMint ||
            type == TransactionRecord::StakeZECA || type == TransactionRecord::MNReward)
            && (status == TransactionStatus::Conflicted || status == TransactionStatus::NotAccepted) );
}