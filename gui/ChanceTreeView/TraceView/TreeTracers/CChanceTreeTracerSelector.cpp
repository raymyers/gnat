////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeTracerSelector.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the
///               CTreeTracerSelector class.  This class is the encapsulation
///               of all the tracers as well as the user interface to select
///               them and the parameters.
///
////////////////////////////////////////////////////////////////////////////////
#include "CChanceTreeTracerSelector.h"
#include "CExpectMTracer.h"
#include <QComboBox>
#include <QMenu>
#include <QSpinBox>
#include <QLabel>

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::CChanceTreeTracerSelector
///
/// @description    This is the constructor of the CChanceTreeTracerSelector class. It
///                 uses parent to continue in creating the QObject tree.
/// @pre            None
/// @post           The default values for the stored trace, and style are set
///                 and the CChanceTreeSelector is initialized and connected to the
///                 retrace() slot.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeTracerSelector::CChanceTreeTracerSelector( QObject *parent )
{
    // Initialize Widgets.
    m_aiConfigMenu = new QMenu;
    m_depthSelector = new QSpinBox;
    m_qsDepthSelector = new QSpinBox;
    m_lowerBoundSelector = new QSpinBox;
    m_upperBoundSelector = new QSpinBox;
    m_aiLabel = new QLabel;
    m_aiConfigButton = new QToolButton();

    // Set default search parameters
    m_traceConf.s_isID = false;
    m_traceConf.s_isDL = false;
    m_traceConf.s_isAB = false;
    m_traceConf.s_isCP = false;
    m_traceConf.s_isHT = false;
    m_traceConf.s_isQS = false;
    m_traceConf.s_allowSSS = false;

    // Enable / disable widgets accordingly.
    m_qsDepthSelector->setEnabled(m_traceConf.s_isQS);
    m_depthSelector->setEnabled(m_traceConf.s_isDL);
    m_lowerBoundSelector->setEnabled(m_traceConf.s_isAB);
    m_upperBoundSelector->setEnabled(m_traceConf.s_isAB);

    // Set the ranges of the spin boxes.
    setupDepthList();
    setupQSDepthList();
    setupBoundLists();


    // Configure ai config menu button.
    m_aiConfigButton->setMenu( m_aiConfigMenu );
    m_aiConfigButton->setIcon( QIcon(":/ai.png" ));
    m_aiConfigButton->setPopupMode( QToolButton::InstantPopup );

    // When menu is activated, trace should be redone.
    //connect( m_aiConfigMenu, SIGNAL( activated(int) ), this, SLOT( activate() ) );

    // When depth are changed, trace should be redone.
    connect( m_depthSelector, SIGNAL( valueChanged(int) ),
             this, SLOT( activate() ) );
    connect( m_qsDepthSelector, SIGNAL( valueChanged(int) ),
             this, SLOT( activate() ) );
    connect( m_lowerBoundSelector, SIGNAL( valueChanged(int) ),
             this, SLOT( activate() ) );
    connect( m_upperBoundSelector, SIGNAL( valueChanged(int) ),
             this, SLOT( activate() ) );

    // Setup ai config menu.
    m_aiConfigMenu->setTitle("AI Config");
    setupConfigMenuActions();
    updateLabel();

}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::activate
///
/// @description  Updates the label to reflect current search
/// @pre          m_aiLabel has been initialized.
/// @post         m_aiLabel now contains a meaningfull search description.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::updateLabel() const
{
    QString newLabel = "";
    if(m_traceConf.s_isHT) newLabel += "HT";
    if(m_traceConf.s_isQS) newLabel += "QS";
    if(m_traceConf.s_isAB) newLabel += "*1";
    if(m_traceConf.s_isCP) newLabel += "C";
    if(m_traceConf.s_isID) newLabel += "ID";
    if(m_traceConf.s_isDL) newLabel += "DLEM";
    else newLabel += "ExpectiMinimax";
    m_aiLabel->setText(newLabel);
}
////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::~CChanceTreeTracerSelector
///
/// @description  This is the destructor for the CChanceTreeTracerSelector class which
///               frees up dynamically allocated memory.
/// @pre          None
/// @post         The tracers are deleted if any exist.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeTracerSelector::~CChanceTreeTracerSelector()
{
    while( !m_tracers.isEmpty() )
        delete m_tracers.takeFirst();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::activate
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
void CChanceTreeTracerSelector::activate()
{
    emit activated();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getAILabel
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

QLabel *CChanceTreeTracerSelector::getAILabel()
{
    return m_aiLabel;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getAIConfigMenu
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
QMenu *CChanceTreeTracerSelector::getAIConfigMenu()
{
    return m_aiConfigMenu;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getAIConfigButton
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
QToolButton *CChanceTreeTracerSelector::getAIConfigButton()
{
     return m_aiConfigButton;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getDepthSelector
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
QSpinBox *CChanceTreeTracerSelector::getDepthSelector()
{
    return m_depthSelector;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getQSDepthSelector
///
/// @description          This function returns a pointer to the Quiescent
///                       Search Depth Selector.
/// @pre                  None
/// @post                 A pointer to the qs depth selector is returned.
///
/// @return QSpinBox *:   This is a pointer to a QSpinBox. Changing the
///                       value of this spinbox will immediately change the
///                       trace displayed if it depends on the QS depth value.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QSpinBox *CChanceTreeTracerSelector::getQSDepthSelector()
{
    return m_qsDepthSelector;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getLowerBoundSelector
///
/// @description          This function returns a pointer to the lower bound
///                       selector.
/// @pre                  None
/// @post                 A pointer to the lower bound selector is returned.
///
/// @return QSpinBox *:   This is a pointer to a QSpinBox. Changing the
///                       value of this spinbox will immediately change the
///                       trace displayed if it depends on the bound.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QSpinBox *CChanceTreeTracerSelector::getLowerBoundSelector()
{
    return m_lowerBoundSelector;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::getUpperBoundSelector
///
/// @description          This function returns a pointer to the upper bound
///                       selector.
/// @pre                  None
/// @post                 A pointer to the upper bound selector is returned.
///
/// @return QSpinBox *:   This is a pointer to a QSpinBox. Changing the
///                       value of this spinbox will immediately change the
///                       trace displayed if it depends on the bound.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QSpinBox *CChanceTreeTracerSelector::getUpperBoundSelector()
{
    return m_upperBoundSelector;
}



////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::setupDepthList
///
/// @description  Performas all initialization steps for the depth selector.
/// @pre          None
/// @post         The spin box for depth is restricted to the values from 0 to
///               255.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::setupDepthList()
{
    m_depthSelector->setMinimum( 1 );
    m_depthSelector->setMaximum( 255 );
}



////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::setupQSDepthList
///
/// @description  Performas all initialization steps for the QS depth selector.
/// @pre          None
/// @post         The spin box for qs depth is restricted to the values from 0
///               to 255.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::setupQSDepthList()
{
    m_qsDepthSelector->setMinimum( 0 );
    m_qsDepthSelector->setMaximum( 255 );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::setupBoundLists
///
/// @description  Performas all initialization steps for the bound selectors.
/// @pre          None
/// @post         The spin boxes are restricted to the values from -256
///               to 255.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::setupBoundLists()
{
    m_lowerBoundSelector->setMinimum( -256 );
    m_lowerBoundSelector->setMaximum( 255 );
    m_upperBoundSelector->setMinimum( -256 );
    m_upperBoundSelector->setMaximum( 255 );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::trace
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
/// @return SChanceTreeTrace *: This is the trace generated by the IChanceTreeTracer
///                       subclass.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeTrace *CChanceTreeTracerSelector::trace( const SChanceTreeNode *rootNode ) const
{
    int depth = m_depthSelector->value();
    int qsDepth = m_qsDepthSelector->value();
    int lowerBound = m_lowerBoundSelector->value();
    int upperBound = m_upperBoundSelector->value();
    updateLabel();
    return m_generalTracer.trace( rootNode, depth, qsDepth, m_traceConf,
                                  lowerBound, upperBound );
}


////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::toggleID
///
/// @description  Toggles Iterative Deepening on/off
/// @pre          None
/// @post         Value of m_traceConf.s_isID is changed. If it was changed to
///               true, Depth Limited is also turned on. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::toggleID()
{
    bool b = !m_traceConf.s_isID;
    m_traceConf.s_isID = b;

    if(b && !m_traceConf.s_isDL)
    {
        m_actionDL->setChecked(true);
        activate();
    }

    if(m_traceConf.s_isDL)
    {
        activate();
        m_aiConfigButton->showMenu();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::toggleDL
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
void CChanceTreeTracerSelector::toggleDL()
{
    bool b = !m_traceConf.s_isDL;
    m_depthSelector->setEnabled(b);
    m_traceConf.s_isDL = b;

    if(!b && m_traceConf.s_isID)
    {
        m_actionID->setChecked(false);
    }
    if(!b && m_traceConf.s_isQS)
    {
        m_actionQS->setChecked(false);
    }



    // If DL has just been toggled manually, not a a prereq to something else.
    if(!m_traceConf.s_isID && !m_traceConf.s_isQS)
    {
        activate();
        m_aiConfigButton->showMenu();
    }

}


////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::toggleAB
///
/// @description  Toggles Alpha Beta Pruning on/off
/// @pre          None
/// @post         Value of m_traceConf.s_isAB is changed. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::toggleAB()
{
    bool b = !m_traceConf.s_isAB;
    m_lowerBoundSelector->setEnabled(b);
    m_upperBoundSelector->setEnabled(b);
    m_traceConf.s_isAB = b;
    m_traceConf.s_isCP = b;
    activate();
    m_aiConfigButton->showMenu();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::toggleHT
///
/// @description  Toggles History Table on/off
/// @pre          None
/// @post         Value of m_traceConf.s_isHT is changed. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::toggleHT()
{
    bool b = !m_traceConf.s_isHT;
    m_traceConf.s_isHT = b;
    activate();
    m_aiConfigButton->showMenu();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::toggleQS
///
/// @description  Toggles Quiessence Search on/off
/// @pre          None
/// @post         Value of m_traceConf.s_isQS is changed. If it was changed to
///               true, Depth Limited is also turned on. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::toggleQS()
{
    bool b = !m_traceConf.s_isQS;
    m_qsDepthSelector->setEnabled(b);
    m_traceConf.s_isQS = b;

    if(b && !m_traceConf.s_isDL)
    {
        m_actionDL->setChecked(true);
    }

    if(m_traceConf.s_isDL)
    {
        activate();
        m_aiConfigButton->showMenu();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::toggleSSS
///
/// @description  Toggles Single Successor Shortcut on/off
/// @pre          None
/// @post         Value of m_traceConf.s_allowSSS is changed. Trace is updated.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::toggleSSS()
{
    m_traceConf.s_allowSSS = !m_traceConf.s_allowSSS;
    activate();
    m_aiConfigButton->showMenu();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector::setupConfigMenuActions
///
/// @description  Initializes AI config menu
/// @pre          m_aiConfigMenu has just been initialized to an empty menu.
/// @post         m_aiConfigMenu now contains appropriate configuration options
///               and is set to pop back up when an option is selected.
///
/// @limitations  "Close" menu item not provided. This might be added later.
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeTracerSelector::setupConfigMenuActions()
{
    // Depth Limited toggle
    m_actionDL = new QAction(tr("&Depth Limited"), this);
    m_actionDL->setCheckable(true);
    m_actionDL->setChecked(m_traceConf.s_isDL);
    connect(m_actionDL, SIGNAL(toggled(bool)), this, SLOT(toggleDL()));
    //connect(m_actionDL, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionDL);

    // Iterative Deepening toggle
    m_actionID = new QAction(tr("&Iterative Deepening"), this);
    m_actionID->setCheckable(true);
    m_actionID->setChecked(m_traceConf.s_isID);
    connect(m_actionID, SIGNAL(toggled(bool)), this, SLOT(toggleID()));
    //connect(m_actionID, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionID);

    // Alpha Beta Pruning toggle
    m_actionAB = new QAction(tr("&Star-1 Pruning"), this);
    m_actionAB->setCheckable(true);
    m_actionAB->setChecked(m_traceConf.s_isAB);
    connect(m_actionAB, SIGNAL(toggled(bool)), this, SLOT(toggleAB()));
    //connect(m_actionAB, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionAB);

    // Quiescent Search toggle
    m_actionQS = new QAction(tr("&Quiescent Search"), this);
    m_actionQS->setCheckable(true);
    m_actionQS->setChecked(m_traceConf.s_isQS);
    connect(m_actionQS, SIGNAL(toggled(bool)), this, SLOT(toggleQS()));
    //connect(m_actionQS, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionQS);

    // History Table toggle
    m_actionHT = new QAction(tr("&History Table"), this);
    m_actionHT->setCheckable(true);
    m_actionHT->setChecked(m_traceConf.s_isHT);
    connect(m_actionHT, SIGNAL(toggled(bool)), this, SLOT(toggleHT()));
    //connect(m_actionHT, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionHT);
/*
    m_aiConfigMenu->addSeparator();

    // SSS toggle
    m_actionSSS = new QAction(tr("&Allow SSS"), this);
    m_actionSSS->setCheckable(true);
    m_actionSSS->setChecked(m_traceConf.s_allowSSS);
    connect(m_actionSSS, SIGNAL(toggled(bool)), this, SLOT(toggleSSS()));
    //connect(m_actionSSS, SIGNAL(toggled(bool)), m_aiConfigButton, SLOT(showMenu ()));

    m_aiConfigMenu->addAction(m_actionSSS);
*/
    m_aiConfigMenu->addSeparator();

    // Close button
    m_aiConfigMenu->addAction( new QAction(tr("&Close"), this) );

    //connect(m_aiConfigMenu, SIGNAL(activated(int)), m_aiConfigButton, SLOT(showMenu ()));

}
