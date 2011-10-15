////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleTracerSelector.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeTracerSelector
///               class.  This class is the encapsulation of all the tracers as
///               well as the user interface to select them and the parameters.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLETRACERSELECTOR_H_
#define _CPUZZLETRACERSELECTOR_H_

#include <QObject>
#include <QAction>
#include <QList>
#include <QToolButton>

class QComboBox;
class QSpinBox;
class QMenu;
class QLabel;
struct SPuzzleTrace;
class CPuzzleGraphModel;
class ISingleAgentTracer;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTracerSelector
///
/// @description  This class is the encapsulation of all the tracers as well as
///               the user interface to select them and the parameters.
///               When any of the parameters are changed it emits the
///               activated() signal.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleTracerSelector : public QObject
{
    Q_OBJECT

public:
    CPuzzleTracerSelector( QObject *parent );
    ~CPuzzleTracerSelector();
    QMenu * getAIConfigMenu();
    QToolButton * getAIConfigButton();
    QSpinBox *getDepthSelector();
    QComboBox *getAISelector();
    QComboBox *getTreeGraphSelector();
    QLabel *getAILabel();
    SPuzzleTrace *trace( const CPuzzleGraphModel *graph ) const;

public slots:

    void toggleID();
    void toggleDL();
    void activate();

signals:
    void activated();

private:
    void setupSelectors();
    void setupDepthList();
    void setupConfigMenuActions();
    void updateLabel() const;

    QMenu                *m_aiConfigMenu;
    QSpinBox             *m_depthSelector;
    QToolButton          *m_aiConfigButton;
    QLabel               *m_aiLabel;
    QStringList           m_tracerNames;
    QStringList           m_treeGraphNames;
    QList<ISingleAgentTracer *>  m_tracers;
    QComboBox *m_aiSelector;
    QComboBox *m_treeGraphSelector;
    
    bool m_isDL;
    bool m_isID;
    QAction * m_actionID;
    QAction * m_actionDL;
    
};

#endif//_CTREETRACERSELECTOR_H_
