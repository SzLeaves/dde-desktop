#include "desktopitemmanager.h"
#include "global.h"


DesktopItemManager::DesktopItemManager(QObject* parent):QObject(parent){

    m_parentWindow = static_cast<QWidget*>(this->parent());

    QSettings settings;
    settings.beginGroup("Desktop");
    int flag = settings.value("sortFlag", QDir::Name).toInt();
    settings.endGroup();
    m_sortFlag = static_cast<QDir::SortFlag>(flag);

    m_shoudbeMovedItem = DesktopItemPointer();
    initConnect();
}

void DesktopItemManager::initComputerItem(){
    QString url = "computer://";
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pComputerItem  = DesktopItemPointer::create(":/skin/images/QFramer.png", this->tr("Computer"), m_parentWindow);
    m_pComputerItem->setUrl(url);
    m_pComputerItem->resize(width, height);

    GridItemPointer pGridItem = gridManager->getItems().at(0)->at(0);
    QRect rect = pGridItem->getRect();
    QPoint pos = rect.topLeft();

    m_settings.beginGroup("DesktopItems");
    pos = m_settings.value(url, pos).toPoint();
    m_settings.endGroup();

    pGridItem = gridManager->getItemByPos(pos);
    if (!pGridItem.isNull()){
        pGridItem->setDesktopItem(true);
    }
    m_pComputerItem->move(pos);
    m_pItems.insert(url, m_pComputerItem);
    m_list_pItems.append(m_pComputerItem);
}

void DesktopItemManager::initTrashItem(){
    QString url = "trash://";
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pTrashItem  = DesktopItemPointer::create(":/skin/images/QFramer.png", this->tr("Trash"), m_parentWindow);
    m_pTrashItem->setUrl(url);
    m_pTrashItem->resize(width, height);

    GridItemPointer pGridItem = gridManager->getItems().at(0)->at(1);
    QRect rect = pGridItem->getRect();
    QPoint pos = rect.topLeft();
    m_settings.beginGroup("DesktopItems");
    pos = m_settings.value(url, pos).toPoint();
    m_settings.endGroup();

    pGridItem = gridManager->getItemByPos(pos);
    if (!pGridItem.isNull()){
        pGridItem->setDesktopItem(true);
    }
    m_pTrashItem->move(pos);
    m_pItems.insert(url, m_pTrashItem);
    m_list_pItems.append(m_pTrashItem);
}


void DesktopItemManager::initConnect(){
    connect(signalManager, SIGNAL(desktopItemsSaved()), this, SLOT(saveItems()));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlag)), this, SLOT(sortedByFlags(QDir::SortFlag)));
    connect(signalManager, SIGNAL(gridOnResorted()), this, SLOT(resort()));
    connect(signalManager, SIGNAL(desktopItemsChanged(DesktopItemInfoMap&)), this, SLOT(addItems(DesktopItemInfoMap&)));
    connect(signalManager, SIGNAL(itemCreated(DesktopItemInfo)), this, SLOT(addItem(DesktopItemInfo)));
    connect(signalManager, SIGNAL(itemDeleted(QString)), this, SLOT(deleteItem(QString)));

    connect(signalManager, SIGNAL(itemShoudBeMoved(QString)), this, SLOT(setShoudByMovedItemByUrl(QString)));
    connect(signalManager, SIGNAL(itemMoved(DesktopItemInfo&)), this, SLOT(renameDesktopItem(DesktopItemInfo&)));
}

void DesktopItemManager::loadDesktopItems(){
    initComputerItem();
    initTrashItem();

}

QString DesktopItemManager::decodeUrl(QString url){
    return QUrl(url).toString();
}

QString DesktopItemManager::getDesktopDisplayName(const DesktopItemInfo &desktopItemInfo){
    QString desktopDisplayName;
    QString displayName = desktopItemInfo.DisplayName;
    if (displayName.startsWith(RichDirPrefix)){
        int l = QString(RichDirPrefix).length();
        desktopDisplayName = displayName.remove(0, l);
    }else{
        desktopDisplayName = displayName;
    }
    return desktopDisplayName;
}

DesktopItemPointer DesktopItemManager::getShoudBeMovedItem(){
    return m_shoudbeMovedItem;
}

void DesktopItemManager::setShoudByMovedItem(DesktopItemPointer pItem){
    m_shoudbeMovedItem = pItem;
}

void DesktopItemManager::setShoudByMovedItemByUrl(QString url){
    QString key = decodeUrl(url);
    if (m_pItems.contains(key)){
        setShoudByMovedItem(m_pItems.value(key));
    }else{
        setShoudByMovedItem(DesktopItemPointer());
    }
}

void DesktopItemManager::addItems(DesktopItemInfoMap &desktopInfoMap){
    for(int i=0; i< desktopInfoMap.values().count(); i++){
        addItem(desktopInfoMap.values().at(i), i);
    }
}

DesktopItemPointer DesktopItemManager::createItem(const DesktopItemInfo &fileInfo){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();

    QString desktopDisplayName = getDesktopDisplayName(fileInfo);
    QString displayName = fileInfo.DisplayName;
    QString uri = fileInfo.URI;
    QString url = decodeUrl(uri);
    QString icon = fileInfo.Icon;

    DesktopItemPointer  pDesktopItem = DesktopItemPointer::create(":/skin/images/QFramer.png", desktopDisplayName, m_parentWindow);
    pDesktopItem->setUrl(url);
    pDesktopItem->setDesktopIcon(icon);
    pDesktopItem->setDesktopItemInfo(fileInfo);
    pDesktopItem->resize(width, height);
    pDesktopItem->show();
    m_pItems.insert(url, pDesktopItem);
    m_list_pItems.append(pDesktopItem);

    return pDesktopItem;
}

void DesktopItemManager::addItem(const DesktopItemInfo& fileInfo, int index){
    DesktopItemPointer pDesktopItem = createItem(fileInfo);
    if (!pDesktopItem.isNull()){
        int row = gridManager->getRowCount();
        m_settings.beginGroup("DesktopItems");
        int _column = (index + 2) /  row;
        int _row = (index + 2) % row;
        GridItemPointer pGridItem = gridManager->getItems().at(_column)->at(_row);
        if (!pGridItem.isNull()){
            QRect rect = pGridItem->getRect();
            QPoint pos = rect.topLeft();
            pos = m_settings.value(pDesktopItem->getUrl(), pos).toPoint();
            pDesktopItem->move(pos);

            pGridItem = gridManager->getItemByPos(pos);
            if (!pGridItem.isNull()){
                pGridItem->setDesktopItem(true);
            }
        }
        m_settings.endGroup();
    }
}


void DesktopItemManager::addItem(const DesktopItemInfo& fileInfo){
    DesktopItemPointer pDesktopItem = createItem(fileInfo);
    if (!pDesktopItem.isNull()){
        GridItemPointer pGridItem = gridManager->getBlankItem();
        if (!pGridItem.isNull()){
            pDesktopItem->move(pGridItem->getPos());
            pGridItem->setDesktopItem(true);
        }
    }
}


void DesktopItemManager::renameDesktopItem(DesktopItemInfo &desktopItemInfo){
    if (!m_shoudbeMovedItem.isNull()){
        QString desktopDisplayName = getDesktopDisplayName(desktopItemInfo);
        m_shoudbeMovedItem->setDesktopName(desktopDisplayName);
        m_shoudbeMovedItem->setDesktopItemInfo(desktopItemInfo);

        QString oldKey = m_shoudbeMovedItem->getUrl();
        if (m_pItems.contains(oldKey)){
            QMap<QString, DesktopItemPointer>::iterator iterator = m_pItems.find(oldKey);
            QString newKey = decodeUrl(desktopItemInfo.URI);
            m_shoudbeMovedItem->setUrl(newKey);
            m_pItems.insert(iterator, newKey, m_shoudbeMovedItem);

            m_settings.beginGroup("DesktopItems");
            if (m_settings.contains(oldKey)){
                m_settings.remove(oldKey);
                m_settings.setValue(newKey, m_shoudbeMovedItem->pos());
            }
            m_settings.endGroup();
        }
    }
}

void DesktopItemManager::deleteItem(QString url){
    QString _url = decodeUrl(url);

    if (m_pItems.contains(_url)){
        DesktopItemPointer pItem = m_pItems.value(_url);

        GridItemPointer pGridItem = gridManager->getItemByPos(pItem->pos());
        if (!pGridItem.isNull()){
            pGridItem->setDesktopItem(false);
        }

        if (m_list_pItems.contains(pItem)){
            m_list_pItems.removeOne(pItem);
        }
        m_pItems.remove(url);
        pItem->close();
    }
}

void DesktopItemManager::saveItems(){
    m_settings.beginGroup("DesktopItems");
    foreach (DesktopItemPointer pItem, m_list_pItems) {
        m_settings.setValue(pItem->getUrl(), pItem->pos());
    }
    m_settings.endGroup();
}


void DesktopItemManager::changeSizeByGrid(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int row = gridManager->getRowCount();

    foreach (DesktopItemPointer pItem, m_list_pItems) {
        pItem->resize(width, height);
        int i = m_list_pItems.indexOf(pItem) / row;
        int j = m_list_pItems.indexOf(pItem) % row;
        GridItemPointer pGridItem = gridManager->getItems().at(i)->at(j);
        if (!pGridItem.isNull()){
            QRect rect = pGridItem->getRect();
            pItem->move(rect.topLeft());
            pGridItem->setDesktopItem(true);
        }
    }
    emit signalManager->desktopItemsSaved();
}

DesktopItemPointer DesktopItemManager::getItemByPos(QPoint pos){
    foreach (DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->pos() == pos){
            return pItem;
        }
    }
    return DesktopItemPointer();
}

QList<DesktopItemPointer> DesktopItemManager::getItems(){
    return m_list_pItems;
}

void DesktopItemManager::sortedByFlags(QDir::SortFlag flag){
    QDir desktopDir(desktopLocation);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot |QDir::Hidden, flag);
    m_list_pItems.clear();
    m_list_pItems.append(m_pComputerItem);
    m_list_pItems.append(m_pTrashItem);

    int row = gridManager->getRowCount();
    int size = desktopInfoList.size();
    for (int i = 0; i < size; i++) {
        QFileInfo fileInfo = desktopInfoList.at(i);
        QString url = "file://" + decodeUrl(fileInfo.absoluteFilePath());
        DesktopItemPointer  pDesktopItem = m_pItems.value(url);
        m_list_pItems.append(pDesktopItem);
    }

    foreach (DesktopItemPointer pItem, m_list_pItems) {
        int i = m_list_pItems.indexOf(pItem) / row;
        int j = m_list_pItems.indexOf(pItem) % row;
        GridItemPointer gridItem = gridManager->getItems().at(i)->at(j);
        if (!gridItem.isNull()){
            QRect rect = gridItem->getRect();
            pItem->move(rect.topLeft());
        }
    }
    emit signalManager->desktopItemsSaved();
    m_sortFlag = flag;
}

void DesktopItemManager::resort(){
    sortedByFlags(m_sortFlag);
}

QDir::SortFlag DesktopItemManager::getSortFlag(){
    return m_sortFlag;
}

DesktopItemManager::~DesktopItemManager()
{

}