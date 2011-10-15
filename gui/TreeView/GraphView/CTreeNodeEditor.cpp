////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeNodeEditor.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CTreeNodeEditor
///               class.  This class is responsible editing tree graph nodes.
///
////////////////////////////////////////////////////////////////////////////////

#include "CTreeNodeEditor.h"
#include "CTreeGraphModel.h"
#include "CTreeNodeArtist.h"
#include "../../CStyleToolkit.h"

#include <QLineEdit>
#include <QtGlobal>
#include <QPainter>

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::CTreeNodeEditor
///
/// @description    This is the default constructor for the CTreeNodeEditor
///                 class.  It initializes member variables.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is a pointer to the canvas.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CTreeNodeEditor::CTreeNodeEditor( QWidget *canvas )
: QObject(canvas)
{
    m_canvas = canvas;
    m_isAddingChildNode = false;
    m_selNode = NULL;
    m_oldParent = NULL;

    //Setup the line edit used for editing node history values.
    m_historyEdit = new QLineEdit( m_canvas );
    m_historyEdit->setAlignment( Qt::AlignCenter );
    m_historyEdit->hide();
    connect( m_historyEdit, SIGNAL( editingFinished() ), this,
             SLOT( historyDone() ) );

    //Setup the line edit used for editing node names.
    m_nameEdit = new QLineEdit( m_canvas );
    m_nameEdit->setAlignment( Qt::AlignCenter );
    m_nameEdit->hide();
    connect( m_nameEdit, SIGNAL( editingFinished() ), this,
             SLOT( nameDone() ) );

    //Setup the line edit used for editing node values.
    m_valueEdit = new QLineEdit( m_canvas );
    m_valueEdit->setAlignment( Qt::AlignCenter );
    m_valueEdit->hide();
    connect( m_valueEdit, SIGNAL( editingFinished() ), this,
             SLOT( valueDone() ) );

    //Setup actions and menus.
    setupActions();
    setupMenus();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::~CTreeNodeEditor
///
/// @description     This is the destructor for the CTreeNodeEditor class and
///                  is provided for maintenance purposes only.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CTreeNodeEditor::~CTreeNodeEditor()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::addChildClicked
///
/// @description  This function is called when the user selects to add a child
///               node.  It modifies the boolean variable indicating a child
///               node is being added so mouse move events are captured.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::addChildClicked()
{
    m_isAddingChildNode = true;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::addChildDone
///
/// @description   This function is used to indicate that the used has placed
///                the new child node which is being added and creates the
///                child node at the indicated point.
/// @pre           None
/// @post          A new child node is added to the previously selected node at
///                the given point.
///
/// @param point:  This point is where the new child node is to be placed.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::addChildDone( QPoint point )
{
    m_model->addChildNode( m_selNode, point );
    m_selNode = NULL;
    m_isAddingChildNode = false;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::addParentClicked
///
/// @description  This function is called when the user selects to add a new
///               parent node.  It adds the new parent node to the previously
///               selected node.
/// @pre          None
/// @post         The previously selected node is given a new parent node
///               which is in turn made a child node of the previously parent
///               node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::addParentClicked()
{
    m_model->addParentNode( m_selNode );
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::canvasContextEvent
///
/// @description   This function handles context menu events.  If the user
///                clicked on a node, the node context menu is executed and
///                true is returned.  Otherwise, false is returned.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QContextMenuEvent which determines
///                where the user clicked.
///
/// @return bool:  Returns true if the user clicked on a node and the node
///                context menu is executed and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeEditor::canvasContextEvent( QContextMenuEvent *e )
{
    if ( m_oldParent != NULL )
        return true;

    //Find if the user clicked on a node or not.
    m_selNode = m_model->findNode( e->pos() );

    //Check if a node was found at the point the user selected.
    if ( m_selNode != NULL )
    {
        //Set the checked value for the Quiescent menu option to properly
        //reflect the node which was clicked on.
        m_nodeQuiescentAction->setChecked( m_selNode->s_quiescent );

        //Set the unhook node to disabled if the node is the root node.
        m_nodeUnhookAction->setDisabled( m_selNode->s_parent == NULL );

        //Pop up the node context menu.
        m_nodeMenu->exec( e->globalPos() );
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::canvasMouseEvent
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
bool CTreeNodeEditor::canvasMouseEvent( QMouseEvent *e )
{
    switch( e->type() )
    {
    case QEvent::MouseButtonPress:
        return ( isAddingChildNode() || m_oldParent != NULL );
    case QEvent::MouseMove:
        //Check if a child node is being added or a node is being moved.
        if ( isAddingChildNode() || m_oldParent != NULL )
        {
            //Update the position and repaint the canvas.
            m_curPoint = e->pos();
            if ( m_oldParent != NULL )
            {
                m_model->editSubTreePosition( m_selNode, ( e->pos() +
                    QPoint(0, CStyleToolkit::instance()->getNodeRadius()*3 )));
            }
            m_canvas->repaint();
            return true;
        }
        return false;
    case QEvent::MouseButtonRelease:
        //Check if a child node is being added.
        if ( isAddingChildNode() )
        {
            //Add the child node at the indicated position.
            addChildDone( e->pos() );
            return true;
        }
        else if ( m_oldParent != NULL )
        {
            unhookDone( e->pos() );
            return true;
        }
        else
            return false;
    default:
        return false;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::deleteNodeClicked
///
/// @description  This function is called when a user selects the option to
///               delete a node.  It deletes the selected node and all its
///               children.
/// @pre          None
/// @post         The selected node and all its children are deleted.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::deleteNodeClicked()
{
    m_model->deleteNode( m_selNode );
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::editHistoryClicked
///
/// @description     This function is called when a user clicks on the edit
///                  history value option.  It pops up the line edit for editing
///                  values to allow the user to type in the new history value
///                  for the node.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::editHistoryClicked()
{
    //Check if the selected node is NULL.  Return if it is.
    if ( m_selNode == NULL )
        return;

    //Set the value in the line edit to be the current value of the node.
    m_historyEdit->setText( QString::number( m_selNode->s_history ) );
    m_historyEdit->resize( m_valueEdit->sizeHint() );

    //Center the line edit on the node if possible, while still leaving it
    //fully on the canvas.
    int radius = CStyleToolkit::instance()->getNodeRadius();
    QRect ngeo( 0, 0, radius * 2, radius * 2 ), lgeo = m_valueEdit->geometry();
    ngeo.moveCenter( m_selNode->s_position );
    lgeo.moveCenter( ngeo.center() );
    lgeo.moveRight( qMin( lgeo.right(), m_canvas->size().width() ) );
    lgeo.moveLeft( qMax( lgeo.left(), 0 ) );

    //Show the value line edit.
    m_historyEdit->setGeometry( lgeo );
    m_historyEdit->selectAll();
    m_historyEdit->show();
    m_historyEdit->setFocus( Qt::PopupFocusReason );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::editValueClicked
///
/// @description     This function is called when a user clicks on the edit
///                  value option.  It pops up the line edit for editing values
///                  to allow the user to type in the new value for the node.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::editValueClicked()
{
    //Check if the selected node is NULL.  Return if it is.
    if ( m_selNode == NULL )
        return;

    //Set the value in the line edit to be the current value of the node.
    m_valueEdit->setText( QString::number( m_selNode->s_value ) );
    m_valueEdit->resize( m_valueEdit->sizeHint() );

    //Center the line edit on the node if possible, while still leaving it
    //fully on the canvas.
    int radius = CStyleToolkit::instance()->getNodeRadius();
    QRect ngeo( 0, 0, radius * 2, radius * 2 ), lgeo = m_valueEdit->geometry();
    ngeo.moveCenter( m_selNode->s_position );
    lgeo.moveCenter( ngeo.center() );
    lgeo.moveRight( qMin( lgeo.right(), m_canvas->size().width() ) );
    lgeo.moveLeft( qMax( lgeo.left(), 0 ) );

    //Show the value line edit.
    m_valueEdit->setGeometry( lgeo );
    m_valueEdit->selectAll();
    m_valueEdit->show();
    m_valueEdit->setFocus( Qt::PopupFocusReason );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::historyDone
///
/// @description  This function is called when the user is done editing the
///               history value of a node.  It saves the new value to the
///               selected node.
/// @pre          None
/// @post         The value of the selected node is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::historyDone()
{
    //Check if there is a node currently selected and return if there is none.
    if( m_selNode == NULL )
        return;

    m_historyEdit->hide();
    m_model->editNodeHistory( m_selNode, m_historyEdit->text().toInt() );
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::isAddingChildNode
///
/// @description   This function returns true if a child node is currently being
///                added and false otherwise.
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if a child node is being added and false
///                otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeEditor::isAddingChildNode()
{
    return m_isAddingChildNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::isEditing
///
/// @description   This function returns true if the user is editing either a
///                node name or value, and false otherwise.
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if either the name or value line edit is visible
///                for the user to edit the appropriate value and false
///                otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeEditor::isEditing()
{
    return ( m_nameEdit->isVisible() || m_valueEdit->isVisible() );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::nameDone
///
/// @description  This function is called when a user has finished editing the
///               name of a node.  It hides the line edit and saves the new
///               name of the node.
/// @pre          None
/// @post         The name of the selected node is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::nameDone()
{
    //Check if the selected node is set to NULL and return if it is.
    if( m_selNode == NULL )
        return;

    m_nameEdit->hide();
    m_model->editNodeName( m_selNode, m_nameEdit->text() );
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::paint
///
/// @description  This function paints the new child node if one is being added.
/// @pre          None
/// @post         The new child node is painted to the screen if one is being
///               added.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::paint()
{
    //Check if a child node is being added and return if none is.
    if ( isAddingChildNode() )
    {
        QPainter painter( m_canvas );

        //Create a bounding rectangle for where the node will go.
        int radius = CStyleToolkit::instance()->getNodeRadius();
        QRect rect( 0, 0, radius * 2, radius * 2 );
        rect.moveCenter( m_curPoint );

        //Set the painter to draw the new node like a regular node, but with the
        //new object color.
        QPen pen = CStyleToolkit::instance()->getPen();
        pen.setColor( CStyleToolkit::instance()->getNewColor() );
        painter.setPen(pen);
        painter.setBrush( CStyleToolkit::instance()->getBrush() );
        painter.setRenderHint( QPainter::Antialiasing );

        //Draw the new node and a line connecting it to the selected node.
        painter.drawLine( m_selNode->s_position, m_curPoint );
        painter.drawEllipse( rect );
    }
    else if ( m_oldParent != NULL )
    {
        QPainter painter( m_canvas );

        //Set the painter to draw the new node like a regular node, but with the
        //new object color.
        QPen pen = CStyleToolkit::instance()->getPen();
        pen.setColor( CStyleToolkit::instance()->getNewColor() );
        painter.setPen(pen);
        painter.setBrush( CStyleToolkit::instance()->getBrush() );
        painter.setFont( CStyleToolkit::instance()->getFont() );
        painter.setRenderHint( QPainter::Antialiasing );

        painter.drawLine( m_selNode->s_position, m_curPoint );
        m_nodeArtist->paintModelHelper( m_selNode, &painter );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::renameNodeClicked
///
/// @description  This function is called when a user selects the option to
///               rename a node.  It pops up the line edit box for the selected
///               node so the user may edit the name of the node.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::renameNodeClicked()
{
    //Check if there is a node currently selected and return if there is none.
    if ( m_selNode == NULL )
        return;

    m_nameEdit->setText( m_selNode->s_name );
    m_nameEdit->resize( m_nameEdit->sizeHint() );

    //Center the line edit on the node if possible while keeping it on the
    //canvas.
    int radius = CStyleToolkit::instance()->getNodeRadius();
    QRect ngeo( 0, 0, radius * 2, radius * 2 ), lgeo = m_nameEdit->geometry();
    ngeo.moveCenter( m_selNode->s_position );
    lgeo.moveCenter( ngeo.center() );
    lgeo.moveRight( qMin( lgeo.right(), m_canvas->size().width() ) );
    lgeo.moveLeft( qMax( lgeo.left(), 0 ) );

    m_nameEdit->setGeometry( lgeo );
    m_nameEdit->selectAll();
    m_nameEdit->show();
    m_nameEdit->setFocus( Qt::PopupFocusReason );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::setArtist
///
/// @description    This function sets the pointer for the node artist.
/// @pre            None
/// @post           None
///
/// @param artist:  This is a pointer to the node artist to use.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::setArtist( CTreeNodeArtist *artist )
{
    m_nodeArtist = artist;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::setModel
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
void CTreeNodeEditor::setModel( CTreeGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::setQuiescentClicked
///
/// @description  This function is called when a user clicks the Quiescent
///               menu option.  It toggles the quiescent flag for the selected
///               node.
/// @pre          None
/// @post         None
///
/// @param on:    <desc>
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::setQuiescentClicked( bool on )
{
    m_model->editNodeQuiescentFlag( m_selNode, on );
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::setupActions
///
/// @description  This function initalizes all the QActions for being used in
///               the node context menu.
/// @pre          None
/// @post         The QAction member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::setupActions()
{
    m_nodeAddChildAction = new QAction( tr( "Add Child" ), this );
    connect( m_nodeAddChildAction, SIGNAL( triggered() ), this,
             SLOT( addChildClicked() ) );

    m_nodeAddParentAction = new QAction( tr( "Add Parent" ), this );
    connect( m_nodeAddParentAction, SIGNAL( triggered() ), this,
             SLOT( addParentClicked() ) );

    m_nodeDeleteAction = new QAction( tr( "Delete" ), this );
    connect( m_nodeDeleteAction, SIGNAL( triggered() ), this,
             SLOT( deleteNodeClicked() ) );

    m_nodeEditValueAction = new QAction( tr( "Edit Value" ), this );
    connect( m_nodeEditValueAction, SIGNAL( triggered() ), this,
             SLOT( editValueClicked() ) );

    m_nodeEditHistoryAction = new QAction( tr( "Edit History Value" ), this );
    connect( m_nodeEditHistoryAction, SIGNAL( triggered() ), this,
             SLOT( editHistoryClicked() ) );
    
    m_nodeQuiescentAction = new QAction( tr( "Quiescent" ), this );
    m_nodeQuiescentAction->setCheckable( true );
    connect( m_nodeQuiescentAction, SIGNAL( triggered( bool ) ), this,
             SLOT(setQuiescentClicked( bool ) ) );

    m_nodeRenameAction = new QAction( tr( "Rename" ), this );
    connect( m_nodeRenameAction, SIGNAL( triggered() ), this,
             SLOT( renameNodeClicked() ) );

    m_nodeUnhookAction = new QAction( tr( "Unhook Node" ), this );
    connect( m_nodeUnhookAction, SIGNAL( triggered() ), this,
             SLOT( unhookClicked() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::setupMenus
///
/// @description  This function initializes the node context menu.
/// @pre          None
/// @post         The node context menu is initalized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::setupMenus()
{
    m_nodeMenu = new QMenu( "Node menu", m_canvas );
    m_nodeMenu->addAction( m_nodeAddChildAction );
    m_nodeMenu->addAction( m_nodeAddParentAction );
    m_nodeMenu->addAction( m_nodeRenameAction );
    m_nodeMenu->addAction( m_nodeEditValueAction );
    m_nodeMenu->addAction( m_nodeEditHistoryAction );
    m_nodeMenu->addAction( m_nodeQuiescentAction );
    m_nodeMenu->addAction( m_nodeDeleteAction );
    m_nodeMenu->addAction( m_nodeUnhookAction );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::unhookClicked
///
/// @description  This function is called whenever a user clicks on the
///               unhook option for a node.  It unhooks the node from the graph.
/// @pre          None
/// @post         The currently selected node is unhooked from the graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::unhookClicked()
{
    //Save the old parent in case the user cancels the node moving.
    m_oldParent = m_selNode->s_parent;
    m_model->unhook( m_selNode );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::unhookDone
///
/// @description   This function is called when the user selects a point to end
///                the moving of an unhooked node.  If a valid node is selected,
///                the unhooked node is reattached to the graph as a child of
///                the selected node.  If no node is selected, the unhooked
///                node is reattached to its old parent node.
/// @pre           None
/// @post          The unhooked node is reattached to the graph.
///
/// @param point:  This is the point where the user clicked to reattach the
///                unhooked node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::unhookDone( QPoint point )
{
    STreeNode *newParent = m_model->findNode( point );

    //Check if a new parent node was selected.  If not, use the old parent node.
    if ( newParent == NULL )
        newParent = m_oldParent;
    m_model->rehook( newParent, m_selNode );
    m_oldParent = NULL;
    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeEditor::valueDone
///
/// @description  This function is called when the user is done editing the
///               value of node.  It saves the new value to the selected node.
/// @pre          None
/// @post         The value of the selected node is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeEditor::valueDone()
{
    //Check if there is a node currently selected and return if there is none.
    if( m_selNode == NULL )
        return;

    m_valueEdit->hide();
    m_model->editNodeValue( m_selNode, m_valueEdit->text().toFloat() );
    m_selNode = NULL;
}
