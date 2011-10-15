////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeTracerSelector.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeTracerSelector
///               class.  This class is the encapsulation of all the tracers as
///               well as the user interface to select them and the parameters.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREETRACERSELECTOR_H_
#define _CCHANCETREETRACERSELECTOR_H_

#include <QObject>
#include <QAction>
#include <QList>
#include <QToolButton>

#include "CExpectMTracer.h"
class QComboBox;
class QSpinBox;
class QMenu;
class QLabel;
////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTracerSelector
///
/// @description  This class is the encapsulation of all the tracers as well as
///               the user interface to select them and the parameters. It has a
///               ComboBox that lists all tracers it knows about and two
///               spinboxes that allow user manipulation of the Depth and
///               QSDepth. When any of the parameters are changed it emits the
///               activated() signal.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeTracerSelector : public QObject
{
    Q_OBJECT

public:
    CChanceTreeTracerSelector( QObject *parent );
    ~CChanceTreeTracerSelector();
    QMenu * getAIConfigMenu();
    QToolButton * getAIConfigButton();
    QSpinBox *getDepthSelector();
    QSpinBox *getQSDepthSelector();
    QSpinBox *getLowerBoundSelector();
    QSpinBox *getUpperBoundSelector();
    QLabel *getAILabel();
    SChanceTreeTrace *trace( const SChanceTreeNode *rootNode ) const;


public slots:

    void toggleID();
    void toggleDL();
    void toggleAB();
    void toggleHT();
    void toggleQS();
    void toggleSSS();
    void activate();

signals:
    void activated();

private:
    void setupDepthList();
    void setupTracerList();
    void setupQSDepthList();
    void setupBoundLists();
    void setupFPThresholdList();
    void setupConfigMenuActions();
    void updateLabel() const;

    QMenu                *m_aiConfigMenu;
    QSpinBox             *m_depthSelector;
    QSpinBox             *m_qsDepthSelector;
    QSpinBox             *m_lowerBoundSelector;
    QSpinBox             *m_upperBoundSelector;
    QToolButton          *m_aiConfigButton;
    QLabel               *m_aiLabel;
    QList<CExpectMTracer *>  m_tracers;
    CExpectMTracer       m_generalTracer;

    STraceConfig m_traceConf;

    QAction * m_actionID;
    QAction * m_actionDL;
    QAction * m_actionAB;
    QAction * m_actionHT;
    QAction * m_actionQS;
    QAction * m_actionSSS;

};

#endif//_CTREETRACERSELECTOR_H_
