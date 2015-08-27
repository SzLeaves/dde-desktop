#include "desktopbox.h"
#include "desktopframe.h"
#include "global.h"
#include "widgets/growingtextedit.h"
#include "widgets/elidelabel.h"
#include "widgets/growingelidetextedit.h"
#include "desktopitemmanager.h"
#include "desktopitem.h"



DesktopBox::DesktopBox(QWidget *parent) : TranslucentFrame(parent)
{
    m_desktopFrame = new DesktopFrame(this);


    connect(signalManager, SIGNAL(renameFinished()), this, SLOT(renameFinished()));
    connect(signalManager, SIGNAL(requestRenamed(QString)), this, SLOT(handleRename()));
}

DesktopFrame* DesktopBox::getDesktopFrame(){
    return m_desktopFrame;
}

void DesktopBox::handleRename(){
    if (!m_desktopFrame->getLastCheckedDesktopItem().isNull() &&\
            m_desktopFrame->getCheckedDesktopItems().length() == 1){
        LOG_INFO() << "handleRename start";
        DesktopItemPointer pItem = m_desktopFrame->getLastCheckedDesktopItem();
        if (!pItem.isNull()){
            LOG_INFO() << "handleRename start" << pItem->getUrl();
            if (!pItem->isShowSimpleMode()){
                pItem->setEdited(true);
                if (pItem->getUrl() == ComputerUrl || pItem->getUrl() == TrashUrl){
                    return;
                }
                pItem->setChecked(false);
                pItem->getTextEdit()->showEditing();
                pItem->getTextEdit()->setFocus();
            }
        }
    }
}

void DesktopBox::renameFinished(){
    DesktopItemPointer pItem = m_desktopFrame->getLastCheckedDesktopItem();
    if (!pItem.isNull()){
        QString  fileName;
        LOG_INFO() << isAppGroup(pItem->getUrl()) << QString("%1%2").arg(RichDirPrefix, pItem->getTextEdit()->toPlainText());
        if (isAppGroup(pItem->getUrl())){
            fileName = QString("%1%2").arg(RichDirPrefix, pItem->getTextEdit()->toPlainText());
        }else{
            fileName = pItem->getTextEdit()->toPlainText();
        }
        emit signalManager->renameJobCreated(pItem->getUrl(), fileName);
        pItem->getTextEdit()->showText();
    }
}



void DesktopBox::keyPressEvent(QKeyEvent *event){
    bool m_isGridOn = m_desktopFrame->isGridOn();

    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Control){
        m_desktopFrame->setCtrlPressed(!m_desktopFrame->isCtrlPressed());
    }

    if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Escape){
        #if !defined(QT_NO_DEBUG)
        close();
        #endif
    }else if (event->key() == Qt::Key_PageUp){
        if (m_currentPage > 0){
            m_currentPage--;
            const QRect availableGeometry = QApplication::desktop()->availableGeometry();
            int desktopWidth = availableGeometry.width();
            int currentX = m_desktopFrame->x() + desktopWidth;
            m_desktopFrame->move(currentX, m_desktopFrame->y());
        }
    }else if (event->key() == Qt::Key_PageDown){
        if (m_currentPage < gridManager->getPageCount() - 1){
            m_currentPage++;
            const QRect availableGeometry = QApplication::desktop()->availableGeometry();
            int desktopWidth = availableGeometry.width();
            int currentX = m_desktopFrame->x() - desktopWidth;
            m_desktopFrame->move(currentX, m_desktopFrame->y());
        }
    }
    #if !defined(QT_NO_DEBUG)
    else if (event->key() == Qt::Key_1){
        emit signalManager->gridSizeTypeChanged(SizeType::Small);
    }else if (event->key() == Qt::Key_2){
        emit signalManager->gridSizeTypeChanged(SizeType::Middle);
    }else if (event->key() == Qt::Key_3){
        emit signalManager->gridSizeTypeChanged(SizeType::Large);
    }else if (event->key() == Qt::Key_4){
        emit signalManager->sortedModeChanged(QDir::Name);
    }else if (event->key() == Qt::Key_5){
        emit signalManager->sortedModeChanged(QDir::Size);
    }else if (event->key() == Qt::Key_6){
        emit signalManager->sortedModeChanged(QDir::Type);
    }else if (event->key() == Qt::Key_7){
        emit signalManager->sortedModeChanged(QDir::Time);
    }else if (event->key() == Qt::Key_F1){
        emit signalManager->gridModeChanged(!m_isGridOn);
    }
    #endif
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_A){
        emit signalManager->keyCtrlAPressed();
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Delete){
        emit signalManager->trashingAboutToExcute(m_desktopFrame->getCheckedFiles());
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Delete){
        LOG_INFO() << m_desktopFrame->getCheckedFiles() << "shift delete";
        emit signalManager->deleteFilesExcuted(m_desktopFrame->getCheckedFiles());
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Up){
        if (m_isGridOn){
            emit signalManager->keyUpPressed();
        }
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Down){
        if (m_isGridOn){
            emit signalManager->keyDownPressed();
        }
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Left){
        if (m_isGridOn){
            emit signalManager->keyLeftPressed();
        }
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Right){
        if (m_isGridOn){
            emit signalManager->keyRightPressed();
        }
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Left){
        if (m_isGridOn){
            emit signalManager->keyShiftLeftPressed();
        }
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Right){
        if (m_isGridOn){
            emit signalManager->keyShiftRightPressed();
        }
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Up){
        if (m_isGridOn){
            emit signalManager->keyShiftUpPressed();
        }
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Down){
        if (m_isGridOn){
            emit signalManager->keyShiftDownPressed();
        }
    }if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F2){
        LOG_INFO() << "==========";
        handleRename();
    }

    TranslucentFrame::keyPressEvent(event);
}


void DesktopBox::keyReleaseEvent(QKeyEvent *event){
    if (event->modifiers() != Qt::ControlModifier && event->key() == Qt::Key_Control){
        m_desktopFrame->setCtrlPressed(!m_desktopFrame->isCtrlPressed());
    }
    TranslucentFrame::keyReleaseEvent(event);
}


void DesktopBox::closeEvent(QCloseEvent *event){
    LOG_INFO() << "closeEvent";
    event->accept();
}

DesktopBox::~DesktopBox()
{

    LOG_INFO() << "~DesktopBox";
}
