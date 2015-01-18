#ifndef APPWINDOW_HH
#define APPWINDOW_HH

#include <QQuickView>
#include <QQmlComponent>
#include <QQmlApplicationEngine>
#include <QQuickItem>

////////////////////////////////////////////////////////////////////////////////
///
/// The AppWindow class is the window that displays the app. It's primary
/// functions are:
///   -Used to dynamically load and switch new QML-files to be shown on screen
///   -Offers basic information that can be accessed in QML by using the
///    context name "app"
///   -display DPI, window size, default touch size, +you can add anything you
///    need with  Q_PROPERTIES
///
////////////////////////////////////////////////////////////////////////////////

class AppWindow : public QQuickView
{
    Q_OBJECT

public:
    /***************************************************************************
     * CONSTRUCTOR AND DESTRUCTOR
     */
    /**
    * @param rootFolderPath The path of to the root folder containing the main
    * QML-file.
    * @param mainQML In simple projects this main-qml can contain the entire
    * app, but for more complex apps this main-qml is simply a dummy file and
    * the actual contents are load and shown individually. The possible dummy
    * file (a QML file that contains one Item and no declared properties,
    * accessable with
    * @param windowSize The initial size of the application window. This size
    * will get overridden if showFullScreen() is called.
    * @param center Whether the window should be centered
    */
    AppWindow(const QString& rootFolderPath,
              const QString& mainQML,
              const QSize& windowSize,
              const bool center=true);

    /** Ensures that all the loaded QML views are destroyed. */
    virtual ~AppWindow();

    /***************************************************************************
     * PROPERTIES
     */
    Q_PROPERTY (float dpi READ getDpi WRITE setDpi NOTIFY dpiChanged)
        void setDpi(int dpi) {m_dpi = dpi; emit dpiChanged();}
        float getDpi() const;
    Q_PROPERTY (float touchSize READ getTouchSize WRITE setTouchSize
                NOTIFY touchSizeChanged)
        float getTouchSize() const;
        void setTouchSize(float) {;}
    Q_PROPERTY (float textSmall READ getTextSmall NOTIFY textSmallChanged)
        float getTextSmall() const;
    Q_PROPERTY (float textMedium READ getTextMedium NOTIFY textMediumChanged)
        float getTextMedium() const;
    Q_PROPERTY (float textLarge READ getTextLarge NOTIFY textLargeChanged)
        float getTextLarge() const;
    Q_PROPERTY (int screenWidth READ getScreenWidth NOTIFY screenWidthChanged)
        float getScreenWidth() const;
    Q_PROPERTY (int screenHeight READ getScreenHeight
                NOTIFY screenHeightChanged)
        float getScreenHeight() const;
    Q_PROPERTY (QSize resolution READ getResolution WRITE setResolution
                NOTIFY resolutionChanged)
        QSize getResolution() const;
        void setResolution(QSize resolution);
    Q_PROPERTY (bool fullScreen READ getFullscreen WRITE setFullScreen
                NOTIFY fullScreenChanged)
        bool getFullscreen() const;
        void setFullScreen(bool value);
    Q_PROPERTY (qreal viewLoadProgress READ getViewLoadProgress
                NOTIFY viewLoadProgressChanged)
        qreal getViewLoadProgress();

    /***************************************************************************
     * PUBLIC FUNCTIONS
     */
    /** Shows the window. Fullscreen mode is turned on by default in Android,
     * otherwise shows in windowed mode
     */
    void showAppWindow();

    /** Returns the root object that resizes to the window */
    QQuickItem *rootObject();

    /** Returns the QQmlEngine of this app */
    QQmlEngine *engine();

    /** Centers the window to the middle of the screen. */
    void centerWindow();

    /** Load and show the view */
    void loadAndShowView(const QString& viewName);

    /** hideAndUnloadView
     * @param viewName
     */
    void hideAndUnloadView(const QString& viewName);

    /** Switches the view to the given one (doesn't delete previous)
     * @param viewName The url of the shown view
     */
    void switchView(const QString &viewName);

    /** Replaces the current view with the given one and deletes all the
     * previous views.
     * @param viewName The url of the shown view.
     */
    void replaceView(const QString &viewName);

    bool showView(const QString& viewName, const QString &layer = "");
    bool hideView(const QString& viewName);
    /** Loads a view from a QML file.
     * previous views.
     * @param viewName The url of the loaded view.
     * @param compilationMode The mode in which the view is loaded. Synchronous
     * by default. Set to Asynchronous for non-blocking load.
     */
    void loadView(const QString& viewName,
                  QQmlComponent::CompilationMode compilationMode =
            QQmlComponent::Asynchronous);
    bool unloadView(const QString& viewName);
    void unloadAllViews();
    void hideAllViews();
    QString getRootFolderPath() const;

    /** Returns a QObject pointer to a QQuickItem with the given objectName.
     * @param objectName Name of the QQuickItem that is fetched.
     * @return A QObject pointer to the wanted QQuickItem. Returns null if the
     * item is not found.
     */
    QObject* getByObjectName(const QString& objectName) const;

    /** Returns a pointer to the specified view.
     * @param Name of the view that is fetched
     * @return A pointer to the specified view
     */
    QQuickItem* getView(const QString& viewName) const;

    /** This function can be used to move active focus to the specified
     * view. If a view with active focus is unloaded, the active focus can be
     * set manually with this function to a new view.
     * @param Name of the view that should be given active focus
     */
    void forceActiveFocus(const QString& viewName);

    /** Used to create a valid, environment indepedendent QString from a
     * relative path. Returns a QString that can be used e.g. in QFiles.
     * @param path The path that is modified.
     */
    QString properPath(const QString& path) const;

    /** Used to create a valid, environment indepedendet QUrl from a
     * relative path. Returns a QUrl that can be used e.g. in QQmlComponents.
     * @param path The path that is modified.
     */
    QUrl properQUrl(const QString& path) const;

signals:
    /***************************************************************************
     * SIGNALS
     */
    void dpiChanged();
    void touchSizeChanged();
    void screenWidthChanged();
    void screenHeightChanged();
    void resolutionChanged();
    void textSmallChanged();
    void textMediumChanged();
    void textLargeChanged();
    void fullScreenChanged();
    void viewLoadProgressChanged();

public slots:
    /***************************************************************************
     * SLOTS
     */
    void viewStatusChanged(QQmlComponent::Status status);
    void progressChanged(qreal progress);
    void updateContentItemHeight();
    void updateContentItemWidth();

private:
    /***************************************************************************
     * PRIVATE VARIABLES
     */
    QString m_rootFolderPath;               ///< The path of the root folder
    QHash<QString,QQuickItem*> m_views;     ///< The available views in the app
    QSize m_resolution;                     ///< The current resolution
    float m_dpi;                            ///< The DPI of the screen
    float m_desktopDpiFactor;               ///< When the app is run on desktop the dpi is multiplied by this factor
    bool m_fullScreen;
    QQmlApplicationEngine m_engine;
    QQuickItem* m_rootObject;
    qreal m_viewLoadProgress;
};

#endif // APPWINDOW_HH
