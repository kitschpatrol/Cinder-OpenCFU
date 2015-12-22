#Cinder-OpenCFU

**Cinder-OpenCFU is a very minimal integration of the [OpenCFU](http://opencfu.sourceforge.net) project with [Cinder](http://libcinder.org). OpenCFU uses image-processing techniques to automate counting bacterial colonies grown in petri dishes.**

<img width="400" src="cinderblock.png" />

## Getting Started
Note that this block includes the OpenCFU project as a git submodule. Clone the block repo recursively:

	git clone https://github.com/kitschpatrol/Cinder-OpenCFU --recursive 

From there, take a look at the OpenCFUBasic example or create a new project through Tinderbox.

## Origin

OpenCFU was created by [Quentin Geissmann](https://github.com/qgeissmann), with improvements by [Nathananael Lampe](https://github.com/natl) and [Mac Cowell](https://github.com/100ideas).


## Compatibility

Tested against the [Cinder master branch](https://github.com/cinder/Cinder/commit/dd16254f0f4ab2276df845f45b604355e64299f2) (~v9.1).

Tinderbox-generated project files are known to work on:

- Mac OS X 10.11 x64 with Xcode 7.2

## Known Issues
Anything in OpenCFU that loads images with OpenCV's imagecodecs won't work, since Cinder's OpenCV library does not include this dependency. You can get around this pretty easily by using Cinder's image loading and handling functions instead. (See the basic example.)

## Dependencies

This block depends on [Cinder's OpenCV3 block](https://github.com/cinder/Cinder-OpenCV3), and trades OpenCV's depdendency-heavy imagecodecs implementations for Cinder's built-in image loading functionality. Beyond that, all dependencies are included in the block.

Note that this block simply builds the wrapped library from source, it does not include any pre-compiled libraries.

##TODO

- Test on Windows.
- Nicer, more Cinder-esque API.
- Asynchronous image processing.
- Hook into Cinder's logging API.
- Better encapsulation of OpenCFU library.