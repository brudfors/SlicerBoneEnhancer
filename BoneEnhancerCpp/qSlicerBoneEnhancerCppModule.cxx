/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// BoneEnhancerCpp Logic includes
#include <vtkSlicerBoneEnhancerCppLogic.h>

// BoneEnhancerCpp includes
#include "qSlicerBoneEnhancerCppModule.h"
#include "qSlicerBoneEnhancerCppModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerBoneEnhancerCppModule, qSlicerBoneEnhancerCppModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerBoneEnhancerCppModulePrivate
{
public:
  qSlicerBoneEnhancerCppModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerBoneEnhancerCppModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerBoneEnhancerCppModulePrivate::qSlicerBoneEnhancerCppModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerBoneEnhancerCppModule methods

//-----------------------------------------------------------------------------
qSlicerBoneEnhancerCppModule::qSlicerBoneEnhancerCppModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerBoneEnhancerCppModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerBoneEnhancerCppModule::~qSlicerBoneEnhancerCppModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerBoneEnhancerCppModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerBoneEnhancerCppModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerBoneEnhancerCppModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerBoneEnhancerCppModule::icon() const
{
  return QIcon(":/Icons/BoneEnhancerCpp.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerBoneEnhancerCppModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerBoneEnhancerCppModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerBoneEnhancerCppModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerBoneEnhancerCppModule
::createWidgetRepresentation()
{
  return new qSlicerBoneEnhancerCppModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerBoneEnhancerCppModule::createLogic()
{
  return vtkSlicerBoneEnhancerCppLogic::New();
}
