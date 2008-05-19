#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>

#include "profile.h"
#include "my_menubar.h"

void MyMenuBar::paintEvent(QPaintEvent *event)
{
    QMenuBar::paintEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        QPixmap pixmap;

        // Paint warning
        if (_updateAvailable)
            pixmap = QPixmap(":/images/update.png");
        else
            pixmap = QPixmap(":/images/no_update.png");

        QPainter painter(this);
        int left = width() - 18;
        int top = 0;
        if (updateIconPressed && mouseOverIcon)
        {
            left++;
            top++;
        }
        painter.drawPixmap(left, top, pixmap);
    }
#endif
}

bool MyMenuBar::updateAvailable() const
{
    return _updateAvailable;
}

void MyMenuBar::setUpdateAvailable(bool value)
{
    if (_updateAvailable != value)
    {
        _updateAvailable = value;
        repaint();
    }
}

MyMenuBar::MyMenuBar(QWidget *parent) : QMenuBar(parent)
{
    _updateAvailable = false;
    updateIconPressed = false;
}

void MyMenuBar::mousePressEvent(QMouseEvent *event)
{
    QMenuBar::mousePressEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        // Icon?
        updateIconPressed = (event->x() >= width() - 18) &&
            (event->x() < width() - 2) &&
            (event->y() >= 0) &&
            (event->y() < 16);
        mouseOverIcon = updateIconPressed;

        if (updateIconPressed)
            repaint();
    }
#endif
}

void MyMenuBar::mouseReleaseEvent(QMouseEvent *event)
{
    QMenuBar::mouseReleaseEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        // Icon?
        if (updateIconPressed)
        {
            updateIconPressed = false;
            repaint();

            if (mouseOverIcon)
            {
                // Launch
                emit iconClicked();
            }
        }
    }
#endif
}

void MyMenuBar::mouseMoveEvent(QMouseEvent *event)
{
    QMenuBar::mouseMoveEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        bool overIcon = (event->x() >= width() - 18) &&
            (event->x() < width() - 2) &&
            (event->y() >= 0) &&
            (event->y() < 16);

        if (overIcon != mouseOverIcon)
        {
            mouseOverIcon = overIcon;
            repaint();
        }
    }
#endif
}
