#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H

#include <QtCore>
#include "desktopfwd.h"


class KeyEventManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyEventManager(QWidget *parent = 0);
    ~KeyEventManager();

    void initConnect();

    void clearMultiCheckedByMouse();


signals:

public slots:
    void onKeyUpPressed();
    void onKeyDownPressed();
    void onKeyLeftPressed();
    void onKeyRightPressed();

    void onKeyShiftUpPressed();
    void onKeyShiftDownPressed();
    void onKeyShiftLeftPressed();
    void onKeyShiftRightPressed();

private:
    DesktopItemPointer m_lastCheckedByKeyboard;

};

#endif // KEYEVENTMANAGER_H
