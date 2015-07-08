![Capture.PNG](https://bitbucket.org/repo/LK76Gq/images/1196567172-Capture.PNG)
# BoneEnhancer #
A 3D Slicer module which enhances bone in images. It is possible to test the module from 3D Slicer pressing *Reload and Test*, which uses a lumbar spine US volume (available in MIDAS).

The module uses *OpenMP* and *Intel MKL*. A free trial of *Intel MKL* can be downloaded from here:

[https://software.intel.com/en-us/intel-mkl/try-buy](https://software.intel.com/en-us/intel-mkl/try-buy)

Also, remember to add the *Intel MKL* dlls to your path: ...\mkl\redist\intel64\mkl, ...\mkl\redist\intel64\compiler.


One more thing: the *Intel MKL* code uses double data at this moment.
## Available Algorithms ##
This section gives details about the algorithms currently available.
### Foroughi ###
Foroughi, P., et al. (2007) Ultrasound bone segmentation using dynamic programming. IEEE Ultrason Symp 13(4):2523–2526 (with some modifications).

**Parameters:**

* **blurredVSBLoG** - Controlls the ratio between the Gaussian blurring and the Laplacian of Gaussian.

* **boneThreshold** - Defines the probability threshold for a pixel to be considered bone or not.

* **shadowSigma** - Standard deviation of the Gaussian weighting function which models the transition of high intensity pixels close to bone surface to the dark pixels deeper under the bone.

* **shadowVSIntensity** - Controlls the ratio between the shadow map and the reflection number.

* **smoothingSigma** - Defines the size of the Gaussian kernel used for blurring.

* **transducerMargin** - Defines the number of rows to exclude from the top part of the image (close to the transducer head).