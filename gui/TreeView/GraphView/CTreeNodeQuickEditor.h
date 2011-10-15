////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeNodeQuickEditor.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeNodeQuickEditor
///               class.  This class is responsible editing tree graph nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREENODEQUICKEDITOR_H_
#define _CTREENODEQUICKEDITOR_H_

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>

class CTreeGraphModel;
class QWidget;
struct STreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeQuickEditor
///
/// @description  This class is used to quickly edit nodes by merely hovering
///               over them and typing letters or numbers to edit the node's
///               name or value, respectively.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CTreeNodeQuickEditor : public QObject
{
    Q_OBJECT

public:
    CTreeNodeQuickEditor( QWidget *canvas );
    ~CTreeNodeQuickEditor();

    bool canvasKeyPressEvent( QKeyEvent *e );
    bool canvasMouseEvent( QMouseEvent *e );

    bool isEditing();

    void setModel( CTreeGraphModel *model );

public slots:
    void stop( bool on = true );

private:
    CTreeGraphModel    *m_model;
    QWidget            *m_canvas;

    QString             m_text;

    bool                m_editingLabel;
    bool                m_editingValue;

    //This variable stores a pointer to the node currently being edited.  It
    //should be NULL if no node is being edited.
    STreeNode          *m_selNode;
};

#endif /*_CTREENODEQUICKEDITOR_H_*/
