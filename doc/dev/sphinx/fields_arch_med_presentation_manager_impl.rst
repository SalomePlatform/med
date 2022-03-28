:orphan:

.. _fields_arch_med_presentation_manager_impl_page:

**********************************************
MEDCALC::MEDPresentationManager implementation
**********************************************

.. literalinclude:: ../../../src/MEDCalc/cmp/MEDPresentationManager_i.cxx
   :language: cpp
   :start-after: begin of makePlot3D
   :end-before: end of makePlot3D

Method ``_makePresentation`` is a private template method of ``MEDPresentationManager_i`` class; it calls the constructor of particular presentation class, in this case ``MEDPresentationPlot3D::MEDPresentationPlot3D``.

.. literalinclude:: ../../../src/MEDCalc/cmp/MEDPresentationManager_i.cxx
   :language: cpp
   :start-after: begin of getPlot3DParameters
   :end-before: end of getPlot3DParameters

Method ``_getParameters`` is a private template method of ``MEDPresentationManager_i`` class; it calls ``getParameters`` method of particular presentation instance, in this case ``MEDPresentationPlot3D::getParameters``.

.. literalinclude:: ../../../src/MEDCalc/cmp/MEDPresentationManager_i.cxx
   :language: cpp
   :start-after: begin of updatePlot3D
   :end-before: end of updatePlot3D

Method ``_updatePresentation`` is a private template method of ``MEDPresentationManager_i`` class, it calls ``updatePipeline`` of particular presentation instance, in this case ``MEDPresentationPlot3D::updatePipeline``.
