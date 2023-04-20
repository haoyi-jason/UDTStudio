#include "nodescreenbcu.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>

#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "screen/nodescreenswidget.h"

#include "bootloader/bootloader.h"

#include "canopen/bootloaderWidget/bootloaderwidget.h"

#include "widgets/bmuwidget.h"

NodeScreenBCU::NodeScreenBCU()
{
    createWidgets();
}

void NodeScreenBCU::resetHardware()
{
    _node->bootloader()->resetProgram();
}

void NodeScreenBCU::createWidgets()
{
    QLayout *glayout = new QVBoxLayout();
    glayout->setContentsMargins(2,2,2,2);

    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2,2,2,2);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget *widget = new QWidget(this);
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2,2,2,2);
    layout->setSpacing(0);

    layout->addWidget(createSumaryWidget());
//    layout->addWidget(createInfoWidget());
//    layout->addWidget(createStatusWidget());

    //layout->addWidget(new BmuWidget());

    widget->setLayout(layout);
    scrollArea->setWidget(widget);
    glayout->addWidget(scrollArea);

    setLayout(glayout);
}
QWidget *NodeScreenBCU::createSumaryWidget()
{
    QGroupBox *groupBox = new QGroupBox("BCU Information");
    QHBoxLayout *hlayout = new QHBoxLayout();

    _summaryIconLabel = new QLabel();
    _summaryIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    _summaryIconLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
    hlayout->addWidget(_summaryIconLabel);

    QFormLayout *sumaryLayout = new QFormLayout();
    sumaryLayout->setHorizontalSpacing(10);
    sumaryLayout->setVerticalSpacing(10);

    IndexLabel *indexLabel;
    indexLabel = new IndexLabel(NodeObjectId(0x1008, 0));
    sumaryLayout->addRow(tr("Device name:"), indexLabel);
    _indexWidgets.append(indexLabel);

    _summaryProfileLabel = new QLabel();
    sumaryLayout->addRow(tr("Profile:"), _summaryProfileLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1009, 0));
    sumaryLayout->addRow(tr("Hardware version:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 0));
    sumaryLayout->addRow(tr("Manufacture date:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1018, 4));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    sumaryLayout->addRow(tr("Serial number:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x100A, 0));
    sumaryLayout->addRow(tr("Software version:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2003, 0));
    sumaryLayout->addRow(tr("Software build:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
//    indexLabel->setScale(1.0);
//    indexLabel->setUnit(" ");
    sumaryLayout->addRow(tr("Packs in Stack"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 2));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    indexLabel->setScale(1.0);
    indexLabel->setUnit(" ");
    sumaryLayout->addRow(tr("Cells Per Pack"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 3));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    indexLabel->setScale(1.0);
    indexLabel->setUnit(" ");
    sumaryLayout->addRow(tr("NTCs Per Pack"), indexLabel);
    _indexWidgets.append(indexLabel);

//    IndexComboBox *indexCombo;
//    indexCombo = new IndexComboBox(NodeObjectId(0x2001,0x04));
//    indexCombo->setMinValue(1);
//    indexCombo->setMaxValue(2);
//    sumaryLayout->addRow(tr("BMU Type"), indexCombo);

//    indexLabel = new IndexLabel(NodeObjectId(0x2002, 1));
//    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
//    indexLabel->setScale(1.0);
//    indexLabel->setUnit("%");
//    sumaryLayout->addRow(tr("SOC"), indexLabel);
//    _indexWidgets.append(indexLabel);

//    indexLabel = new IndexLabel(NodeObjectId(0x2002, 2));
//    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
//    indexLabel->setScale(0.1);
//    indexLabel->setUnit(" ");
//    sumaryLayout->addRow(tr("SOH"), indexLabel);
//    _indexWidgets.append(indexLabel);

//    indexLabel = new IndexLabel(NodeObjectId(0x2002, 3));
//    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
//    indexLabel->setScale(1.0);
//    indexLabel->setUnit("V");
//    sumaryLayout->addRow(tr("Stack Voltage"), indexLabel);
//    _indexWidgets.append(indexLabel);

//    indexLabel = new IndexLabel(NodeObjectId(0x2002, 4));
//    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
//    indexLabel->setScale(0.1);
//    indexLabel->setUnit("A");
//    sumaryLayout->addRow(tr("Current"), indexLabel);
//    _indexWidgets.append(indexLabel);

    hlayout->addItem(sumaryLayout);

//    QVBoxLayout *buttonlayout = new QVBoxLayout();
//    buttonlayout->setSpacing(3);
//    QPushButton *updateFirmwareButton = new QPushButton(tr("&Update firmware"));
//    updateFirmwareButton->setFixedWidth(200);
//    connect(updateFirmwareButton,
//            &QPushButton::released,
//            this,
//            [=]()
//            {
//                updateFirmware();
//            });
//    buttonlayout->addWidget(updateFirmwareButton);

//    QPushButton *resetHardwareButton = new QPushButton(tr("&Reset hardware"));
//    resetHardwareButton->setFixedWidth(200);
//    connect(resetHardwareButton,
//            &QPushButton::released,
//            this,
//            [=]()
//            {
//                resetHardware();
//            });
//    buttonlayout->addWidget(resetHardwareButton);

//    buttonlayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
//    hlayout->addItem(buttonlayout);

    groupBox->setLayout(hlayout);
    return groupBox;

}
QWidget *NodeScreenBCU::createInfoWidget()
{
    IndexLabel *indexLabel;
    QGroupBox *groupBox = new QGroupBox(tr("BCU Info"));
    QFormLayout *sumaryLayout = new QFormLayout();

    indexLabel = new IndexLabel(NodeObjectId(0x2000, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 100.0);
    indexLabel->setUnit(" V");
    sumaryLayout->addRow(tr("Board voltage:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 10.0);
    indexLabel->setUnit(" °C");
    sumaryLayout->addRow(tr("CPU temperature:"), indexLabel);
    _indexWidgets.append(indexLabel);

    groupBox->setLayout(sumaryLayout);
    return groupBox;
}

QWidget *NodeScreenBCU::createStatusWidget()
{

    IndexLabel *indexLabel;
    QGroupBox *groupBox = new QGroupBox(tr("Status"));
    QFormLayout *sumaryLayout = new QFormLayout();

    indexLabel = new IndexLabel(NodeObjectId(0x2000, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 100.0);
    indexLabel->setUnit(" V");
    sumaryLayout->addRow(tr("Board voltage:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 10.0);
    indexLabel->setUnit(" °C");
    sumaryLayout->addRow(tr("CPU temperature:"), indexLabel);
    _indexWidgets.append(indexLabel);

    groupBox->setLayout(sumaryLayout);
    return groupBox;
}

void NodeScreenBCU::updateInfos(BCU *bcu)
{

}

QString NodeScreenBCU::title() const
{
    return QString(tr("BCU"));
}

void NodeScreenBCU::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis);

    for(AbstractIndexWidget *indexWidget:qAsConst(_indexWidgets)){
        indexWidget->setNode(node);
    }

    updateInfos((BCU*)node);

}
