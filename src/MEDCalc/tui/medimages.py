#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2024  CEA, EDF
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

import sys
if sys.platform == "win32":
  import MEDCouplingCompat as MC
else:
  import medcoupling as MC

import MEDLoader as ML

from PIL import Image
from PIL import ImageOps
import numpy

class FieldBuilder:

    def image2med(self, imageFilepath, medFilepath=None):

        # Load the image file in a numpy array using PIL.
        img=Image.open(imageFilepath)
        imgbw=ImageOps.grayscale(img)
        # WARN: We keep only the grayscale. Maybe, it could be useful
        # to get the RGB scales each on one component of the field.

        # Creating a cartesian mesh with a grid of the size of the image
        # The sizes defined the number of pixel in a direction, then the
        # number of cells to create in the mesh in that direction.
        width,height=imgbw.size
        mesh=self.createMesh("grid_%sx%s"%(width,height),width,height)
        field=self.createField("imagefield",mesh,imgbw)

        # The MEDLoader can be used to save all the stuff in a med file. You
        # just have to specify the field and the MEDLoader will save the
        # underlying mesh.
        createFromScratch=True
        ML.WriteField(medFilepath,field,createFromScratch)

    def createMesh(self, meshname, sizeX, sizeY):
        """
        Creating a cartesian mesh with a grid of the size of the image.
        sizeX and sizeY should be respectively the width and height of the
        image.
        """
        # >>>
        # WARNING: remember the problem of tics and spaces. The data values
        # are considered as values defined on cells. With size values in a
        # direction, we have to create size+1 mesh nodes in that direction.
        # <<<

        # The mesh is created using MEDCoupling
        cmesh=MC.MEDCouplingCMesh.New();
        cmesh.setName(meshname)

        # We use an arbitrary step between cells (the value does not matter)
        stepX = 0.1
        nbNodesX = sizeX+1
        arrX = [float(i * stepX) for i in range(nbNodesX)]
        coordsX=MC.DataArrayDouble.New()
        coordsX.setValues(arrX,nbNodesX,1)

        # For the Y dimension, we have to reverse the coordinates (the
        # first pixel is at y=height and not at y=0).
        stepY = 0.1
        nbNodesY = sizeY+1
        lengthY = sizeY*stepY
        arrY=[float(lengthY - i * stepY) for i in range(nbNodesY)]
        coordsY=MC.DataArrayDouble.New()
        coordsY.setValues(arrY,nbNodesY,1)

        cmesh.setCoords(coordsX,coordsY)
        print("Imagem mesh dimension: %d"%cmesh.getSpaceDimension())

        # WARN: In the current state of development of MEDLoader, only
        # unstructured meshes are supported for writing function in med
        # files. We just have to convert the cartesian mesh in an unstructured
        # mesh before creating the field.
        umesh=cmesh.buildUnstructured();
        umesh.setName(cmesh.getName())

        return umesh

    def createField(self, fieldname, mesh, image):
        """
        Creating a scalar field on the mesh using image data
        """
        # Create the field using MEDCoupling
        field = MC.MEDCouplingFieldDouble.New(MC.ON_CELLS,MC.ONE_TIME);
        field.setName(fieldname);
        field.setMesh(mesh);
        # OPTIONAL: We set an arbitrary time step for test purpose
        field.setIteration(0);
        field.setOrder(0)

        imagedata=list(image.getdata())
        width,height=image.size
        nbCells = width*height
        dataArray=MC.DataArrayDouble.New();
        nbComponents=1 # For a scalar field

        dataArray.setValues(imagedata,nbCells,nbComponents)
        field.setArray(dataArray);

        return field

#
# ===================================================================
# use case functions
# ===================================================================
#

def getTestImagePath():
    import os
    FIELDS_ROOT_DIR=os.environ["FIELDS_ROOT_DIR"]
    RESDIR=os.path.join(FIELDS_ROOT_DIR, "share", "salome", "resources", "fields", "medcalc_testfiles")
    imgFileName="irm_test1.png"
    imgFilePath=os.path.join(RESDIR,imgFileName)
    return imgFilePath

def TEST_pil():
    imgFilePath = getTestImagePath()
    img=Image.open(imageFilepath)
    imgbw=ImageOps.grayscale(img)


def TEST_image2med():
    imgFilePath = getTestImagePath()
    builder = FieldBuilder()
    builder.image2med(imgFilePath,"image.med")

# ===================================================================
if __name__ == "__main__":
    TEST_pil()
    #TEST_image2med()
