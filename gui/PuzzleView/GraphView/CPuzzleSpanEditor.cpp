////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleSpanEditor.cpp
///
/// @author       Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the CPuzzleSpanEditor class which is used
///               to handle end user interaction with spans.
///
/// comment
////////////////////////////////////////////////////////////////////////////////

#include "CPuzzleSpanEditor.h"
#include "CPuzzleGraphModel.h"
#include "../CPuzzleGraph.h"
#include "../../CStyleToolkit.h"
#include "CPuzzleSpanCreator.h"
#include <QLineEdit>
#include <QtGlobal>
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::CPuzzleSpanEditor
///
/// @description     Constructor
///
/// @pre             None
/// @post            The class is initizlized and ready for use
///
/// @param canvas:   Pointer to a QWidget that nodes and spans are drawn on
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleSpanEditor::CPuzzleSpanEditor(QWidget *canvas) : QObject(canvas)
{
    m_canvas   = canvas;
	m_selSpan  =  0;
	m_selIndex = -1;
	m_selType  =  0;

    m_lineEdit = new QLineEdit(m_canvas);
    m_lineEdit->setAlignment(Qt::AlignCenter);
    m_lineEdit->hide();
    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(done()));

    setupActions();
    setupMenus();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::~CPuzzleSpanEditor
///
/// @description     Destructor
///
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleSpanEditor::~CPuzzleSpanEditor(void) { }

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::canvasContextEvent
///
/// @description     This function is used to hande mouse clickes on the parent
///                  widget (the canvas)
///
/// @pre             None
/// @post            None
///
/// @param e:        Pointer to a QContextMenuEvent describing the type of user
///                  interaction that has occured on the canvas
///
/// @return bool:    true:  if a span was clicked on
///                  false: if a span was not clicked on
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleSpanEditor::canvasContextEvent(QContextMenuEvent *e)
{

	// If the user double clicked a weight, bring up the weight line edit
	// widget...if the user double clicked nothing bail.
	if(e->type() == QEvent::MouseButtonDblClick)
	{
		m_model->findWeight(e->pos(), m_selIndex, m_selType);
		if(m_selIndex != -1)
		{
			m_selSpan = m_model->getSpanAt(m_selIndex);
			weightClicked();
			return true;
		}
		else return false;
	}

	// Check if a weight was clicked, then check in an edge was clicked.
	// If nothing was clicked bail.
        m_model->findEdge(e->pos(), m_selIndex, m_selType);
	if(m_selIndex == -1)
	{

		m_model->findWeight(e->pos(), m_selIndex, m_selType);
		if(m_selIndex == -1) return false;
	}

	// Get the edge index
	m_selSpan = m_model->getSpanAt(m_selIndex);

	// Set up the edge edit menu
	if(m_selSpan->s_flag == SPuzzleSpan::Bidirectional)
	{
		m_bidirectionalAction->setEnabled(false);
		m_directionalAction->setEnabled(true);
	}
	else
	{
		m_bidirectionalAction->setEnabled(true);
		m_directionalAction->setEnabled(false);
	}

	// Execute the span edit menue
    m_edgeMenu->exec(e->globalPos());
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::IsEditing
///
/// @description     Povides information on the type of user interaction taking
///                  place.
///
/// @pre             None
/// @post            None
///
/// @return bool:    true:  if the user is in the process of editing
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleSpanEditor::isEditing()
{
    return m_lineEdit->isVisible();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::done
///
/// @description     This function is used to apply user edited edge weights to
///                  spans.
///
/// @pre             None
/// @post            None
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::done()
{
	qreal weight = m_lineEdit->text().toDouble();

	if     (m_selType == 1) { m_model->editSpanWeightFrom1(m_selIndex, weight); }
	else if(m_selType == 2) { m_model->editSpanWeightFrom2(m_selIndex, weight); }
	else if(m_selType == 3) { m_model->editSpanWeightFrom1(m_selIndex, weight);
	                          m_model->editSpanWeightFrom2(m_selIndex, weight); }

	m_lineEdit->hide();
	m_selSpan  =  0;
    m_selIndex = -1;
	m_selType  =  0;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::bidirectionClicked
///
/// @description     This function is called when the user makes a directional
///                  span bidirectional.
///
/// @pre             None
/// @post            The change is applied
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::bidirectionalClicked(void)
{
	qreal weight;

	if     (m_selType == 1) weight = m_selSpan->s_weightFrom1;
	else if(m_selType == 2) weight = m_selSpan->s_weightFrom2;

	m_model->editSpanWeightFrom1(m_selIndex, weight);
	m_model->editSpanWeightFrom2(m_selIndex, weight);
	m_model->editSpanFlag       (m_selIndex, SPuzzleSpan::Bidirectional);

	m_selSpan  =  0;
    m_selIndex = -1;
	m_selType  =  0;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::directionClicked
///
/// @description     This function is called when the user makes a bidirectional
///                  span directional.
///
/// @pre             None
/// @post            The change is applied
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::directionalClicked(void)
{
	qreal weight = m_selSpan->s_weightFrom1;

	m_model->editSpanWeightFrom1(m_selIndex, weight);
	m_model->editSpanWeightFrom2(m_selIndex, weight);
	m_model->editSpanFlag       (m_selIndex, SPuzzleSpan::Split);

	m_selSpan  =  0;
    m_selIndex = -1;
	m_selType  =  0;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::weightClicked
///
/// @description     This function is called the user elects to edit a span
///                  weight
///
/// @pre             None
/// @post            The new weight is applied to the span
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::weightClicked()
{
	QFontMetrics          metrics(CStyleToolkit::instance()->getFont());
	QRect                 rect;
	SPuzzleSpan::SpanFlag flag = m_selSpan->s_flag;
	qreal                 w;

	float  a, b, c;	// Plane equation
	float  x0, y0;	// Node1 position
	float  x1, y1;	// Node2 position
	float  x,  y;	// Midpoint (sort of)
	float  m;		// Length of selected edge

	// Get edge end points
	x0 = (float)m_selSpan->s_node1->s_position.x();
	y0 = (float)m_selSpan->s_node1->s_position.y();
	x1 = (float)m_selSpan->s_node2->s_position.x();
	y1 = (float)m_selSpan->s_node2->s_position.y();

	// Calc midpoint of the edge
	x = 0.5f * (x0+x1);
	y = 0.5f * (y0+y1);

	// Calc the plane equation
	a = y0 - y1;
	b = x1 - x0;
	c = -1.0f * (a*x0 + b*y0);
	m = -1.0f / sqrtf(a*a + b*b);
	a *= m; b *= m; c *= m;

	// Straight edge case
	if(flag != SPuzzleSpan::Split)
	{
		// Select the correct weight
		if     (m_selType == 1) w = m_selSpan->s_weightFrom1;
		else if(m_selType == 2) w = m_selSpan->s_weightFrom2;
		else if(m_selType == 3) w = m_selSpan->s_weightFrom1;

		// Fetch edge weight text box
		rect = metrics.boundingRect(QString::number(w));

		// Calc midpoint of edge weight text box
		x += 5.0f*a + 0.5f*a*(float)rect.width();
		y += 5.0f*b + 0.5f*b*(float)rect.height();
	}
	// Curve edge case
	else
	{
		if     (m_selType == 1) { w = m_selSpan->s_weightFrom1; }
		else if(m_selType == 2) { w = m_selSpan->s_weightFrom2;
		                          a*=-1.0f; b*=-1.0f; c*=-1.0f; }

		// Fetch edge weight text box
		rect = metrics.boundingRect(QString::number(w));

		// Calc midpoint of text box on 
		x += (5.0f + CStyleToolkit::instance()->getNodeRadius()) * a;
		y += (5.0f + CStyleToolkit::instance()->getNodeRadius()) * b;
		x += 0.5f * (float)rect.width()  * a;
		y += 0.5f * (float)rect.height() * b;
	}

	// Initialize geometry of the edit widget
	m_lineEdit->setText(QString::number(w));
	m_lineEdit->resize(m_lineEdit->sizeHint());

	// Cener the edit widget over the midpoint
	rect = m_lineEdit->rect();
	x -= (float)rect.width()  / 2.0f;
	y -= (float)rect.height() / 2.0f;

	// Show the edit widget
	m_lineEdit->setGeometry((int)x, (int)y, rect.width(), rect.height());
    m_lineEdit->selectAll();
    m_lineEdit->show();
    m_lineEdit->setFocus(Qt::PopupFocusReason);
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::deleteClicked
///
/// @description     This function is called the user elects to delete a span
///
/// @pre             None
/// @post            The span is delete from the graph model
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::deleteClicked()
{
	if(m_model->getSpanAt(m_selIndex)->s_flag == SPuzzleSpan::Split)
	{
		if(m_selType == 1)
		{
			m_model->editSpanFlag(m_selIndex, SPuzzleSpan::From2);
			m_model->editSpanWeightFrom1(m_selIndex, 0);
		}
		else if(m_selType == 2)
		{
			m_model->editSpanFlag(m_selIndex, SPuzzleSpan::From1);
			m_model->editSpanWeightFrom2(m_selIndex, 0);
		}
	}
	else m_model->deleteSpanAt(m_selIndex);

	m_selSpan  =  0;
    m_selIndex = -1;
	m_selType  =  0;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::setModel
///
/// @description     Sets the graph model to be manipulated by the user
///
/// @pre             None
/// @post            None
///
/// @param model:    Pointer to the graph structure that is to be manipulated
///                  by the user (this is the graph model being drawn on the
///                  parent widget).
///
/// @return void:    No return information is provided
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::setModel(CPuzzleGraphModel *model)
	{ m_model = model; }

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::setupActions
///
/// @description     This function initializes the menu actions
///
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::setupActions()
{
    m_bidirectionalAction = new QAction(tr("Make Bidirectional"), this);
    connect(m_bidirectionalAction,
		    SIGNAL(triggered()), this,
            SLOT(bidirectionalClicked()));

	m_directionalAction = new QAction(tr("Make Directional"), this);
	connect(m_directionalAction,
		   SIGNAL(triggered()), this,
		   SLOT(directionalClicked()));

    m_weightAction = new QAction(tr("Edit Weight"), this);
    connect(m_weightAction,
		    SIGNAL(triggered()), this,
            SLOT(weightClicked()));

    m_deleteAction = new QAction(tr("Delete"), this);
    connect(m_deleteAction,
		    SIGNAL(triggered()), this,
            SLOT(deleteClicked()));
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor::setupMenus
///
/// @description     This function initilizes the menu elements
///
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanEditor::setupMenus()
{
    m_edgeMenu = new QMenu("Edge menu", m_canvas);

    m_edgeMenu->addAction(m_bidirectionalAction);
	m_edgeMenu->addAction(m_directionalAction);
    m_edgeMenu->addAction(m_weightAction);
    m_edgeMenu->addAction(m_deleteAction);
}
