#include "dmxframe_dataslots_delegate.hpp"

#include <QSpinBox>

//-----------------------------------------------------------------------------

QWidget* DmxFrameDataSlotsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto* editWidget = QItemDelegate::createEditor(parent, option, index);
    if (auto* spinbox = qobject_cast<QSpinBox*>(editWidget))
    {
        connect(spinbox, &QSpinBox::valueChanged, this, [this, editWidget] {
            const_cast<DmxFrameDataSlotsDelegate*>(this)->commitData(editWidget);
        });
        spinbox->setRange(0, 255);
    }
    return editWidget;
}
