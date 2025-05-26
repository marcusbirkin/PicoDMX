#include "dmxframe_dataslots_model.hpp"

#include <QStringBuilder>

#include <utility>

//-----------------------------------------------------------------------------

DmxFrameDataSlotsModel::DmxFrameDataSlotsModel(DmxFrame& frame, QObject *parent)
    : QAbstractTableModel{parent}, frame{frame}
{
    connect(&frame, &DmxFrame::dataSlotChanged,
        this, [&](uint16_t slot)
        {
            const auto index = slotToIndex(slot);
            emit dataChanged(index, index, {Qt::DisplayRole});
        });

    connect(&frame, &DmxFrame::dataSlotCountChanged,
        this, [&](const uint16_t oldValue, const uint16_t newValue)
        {
            refreshRowCount();

            auto topLeft = slotToIndex(oldValue);
            auto bottomRight = slotToIndex(newValue);
            if (topLeft > bottomRight)
            {
                topLeft = std::exchange(bottomRight, topLeft);
            }
            emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
        });

    refreshRowCount();
}

//-----------------------------------------------------------------------------

QVariant DmxFrameDataSlotsModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            return section + 1;
        }
        const auto rowBegin = (section * columnCount()) + 1;
        const auto rowEnd = rowBegin + (columnCount() - 1);
        return QString("%1...%2").arg(QString::number(rowBegin), QString::number(rowEnd));
    }
    default: return QVariant();
    }
}

//-----------------------------------------------------------------------------

QVariant DmxFrameDataSlotsModel::data(const QModelIndex& index, const int role) const
{
    const auto slot = indexToSlot(index);
    if (slot >= frame.getDataSlotCount())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::EditRole: return frame.getDataSlots()[slot];
        case Qt::DisplayRole:
        {
            const auto slotValue = frame.getDataSlots()[slot];
            const auto percent = (slotValue * 100) / 255;
            return QString("%1 (%2%)").arg(slotValue, 3, 10, QChar('0')).arg(percent);
        }
        default: return QVariant();
    }
}

//-----------------------------------------------------------------------------

bool DmxFrameDataSlotsModel::setData(const QModelIndex &index, const QVariant &value, const int role)
{
    const auto slot = indexToSlot(index);
    if (slot >= frame.getDataSlotCount())
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            bool ok;
            const auto slotValue = value.toInt(&ok);
            if (ok && (slotValue == static_cast<uint8_t>(slotValue)))
            {
                frame.setDataSlot(slot, slotValue);
                return true;
            }
        }
        default: return false;
    }
}

//-----------------------------------------------------------------------------

Qt::ItemFlags DmxFrameDataSlotsModel::flags(const QModelIndex &index) const
{
    if (indexToSlot(index) < frame.getDataSlots().size())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::NoItemFlags;
}

//-----------------------------------------------------------------------------

uint16_t DmxFrameDataSlotsModel::indexToSlot(const QModelIndex &index) const
{
    return (index.row() * columnCount()) + index.column();
}

//-----------------------------------------------------------------------------

QModelIndex DmxFrameDataSlotsModel::slotToIndex(uint16_t slot) const
{
    const int row = slot / columnCount();
    const int column = slot % columnCount();
    return index(row, column);
}

//-----------------------------------------------------------------------------

void DmxFrameDataSlotsModel::refreshRowCount()
{
    const auto oldRowCount = rowCount();
    const auto newRowCount = std::ceil(static_cast<float>(frame.getDataSlots().size()) / columnCount());
    if (newRowCount > oldRowCount)
    {
        beginInsertRows(QModelIndex(), oldRowCount, newRowCount - 1);
        nRows = newRowCount;
        endInsertRows();
    }
    else if (newRowCount < oldRowCount)
    {
        beginRemoveRows(QModelIndex(), newRowCount, oldRowCount - 1);
        nRows = newRowCount;
        endRemoveRows();
    }
}
