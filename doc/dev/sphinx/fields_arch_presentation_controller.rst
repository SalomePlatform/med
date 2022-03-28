:orphan:

.. _fields_arch_presentation_controller_page:

**********************
PresentationController
**********************

Create Plot3D menu and toolbar items
------------------------------------

.. literalinclude:: ../../../src/MEDCalc/gui/PresentationController.cxx
   :language: cpp
   :start-after: begin of plot3d gui items 
   :end-before: end of plot3d gui items

.. literalinclude:: ../../../src/MEDCalc/gui/PresentationController.cxx
   :language: cpp
   :start-after: begin of onVisualizePlot3D
   :end-before: end of onVisualizePlot3D

Translate GUI events into ``medcalc`` Python API
------------------------------------------------

.. literalinclude:: ../../../src/MEDCalc/gui/PresentationController.cxx
   :language: cpp
   :start-after: begin of plot3d prs creation
   :end-before: end of plot3d prs creation

.. literalinclude:: ../../../src/MEDCalc/gui/PresentationController.cxx
   :language: cpp
   :start-after: begin of plot3d prs update
   :end-before: end of plot3d prs update

