////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleTracerSelector.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the
///               CPuzzleTracerSelector class.  This class is the encapsulation
///               of all the tracers as well as the user interface to select
///               them and the parameters.
///
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleTracerSelector.h"
#include "CUDTracer.h"
#include "CBDTracer.h"
#include "IOpenList.h"
#include "IPuzzleTracer.h"
#include "../../GraphView/CPuzzleGraphModel.h"
#include <QComboBox>
#include <QMenu>
#include <QSpinBox>
#include <QLabel>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::CPuzzleTracerSelector
///
/// @description    This is the constructor of the CPuzzleTracerSelector class. It
///                 uses parent to continue in creating the QObject tree.
/// @pre            None
/// @post           Components of CPuzzleSelector (ai selector, tree/graph selector,
///                 depth selector, config menu) are initialized and connected to the
///                 retrace() slot.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleTracerSelector::CPuzzleTracerSelector( QObject *parent )
{
    m_aiConfigMenu = new QMenu;
    m_depthSelector = new QSpinBox;
    //m_qsDepthSelector = new QSpinBox;
    m_aiSelector = new QComboBox;
    m_treeGraphSelector = new QComboBox;
    m_aiLabel = new QLabel;
    m_aiConfigButton = new QToolButton();
    //m_traceConf.s_name = "Generalized Search";


    m_isDL = false;
    m_isID = false;

    setupDepthList();
    setupSelectors();
    updateLabel();
    m_aiConfigMenu->setTitle("AI Config");

    setupConfigMenuActions();
    m_depthSelector->setEnabled(m_isDL);

    m_aiConfigButton->setMenu( m_aiConfigMenu );
    m_aiConfigButton->setIcon( QIcon(":/ai.png" ));
    m_aiConfigButton->setPopupMode( QToolButton::InstantPopup );

    connect( m_aiConfigMenu, SIGNAL( activated(int) ), this, SLOT( activate() ) );


    connect( m_depthSelector, SIGNAL( valueChanged(int) ),
             this, SLOT( activate() ) );

    connect( m_treeGraphSelector, SIGNAL( activated(int) ), this, SLOT( activate() ) );



    connect( m_aiSelector, SIGNAL( activated(int) ), this, SLOT( activate() ) );

}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTracerSelector::setupTracerList
///
/// @description  Performas all initialization steps for the tracer selector.
///               When adding traces to the program this is the only function
///               that needs to be updated for that tracer to appear.
/// @pre          None
/// @post         The combo box for tracers filled with their names, and a
///               parallel QList is filled with instances of each tracer.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::setupSelectors()
{
    m_aiSelector->clear();
    m_treeGraphSelector->clear();
    m_tracers.clear();
    m_treeGraphNames.clear();

    //m_tracers.append( new ... );
    m_tracers.append( new CUDTracer<CDFOpenList> );
    m_tracerNames.append( "DF" );
    m_aiSelector->addItem("Depth First");

    m_tracers.append( new CUDTracer<CBFOpenList> );
    m_tracerNames.append( "BF" );
    m_aiSelector->addItem("Breadth First");

    m_tracers.append( new CUDTracer<CUCOpenList> );
    m_tracerNames.append( "UC" );
    m_aiSelector->addItem("Uniform Cost");

    m_tracers.append( new CUDTracer<CGBFOpenList> );
    m_tracerNames.append( "GBF" );
    m_aiSelector->addItem("Greedy Best First");

    m_tracers.append( new CUDTracer<CAStarOpenList> );
    m_tracerNames.append( "A*" );
    m_aiSelector->addItem("A-Star");

    m_tracers.append( new CBDTracer<CDFOpenList> );
    m_tracerNames.append( "Bi-D DF" );
    m_aiSelector->addItem("Bi-Directional Depth First");

    m_tracers.append( new CBDTracer<CBFOpenList> );
    m_tracerNames.append( "Bi-D BF" );
    m_aiSelector->addItem("Bi-Directional Breadth First");

    m_tracers.append( new CBDTracer<CUCOpenList> );
    m_tracerNames.append( "Bi-D UC" );
    m_aiSelector->addItem("Bi-Directional Uniform Cost");

    /*
    m_tracers.append( new CBDTracer<CGBFOpenList> );
    m_tracerNames.append( "Bi-D GBF" );
    m_aiSelector->addItem("Bi-Directional Greedy Best First");

    m_tracers.append( new CBDTracer<CAStarOpenList> );
    m_tracerNames.append( "Bi-D A*" );
    m_aiSelector->addItem("Bi-Directional A-Star");
    */


    m_treeGraphNames.append( "TS" );
    m_treeGraphSelector->addItem("Tree Search");

    m_treeGraphNames.append( "GS" );
    m_treeGraphSelector->addItem("Graph Search");

}

QComboBox *CPuzzleTracerSelector::getAISelector()
{
    return m_aiSelector;
}

QComboBox *CPuzzleTracerSelector::getTreeGraphSelector()
{
    return m_treeGraphSelector;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::activate
///
/// @description  Updates the label to reflect current search
/// @pre          m_aiLabel has been initialized.
/// @post         m_aiLabel now contains a meaningfull search description.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::updateLabel() const
{
    QString newLabel = "";
    /*if(m_traceConf.s_isHT) newLabel += "HT";
    if(m_traceConf.s_isQS) newLabel += "QS";
    if(m_traceConf.s_isAB) newLabel += "AB";
    if(m_traceConf.s_isID) newLabel += "ID";
    if(m_traceConf.s_isDL) newLabel += "DLM";
    else newLabel += "Minimax";
    */
    if(m_isID) newLabel += "ID";
    else if(m_isDL) newLabel += "DL";
    newLabel += m_tracerNames[m_aiSelector->currentIndex()];
    newLabel += m_treeGraphNames[m_treeGraphSelector->currentIndex()];
    m_aiLabel->setText(newLabel);
}
////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::~CPuzzleTracerSelector
///
/// @description  This is the destructor for the CPuzzleTracerSelector class which
///               frees up dynamically allocated memory.
/// @pre          None
/// @post         The tracers are deleted if any exist.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleTracerSelector::~CPuzzleTracerSelector()
{
    //while( !m_tracers.isEmpty() )
        //delete m_tracers.takeFirst();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::activate
///
/// @description  This function merely emits the activated signal. It is used to
///               combine and forward the activated signal from the different
///               QWidgets.
/// @pre          None
/// @post         The tracers are deleted if any exist.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::activate()
{
    emit activated();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::getAILabel
///
/// @description          This function returns a pointer to the AI Label.
/// @pre                  None
/// @post                 A pointer to the AI Label is returned.
///
/// @return QLabel *:     This is a pointer to a QLabel.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////

QLabel *CPuzzleTracerSelector::getAILabel()
{
    return m_aiLabel;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::getAIConfigMenu
///
/// @description          This function returns a pointer to the AI Config Menu.
/// @pre                  None
/// @post                 A pointer to the AI Config Menu is returned.
///
/// @return QMenu *:      This is a pointer to a QMenu.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QMenu *CPuzzleTracerSelector::getAIConfigMenu()
{
    return m_aiConfigMenu;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::getAIConfigButton
///
/// @description          This function returns a pointer to the AI Config Button.
/// @pre                  None
/// @post                 A pointer to the AI Config Button is returned.
///
/// @return QToolButton *:This is a pointer to a QToolButton.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QToolButton *CPuzzleTracerSelector::getAIConfigButton()
{
     return m_aiConfigButton;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::getDepthSelector
///
/// @description          This function returns a pointer to the Depth
///                       Selector.
/// @pre                  None
/// @post                 A pointer to the depth selector is returned.
///
/// @return QSpinBox *:   This is a pointer to a QSpinBox. Changing the
///                       value of this spinbox will immediately change the
///                       trace displayed if it depends on the depth value.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QSpinBox *CPuzzleTracerSelector::getDepthSelector()
{
    return m_depthSelector;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::setupDepthList
///
/// @description  Performas all initialization steps for the depth selector.
/// @pre          None
/// @post         The spin box for depth is restricted to the values from 1 to
///               255.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::setupDepthList()
{
    m_depthSelector->setMinimum( 0 );
    m_depthSelector->setMaximum( 255 );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::trace
///
/// @description  This is a forwarding function that encapsulates the interface
///               with the tracers. Calling it will call the trace() function
///               on the currently selected tracer and it will pull the depth
///               and qs depth values from the spin boxes.
/// @pre          None
/// @post         The trace is returned as determined by the selector widgets'
///               current states.
///
/// @param rootNode: This is the root of the tree that is to be traced.
///
/// @return STreeTrace *: This is the trace generated by the ITreeTracer
///                       subclass.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleTrace *CPuzzleTracerSelector::trace( const CPuzzleGraphModel *graph ) const
{
    int depth;
    if(m_isDL)
        depth = m_depthSelector->value();
    else
        depth = -1;
    //int qsDepth = m_qsDepthSelector->value();
    //updateLabel();
    ISingleAgentTracer *tracer = m_tracers[m_aiSelector->currentIndex()];

    updateLabel();
    //m_aiLabel->setText("a");
    CUDTracer<CBFOpenList> a;
    //return a.trace(-1,false,false,graph);
    return tracer->trace( depth,
                          m_isID,
                          (m_treeGraphSelector->currentIndex() == 1),
                          graph );
    //return tracer->trace( graph );
    //return m_DFTSTracer->trace( graph );//rootNode );
}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::toggleID
///
/// @description  Toggles Iterative Deepening on/off
/// @pre          None
/// @post         Value of m_traceConf.s_isID is changed. If it was changed to
///               true, Depth Limited is also turned on. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::toggleID()
{
    bool b = m_actionID->isChecked();
    m_isID = b;

    if(b && !m_isDL)
    {
    //    m_aiConfigMenu->close();
        m_actionDL->setChecked(true);
        //activate();
    }
    else
    {
        activate();
        m_aiConfigButton->showMenu();
    }


}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::toggleDL
///
/// @description  Toggles Depth Limited on/off
/// @pre          None
/// @post         Value of m_traceConf.s_isDL is changed. If it was changed to
///               false, Iterative Deepening and Quiessence Search are also
///               turned off. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::toggleDL()
{
    bool b = m_actionDL->isChecked();
    m_isDL = b;
    m_depthSelector->setEnabled(b);

    if(!b && m_isID)
    {
    //    m_aiConfigMenu->close();
        m_actionID->setChecked(false);
    }
    else
    {
        activate();
        m_aiConfigButton->showMenu();
    }


}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector::setupConfigMenuActions
///
/// @description  Initializes AI config menu
/// @pre          m_aiConfigMenu has just been initialized to an empty menu.
/// @post         m_aiConfigMenu now contains appropriate configuration options
///               and is set to pop back up when an option is selected.
///
/// @limitations  "Close" menu item not provided. This might be added later.
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTracerSelector::setupConfigMenuActions()
{

    // Depth Limited toggle
    m_actionDL = new QAction(tr("&Depth Limited"), this);
    m_actionDL->setCheckable(true);
    m_actionDL->setChecked(m_isDL);
    connect(m_actionDL, SIGNAL(toggled(bool)), this, SLOT(toggleDL()));
    //connect(m_actionDL, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionDL);

    // Iterative Deepening toggle
    m_actionID = new QAction(tr("&Iterative Deepening"), this);
    m_actionID->setCheckable(true);
    m_actionID->setChecked(m_isID);
    connect(m_actionID, SIGNAL(toggled(bool)), this, SLOT(toggleID()));
    //connect(m_actionID, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionID);

    m_aiConfigMenu->addSeparator();

    // Close button
    m_aiConfigMenu->addAction( new QAction(tr("&Close"), this) );

    //connect(m_aiConfigMenu, SIGNAL(activated(int)), m_aiConfigButton, SLOT(showMenu ()));


}
