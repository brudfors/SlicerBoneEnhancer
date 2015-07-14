import os
import unittest
from __main__ import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging
from vtk.util import numpy_support

#
# BoneEnhancerPy
#

class BoneEnhancerPy(ScriptedLoadableModule):

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "BoneEnhancer" # TODO make this more human readable by adding spaces
    self.parent.categories = ["Filtering"]
    self.parent.dependencies = []
    self.parent.contributors = ["Mikael Brudfors (UC3M)"] # replace with "Firstname Lastname (Organization)"
    self.parent.helpText = """
    This is an example of scripted loadable module bundled in an extension.
    It performs a simple thresholding on the input volume and optionally captures a screenshot.
    """
    self.parent.acknowledgementText = """
    This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
    and Steve Pieper, Isomics, Inc. and was partially funded by NIH grant 3P41RR013218-12S1.
""" # replace with organization, grant and thanks.

############################################################ BoneEnhancerPyWidget
class BoneEnhancerPyWidget(ScriptedLoadableModuleWidget):
  """Uses ScriptedLoadableModuleWidget base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """
 
  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)
    
    ############################################################ Define algorithms
    self.foroughi2007 = AlgorithmParams(("Foroughi2007 (with minor modifications)", "Runs Foroughi's algorithm on the input US volume.", "Extract Bone Features"),
              {"Smoothing Sigma" : (1, 1, 1, 10, 5.0, "Smoothing Sigma ToolTip"),
               "Transducer Margin" : (0, 1, 0, 100, 60, "Transducer Margin ToolTip"),
               "Shadow Sigma" : (1, 1, 1, 10, 6.0, "Shadow Sigma ToolTip"),
               "Bone Threshold" : (1, 0.1, 0, 1, 0.4, "Bone Threshold ToolTip"),
               "Blurred vs. BLoG" : (0, 1, 1, 10, 3, "Blurred vs. BLoG ToolTip"),
               "Shadow vs. Intensity" : (0, 1, 1, 10, 5, "Shadow vs. Intensity ToolTip")})

    ############################################################ BoneEnhancer
    boneEnhancerCollapsibleButton = ctk.ctkCollapsibleButton()
    boneEnhancerCollapsibleButton.text = "BoneEnhancer"
    self.layout.addWidget(boneEnhancerCollapsibleButton)
    boneEnhancerFormLayout = qt.QFormLayout(boneEnhancerCollapsibleButton)

    self.ultrasoundImageSelector = slicer.qMRMLNodeComboBox()
    self.ultrasoundImageSelector.nodeTypes = ( ("vtkMRMLScalarVolumeNode"), "" )
    self.ultrasoundImageSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 0 )
    self.ultrasoundImageSelector.selectNodeUponCreation = True
    self.ultrasoundImageSelector.addEnabled = False
    self.ultrasoundImageSelector.removeEnabled = False
    self.ultrasoundImageSelector.noneEnabled = False
    self.ultrasoundImageSelector.showHidden = False
    self.ultrasoundImageSelector.showChildNodeTypes = False
    self.ultrasoundImageSelector.setMRMLScene( slicer.mrmlScene )
    self.ultrasoundImageSelector.setToolTip( "Pick the input to the algorithm." )
    boneEnhancerFormLayout.addRow("Ultrasound Image: ", self.ultrasoundImageSelector)

    # Select algorithm
    self.algorithmGroupBox = ctk.ctkCollapsibleGroupBox()
    self.algorithmGroupBox.setTitle("Select Algorithm")
    algorithmFormLayout = qt.QFormLayout(self.algorithmGroupBox)
    boneEnhancerFormLayout.addRow(self.algorithmGroupBox)

    algorithmFormLayout.addRow(self.foroughi2007.GetRadioButton())
    
    # Parameters
    self.parametersGroupBox = ctk.ctkCollapsibleGroupBox()
    self.parametersGroupBox.setTitle("Parameters")
    parametersFormLayout = qt.QFormLayout(self.parametersGroupBox)
    boneEnhancerFormLayout.addRow(self.parametersGroupBox)
             
    for paramKey in self.foroughi2007.GetParamKeys():
      parametersFormLayout.addRow(self.foroughi2007.GetLabel(paramKey), self.foroughi2007.GetSlider(paramKey))            
    # Runtime
    self.runtimeGroupBox = ctk.ctkCollapsibleGroupBox()
    self.runtimeGroupBox.setTitle("Runtime")
    runtimeFormLayout = qt.QFormLayout(self.runtimeGroupBox)
    boneEnhancerFormLayout.addRow(self.runtimeGroupBox)
    
    self.runtimeLabel = qt.QLabel()
    self.runtimeLabel.setText("... s.")
    self.runtimeLabel.setWordWrap(True)
    self.runtimeLabel.setStyleSheet("QLabel { background-color : black; \
                                           color : #66FF00; \
                                           height : 60px; \
                                           border-style: outset; \
                                           border-width: 5px; \
                                           border-radius: 10px; \
                                           font: bold 14px; \
                                           padding: 0px;\
                                           font-family : SimSun; \
                                           qproperty-alignment: AlignCenter}")
    runtimeFormLayout.addRow(self.runtimeLabel)
    
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Run the algorithm."
    self.applyButton.enabled = False
    self.applyButton.checkable = True
    boneEnhancerFormLayout.addRow(self.applyButton)
       
    ############################################################ Connections
    self.applyButton.connect('clicked(bool)', self.onApplyButton)
    self.ultrasoundImageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    
    self.layout.addStretch(1)
    self.onSelect()
    self.ModuleLayoutID = -1
    
    self.setModuleLayout()
    
  def onSelect(self):
    self.applyButton.enabled = self.ultrasoundImageSelector.currentNode()
  
  def onApplyButton(self):
    logic = BoneEnhancerPyLogic()
    
    boneEnhancedImage = slicer.util.getNode('BoneEnhancedImage')    
    if not boneEnhancedImage:
      boneEnhancedImage = logic.createVolumeNode(self.ultrasoundImageSelector.currentNode(), 'BoneEnhancedImage')  
      
    if self.ultrasoundImageSelector.currentNode().GetImageData().GetScalarType() is not vtk.VTK_DOUBLE:
      logging.warning('Input image scalar type not double! Casting to double.')
      logic.castVolumeNodeToDouble(self.ultrasoundImageSelector.currentNode())   
      
    logic.calculateBoneEnhancedImage(self.ultrasoundImageSelector.currentNode(), boneEnhancedImage, self.foroughi2007.GetParamsVTK(), self.runtimeLabel, self.applyButton)

    self.updateSliceViews(boneEnhancedImage, self.ultrasoundImageSelector.currentNode())
    
  def updateSliceViews(self, boneEnhancedImage, USVolumeNode):
    layoutManager = slicer.app.layoutManager()
    
    # Update bone enhanced image
    for name in ['RedBone', 'YellowBone', 'GreenBone']:      
      sliceWidget = layoutManager.sliceWidget(name)    
      sliceLogic = sliceWidget.sliceLogic()
      sliceLogic.GetSliceCompositeNode().SetBackgroundVolumeID(boneEnhancedImage.GetID()) 
      sliceLogic.FitSliceToAll() 
    
    # Update ultrasound image
    for name in ['Red', 'Yellow', 'Green']:      
      sliceWidget = layoutManager.sliceWidget(name)    
      sliceLogic = sliceWidget.sliceLogic()
      sliceLogic.GetSliceCompositeNode().SetBackgroundVolumeID(USVolumeNode.GetID()) 
      sliceLogic.FitSliceToAll() 
      
  def setModuleLayout(self):
    layoutManager = slicer.app.layoutManager()
    if self.ModuleLayoutID == -1:
      ModuleLayout = ("<layout type=\"vertical\">"
                   " <item>"
                   "  <layout type=\"horizontal\">"
                   "   <item>"
                   "    <view class=\"vtkMRMLSliceNode\" singletontag=\"Red\">"
                   "     <property name=\"orientation\" action=\"default\">Axial</property>"
                   "     <property name=\"viewlabel\" action=\"default\">R</property>"
                   "     <property name=\"viewcolor\" action=\"default\">#F34A33</property>"
                   "    </view>"
                   "   </item>"
                   "   <item>"
                   "    <view class=\"vtkMRMLSliceNode\" singletontag=\"Yellow\">"
                   "     <property name=\"orientation\" action=\"default\">Sagittal</property>"
                   "     <property name=\"viewlabel\" action=\"default\">Y</property>"
                   "     <property name=\"viewcolor\" action=\"default\">#EDD54C</property>"
                   "    </view>"
                   "   </item>"      
                   "   <item>"
                   "    <view class=\"vtkMRMLSliceNode\" singletontag=\"Green\">"
                   "     <property name=\"orientation\" action=\"default\">Coronal</property>"
                   "     <property name=\"viewlabel\" action=\"default\">G</property>"
                   "     <property name=\"viewcolor\" action=\"default\">#6EB04B</property>"
                   "    </view>"
                   "   </item>"
                   "  </layout>"
                   " </item>"
                   " <item>"
                   "  <layout type=\"horizontal\">"
                   "   <item>"
                   "    <view class=\"vtkMRMLSliceNode\" singletontag=\"RedBone\">"
                   "     <property name=\"orientation\" action=\"default\">Axial</property>"
                   "     <property name=\"viewlabel\" action=\"default\">R</property>"
                   "     <property name=\"viewcolor\" action=\"default\">#F34A33</property>"
                   "    </view>"
                   "   </item>"
                   "   <item>"
                   "    <view class=\"vtkMRMLSliceNode\" singletontag=\"YellowBone\">"
                   "     <property name=\"orientation\" action=\"default\">Sagittal</property>"
                   "     <property name=\"viewlabel\" action=\"default\">Y</property>"
                   "     <property name=\"viewcolor\" action=\"default\">#EDD54C</property>"
                   "    </view>"
                   "   </item>"      
                   "   <item>"
                   "    <view class=\"vtkMRMLSliceNode\" singletontag=\"GreenBone\">"
                   "     <property name=\"orientation\" action=\"default\">Coronal</property>"
                   "     <property name=\"viewlabel\" action=\"default\">G</property>"
                   "     <property name=\"viewcolor\" action=\"default\">#6EB04B</property>"
                   "    </view>"
                   "   </item>"
                   "  </layout>"
                   " </item>"                
                   " </layout>")  
      self.ModuleLayoutID = 501
      layoutManager.layoutLogic().GetLayoutNode().AddLayoutDescription(self.ModuleLayoutID, ModuleLayout)
      
    layoutManager.setLayout(self.ModuleLayoutID)

############################################################ BoneEnhancerPyLogic
class BoneEnhancerPyLogic(ScriptedLoadableModuleLogic):

  def calculateBoneEnhancedImage(self, inputVolumeNode, boneEnhancedImage, paramsVTK, runtimeLabel=None, applyButton=None):     
    logging.info('Extracting BSP started')
    runtime = slicer.modules.boneenhancercpp.logic().ImageProcessingConnector(inputVolumeNode, boneEnhancedImage, paramsVTK, 'Foroughi2007')
    runtime = str(round(runtime, 3)) 
    message = runtime + ' s.'
    if runtimeLabel:
      runtimeLabel.setText(message)
    logging.info('Extracting BSP completed (' + message + ')')
    
    boneEnhancedImage.Modified()
    if applyButton:
      applyButton.checked = False
    
    return True
    
  def createVolumeNode(self, inputVolumeNode, name):
    inputImageData = inputVolumeNode.GetImageData()
    imageSize=inputImageData.GetDimensions()
    imageSpacing=inputVolumeNode.GetSpacing()
    imageOrigin=inputVolumeNode.GetOrigin()
    voxelType=vtk.VTK_DOUBLE 
    # Create an empty image volume
    imageData=vtk.vtkImageData()
    imageData.SetDimensions(imageSize)
    imageData.AllocateScalars(voxelType, 1)    
    # Create volume node
    scene = slicer.mrmlScene
    volumeNode=slicer.vtkMRMLScalarVolumeNode()
    volumeNode.SetSpacing(imageSpacing)
    volumeNode.SetOrigin(imageOrigin)
    volumeNode.SetAndObserveImageData(imageData)
    volumeNode.SetName(scene.GenerateUniqueName(name))
    # Add volume to scene
    slicer.mrmlScene.AddNode(volumeNode)
    
    return volumeNode

  def castVolumeNodeToDouble(self, volumeNode):
    castFilter = vtk.vtkImageCast()
    castFilter.SetInputData(volumeNode.GetImageData())
    castFilter.SetOutputScalarTypeToDouble()
    castFilter.Update()
    
    volumeNode.SetAndObserveImageData(castFilter.GetOutput())
        
    return True
    
############################################################ AlgorithmParams
# Defines parameters for an algorithm through a ctkSliderWidget, a QRadioButton and a QLabel.
class AlgorithmParams:

  def __init__(self, algo, params):
    self.name = algo[0]
    self.paramKeys = sorted(params.keys(), key=str.lower) # Sort keys alphabetically
    self.CreateRadioButton(algo[1])    
    self.runPushButtonText = algo[2]
    self.CreateSliders(params)
    self.CreateLabels(params)
  def CreateRadioButton(self, toolTip):
    self.radioButton = qt.QRadioButton()
    self.radioButton.text = self.name
    self.radioButton.setToolTip(toolTip)
  def GetRunPushButtonText(self):
    return self.runPushButtonText
  def GetRadioButton(self):
    return self.radioButton
  def CreateSliders(self, params):
    self.sliders = {}
    for param in params:
      self.sliders[param] = ctk.ctkSliderWidget()
      self.sliders[param].setDecimals(params[param][0])
      self.sliders[param].singleStep = params[param][1]
      self.sliders[param].minimum =  params[param][2]
      self.sliders[param].maximum = params[param][3]
      self.sliders[param].value = params[param][4]
      self.sliders[param].setToolTip(params[param][5])
  def GetSlider(self, param):
    return self.sliders[param]
  def GetSliderValue(self, param):
    return float(self.sliders[param].value) # Return a float for compatibility with, e.g. C++
  def CreateLabels(self, params):
    self.labels = {}
    for param in params:
      self.labels[param] = qt.QLabel()
      self.labels[param].setText(param) 
  def GetLabel(self, param):
    return self.labels[param]
  def GetParamKeys(self):
    return self.paramKeys
  def GetParamsVTK(self):
    params = []
    for param in self.paramKeys:
      params.append(self.sliders[param].value)    
    paramsVtkDoubleArray = numpy_support.numpy_to_vtk(num_array=params, deep=True, array_type=vtk.VTK_DOUBLE)
    return paramsVtkDoubleArray

############################################################ BoneEnhancerPyTest
class BoneEnhancerPyTest(ScriptedLoadableModuleTest):

  def setUp(self):
    slicer.mrmlScene.Clear(0)
    layoutManager = slicer.app.layoutManager()
    layoutManager.setLayout(1)
    
  def runTest(self):
    self.setUp()
    self.test_BSP()

  def test_BSP(self):
    self.delayDisplay("Testing BSP")
    
    import urllib
    downloads = (('http://slicer.kitware.com/midas3/download/item/205865/3DUS_Lumbar.nrrd', 
                 '3DUS_Lumbar.nrrd', slicer.util.loadVolume),)        

    for url,name,loader in downloads:
      filePath = slicer.app.temporaryPath + '/' + name
      if not os.path.exists(filePath) or os.stat(filePath).st_size == 0:
        logging.info('Requesting download %s from %s...\n' % (name, url))
        urllib.urlretrieve(url, filePath)
      if loader:
        logging.info('Loading %s...' % (name,))
        loader(filePath)
      else:
        self.delayDisplay('Test failed, no volume loader')
        return
    self.delayDisplay('Finished with download and loading')
    volumeNode = slicer.util.getNode(pattern="3DUS_Lumbar")
 
    params = AlgorithmParams(("Foroughi [1]", "Runs Foroughi's algorithm on the input US volume.", "Extract Bone Features"),
              {"Smoothing Sigma" : (1, 1, 1, 10, 3, "Smoothing Sigma ToolTip"),
               "Transducer Margin" : (0, 1, 0, 100, 15, "Transducer Margin ToolTip"),
               "Shadow Sigma" : (1, 1, 1, 10, 2, "Shadow Sigma ToolTip"),
               "Bone Threshold" : (1, 0.1, 0, 1, 0.3, "Bone Threshold ToolTip"),
               "Blurred vs. BLoG" : (0, 1, 1, 10, 1, "Blurred vs. BLoG ToolTip"),
               "Shadow vs. Intensity" : (0, 1, 1, 10, 5, "Shadow vs. Intensity ToolTip")})
               
    logic = BoneEnhancerPyLogic()
    self.assertTrue(logic.createVolumeNode(volumeNode, 'BoneEnhancedImage'))    
    self.assertTrue(logic.calculateBoneEnhancedImage(volumeNode, slicer.util.getNode('BoneEnhancedImage'), params.GetParamsVTK()))
    self.delayDisplay('Testing BSP passed!')
