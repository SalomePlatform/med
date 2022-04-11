.. _fields_arch_plot3d_implementation_example_page:

*******************************************
Implementation example: Plot3D presentation
*******************************************

This section describes how a FIELDS presentation is implemented using Plot3D presentation as an example.

General description of Plot3D presentation
------------------------------------------

Plot3D is a presentation of scalar values of meshes lying on a cutting plane.
The relief and coloring of the resulting presentation both correspond to the values applied to the cells of a mesh.

In general this presentation can be built using the following sequence of the ParaVis filters:

**MedReadear** -> **Slice** -> **Calculator** -> **WarpByScalar**

* **MedReadear** is used to read a MED file.

* **Slice** filter defines a cutting plane within the 3D mesh, on the basis of which the relief presentation is built. 
  This plane is a 2D mesh with values (scalar or vector) applied to the cells of a plane. If initial mesh is a 2D mesh, then **Slice** filter is unnecessary.

* **Calculator** filter converts vector values on cells to the scalar values. In case of scalar values **Calculator** filter is unnecessary.

* Finally, **WarpByScalar** filter is applied to the 2D mesh with scalar values on cells to build colored relief presentation.

Currently, only two parameters can be changed for this type of presentation, namely:

* *Position of a cutting plane* allows selecting the position of the cutting plane within the bounding box of a mesh (ranging from 0 to 1). This it is a property of **Slice** filter.
* *Cutting plane normal* allows specifying an orientation of the cut plane in 3D space. This is also a property of **Slice** filter. 

All other parameters are inherited from the parent presentation class.

GUI controls
------------

First step consists in implementing the corresponging GUI controls. This step includes the following actions:

* Add needed controls to UI using Qt *Designer*

  .. image:: images/image_ui_designer_prs_panel.png
     :align: center

* Implement C++ wrappings for newly created GUI controls and add corresponding signal-slot connections: see :ref:`code <fields_arch_widget_presentation_parameters_page>`.

* Add new types of events into :ref:`PresentationEvent.hxx<fields_arch_presentation_event_page>` file:

   .. code-block:: cpp

      ... 
      EVENT_VIEW_OBJECT_PLOT3D,
      ...
      EVENT_CHANGE_NORMAL,
      EVENT_CHANGE_PLANE_POS,

* Implement :ref:`MEDWidgetHelperPlot3D<fields_arch_medwidget_helper_plot3d_page>` class. This class sends events to ``PresentationController`` on each user action in the *Presentation Parameters*
  panel and updates a content of the *Presentation Parameters* panel in case if changes in a presentation were made, via the ``medcalc`` Python API.

* Modify ``PresentationController`` class. This class is used to add items into the *Presentations* menu and toolbar, and to translate GUI events into the ``medcalc`` Python API: see :ref:`code<fields_arch_presentation_controller_page>`.

Extend ``MEDCALC::MEDPresentationManager``
------------------------------------------

Next step consists in extending of the ``MEDCALC::MEDPresentationManager`` CORBA service; it assumes the following actions:

* :ref:`Extend<fields_arch_med_presentation_manager_page>` ``MEDPresentationManager`` IDL interface.

* :ref:`Implement<fields_arch_med_presentation_manager_impl_page>` methods, declared in the IDL interface.

Extend ``medcalc`` API
----------------------

Next step consists in :ref:`extending<fields_arch_med_presentation_py_page>` ``medcalc`` Python API, by adding functions to make and update Plot3D presentation and retrieve parameters of Plot3D presentations.

Implement pipeline
------------------

Final step consists in building the pipeline, described above, in the frames of the ``MEDPresentationPlot3D`` :ref:`class<fields_arch_med_presentation_plot3d_page>`.
