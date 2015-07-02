# BoneEnhancer #
A 3D Slicer module which enhances bone in images. It is possible to test the module from 3D Slicer pressing *Reload and Test*, which uses a lumbar spine US volume (available in MIDAS).

The module uses *OpenMP* and *Intel MKL*. At the moment the path to the *Intel MKL* include directory as well as the libraries used are hardcoded into BoneEnhancerCpp/Logic/CMakeLists.txt. Also,  remember to add the *Intel MKL* dlls to your path: ...\mkl\redist\intel64\mkl, ...\mkl\redist\intel64\compiler.

A free trial of *Intel MKL* can be downloaded from here:

[https://software.intel.com/en-us/intel-mkl/try-buy](https://software.intel.com/en-us/intel-mkl/try-buy)

One more thing: the *Intel MKL* code uses double data at this moment.
## Available Algorithms ##
### Foroughi ###

**Parameters:**
* blurredVSBLoG

* boneThreshold
* shadowSigma

* shadowVSIntensity

* smoothingSigma

* transducerMargin