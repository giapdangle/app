#include "appobjecthandler.hh"
#include "appwindow.hh"
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlIncubator>
#include <QCoreApplication>
#include <QQmlIncubationController>

/*******************************************************************************
 * CONSTRUCTOR AND DESTRUCTOR
 */
AppObjectHandler::AppObjectHandler(AppWindow* window,
                                   QObject* parent)
    : QObject(parent)
    , m_window(window)
{
}

AppObjectHandler::~AppObjectHandler()
{
    qDeleteAll(m_components);
}

/*******************************************************************************
 * METHODS
 */
void AppObjectHandler::loadComponent(QString qmlPath,
                                     QQmlComponent::CompilationMode compilationMode,
                                     QQmlEngine* engine)
{
    if (engine == 0)  {
        engine = m_window->engine();
    }
    if (m_components.contains(qmlPath))
    {
        qWarning() << Q_FUNC_INFO << ": The component "+qmlPath+" is already loaded!";
        return;
    }
    QQmlComponent* component = new QQmlComponent(engine,
                                                 m_window->properQUrl(m_window->getRootFolderPath()+qmlPath),
                                                 compilationMode);
    if (component->isLoading())
    {
        QObject::connect(component, SIGNAL(statusChanged(QQmlComponent::Status)),
                         this, SLOT(componentStatusChanged(QQmlComponent::Status)));
    }
    else
    {
        componentStatusChanged(component->status());
    }
    if (!component)
    {
        qWarning() << Q_FUNC_INFO << ": The component "+qmlPath+" was not found!";
    }
    m_components[qmlPath] = component;
}

void AppObjectHandler::unloadComponent(QString qmlPath)
{
    if (!m_components.contains(qmlPath))
    {
        qWarning() << Q_FUNC_INFO << ": The component "+qmlPath+" is not loaded!";
        return;
    }
    delete m_components[qmlPath];
    m_components.remove(qmlPath);
}

QQuickItem * AppObjectHandler::getQuickItemFromComponent(QString qmlPath)
{
    // Create the visual enemy and place it into the correct layer
    if (m_components.contains(qmlPath))
    {
        QQmlComponent *component = m_components[qmlPath];
        QQuickItem *quickItem = 0;
        if (component == 0)
        {
            qWarning() << "AppObject::getQuickItemFromComponent(): The component "+qmlPath+" is null!";
        }
        else
        {
            while (component->isLoading())
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 15);
            }
            QQmlIncubator incubator;
            component->create(incubator);
            while (!incubator.isReady()) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 15);
            }
            quickItem = qobject_cast<QQuickItem *>(incubator.object());
        }
        return quickItem;
    }
    else
    {
        qDebug() << "AppObject::getQuickItemFromComponent(): The component "+qmlPath+" is not preloaded, loading";
        loadComponent(qmlPath,QQmlComponent::PreferSynchronous);
        QQuickItem *quickItem = 0;
        quickItem = getQuickItemFromComponent(qmlPath);
        return quickItem;
    }
}

void AppObjectHandler::componentStatusChanged(QQmlComponent::Status status)
{
    QDebug deb = qDebug();
    deb << "AppObjectHandler: Asynchronous component loading finished, status is now:";
    if (status == QQmlComponent::Ready)
    {
        deb << "Ready";
    }
    else if (status == QQmlComponent::Error)
    {
        deb << "Error";
    }
    else
    {
        deb << "Loading or Null";
    }
}
