# app

| Key Technologies |
| :--------------- |
| C++              |
| QML              |
| Qt               |

Library for easier creation of hybrid C++/QML applications. Main classes and features they provide:

*AppWindow:
**Support for loading multiple QML files into memory and swithing between them in the application logic.
*AppObject:
**Represents an object that typically has a visual representation as certain QML files. Can load multiple QML files as QQuickItems and place them into the window. Includes multiple helper functions for interacting with the QQuickItems.
*AppObjectHandler:
**A generic container class for AppObjects. Used to store and control a group of AppObjects.
