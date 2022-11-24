#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>

class HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    HeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

    void addMandatorySection(int section);

private:
    QSet<int> _mandatorySection;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // HEADERVIEW_H
