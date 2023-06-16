# Copyright (C) 2011-2023  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : Guillaume Boulant (EDF)

import medcalc
import MEDCALC
import SALOME

from medcalc.medcorba import factory
dataManager = factory.getDataManager()
calculator  = factory.getCalculator()

# Some helper functions to deal with the fields and meshes
import sys
if sys.platform == "win32":
  import MEDCouplingCompat as medcoupling
else:
  import medcoupling
__mapTypeOfFieldLabel = {
  medcoupling.ON_CELLS:    "ON_CELLS",
  medcoupling.ON_NODES:    "ON_NODES",
  medcoupling.ON_GAUSS_PT: "ON_GAUSS_PT",
  medcoupling.ON_GAUSS_NE: "ON_GAUSS_NE"
  }
#
def _typeOfFieldLabel(typeOfField):
  # A field name could identify several MEDCoupling fields, that
  # differ by their spatial discretization on the mesh (values on
  # cells, values on nodes, ...). This spatial discretization is
  # specified (at least) by the TypeOfField that is an integer value
  # in this list:
  # 0 = ON_CELLS
  # 1 = ON_NODES
  # 2 = ON_GAUSS_PT
  # 3 = ON_GAUSS_NE
  try:
    return __mapTypeOfFieldLabel[typeOfField]
  except IndexError as e:
    return "UNKNOWN"
#

def newFieldProxy(fieldHandlerId):
  '''
  This creates a new FieldProxy wrapping the field whose
  fieldHandlerId is passed in argument. The function requests the
  DataManager to get the fieldHandler from its id.
  '''
  fieldHandler = dataManager.getFieldHandler(fieldHandlerId)
  return FieldProxy(fieldHandler)
#

# This define the map between attributes of a FieldProxy and those of
# the associated FieldHandler
PROXY_ATTRIBUTES_MAP = {"id":None,
                        "fieldseriesId":None,
                        "fieldname":"name",
                        "meshname":None,
                        "meshid":None,
                        "type":None,
                        "iteration":"iteration",
                        "order":"order",
                        "source":"source"}

class FieldProxy:
  """
  This object is a proxy to manipulate a remote MEDCoupling field
  from within the SALOME python interpreter. Remote means that the
  MEDCoupling field is in the SALOME container and not in the
  client. See UserGuide class for detailed documentation of what can
  be done with a field proxy.
  """
  def __init__( self, fieldHandler ):
    """
    This defines the wrapping on the field specified by its
    fieldHandler id.
    """
    self.__fieldHandler = fieldHandler
    self.__restriction  = None
    from salome_utils import verbose
    if verbose():
      print(self.__repr__())
  #
  def __getattr__(self, name ):
    """
    This method realizes the read proxy pattern toward the field
    handler.
    """
    # WRN: Note that the modification of this function can lead to
    # coercion problem. Modify this function with extreme care.
    return getattr( self.__fieldHandler, name )
  #
  def __setattr__(self, name, value):
    """
    This method realizes the write proxy pattern toward the field
    handler. Only some attributes are writable. The list is
    specified in the PROXY_ATTRIBUTES_MAP table.
    """
    if name in list(PROXY_ATTRIBUTES_MAP.keys()):
      if PROXY_ATTRIBUTES_MAP[name] is not None:
        medcalc.wrn("The modification of this attribute can't be done that way")
        msg="Use f.update(%s=\"%s\") instead to ensure synchronisation of data."
        medcalc.inf(msg%(PROXY_ATTRIBUTES_MAP[name],value))
      else:
        medcalc.err("The modification of the attribute %s is not possible"%name)
    else:
      self.__dict__[name] = value
  #
  def __repr__(self):
    """
    Return a string containing a printable representation of this
    object (what is displayed when typing the variable name and
    carriage return).
    """
    # We need first to update the handler because some data can
    # have changed (the meshid for example in case of change of
    # underlying mesh).
    # __GBO__ : TODO check the performance
    self.__fieldHandler = dataManager.getFieldHandler(self.id)
    text = "field name (id)\t = %s (%s)\n"%(self.fieldname, self.id)
    text+= "mesh name (id) \t = %s (%s)\n"%(self.meshname,self.meshid)
    text+= "discretization \t = %s\n"%_typeOfFieldLabel(self.type)
    text+= "(iter, order)  \t = (%s,%s)\n"%(str(self.iteration),str(self.order))
    text+= "data source    \t = %s"%self.source
    return text
  #
  def __str__(self):
    """
    This is what is displayed when you type 'print(myField)'. Note
    that this function prints the values of the field and then you
    must be aware that a huge amount of data could be
    displayed. Moreover, it means that this operation triggers the
    loading of the associated MEDCouplingFied data in the SALOME
    container.
    """
    text = dataManager.getFieldRepresentation(self.id)
    return text
  #
  def __add__(self, operande):
    """
    This makes the addition of two fields or the addition of a
    scalar to a field. It depends weither the operande is a
    FieldProxy or a simple scalar numerical value.
    """
    # The medcalc calculator could raise exceptions coming from
    # MEDCoupling. Note that the fieldproxy instances are used
    # from within the python console, and for ergonomic reason, we
    # choose to not raise the possible exceptions to the console
    # by a clear message. Keep this in mind for unit test. You
    # have to test the return value, which should not be
    # null. This principle is applied for all operations.
    try:
      if isinstance(operande, FieldProxy):
        # The operande is an other field
        medcalc.inf("Addition of  %s and %s"%(self.fieldname, operande.fieldname))
        rfieldHandler = calculator.add(self.__fieldHandler, operande.__fieldHandler)
      else:
        # The operande is a scalar numerical value that must be
        # considered as an offset in a linear transformation
        factor = 1
        offset = operande
        medcalc.inf("Application of the offset %s to %s" % (offset, self.fieldname))
        rfieldHandler = calculator.lin(self.__fieldHandler, factor, offset)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __radd__(self, operande):
    """
    The user typed 'operande+self', we replace by 'self+operande'
    to automatically activate the __add__ method of fieldpoxy.
    """
    return self+operande
  #
  def __iadd__(self, operande):
    """
    These methods implements the augmented arithmetic assignments (+=)
    """
    medcalc.wrn("NOT IMPLEMENTED YET")
  #
  def __sub__(self,operande):
    """
    This makes the substraction of two fields or the substraction
    of a scalar to a field. It depends weither the operande is a
    FieldProxy or a simple scalar numerical value.
    """
    try:
      if isinstance(operande, FieldProxy):
        # The operande is an other field
        medcalc.inf("Substraction of %s by %s"%(self.fieldname, operande.fieldname))
        rfieldHandler = calculator.sub(self.__fieldHandler, operande.__fieldHandler)
      else:
        # The operande is a scalar numerical value that must be
        # considered as an offset in a linear transformation
        factor = 1
        offset = -operande
        medcalc.inf("Application of the offset %s to %s" % (offset, self.fieldname))
        rfieldHandler = calculator.lin(self.__fieldHandler, factor, offset)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __rsub__(self, operande):
    """
    The user typed 'operande-self' where operande is not a field
    proxy. This function process the situation.
    """
    # The operande is a numerical value (because otherwise, the
    # "sub" method would have been called instead). We may apply
    # the command '(self-operande)*(-1)' to activate the __sub__
    # method of fieldpoxy.
    #
    #return (self-operande)*(-1)
    #
    # We prefer to apply a linear transformation because it can be
    # done in one single request to the med calculator.

    factor = -1
    offset = operande
    medcalc.inf("Linear transformation %s%s*%s" % (offset, factor, self.fieldname))
    try:
      rfieldHandler = calculator.lin(self.__fieldHandler, factor, offset)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __mul__(self, operande):
    """
    This makes the multiplication of two fields or the
    multiplication of a scalar to a field. It depends weither the
    operande is a FieldProxy or a simple scalar numerical value.
    """
    try:
      if isinstance(operande, FieldProxy):
        # The operande is an other field
        medcalc.inf("Multiplication of %s by %s"%(self.fieldname, operande.fieldname))
        rfieldHandler = calculator.mul(self.__fieldHandler, operande.__fieldHandler)
      else:
        # The operande is a scalar numerical value that must be
        # considered as an offset in a linear transformation
        factor = operande
        offset = 0
        medcalc.inf("Scaling %s by factor %s" % (self.fieldname, factor))
        rfieldHandler = calculator.lin(self.__fieldHandler, factor, offset)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __rmul__(self, operande):
    """
    The user typed 'operande*self', we want to execute
    'self*operande' to activate the __mul__ method of fieldpoxy.
    """
    return self*operande
  #
  def __div__(self, operande):
    """
    This makes the division of two fields or the division of field
    by a scalar. It depends weither the operande is a FieldProxy
    or a simple scalar numerical value.
    """
    try:
      if isinstance(operande, FieldProxy):
        # The operande is an other field
        medcalc.inf("Division of %s by %s"%(self.fieldname, operande.fieldname))
        rfieldHandler = calculator.div(self.__fieldHandler, operande.__fieldHandler)
      else:
        # The operande is a scalar numerical value that must be
        # considered as an offset in a linear transformation
        factor = 1./operande
        offset = 0
        medcalc.inf("Scaling %s by factor 1/%s" % (self.fieldname, operande))
        rfieldHandler = calculator.lin(self.__fieldHandler, factor, offset)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __rdiv__(self, operande):
    """
    The user typed 'operande/self', we want to execute for each
    value of the field the operation 'operande/value'.
    """
    medcalc.inf("Division of %s by %s" % (operande, self.fieldname))
    function  = "%s/u"%operande
    nbResComp = MEDCALC.NBCOMP_DEFAULT
    try:
      rfieldHandler = calculator.fct(self.__fieldHandler,function,nbResComp)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __pow__(self, power):
    """
    This compute the power of the field to the specified value.
    """
    function  = "abs(u)^%s"%power
    return self.ope(function,duplicate=True)
  #
  def __abs__(self):
    """
    This compute the absolute value of the field. We use here
    """
    return self.ope(function="abs(u)",duplicate=True)
  #
  def __neg__(self):
    """
    This computes the negative of this field (when you type -f)
    """
    return -1*self
  #
  def dup(self):
    """
    This creates a duplicate of the field. The whole data are
    duplicated.
    """
    medcalc.inf("Duplication of %s"%self.fieldname)
    try:
      rfieldHandler = calculator.dup(self.__fieldHandler)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def ope(self, function, duplicate=True):
    """
    This can be used to apply a transformation function to this
    field. The transformation is specified using a literal
    equation given as a string where u stands for the field.
    """
    # _GBO_ TO BE IMPLEMENTED: the case where duplicate = False
    # must modify the object itself and not create a new field
    medcalc.inf("Operate the equation \"%s\" to %s"%(function,self.fieldname))
    try:
      rfieldHandler = calculator.fct(self.__fieldHandler,
                                     function,
                                     MEDCALC.NBCOMP_DEFAULT)
    except SALOME.SALOME_Exception as ex:
      medcalc.err(ex.details.text)
      return None

    return FieldProxy(rfieldHandler)
  #
  def __call__(self, restriction=None):
    """
    This could be used to return a fieldproxy binded on the same
    fieldHandler than self, but with options that restrict the
    usage to a domain specified by the given arguments (restricted
    to a component, to a part of the mesh, ...).
    """
    medcalc.wrn("Not implemented yet. Return the field itself")
    self.__restriction = restriction
    return self
  #
  def update(self,name=None,iteration=None,order=None,source=None):
    """
    This function can be used to update the meta-data associated
    to this field. It can modify the name, the iteration, the
    order and the source.
    """
    if name is None:
      name = self.fieldname
    if iteration is None:
      iteration = self.iteration
    if order is None:
      order = self.order
    if source is None:
      source = self.source

    dataManager.updateFieldMetadata(self.id,name,iteration,order,source)
    self.__fieldHandler.fieldname = name
    self.__fieldHandler.iteration = iteration
    self.__fieldHandler.order     = order
    self.__fieldHandler.source    = source
    # WARN: Note that you have to update directly the fieldHandler
    # object because of the presence of the method __setattr__
    # that make the proxy to this update method

    # Finally, we have to notify the GUI for update of field prestations
    #self.__notifyGui_update()
    from medcalc.medevents import notifyGui_updateField
    notifyGui_updateField(self.id)

    # Print for visual control
    from salome_utils import verbose
    if verbose():
      print(self.__repr__())
  #
#

#
# ===================================================================
# unit test functions
# ===================================================================
#

def TEST_typeOfFieldLabel():
  print(typeOfFieldLabel(0))
  print(typeOfFieldLabel(5))
#

# ===================================================================
if __name__ == "__main__":
  TEST_typeOfFieldLabel()
#
