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

// .NAME vtkSlicerBoneEnhancerCppLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerBoneEnhancerCppLogic_h
#define __vtkSlicerBoneEnhancerCppLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerBoneEnhancerCppModuleLogicExport.h"

class vtkMRMLScalarVolumeNode;
class vtkDoubleArray;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_BONEENHANCERCPP_MODULE_LOGIC_EXPORT vtkSlicerBoneEnhancerCppLogic :
	public vtkSlicerModuleLogic
{
public:
  static vtkSlicerBoneEnhancerCppLogic *New();
  vtkTypeMacro(vtkSlicerBoneEnhancerCppLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Image processing connector method. */
  float ImageProcessingConnector(vtkMRMLScalarVolumeNode* inputVolumeNode, vtkMRMLScalarVolumeNode* outputVolumeNode, vtkDoubleArray* params, std::string algorithmName);

protected:
  vtkSlicerBoneEnhancerCppLogic();
  virtual ~vtkSlicerBoneEnhancerCppLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
  
private:
  vtkSlicerBoneEnhancerCppLogic(const vtkSlicerBoneEnhancerCppLogic&); // Not implemented
  void operator=(const vtkSlicerBoneEnhancerCppLogic&); // Not implemented

  /*! 2D convolution using Intel MKL. */
  void Conv2(const double* inputBuffer, const double* kernelBuffer, double* tempBuffer, double* outputBuffer, int nx, int ny, int kx, int ky);

  /*! Performs cropping of a 2D matrix stored in a buffer. */
  void ResizeMatrix(const double* inputBuffer, double* outputBuffer, int xClipping, int yClipping, int xInputSize, int yInputSize);

  /*! Returns the maximum value of an image stored in a buffer. */
  double GetMaxPixelValue(const double* buffer, int size);

  /*! Normalizes an image stored in a buffer. */
  void Normalize(double* buffer, int size, bool doInverse);

  /*! Extracts the bone surface probability from an US volume. */
  void Foroughi2007(double* inputBuffer, double* outputBuffer, double smoothingSigma, int transducerMargin, double shadowSigma, double boneThreshold, int blurredVSBLoG, int shadowVSIntensity, int nx, int ny, int nz);
};

#endif
