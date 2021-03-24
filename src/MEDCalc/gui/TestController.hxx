// Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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

#ifndef SRC_MEDCALC_GUI_TESTCONTROLLER_HXX_
#define SRC_MEDCALC_GUI_TESTCONTROLLER_HXX_

#include <QObject>
#include <QString>

class pqTestUtility;
class MEDModule;
class SUIT_Desktop;
class QAction;
namespace MEDCALC {
  class MedEvent;
}

class TestController: public QObject {
  Q_OBJECT

public:
  TestController(MEDModule* mod);
  virtual ~TestController();

  void createActions();

  bool isMainEventLoopStarted() const { return _myEventLoopStarted; }

  bool eventFilter(QObject *obj, QEvent *event);
  
protected:
  virtual void customEvent(QEvent * event);

public slots:
  void processWorkspaceEvent(const MEDCALC::MedEvent* event);

  void onRecordTest();
  void onPlayTest();
//  void onPlayTestFile();
  void onLockViewSize() const;
  void onTakeSnapshot() const;
  void onRequestTermination();

protected:
  MEDModule* _salomeModule;
  SUIT_Desktop * _desk;

  pqTestUtility * _tester;
  QAction * _lock_action;

private slots:
  void onMainEventLoopStarting();

private:
  const int _playEventType;
  const int _quitEventType;

  bool _aboutToPlayTest;

  bool _myEventLoopStarted;
};

#endif /* SRC_MEDCALC_GUI_TESTCONTROLLER_HXX_ */
