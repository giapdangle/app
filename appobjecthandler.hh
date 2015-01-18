#ifndef APPOBJECTHANDLER_HH
#define APPOBJECTHANDLER_HH

#include <QObject>
#include <QQuickItem>
#include <QQmlComponent>
class AppWindow;

////////////////////////////////////////////////////////////////////////////////
///
/// The AppObjectHandler is primarily used to store and access AppObjects. This
/// base class does not have a built-in data container, and you should implement
/// your own container along with access and input methods. It is primarily
/// meant for storing only one kind of AppObjects, and for each kind of
/// AppObject you should use a separate handler.
///
////////////////////////////////////////////////////////////////////////////////

class AppObjectHandler : public QObject
{
    Q_OBJECT

public:
    /***************************************************************************
     * CONSTRUCTOR AND DESTRUCTOR
     */
    /**
     * @param The window that shows the AppObjects stored by this handler
     * @param A possibly empty list of QStrings indicating the path to the
     * QML-files that should be preloaded for this handler. These preloaded
     * QML-components can then be quickly created for the AppObject's use
     * @param The possible parent for this handler
     */
    AppObjectHandler(AppWindow* window, QObject* parent=0);

    /** Destructs the preloaded QQmlComponents */
    virtual ~AppObjectHandler();

    /***************************************************************************
     * PUBLIC FUNCTIONS
     */
    /**
     * Returns a pointer to a newly created QQuickItem. This pointer has no
     * owner or visual parent.
     */
    QQuickItem* getQuickItemFromComponent(QString qmlPath);

    /**
     * Loads a QQmlComponent with the given compilation mode (Asynchronous,
     * PreferSynchronous).
     * @param qmlPath The path to the component that is loaded
     * @param compilationMode The compilation mode
     */
    void loadComponent(QString qmlPath,
                       QQmlComponent::CompilationMode compilationMode=QQmlComponent::Asynchronous,
                       QQmlEngine* engine=0);
    /**
      * Unloads (deletes) the preloaded component.
      */
    void unloadComponent(QString qmlPath);

public slots:
    /***************************************************************************
     * SLOTS
     */
    void componentStatusChanged(QQmlComponent::Status status);

protected:
    /***************************************************************************
     * PROTECTED VARIABLES
     */
    AppWindow* m_window;                         ///< The window that shows everything.
    QHash<QString, QQmlComponent*> m_components; ///< The preloaded QQmlComponents for quickly creating needed QQuickItems
};

#endif // APPOBJECTHANDLER_HH
