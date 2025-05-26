#pragma once

#include "dmxframe.hpp"

#include <QAbstractTableModel>

//-----------------------------------------------------------------------------

class DmxFrameDataSlotsModel : public QAbstractTableModel
{
public:
    explicit DmxFrameDataSlotsModel(DmxFrame& frame, QObject *parent = nullptr);

    int rowCount(const QModelIndex& = QModelIndex()) const override { return nRows; }
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 10; }
    QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex&, int = Qt::DisplayRole) const override;
    bool setData(const QModelIndex&, const QVariant&, int = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    uint16_t indexToSlot(const QModelIndex &index) const;
    QModelIndex slotToIndex(uint16_t slot) const;
    void refreshRowCount();

    DmxFrame& frame;
    int nRows{0};
};
