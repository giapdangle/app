#ifndef APPOBJECT_HH
#define APPOBJECT_HH

#include "appwindow.hh"
#include "appobjecthandler.hh"
#include <QObject>
#include <QQuickItem>

////////////////////////////////////////////////////////////////////////////////
///
/// The AppObject owns and handles different visual QQuickItems related to a
/// certain visual entity in the app. It is owned by/children of an
/// AppObjectHandler. The AppObject cannot itself be a QQuickItem whose children
/// these other QQuickItems would be, because it has to able to relocate the
/// QQuickItems to different "layers".
///
////////////////////////////////////////////////////////////////////////////////

class AppObject : public QObject
{
    Q_OBJECT

public:
    /***************************************************************************
     * CONSTRUCTOR AND DESTRUCTOR
     */
    /**
     * @param window the AppWindow that these objects are placed in
     * @param handler the AppObjectHandler that owns this object
     */
    AppObject(AppWindow* window, AppObjectHandler* handler);

    /**
     * Deletes all the dynamically allocated QQuickItems that are owned by this
     * object. The destcructor is virtual so that any inheriting class's
     * destructor will be called as well.
     */
    virtual ~AppObject();

    /***************************************************************************
     * PUBLIC FUNCTIONS
     */
    /** Used for manipulation before laying on some layer. */
    QQuickItem* getQuickItem(const QString& qmlPath);

    /** Sets a name and layer for a QQuickItem */
    void setQuickItem(QQuickItem* item,
                      const QString& name,
                      const QString& layer);

    /** Combines getQuickItem() and setQuickItem(). */
    void addQuickItem(const QString& qmlPath,
                      const QString& name,
                      const QString& layer);

    /** Removes a QuickItem from this Object. */
    void removeQuickItem(const QString& name);

    /** Calls the setProperty method of all the QuickItems. */
    void setProperties(const char* property, const QVariant& value);

    /** Calls the setProperty method of the target QuickItem.*/
    void setProperty(const QString& target,
                     const char* property,
                     const QVariant& value);

    /** Change the layer of the target object. */
    void changeLayer(const QString& target, const QString& newLayer);

    // Setters. These functions set the property of all the QQuickItems in the
    // m_hashItems.
    void setX(float x);
    void setY(float y);
    void setZ(int z);
    void setCenterX(float centerX);
    void setCenterY(float centerY);
    void setWidth(float width);
    void setHeight(float height);
    void setRotation(float rotation);

    // Getters
    float getX() const        {return m_x;}
    float getY() const        {return m_y;}
    int getZ() const          {return m_z;}
    float getCenterX() const  {return m_centerX;}
    float getCenterY() const  {return m_centerY;}
    float getWidth() const    {return m_width;}
    float getHeight() const   {return m_height;}
    float getRotation() const {return m_rotation;}

protected:
    /***************************************************************************
     * PROTECTED VARIABLES
     */
    AppWindow* m_window;                      // The window where these objects are placed in
    AppObjectHandler* m_handler;              // Gives quick access to the parent handler
    QHash<QString, QQuickItem*> m_hashItems;  // All the visual parts of this object are stored in a hash
    float m_x;
    float m_y;
    int m_z;
    float m_centerX;
    float m_centerY;
    float m_width;
    float m_height;
    float m_rotation;
};

#endif // APPOBJECT_HH
