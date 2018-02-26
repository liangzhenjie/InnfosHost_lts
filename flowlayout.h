#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H
#include <QLayout>
#include <QStyle>
#include <QRect>


class FlowLayout : public QLayout
{
public:
    explicit FlowLayout(QWidget * parent=nullptr,int margin=-1,int hSpacing = -1,int vSpacing = -1);
    explicit FlowLayout(int margin=-1,int hSpacing = -1,int vSpacing = -1);
    ~FlowLayout();
    void addItem(QLayoutItem *)Q_DECL_OVERRIDE;

private:
    QList <QLayoutItem*>itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif // FLOWLAYOUT_H
