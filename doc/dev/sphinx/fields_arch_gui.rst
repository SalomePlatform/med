.. _fields_arch_gui_page:

*****************
FIELDS module GUI 
*****************

FIELDS module uses the following components to manage and display presentations:

* Standard SALOME *Object browser* represents structure of MED file within FIELDS module. When MED file is loaded, the following steps are performed:

  #. Reading structure of MED file. For these purpose, ``MEDCALC::MedDataManager`` CORBA service (which uses *MEDCoupling* library) is used.
  #. Creating structure of MED file (full path to MED file, name of a mesh, name and type (``ON_NODES`` or ``ON_CELLS``) of each field in MED file, and all timesteps for each field).
     ``MED_ORB::MED`` CORBA component is used for that, and each item of the data tree structure is represented via standard SALOME data object ``SObject``.

* *Presentation Parameters* panel allows managing parameters of presentations. This panel is implemented in the following way:

  #. Interface of *Presentation Parameters* panel is described via `Qt Designer tool <https://doc.qt.io/qt-5/qtdesigner-manual.html>`__;
     for details see ``WidgetPresentationParameters.ui`` (UI form) and ``WidgetPresentationParameters.cxx`` (C++ wrapping of UI) files.
     The UI file contains all needed GUI controls which are used to manage parameters of all FIELDS presentations, i.e. single UI file is used for all types of presentations.
     All these parameters are hidden by default; and parameters, required for specific presentation are shown at first usage, for example:
     
      .. code-block:: cpp

        void WidgetPresentationParameters::setSliceOrientation(MEDCALC::SliceOrientationType orient) {
           ...
           // Show the widgets:
           _ui.labelSliceOrient->show();
           _ui.comboBoxSliceOrient->show();
           ...
           ...
           ...    
        }

     Widget which contains all these parameters is displayed inside the dockable window which is created during the initialization of FIELDS module.

  #. Interaction between GUI controls and ``medcalc`` Python package is implemented via several helper classes. The hierarchy of these classes is presented on the following diagram:

      .. image:: images/image_helpers_hierarchy.png
         :align: center
    
    * ``MEDWidgetHelper`` - base abstract class to manage common parameters of all FIELDS presentations.
    * ``MEDWidgetHelperComponent`` - base abstract class to manage parameters of all presentations having only one component to change.

      These base classes contain one pure virtual method 

       .. code-block:: cpp
        
          virtual std::string getPythonTag() const = 0;

      This method must be implemented in all descendant classes; it should return the type of a presentation ("ScalarMap", "Slices" and etc.)

    * All other classes (such as ``MEDWidgetHelperScalarMap``, ``MEDWidgetHelperSlices`` etc.) are descendants of classes ``MEDWidgetHelper`` or ``MEDWidgetHelperComponent``.

    Helper classes send ``PresentationEvent`` event via emitting ``presentationUpdateSignal`` signal on any user action made in *Presentation Parameters* panel.

  #. ``PresentationController`` class is aimed for 
     
    * Converting ``PresentationEvent`` events to calls of ``medcalc`` Python package API, and executing related commands in the embedded Python console, for example:

      .. code-block:: cpp
       
          QStringList commands;
          ...
          if ( event->eventtype == PresentationEvent::EVENT_CHANGE_COMPONENT ) {
           
            // Get presentation type by calling getPythonTag() method
            std::string typ = getPresTypeFromWidgetHelper(event->presentationId); 

            // Convert 'PresentationEvent' to set of python commands
            commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(typ)).arg(event->presentationId);
            commands += QString("params.displayedComponent = '%1'").arg(QString::fromStdString(event->aString));
            commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(typ)).arg(event->presentationId);
          }
          ...
          _consoleDriver->exec(commands); 

    * Updating content of *Presentation Parameters* panel according to the presentation, selected in the *Object browser*.

* Standard SALOME *Displayer* mechanism is used to manage visiblity of FIELDS presentations in the ParaView viewer.
