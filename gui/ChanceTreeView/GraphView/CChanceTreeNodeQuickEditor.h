////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeNodeQuickEditor.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeNodeQuickEditor
///               class.  This class is responsible editing tree graph nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREENODEQUICKEDITOR_H_
#define _CCHANCETREENODEQUICKEDITOR_H_

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>

class CChanceTreeGraphModel;
class QWidget;
struct SChanceTreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeQuickEditor
///
/// @description  This class is used to quickly edit nodes by merely hovering
///               over them and typing letters or numbers to edit the node's
///               name or value, respectively.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeNodeQuickEditor : public QObject
{
    Q_OBJECT

public:
    CChanceTreeNodeQuickEditor( QWidget *canvas );
    ~CChanceTreeNodeQuickEditor();

    bool canvasKeyPressEvent( QKeyEvent *e );
    bool canvasMouseEvent( QMouseEvent *e );

    bool isEditing();

    void setModel( CChanceTreeGraphModel *model );

public slots:
    void stop( bool on = true );

private:
    CChanceTreeGraphModel    *m_model;
    QWidget            *m_canvas;

    QString             m_text;

    bool                m_editingLabel;
    bool                m_editingValue;

    //This variable stores a pointer to the node currently being edited.  It
    //should be NULL if no node is being edited.
    SChanceTreeNode          *m_selNode;
};

#endif /*_CTREENODEQUICKEDITOR_H_*/
