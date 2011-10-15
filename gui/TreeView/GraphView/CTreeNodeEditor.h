////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeNodeEditor.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeNodeEditor
///               class.  This class is responsible editing tree graph nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREENODEEDITOR_H_
#define _CTREENODEEDITOR_H_

#include <QObject>
#include <QMenu>
#include <QContextMenuEvent>

class CTreeGraphModel;
class QLineEdit;
class QWidget;
class CTreeNodeArtist;
struct STreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor
///
/// @description  This class is responsible for editing nodes.  It also handles
///               the context menu when right clicking on a node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CTreeNodeEditor : public QObject
{
    Q_OBJECT

public:
    CTreeNodeEditor( QWidget *canvas );
    ~CTreeNodeEditor();

    bool canvasContextEvent( QContextMenuEvent *e );
    bool canvasMouseEvent( QMouseEvent *e );

    bool isAddingChildNode();
    bool isEditing();

    void paint();
    void setArtist( CTreeNodeArtist *artist );
    void setModel( CTreeGraphModel *model );

public slots:
    void historyDone();
    void nameDone();
    void valueDone();

private slots:
    void addChildClicked();
    void addParentClicked();
    void deleteNodeClicked();
    void editHistoryClicked();
    void editValueClicked();
    void renameNodeClicked();
    void setQuiescentClicked( bool on );
    void unhookClicked();

private:
    void addChildDone( QPoint point );
    void setupActions();
    void setupMenus();
    void unhookDone( QPoint point );

    //This variable stores a pointer to the model for editing nodes.
    CTreeGraphModel  *m_model;
    //This variable stores a pointer to the canvas for drawing objects to it.
    QWidget          *m_canvas;
    //This variable stores a pointer to the line edit used for editing history.
    QLineEdit        *m_historyEdit;
    //This variable stores a pointer to the line edit used for editing names.
    QLineEdit        *m_nameEdit;
    //This variable stores a pointer to the line edit used for editing values.
    QLineEdit        *m_valueEdit;

    //This variable stores a pointer to the node artist.
    CTreeNodeArtist  *m_nodeArtist;

    //This variable stores a pointer to the node currently being edited.  It
    //should be NULL if no node is being edited.
    STreeNode        *m_selNode;

    //This boolean is used to indicate if a new child node is currently being
    //added.
    bool   m_isAddingChildNode;
    //This variable stores a pointer to the old parent node which the unhooked
    //node reverts to if the user tries to place on empty space.
    STreeNode *m_oldParent;
    //This variable stores the current position of the mouse used for painting.
    QPoint m_curPoint;

    //This variable stores a pointer to the context menu used when a node is
    //right clicked.
    QMenu *m_nodeMenu;

    //This QAction is used when a user wants to add a child node.
    QAction *m_nodeAddChildAction;
    //This QAction is used when a user wants to add a parent node.
    QAction *m_nodeAddParentAction;
    //This QAction is used when a user wants to delete a node.
    QAction *m_nodeDeleteAction;
    //This QAction is used when a user wants to edit the history value.
    QAction *m_nodeEditHistoryAction;
    //This QAction is used when a user wants to edit the value of a node.
    QAction *m_nodeEditValueAction;
    //This QAction is used when a user clicks on the Quiescent context menu
    //option.
    QAction *m_nodeQuiescentAction;
    //This QAction is used when a user wants to rename a node.
    QAction *m_nodeRenameAction;
    //This QAction is used when a user wants to unhook a node to move it to a
    //different parent node.
    QAction *m_nodeUnhookAction;
};

#endif /*_CTREENODEEDITOR_H_*/
