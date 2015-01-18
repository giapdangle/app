#include "appwindow.hh"
#include <QScreen>
#include <QString>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QGuiApplication>
#include <QtAlgorithms>
#include <QQmlIncubator>
#include <QDir>

/*******************************************************************************
 * CONSTRUCTOR AND DESTRUCTOR
*/
AppWindow::AppWindow(const QString& rootFolderPath,
                     const QString& mainQML,
                     const QSize& windowSize,
                     const bool center)
    : QQuickView()
    , m_rootFolderPath(rootFolderPath)
    , m_resolution(windowSize)
    , m_desktopDpiFactor(1)
    , m_fullScreen(true)
{
    // Set the dpi according to platform
    QScreen* screen = QGuiApplication::primaryScreen();
    m_dpi = screen->physicalDotsPerInch();
    #if !defined(Q_OS_ANDROID)
        m_dpi = 205;
    #endif
    emit dpiChanged();

    // Connect signals here
    connect(this, SIGNAL(dpiChanged()), this, SIGNAL(touchSizeChanged()));
    connect(this, SIGNAL(dpiChanged()), this, SIGNAL(textMediumChanged()));
    connect(this, SIGNAL(dpiChanged()), this, SIGNAL(textLargeChanged()));
    connect(this, SIGNAL(dpiChanged()), this, SIGNAL(textSmallChanged()));

    // This object can be accessed in Qml with name "app"
    m_engine.rootContext()->setContextProperty("app", this);

    QQuickWindow::setColor("black");
    #if !defined(Q_OS_ANDROID)
        QWindow::resize(windowSize);
        if (center) {
            centerWindow();
        }
    #endif
    QQuickView::setResizeMode(QQuickView::SizeRootObjectToView);

    // Create the root object
    m_rootObject = contentItem();
    loadView(mainQML,QQmlComponent::PreferSynchronous);
    showView(mainQML);
}

AppWindow::~AppWindow()
{
    qDeleteAll(m_views);
}

/*******************************************************************************
 * PROPERTIES
 */
float AppWindow::getDpi() const
{
    return m_dpi;
}
float AppWindow::getTouchSize() const
{
    return m_dpi*0.32;
}
float AppWindow::getTextSmall() const
{
    return m_dpi*0.11;
}
float AppWindow::getTextMedium() const
{
    return m_dpi*0.14;
}
float AppWindow::getTextLarge() const
{
    return m_dpi*0.17;
}
bool AppWindow::getFullscreen() const
{
    return m_fullScreen;
}
void AppWindow::setFullScreen(bool value)
{
    if (value == true)
    {
        qDebug() << Q_FUNC_INFO << ": Entering fullscreen mode";
        QWindow::showFullScreen();
    }
    else
    {
        qDebug() << Q_FUNC_INFO << ": Entering windowed mode";
        QWindow::show();
    }
    m_fullScreen = value;
    emit fullScreenChanged();
}
float AppWindow::getScreenWidth() const
{
    QScreen* screen = QGuiApplication::primaryScreen();
    int screenWidth = screen->availableSize().width();
    return screenWidth;
}
float AppWindow::getScreenHeight() const
{
    QScreen* screen = QGuiApplication::primaryScreen();
    int screenHeight = screen->availableSize().height();
    return screenHeight;
}
QSize AppWindow::getResolution() const
{
    return m_resolution;
}
QString AppWindow::getRootFolderPath() const
{
    return m_rootFolderPath;
}
void AppWindow::setResolution(QSize resolution)
{
    QWindow::resize(resolution);
    m_resolution = resolution;
    emit resolutionChanged();
}
qreal AppWindow::getViewLoadProgress()
{
    return m_viewLoadProgress;
}

/*******************************************************************************
 * METHODS
 */
QObject* AppWindow::getByObjectName(const QString& objectName) const
{
    QObject* item = m_rootObject->findChild<QObject*>(objectName);
    if (!item)
    {
        qWarning() << Q_FUNC_INFO << ": " + objectName + " cannot be found!";
    }
    return item;
}

void AppWindow::loadAndShowView(const QString &viewName)
{
    loadView(viewName);
    showView(viewName);
}

void AppWindow::hideAndUnloadView(const QString &viewName)
{
    hideView(viewName);
    unloadView(viewName);
}

void AppWindow::showAppWindow()
{
    #if defined(Q_OS_ANDROID)
        setFullScreen(true);
    #else
        setFullScreen(false);
    #endif
}

QQuickItem* AppWindow::rootObject()
{
    return m_rootObject;
}

QQmlEngine* AppWindow::engine()
{
    return &m_engine;
}

void AppWindow::centerWindow()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    int screenWidth = screen->availableSize().width();
    int screenHeight = screen->availableSize().height();
    int appWidth = QWindow::width();
    int appHeight = QWindow::height();
    int x = (screenWidth - appWidth) / 2;
    int y = (screenHeight - appHeight) / 2;
    QWindow::setPosition(x,y);
}

void AppWindow::loadView(const QString &viewName,
                         QQmlComponent::CompilationMode compilationMode)
{
    if (m_views.contains(viewName))
    {
        qDebug() << Q_FUNC_INFO << ": The view is already loaded";
    }
    else
    {
        QQmlComponent component(&m_engine,
                                properQUrl(m_rootFolderPath+viewName),
                                compilationMode);
        QObject::connect(&component,
                         SIGNAL(statusChanged(QQmlComponent::Status)),
                         this,
                         SLOT(viewStatusChanged(QQmlComponent::Status)));
        QObject::connect(&component,
                         SIGNAL(progressChanged(qreal)),
                         this,
                         SLOT(progressChanged(qreal)));
        while (component.isLoading())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        }
        if (component.status() == QQmlComponent::Ready)
        {
            QQmlIncubator incubator;
            component.create(incubator);
            while (!incubator.isReady())
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
            }
            QQuickItem *view = qobject_cast<QQuickItem*>(incubator.object());
            m_views[viewName] = view;
            m_views[viewName]->setParent(rootObject());
        }
        else
        {
            qDebug() << Q_FUNC_INFO << ": Error in loading view";
        }
    }
}

void AppWindow::switchView(const QString &viewName)
{
    if (showView(viewName))
    {
        for (auto iter = m_views.begin(); iter != m_views.end(); ++iter)
        {
            if (iter.key() != viewName)
            {
                iter.value()->setParentItem(0);
            }
        }
    }
    else
    {
        qDebug() << "AppWindow::switchView(): The view is not loaded";
    }
}

void AppWindow::replaceView(const QString &viewName)
{
    //Using this retains the currently showed screen until new is loaded
    QQuickWindow::setClearBeforeRendering(false);
    for (auto iter = m_views.begin(); iter != m_views.end(); ++iter)
    {
        if (iter.key() != viewName)
        {
            delete *iter;
        }
    }
    m_views.clear();
    loadView(viewName);
    showView(viewName);
    //Return to normal
    QQuickWindow::setClearBeforeRendering(true);
}

bool AppWindow::showView(const QString &viewName, const QString &layer)
{
    if (!m_views.contains(viewName))
    {
        qDebug() << "AppWindow::showView(): The view is not loaded";
        return false;
    }
    if (layer == "")
    {
        m_views[viewName]->setParentItem(rootObject());
    }
    else
    {
        QQuickItem *itemLayer = rootObject()->findChild<QQuickItem*>(layer);
        if (itemLayer)
        {
            m_views[viewName]->setParentItem(itemLayer);
        }
        else
        {
            qDebug() << "AppWindow::showView(): Unknown layer";
        }
    }
    return true;
}

bool AppWindow::hideView(const QString &viewName)
{
    if (!m_views.contains(viewName))
    {
        qDebug() << "AppWindow::hideView(): The view is not loaded";
        return false;
    }
    else
    {
        m_views[viewName]->setParentItem(0);
        return true;
    }
}


bool AppWindow::unloadView(const QString &viewName)
{
    if (!m_views.contains(viewName))
    {
        qDebug() << "AppWindow::unloadView(): The view is not loaded";
        return false;
    }
    else
    {
        m_views[viewName]->deleteLater();
        m_views.remove(viewName);
        return true;
    }
}

QQuickItem* AppWindow::getView(const QString &viewName) const
{
    if (!m_views.contains(viewName)) {
        qDebug() << "AppWindow::getView(): The view is not loaded";
        return 0;
    }
    else
    {
        return m_views[viewName];
    }
}

void AppWindow::forceActiveFocus(const QString &viewName)
{
    if (!m_views.contains(viewName))
    {
        qDebug() << "AppWindow::forceActiveFocus(): The view is not loaded";
    }
    else
    {
        m_views[viewName]->forceActiveFocus();
    }
}

void AppWindow::unloadAllViews()
{
    for (auto iter = m_views.begin(); iter != m_views.end(); ++iter)
    {
        (*iter)->deleteLater();
    }
    m_views.clear();
}

void AppWindow::hideAllViews()
{
    for (auto iter = m_views.begin(); iter != m_views.end(); ++iter)
    {
        iter.value()->setParentItem(0);
    }
}

QString AppWindow::properPath(const QString &path) const
{
    return ":/"+path;
}

QUrl AppWindow::properQUrl(const QString &path) const
{
    return (QUrl("qrc:///"+path));
}

/*******************************************************************************
 * SLOTS
 */
void AppWindow::viewStatusChanged(QQmlComponent::Status status)
{
    QDebug deb = qDebug();
    deb << Q_FUNC_INFO << "Asynchronous view loading finished, status is now:";
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

void AppWindow::updateContentItemHeight()
{
    contentItem()->setHeight(QWindow::height());
}

void AppWindow::updateContentItemWidth()
{
    contentItem()->setWidth(QWindow::width());
}

void AppWindow::progressChanged(qreal progress)
{
    m_viewLoadProgress = progress;
    emit viewLoadProgressChanged();
}
