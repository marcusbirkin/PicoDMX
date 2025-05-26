#pragma once

#include <QItemDelegate>

//-----------------------------------------------------------------------------

/*!
 * \brief Delegate providing ranged QSpinBoxes
 */
class DmxFrameDataSlotsDelegate : public QItemDelegate
{
public:
    DmxFrameDataSlotsDelegate(QWidget *parent) : QItemDelegate(parent) {}

protected :
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
