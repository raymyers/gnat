////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeNodeQuickEditor.cpp
///
/// @author       Ray Myers 
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CChanceTreeNodeEditor
///               class.  This class is responsible editing tree graph nodes.
///
////////////////////////////////////////////////////////////////////////////////

#include "CChanceTreeNodeQuickEditor.h"
#include "CChanceTreeGraphModel.h"
#include "../../CStyleToolkit.h"
#include <QWidget>

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeQuickEditor::CChanceTreeNodeQuickEditor
///
/// @description    This is the default constructor for the CChanceTreeNodeEditor
///                 class.  It initializes member variables.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is a pointer to the canvas.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeNodeQuickEditor::CChanceTreeNodeQuickEditor( QWidget *canvas )
: QObject(canvas)
{
    m_canvas = canvas;
    m_text = "";
    m_editingLabel = false;
    m_editingValue = false;
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeQuickEditor::~CChanceTreeNodeQuickEditor
///
/// @description     This is the destructor for the CChanceTreeNodeEditor class and
///                  is provided for maintenance purposes only.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeNodeQuickEditor::~CChanceTreeNodeQuickEditor()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeQuickEditor::canvasKeyPressEvent
///
/// @description   This function handles key press events for editing the name
///                or values of a node.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QKeyEvent which contains information
///                about what triggered this function call.
///
/// @return bool:  Returns true if the event was used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CChanceTreeNodeQuickEditor::canvasKeyPressEvent( QKeyEvent *e )
{
    //Check if there is no node selected and return false to indicate the
    //event wasn't used.
    if( m_selNode == NULL )
        return false;

    //Grab the character that was pressed.
    char key = (char)e->key();

    //Check if a letter was pressed.
    if( key >= Qt::Key_A && key <= Qt::Key_Z )
    {
        //Check if the name was being edited.  If not, stop any editing that
        //was occurring and start the new name with the letter pressed.
        if( !m_editingLabel )
        {
            stop();
            m_editingLabel = true;
        }
        m_text += key;
        m_model->editNodeName( m_selNode, m_text );
        m_canvas->update();
    }
    //Check if a number or the . or - was pressed.
    else if( (key >= Qt::Key_0 && key <= Qt::Key_9)
             || key == Qt::Key_Period || key == Qt::Key_Minus )
    {
        //Check if the value was being edited.  If not, stop any editing that
        //was occurring and start the new value with the key pressed.
        if( !m_editingValue )
        {
            stop();
            m_editingValue = true;
        }
        m_text += key;
        m_model->editNodeValue( m_selNode, m_text.toFloat() );
        m_canvas->update();
    }
    else
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeQuickEditor::canvasMouseEvent
///
/// @description   This function handles mouse events.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QMouse event.
///
/// @return bool:  Returns true if the mouse event is used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CChanceTreeNodeQuickEditor::canvasMouseEvent( QMouseEvent *e )
{
    if( e->type() != QEvent::MouseMove )
        return false;

    if( m_model == NULL )
        return false;

    SChanceTreeNode *node = m_model->findNode( e->pos() );
    if( node != m_selNode )
    {
        m_selNode = node;
        stop();
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeQuickEditor::isEditing
///
/// @description   This function returns true if the user is editing either a
///                node name or value, and false otherwise.
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if a node name or value is being edited.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CChanceTreeNodeQuickEditor::isEditing()
{
    return ( m_editingLabel || m_editingValue );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeEditor::setModel
///
/// @description   This function sets the model pointer so the edit functions
///                may be properly called.
/// @pre           None
/// @post          The model pointer is updated.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeQuickEditor::setModel( CChanceTreeGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeEditor::stop
///
/// @description  This function stops any editing that is occurring and resets
///               the locally stored name or value that was being used.  If
///               quick edit is being turned off (by passing a false value) then
///               the currently selected node is set to NULL to avoid errors.
/// @pre          None
/// @post         Any editing that was occurring is stopped.
///
/// @param on:    This variable is used to indicate if quick edit is being
///               turned on or off.
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeQuickEditor::stop( bool on )
{
    m_editingLabel = false;
    m_editingValue = false;
    m_text = "";
    if ( !on )
        m_selNode = NULL;
}
