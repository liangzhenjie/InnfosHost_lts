#ifndef INNFOSUTIL_H
#define INNFOSUTIL_H
#include <QPalette>
#include <QWidget>


static void modifyPalette(QWidget * pWidget,QPalette::ColorRole role,const QColor color)
{
    if(pWidget)
    {
        QPalette pal = pWidget->palette();
        pal.setColor(role,color);
        pWidget->setPalette(pal);
    }
}

#endif // INNFOSUTIL_H
