:orphan:

.. _fields_arch_med_presentation_py_page:

******************************
``medpresentation`` Python API
******************************

.. literalinclude:: ../../../src/MEDCalc/tui/medpresentation.py
   :language: python
   :start-after: begin of MakePlot3D
   :end-before: end of MakePlot3D

.. literalinclude:: ../../../src/MEDCalc/tui/medpresentation.py
   :language: python
   :start-after: begin of GetPlot3DParameters
   :end-before: end of GetPlot3DParameters

Method ``__GetGENERICParameters`` is an internal helper method that calls ``getPlot3DParameters`` function of ``MEDPresentationManager`` CORBA service.

.. literalinclude:: ../../../src/MEDCalc/tui/medpresentation.py
   :language: python
   :start-after: begin of UpdatePlot3D
   :end-before: end of UpdatePlot3D

Method ``__GetGENERICParameters`` is an internal helper method that calls ``updatePlot3D`` function of ``MEDPresentationManager`` CORBA service.
