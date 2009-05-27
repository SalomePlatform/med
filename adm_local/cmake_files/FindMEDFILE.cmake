
SET(MED2HOME $ENV{MED2HOME})
IF(NOT MED2HOME)
  FIND_PROGRAM(MDUMP mdump)
  SET(MED2HOME ${MDUMP})
  GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
  GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
ENDIF(NOT MED2HOME)

FIND_PATH(MED2_INCLUDE_DIR med.h ${MED2HOME}/include)
FIND_LIBRARY(MEDC_LIB medC ${MED2HOME}/lib)
FIND_LIBRARY(MED_LIB med ${MED2HOME}/lib)
FIND_LIBRARY(MEDIMPORTCXX_LIB medimportcxx ${MED2HOME}/lib)

SET(MED2_INCLUDES ${HDF5_INCLUDES} -I${MED2_INCLUDE_DIR} -D${MACHINE})
SET(MED2_LIBS ${MEDC_LIB} ${MED_LIB} ${MEDIMPORTCXX_LIB})
