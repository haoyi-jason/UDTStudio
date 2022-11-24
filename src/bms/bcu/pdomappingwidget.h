#ifndef PDOMAPPINGWIDGET_H
#define PDOMAPPINGWIDGET_H

#include <QWidget>

#include "pdomappingview.h"

#include <QLabel>
#include <QToolBar>

class PDOMappingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PDOMappingWidget(QWidget *parent = nullptr);
    PDO *pdo() const;

public slots:
    void setPdo(PDO *pdo);

    void clearMapping();
    void setEnabled(bool enabled);

protected slots:
    void updateEnabled(bool enabled);

protected:
    PDO *_pdo;

    void createWidget();
    QLabel *_pdoNameLabel;
    QToolBar *_toolBar;
    QAction *_enableAction;
    QLabel *_pdoIndexLabel;
    PDOMappingView *_pdoMappingView;
};

#endif // PDOMAPPINGWIDGET_H
