.. _fields_arch_medcacl_py_page:

*****************************
FIELDS medcalc python package
*****************************

The ``medcalc`` python package contains several modules, but only three of them are used directly to manage FIELDS presentations:

* ``medcorba.py`` - performs initialization of the :ref:`FIELDS module's CORBA stuff<fields_arch_corba_services_page>`.

* ``medevents.py`` - implements mechanism of notifications from FIELDS ``medcalc`` python package to :ref:`FIELDS GUI<fields_arch_gui_page>`.
  This mechanism allows synchronizing changes made in Python, with the GUI; for example, when a parameter is changed via Python command, 
  the corresponding GUI item is updated in the *Presenation Parameters* panel.

* ``medpresentation.py`` - provides functionality for creating and updating FIELDS presentations.
