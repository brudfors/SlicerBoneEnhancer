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

// BoneEnhancerCpp Logic includes
#include "vtkSlicerBoneEnhancerCppLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLSelectionNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkDoubleArray.h>
#include <vtkTimerLog.h>

// STD includes
#include <cassert>

// Slicer includes
#include <vtkSlicerVolumesLogic.h>

// Other includes
#include "mkl.h"
#ifdef NDEBUG
#include <omp.h>
#endif

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerBoneEnhancerCppLogic);

//----------------------------------------------------------------------------
vtkSlicerBoneEnhancerCppLogic::vtkSlicerBoneEnhancerCppLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerBoneEnhancerCppLogic::~vtkSlicerBoneEnhancerCppLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
	vtkNew<vtkIntArray> events;
	events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
	events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
	events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
	this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic::RegisterNodes()
{
	assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic::UpdateFromMRMLScene()
{
	assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
// An image processing connector method, which takes both an input, and a output volume node from 3D Slicer,
// an array of parameters, and the name of the algorithm to execute.
float vtkSlicerBoneEnhancerCppLogic
::ImageProcessingConnector(vtkMRMLScalarVolumeNode* inputVolumeNode, vtkMRMLScalarVolumeNode* outputVolumeNode, vtkDoubleArray* params, std::string algorithmName)
{
  int* dims = inputVolumeNode->GetImageData()->GetDimensions();
  int nx = dims[0];
  int ny = dims[1];
  int nz = dims[2];

  float runtime = 0.0;
  if (algorithmName == "Foroughi2007")
  {
    // Define necessary parameters
    int blurredVSBLoG = params->GetValue(0);
    double boneThreshold = params->GetValue(1);
    double shadowSigma = params->GetValue(2);
    int shadowVSIntensity = params->GetValue(3);
    double smoothingSigma = params->GetValue(4);
    int transducerMargin = params->GetValue(5);		

    vtkSmartPointer<vtkTimerLog> timer = vtkSmartPointer<vtkTimerLog>::New();
    timer->StartTimer();

    // Extract BSP from input volume (through pointer) and put result into the output volume's buffer
    this->Foroughi2007(static_cast<double*>(inputVolumeNode->GetImageData()->GetScalarPointer(0,0,0)), static_cast<double*>(outputVolumeNode->GetImageData()->GetScalarPointer(0,0,0)), smoothingSigma, transducerMargin, shadowSigma, boneThreshold, blurredVSBLoG, shadowVSIntensity, nx, ny, nz);

    timer->StopTimer();
    runtime = timer->GetElapsedTime();
  }
  else
  {
    std::cout << "No algorithm defined!" << std::endl;
  }

  return runtime;
}

//-----------------------------------------------------------------------------
// Implementation of: Foroughi, P., et al. (2007) Ultrasound bone segmentation using dynamic programming. IEEE Ultrason Symp 13(4):2523–2526
// (with some modifications), which extracts the bone surface probability (BSP) from an US volume.
// By: Mikael Brudfors, March 2014
void vtkSlicerBoneEnhancerCppLogic
::Foroughi2007(double* inputBuffer, double* outputBuffer, double smoothingSigma, int transducerMargin, double shadowSigma, double boneThreshold, int blurredVSBLoG, int shadowVSIntensity, int nx, int ny, int nz)
{
  int sliceSize = nx * ny;
  int volumeSize = nx * ny * nz;
  int nG = floor(smoothingSigma*3)*2+1;

  // Allocating memory for matrices aligned on 64-byte boundary for better performance
  double* gaussianBuffer = (double*)mkl_malloc( sliceSize * sizeof( double ), 64 );
  double* laplacianOfGaussianBuffer = (double*)mkl_malloc( sliceSize * sizeof( double ), 64 );
  double* gaussianBufferTemp = (double*)mkl_malloc( (nx + nG - 1) * (ny  + nG - 1) * sizeof( double ), 64 );
  double* laplacianOfGaussianBufferTemp = (double*)mkl_malloc( (nx + 2) * (ny  + 2) * sizeof( double ), 64 );
  double* reflectionNumberBuffer = (double*)mkl_malloc(sliceSize * sizeof( double ), 64 );
  double* shadowValueBuffer = (double*)mkl_malloc( sliceSize * sizeof( double ), 64 );
  double* shadowModel = (double*)mkl_malloc( ny * sizeof( double ), 64 );
  double* gaussianKernel = (double*)mkl_malloc(nG * nG * sizeof( double ), 64 );
  double* laplacianKernel = (double*)mkl_malloc(3 * 3 * sizeof( double ), 64 );

  // Calculate shadow model
  for(int i = 0; i < ny; ++i)
  {
    if (i < ny - 5) 
    { 
      shadowModel[i] = 1 - exp( - (i*i - 1)/(2*shadowSigma*shadowSigma)); 
    }
    else 
    { 
      shadowModel[i] = 0.0; 
    }
  }

  // Calculate Gaussian kernel
  int idx = 0;
  int intervall = (nG - 1) / 2;
  for(double x = -intervall; x <= intervall; ++x)
  {
    for(double y = -intervall; y <= intervall; ++y)
    {
      gaussianKernel [idx] = exp( -( (x*x)/(2*smoothingSigma*smoothingSigma) + (y*y)/(2*smoothingSigma*smoothingSigma) ) );
      ++idx;
    }
  }

  // Calculate Laplacian kernel
  laplacianKernel[0] = 0;
  laplacianKernel[1] = -1;
  laplacianKernel[2] = 0;
  laplacianKernel[3] = -1;
  laplacianKernel[4] = 4;
  laplacianKernel[5] = -1;
  laplacianKernel[6] = 0;
  laplacianKernel[7] = -1;
  laplacianKernel[8] = 0;

  // Loop through each slice
  for(idx = 0; idx < nz; ++idx)
  {
    // Index of slice in buffer
    int slice = sliceSize * idx;

    // If slice has not all zero pixels...
    if (GetMaxPixelValue(&inputBuffer[slice], sliceSize) > 0)
    {
      // Convolve with Gaussian kernel and normalize result between zero and one
      this->Conv2(&inputBuffer[slice], gaussianKernel, gaussianBufferTemp, gaussianBuffer, nx, ny, nG, nG);
      this->Normalize(gaussianBuffer, sliceSize, false);

      // Convolve blurred image with Laplacian kernel
      this->Conv2(gaussianBuffer, laplacianKernel, laplacianOfGaussianBufferTemp, laplacianOfGaussianBuffer, nx, ny, 3, 3);

      // Main loop calculating reflection number and shadow value
      double sumG, sumGI, sumHist;
      int i, pixelIdx, x, y;
      #ifdef NDEBUG
			#pragma omp parallel for reduction(+:sumG,sumGI, sumHist), private(i, x, pixelIdx)
      #endif		
      for (y = 0; y < ny; ++y)
      {
        for (x = 0; x < nx; ++x)
        {
          pixelIdx = x + y * nx;

          // Only include pixels with intensity value larger than a specified threshold
          if (gaussianBuffer[pixelIdx] >= boneThreshold && pixelIdx > transducerMargin * nx)
          {
            // Set outermost border pixels to zero and exclude negative pixels
            if ((x==nx-1 || x==0 || y==ny-1 || y==0) || laplacianOfGaussianBuffer[pixelIdx] <= 0) 
            { 
              laplacianOfGaussianBuffer[pixelIdx] = 0.0;	
            }
            else
            {
              // Divide by small number to increase image intensity (What! :)
              laplacianOfGaussianBuffer[pixelIdx] = laplacianOfGaussianBuffer[pixelIdx] / 0.005;
            }

            // Calculate reflection number
            reflectionNumberBuffer[pixelIdx] = pow(gaussianBuffer[pixelIdx], blurredVSBLoG) + laplacianOfGaussianBuffer[pixelIdx];

            // Calculate shadow value
            sumG = 0;
            sumGI = 0;
            for (i = y; i < ny; ++i) 
            {
              sumG += shadowModel[i - y];
              sumGI += shadowModel[i - y] * gaussianBuffer[x+i*nx];
            }
            shadowValueBuffer[pixelIdx] = sumGI / sumG;
          }
          else 
          { 
            reflectionNumberBuffer[pixelIdx] = 0.0;	
            shadowValueBuffer[pixelIdx] = 0.0;	
          }			
        }
      }

      // Normalize both reflection numbers and shadow values
      this->Normalize(reflectionNumberBuffer, sliceSize, false);
      this->Normalize(shadowValueBuffer, sliceSize, true);

      // Calculate BSP
      vdPowx(sliceSize, shadowValueBuffer, shadowVSIntensity, shadowValueBuffer);
      vdMul(sliceSize, shadowValueBuffer, reflectionNumberBuffer, &outputBuffer[slice]);

      // Normalize BSP
      this->Normalize(&outputBuffer[slice], sliceSize, false);
    }
  }

  // Free memory
  mkl_free(gaussianBuffer);
  mkl_free(laplacianOfGaussianBuffer);
  mkl_free(gaussianBufferTemp);
  mkl_free(laplacianOfGaussianBufferTemp);
  mkl_free(reflectionNumberBuffer);
  mkl_free(shadowValueBuffer);
  mkl_free(shadowModel);
  mkl_free(gaussianKernel);
  mkl_free(laplacianKernel);

  mkl_free_buffers();
}

//-----------------------------------------------------------------------------
// Performs a 2D convolution using Intel MKL defined by the kernel buffer.
void vtkSlicerBoneEnhancerCppLogic
::Conv2(const double* inputBuffer, const double* kernelBuffer, double* tempBuffer, double* outputBuffer, int nx, int ny, int kx, int ky)
{
  int inputShape[] = {nx, ny};
  int kernelShape[] = {kx, ky};
  int resultShape[] = {nx + kx - 1, ny + ky - 1};

  VSLConvTaskPtr task;
  int status = vsldConvNewTask(&task,VSL_CONV_MODE_AUTO, 2,inputShape, kernelShape, resultShape);

  status = vsldConvExec(task, inputBuffer, NULL, kernelBuffer, NULL, tempBuffer, NULL);
  vslConvDeleteTask(&task);

  this->ResizeMatrix(tempBuffer, outputBuffer, kx, ky, nx + kx - 1, ny + ky - 1);
}

//-----------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic
::ResizeMatrix(const double* inputBuffer, double* outputBuffer, int xClipping, int yClipping, int xInputSize, int yInputSize)
{
  int xStart = (xClipping + 2 - 1) / 2 - 1;
  int yStart = (yClipping + 2 - 1) / 2 - 1;
  int xStop = xInputSize - xStart;
  int yStop = yInputSize - yStart;		

  int idx = 0;
  for (int y = 0; y < yInputSize; ++y)
  {
    for (int x = 0; x < xInputSize; ++x)
    {
      if (x >= xStart && x < xStop && y >= yStart && y < yStop)
      {
        outputBuffer[idx] = inputBuffer[x + y * xInputSize];
        ++idx;
      }
    }
  }
}

//-----------------------------------------------------------------------------
double vtkSlicerBoneEnhancerCppLogic
::GetMaxPixelValue(const double* buffer, int size)
{
  double maxPixelValue = 0;
  for(int i = 0; i < size; ++i)
  {
    if (buffer[i] > maxPixelValue) 
    {
      maxPixelValue = buffer[i];
    }
  }
  return maxPixelValue;
}

//-----------------------------------------------------------------------------
void vtkSlicerBoneEnhancerCppLogic
::Normalize(double* buffer, int size, bool doInverse)
{
  double maxPixelValue = GetMaxPixelValue(buffer, size);

  if (!doInverse)
  {
    for (int i = 0; i < size; ++i)
    {
      buffer[i] =  buffer[i] / maxPixelValue;
    }
    return;
  }

  for (int i = 0; i < size; ++i)
  {
    buffer[i] = 1 - buffer[i] / maxPixelValue;
  }
}