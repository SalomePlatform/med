// Copyright (C) 2016-2023  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "TestController.hxx"
#include "MEDModule.hxx"

#include <Basics_Utils.hxx>  // STDLOG() macro

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDEventListener)

#include <pqTestUtility.h>
#include <pqXMLEventObserver.h>
#include <pqXMLEventSource.h>

#include <pqQVTKWidgetEventPlayer.h>
#include <pqQVTKWidgetEventTranslator.h>

#include <pqTabbedMultiViewWidget.h>
#include <pqApplicationCore.h>
#include <pqTestingReaction.h>
#include <pqSaveScreenshotReaction.h>

#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QIcon>
#include <QTimer>
#include <QEvent>

class PlayTestEvent: public QEvent {
public:
  PlayTestEvent(QEvent::Type type, const std::string & filename): QEvent(type), _filename(filename) {}
  virtual ~PlayTestEvent() {}
  const std::string _filename;
};

TestController::TestController(MEDModule* mod):
  _salomeModule(mod),
  _desk(SUIT_Session::session()->activeApplication()->desktop()),
  _tester(0), _lock_action(0),
  _quitEventType(QEvent::registerEventType()),
  _playEventType(QEvent::registerEventType()),
  _aboutToPlayTest(false),
  _myEventLoopStarted(false)
{
  STDLOG("Creating a TestController");
  _tester = new pqTestUtility(_desk);
  _tester->addEventObserver("xml", new pqXMLEventObserver(_desk));
  _tester->addEventSource("xml", new pqXMLEventSource(_desk));

  QApplication::instance()->installEventFilter(this);
  //QTimer::singleShot(0, this, SLOT(onMainEventLoopStarting()));
}

TestController::~TestController()
{
  if (_tester)
    delete _tester;
  _tester = 0;
}

void
TestController::createActions() {
  //
  // Main actions
  //
  QString label   = tr("LAB_RECORD_TEST");
  QString tooltip = tr("TIP_RECORD_TEST");
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(onRecordTest()),QString(),tooltip);

  // This action has to be placed in the general file menu
  int menuId = _salomeModule->createMenu( tr( "MEN_FILE" ), -1,  1 );
  _salomeModule->action(actionId)->setIconVisibleInMenu(false);
  _salomeModule->createMenu(actionId, menuId, 60);

  label   = tr("LAB_PLAY_TEST");
  tooltip = tr("TIP_PLAY_TEST");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(onPlayTest()),QString(),tooltip);
  _salomeModule->action(actionId)->setIconVisibleInMenu(false);
  _salomeModule->createMenu(actionId, menuId, 70);

  label   = tr("LAB_LOCK_TEST");
  tooltip = tr("TIP_LOCK_TEST");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(onLockViewSize()),QString(),tooltip);
  _salomeModule->action(actionId)->setIconVisibleInMenu(false);
  _salomeModule->action(actionId)->setCheckable(true);
  _lock_action = _salomeModule->action(actionId);
  _salomeModule->createMenu(actionId, menuId, 70);

  label   = tr("LAB_SNAP_TEST");
  tooltip = tr("TIP_SNAP_TEST");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(onTakeSnapshot()),QString(),tooltip);
  _salomeModule->action(actionId)->setIconVisibleInMenu(false);
  _salomeModule->createMenu(actionId, menuId, 70);
}

void TestController::onRecordTest()
{
  QString fileName =
      QFileDialog::getSaveFileName(_desk, "Save test", QString(), QString("XML file (*.xml)"));
  if (!fileName.isEmpty())
    {
      QApplication::setActiveWindow(_desk); //mandatory otherwise record pop up doesn't show up
      _tester->recordTests(fileName);
    }
}


void TestController::onPlayTest()
{
  QString fileName =
      QFileDialog::getOpenFileName(_desk, "Open test", QString(), QString("XML file (*.xml)"));
  if (!fileName.isEmpty())
    _tester->playTests(fileName);
}

void TestController::onLockViewSize() const
{
  pqTestingReaction::lockViewSize(_lock_action->isChecked());
}

void TestController::onTakeSnapshot() const
{
  pqSaveScreenshotReaction::saveScreenshot();
}

void TestController::onRequestTermination()
{
  // Check if test playing
  if (_tester->playingTest() || _aboutToPlayTest)
    {
      QEvent * e = new QEvent((QEvent::Type)_quitEventType);
      QApplication::postEvent(this, e);
    }
  else
    {
      _salomeModule->requestSALOMETermination();
    }
}

void
TestController::customEvent(QEvent * event)
{
  if (event->type() == _quitEventType)
    {
      if(!isMainEventLoopStarted())
          // Repost (=delay)
          QApplication::postEvent(this, new QEvent((QEvent::Type)_quitEventType));
      else
          onRequestTermination();
    }
  else if (event->type() == _playEventType)
    {
      PlayTestEvent * e = dynamic_cast<PlayTestEvent *>(event);
      if (e)
        {
//          // Wait for main event loop to start:
          if(!isMainEventLoopStarted())
              // Repost (=delay)
              QApplication::postEvent(this, new PlayTestEvent((QEvent::Type)_playEventType, e->_filename));
          else
            {
              STDLOG("About to play test " << e->_filename);
              _tester->playTests(e->_filename.c_str());
              _aboutToPlayTest = false;
              STDLOG("Done playing test " << e->_filename);
            }
        }
    }
  else
    { QObject::customEvent(event);  }
}

void
TestController::processWorkspaceEvent(const MEDCALC::MedEvent* event)
{
  if ( event->type == MEDCALC::EVENT_PLAY_TEST ) {
      /* [ABN] Post an event. Indeed, calling the function directly would prevent the proper refresh of the
       * GUI which also needs to go through the MED event loop (WorkspaceController::processWorkspaceEvent)
       */
      _aboutToPlayTest = true; // to prevent an early quit!
      PlayTestEvent * e  = new PlayTestEvent((QEvent::Type)_playEventType, std::string(event->filename));
      QApplication::postEvent(this, e);
  }
  else if ( event->type == MEDCALC::EVENT_QUIT_SALOME ) {
      // [ABN] again: post as an event to give a chance to other events (piled up by test
      // scenarios for example) to execute:
      QEvent * e = new QEvent((QEvent::Type)_quitEventType);
      QApplication::postEvent(this, e);
  }
}

void TestController::onMainEventLoopStarting()
{
  _myEventLoopStarted = true;
  QApplication::instance()->removeEventFilter(this);
}

bool TestController::eventFilter(QObject *obj, QEvent *event)
{
  if ( obj == QApplication::instance() && event->type() == 9999 )
    onMainEventLoopStarting();
  return QObject::eventFilter(obj, event);
}
