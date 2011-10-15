////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeEditor.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleNodeEditor
///               class which is responsible for editing nodes.
///
////////////////////////////////////////////////////////////////////////////////

#include "CPuzzleNodeEditor.h"
#include "CPuzzleGraphModel.h"
#include "../../CStyleToolkit.h"
#include "CPuzzleSpanCreator.h"

#include <QLineEdit>
#include <QtGlobal>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::CPuzzleNodeEditor
///
/// @description    This is the default class constructor which initializes
///                 member variables.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is a pointer to the canvas on which all the nodes are
///                 drawn.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeEditor::CPuzzleNodeEditor( QWidget *canvas )
: QObject(canvas)
{
    m_canvas = canvas;

    m_nameEdit = new QLineEdit( m_canvas );
    m_nameEdit->setAlignment( Qt::AlignCenter );
    m_nameEdit->hide();
    connect( m_nameEdit, SIGNAL( editingFinished() ), this, SLOT( nameDone() ) );

    m_heuristicEdit = new QLineEdit( m_canvas );
    m_heuristicEdit->setAlignment( Qt::AlignCenter );
    m_heuristicEdit->hide();
    connect( m_heuristicEdit, SIGNAL( editingFinished() ), this, SLOT( heuristicDone() ) );


    m_selNode = -1;

    setupActions();
    setupMenus();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::~CPuzzleNodeEditor
///
/// @description  This is the class destructor and is provided for maintenence
///               purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeEditor::~CPuzzleNodeEditor()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::canvasContextEvent
///
/// @description   This function handles context menu events.  It spawns the
///                context menu for nodes if the user clicks on a node.
/// @pre           None
/// @post          The context menu is executed if a node was clicked on.
///
/// @param e:      This is a pointer to the context menu event which triggered
///                this function call.
///
/// @return bool:  Returns true if the event is used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeEditor::canvasContextEvent( QContextMenuEvent *e )
{
    m_selNode = m_model->findNode( e->pos() );
    if ( m_selNode != -1 )
    {
        m_spanCreator->setAnchorNode( m_selNode );
        SPuzzleNode::NodeFlag flag = m_model->getNodeAt( m_selNode )->s_flag;

        m_nodeStartAction->setChecked( flag & SPuzzleNode::StartNode );
        m_nodeGoalAction->setChecked( flag & SPuzzleNode::EndNode );

        if ( m_model->getStartNode() == m_model->getNodeAt( m_selNode ) ||
             m_model->getStartNode() == NULL )
            m_nodeStartAction->setEnabled( true );
        else
            m_nodeStartAction->setEnabled( false );

        m_nodeMenu->exec( e->globalPos() );
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::deleteNodeClicked
///
/// @description  This function is called when the user clicks on the menu
///               option to delete a node.
/// @pre          None
/// @post         The selected node is deleted.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::deleteNodeClicked()
{
    m_model->deleteNodeAt( m_selNode );
    m_selNode = -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::nameDone
///
/// @description  This function is called when the user finishs editing a node
///               name.  It hides the line edit and then updates the name of the
///               node.
/// @pre          None
/// @post         The node's name is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::nameDone()
{
    if( m_selNode == -1 )
        return;

    m_nameEdit->hide();
    m_model->editNodeName( m_selNode, m_nameEdit->text() );
    m_selNode = -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::heuristicDone
///
/// @description  This function is called when the user finishs editing a node
///               heuristic value.  It hides the line edit and then updates 
///               the name of the node.
/// @pre          None
/// @post         The node's heuristic value is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::heuristicDone()
{
	if( m_selNode == -1 )
		return;

	m_heuristicEdit->hide();
	m_model->editNodeHeuristic(m_selNode, m_heuristicEdit->text().toDouble() );
	m_selNode = -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::isEditing
///
/// @description   This function is used to indicate if a node's name or
///                heuristic value is being edited
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if a node's name or heuristic value is being
///                edited and false otherwise
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeEditor::isEditing()
{
	if(m_nameEdit->isVisible())      return true;
	if(m_heuristicEdit->isVisible()) return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::renameNodeClicked
///
/// @description  This function is called when the user clicks on the menu
///               option to rename the selected node.  It pops up the line edit
///               so the user may edit the name of the node.
/// @pre          None
/// @post         The line edit is made visible.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::renameNodeClicked()
{
    if ( m_selNode == -1 )
        return;

    m_nameEdit->setText( m_model->getNodeAt( m_selNode )->s_name );
    m_nameEdit->resize( m_nameEdit->sizeHint() );

    int radius = CStyleToolkit::instance()->getNodeRadius();
    QRect ngeo( 0, 0, radius * 2, radius * 2 ), lgeo = m_nameEdit->geometry();
    ngeo.moveCenter( m_model->getNodeAt( m_selNode )->s_position );
    lgeo.moveCenter( ngeo.center() );
    lgeo.moveRight( qMin( lgeo.right(), m_canvas->size().width() ) );
    lgeo.moveLeft( qMax( lgeo.left(), 0 ) );

    m_nameEdit->setGeometry( lgeo );
    m_nameEdit->selectAll();
    m_nameEdit->show();
    m_nameEdit->setFocus( Qt::PopupFocusReason );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::editHeuristicClicked
///
/// @description  This function is called when the user clicks on the menu
///               option to edit the heuristic value of a node.
///
/// @pre          None
/// @post         The line edit is made visible.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::editHeuristicClicked()
{
    if ( m_selNode == -1 )
        return;

    m_heuristicEdit->setText( QString::number(m_model->getNodeAt( m_selNode )->s_heuristic) );
    m_heuristicEdit->resize( m_heuristicEdit->sizeHint() );

    int radius = CStyleToolkit::instance()->getNodeRadius();
    QRect ngeo( 0, 0, radius * 2, radius * 2 ), lgeo = m_heuristicEdit->geometry();
    ngeo.moveCenter( m_model->getNodeAt( m_selNode )->s_position );
    lgeo.moveCenter( ngeo.center() );
    lgeo.moveRight( qMin( lgeo.right(), m_canvas->size().width() ) );
    lgeo.moveLeft( qMax( lgeo.left(), 0 ) );

    m_heuristicEdit->setGeometry( lgeo );
    m_heuristicEdit->selectAll();
    m_heuristicEdit->show();
    m_heuristicEdit->setFocus( Qt::PopupFocusReason );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::setGoalNodeClicked
///
/// @description  This function updates the goal node flag of the selected node
///               to the indicated value.
/// @pre          None
/// @post         The selected node's goal node flag is updated.
///
/// @param on:    This boolean is used to indicate if the goal node flag is
///               being turned on or off.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::setGoalNodeClicked( bool on )
{
    m_model->editNodeEndFlag( m_selNode, on );
    m_selNode = -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::setModel
///
/// @description   This function sets the model pointer so that the painting
///                functions can properly access the span lists to paint
///                the spans needed.
/// @pre           None
/// @post          The model pointer is set.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::setModel( CPuzzleGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::setSpanCreator
///
/// @description         This function sets the span creator pointer and adds
///                      the span creator's menu to the node editor's context
///                      menu for adding new edges.
/// @pre                 None
/// @post                None
///
/// @param spanCreator:  This is a pointer to the span creator object.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::setSpanCreator( CPuzzleSpanCreator *spanCreator )
{
    m_spanCreator = spanCreator;
    m_nodeMenu->insertMenu( m_nodeRenameAction,
                            m_spanCreator->getAddSpanMenu() );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::setStartNodeClicked
///
/// @description  This function updates the start node flag of the selected node
///               to the indicated value.
/// @pre          None
/// @post         The selected node's start node flag is updated.
///
/// @param on:    This boolean is used to indicate if the start node flag is
///               being turned on or off.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::setStartNodeClicked( bool on )
{
    m_model->editNodeStartFlag( m_selNode, on );
    m_selNode = -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::setupActions
///
/// @description  This function creates and initializes the QActions used by the
///               context menu.
/// @pre          None
/// @post         The context menu actions are created and initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::setupActions()
{
    m_nodeStartAction = new QAction( tr( "Start" ), this );
    m_nodeStartAction->setCheckable( true );
    connect( m_nodeStartAction, SIGNAL( triggered( bool ) ), this,
             SLOT(setStartNodeClicked( bool ) ) );

    m_nodeGoalAction = new QAction( tr( "Goal" ), this );
    m_nodeGoalAction->setCheckable( true );
    connect( m_nodeGoalAction, SIGNAL( triggered( bool ) ), this,
             SLOT(setGoalNodeClicked( bool ) ) );

    m_nodeRenameAction = new QAction( tr( "Rename" ), this );
    connect( m_nodeRenameAction, SIGNAL( triggered() ), this,
             SLOT( renameNodeClicked() ) );

    m_editHeuristicAction = new QAction( tr( "Edit heuristic" ), this );
    connect( m_editHeuristicAction, SIGNAL( triggered() ), this,
             SLOT( editHeuristicClicked() ) );

    m_nodeDeleteAction = new QAction( tr( "Delete" ), this );
    connect( m_nodeDeleteAction, SIGNAL( triggered() ), this,
             SLOT( deleteNodeClicked() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor::setupMenus
///
/// @description  This function creates and initializes the context menu.
/// @pre          None
/// @post         The context menu is created and initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeEditor::setupMenus()
{
    m_nodeTypeSubMenu = new QMenu( "Type", m_canvas );
    m_nodeTypeSubMenu->addAction( m_nodeStartAction );
    m_nodeTypeSubMenu->addAction( m_nodeGoalAction );

    m_nodeMenu = new QMenu( "Node menu", m_canvas );
    m_nodeMenu->addMenu( m_nodeTypeSubMenu );
    m_nodeMenu->addAction( m_nodeRenameAction );
	m_nodeMenu->addAction( m_editHeuristicAction );
    m_nodeMenu->addAction( m_nodeDeleteAction );
}
