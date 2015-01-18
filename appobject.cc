#include "appobject.hh"
#include "QDebug"
#include <QQmlIncubator>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlIncubationController>

/*******************************************************************************
 * CONSTRUCTOR AND DESTRUCTOR
 */
AppObject::AppObject(AppWindow* window, AppObjectHandler* handler)
    : QObject(handler)
    , m_window(window)
    , m_handler(handler)
    , m_x(0)
    , m_y(0)
    , m_z(0)
    , m_centerX(0)
    , m_centerY(0)
    , m_width(0)
    , m_height(0)
    , m_rotation(0)
{
}

AppObject::~AppObject()
{
    qDeleteAll(m_hashItems);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void AppObject::addQuickItem(const QString& qmlPath, const QString& name, const QString& layer)
{
    // Create the visual enemy and place it into the correct layer
    QQuickItem* quickItem = m_handler->getQuickItemFromComponent(qmlPath);
    if (quickItem == 0)
    {
        qWarning() << Q_FUNC_INFO << ": The component "+qmlPath+" cannot be found!";
    }
    else
    {
        QObject* itemLayer = m_window->getByObjectName(layer);
        if (!itemLayer)
        {
            qWarning() << Q_FUNC_INFO << ": The layer "+layer+" cannot be found!";
        }
        quickItem->setParent(itemLayer);
        quickItem->setParentItem(qobject_cast<QQuickItem*>(itemLayer));
        m_hashItems[name] = quickItem;
    }
}

QQuickItem* AppObject::getQuickItem(const QString& qmlPath)
{
    // Create the visual enemy and place it into the correct layer
    QQuickItem* quickItem = m_handler->getQuickItemFromComponent(qmlPath);
    if (quickItem == 0)
    {
        qWarning() << Q_FUNC_INFO << ": The component "+qmlPath+" cannot be found!";
    }
    return quickItem;
}

void AppObject::setQuickItem(QQuickItem* item, const QString& name, const QString& layer)
{
    QObject* itemLayer = m_window->getByObjectName(layer);
    if (!itemLayer)
    {
        qWarning() << Q_FUNC_INFO << ": The layer "+layer+" cannot be found!";
    }
    item->setParent(itemLayer);
    item->setParentItem(qobject_cast<QQuickItem*>(itemLayer));
    m_hashItems[name] = item;
}

void AppObject::removeQuickItem(const QString& name)
{
    m_hashItems[name]->deleteLater();
    m_hashItems.remove(name);
}

void AppObject::setProperties(const char* property, const QVariant &value)
{
    QHash<QString, QQuickItem*>::const_iterator i = m_hashItems.constBegin();
    while (i != m_hashItems.constEnd()) {
        bool propertyExists = i.value()->setProperty(property,value);
        if (!propertyExists)
        {
            qWarning() << Q_FUNC_INFO << ": The property " << property << " on " + i.key() + " was not found but was created!";
        }
        ++i;
    }
}

void AppObject::setProperty(const QString& target, const char* property, const QVariant &value)
{
    QQuickItem* item = m_hashItems[target];
    if (item)
    {
        bool propertyExists = item->setProperty(property, value);
        if (!propertyExists)
        {
            qWarning() << Q_FUNC_INFO << ": The property " << property << " on " + target + " was not found but was created!";
        }
    }
    else
    {
        qWarning() << Q_FUNC_INFO << ": The target "+target+" was not found!";
    }
}

void AppObject::changeLayer(const QString &target, const QString &layerName)
{
    QObject *itemLayer = m_window->rootObject()->findChild<QObject*>(layerName);
    QQuickItem * item = m_hashItems[target];
    if (item)
    {
        item->setParent(itemLayer);
        item->setParentItem(qobject_cast<QQuickItem *>(itemLayer));
    }
    else
    {
        qWarning() << "AppObject::setProperty(): The target "+target+" was not found!";
    }
}

void AppObject::setX(float x)
{
    m_x = x;
    m_centerX = x+m_width/2.0;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setX(x);
    }
}

void AppObject::setY(float y)
{
    m_y = y;
    m_centerY = y+m_height/2.0;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setY(y);
    }
}

void AppObject::setZ(int z)
{
    m_z = z;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setZ(z);
    }
}

void AppObject::setCenterX(float centerX)
{
    m_centerX = centerX;
    m_x = centerX-m_width/2.0;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setX(m_x);
    }
}

void AppObject::setCenterY(float centerY)
{
    m_centerY = centerY;
    m_y = centerY-m_height/2.0;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setX(m_y);
    }
}

void AppObject::setWidth(float width)
{
    m_width = width;
    m_centerX = m_x+m_width/2.0;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setWidth(width);
    }
}

void AppObject::setHeight(float height)
{
    m_height = height;
    m_centerY = m_y+m_height/2.0;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setHeight(height);
    }
}

void AppObject::setRotation(float rotation)
{
    m_rotation = rotation;
    for (auto iter = m_hashItems.begin(); iter != m_hashItems.end(); ++iter)
    {
        (*iter)->setProperty("rotation",rotation);
    }
}

