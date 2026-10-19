<!-- title({ prefix: "Cinder-" }) -->

# Cinder-OpenCFU

<!-- /title -->

<!-- badges -->

[![License: GPL-3.0-or-later](https://img.shields.io/badge/License-GPL--3.0--or--later-yellow.svg)](https://spdx.org/licenses/GPL-3.0-only)

<!-- /badges -->

<!-- short-description -->

**CinderBlock for counting bacterial colonies with a very minimal integration of the OpenCFU project.**

<!-- /short-description -->

## Overview

Cinder-OpenCFU is a very minimal integration of the [OpenCFU](https://opencfu.sourceforge.net) project with [Cinder](https://libcinder.org) as a [CinderBlock](https://libcinder.org/docs/guides/cinder-blocks/index.html). OpenCFU uses image-processing techniques to automate counting bacterial colonies grown in petri dishes.

<img width="400" src="cinderblock.png" alt="Bacterial colonies in a petri dish, with detected colonies circled and marked with a cross" />

## Getting started

### Dependencies

This block depends on [Cinder's OpenCV3 block](https://github.com/cinder/Cinder-OpenCV3), and trades OpenCV's dependency-heavy imagecodecs implementations for Cinder's built-in image loading functionality. Beyond that, all dependencies are included in the block.

Note that this block simply builds the wrapped library from source; it does not include any pre-compiled libraries.

Tested against the [Cinder master branch](https://github.com/cinder/Cinder/commit/dd16254f0f4ab2276df845f45b604355e64299f2) (0.9.1dev).

TinderBox-generated project files are known to work on:

- Mac OS X 10.11 x64 with Xcode 7.2
- Windows 10 x86 and x64 with Visual Studio 2015 Community Edition

### Installation

From your Cinder `blocks` directory:

```sh
git clone --recursive https://github.com/kitschpatrol/Cinder-OpenCFU.git OpenCFU
```

Note that this block includes the OpenCFU project as a git submodule, so be sure to clone the block repo recursively. If you already cloned it without `--recursive`, run `git submodule update --init` from the block folder.

If you don't have the OpenCV3 block yet, clone it into `blocks/OpenCV3` too:

```sh
git clone https://github.com/cinder/Cinder-OpenCV3.git OpenCV3
```

Then add **OpenCFU** to a project with [TinderBox](https://libcinder.org/docs/guides/tinderbox/index.html), or take a look at the [OpenCFUBasic sample](#opencfubasic).

## Usage

The block exposes OpenCFU's own classes. Convert a Cinder surface to a `cv::Mat`, configure a `ProcessingOptions`, and run a `Processor`:

```cpp
#include "cinder/ImageIo.h"
#include "cinder/app/App.h"
#include "CinderOpenCV.h"
#include "Processor.hpp"

Result countColonies(const ci::Surface8u &surface) {
	ProcessingOptions opts;
	opts.setImage(ci::toOcv(surface));
	opts.setThr(3);
	opts.setThrMode(OCFU_THR_INV);
	opts.setMinMaxRad(std::make_pair(30, 200));
	opts.setTrainedClassifierPath(ci::app::getAssetPath("trainedClassifier.xml").string());
	opts.setTrainedClassifierPSPath(ci::app::getAssetPath("trainedClassifierPS.xml").string());

	Processor processor(opts);
	processor.runAll();
	return processor.getNumResult();
}
```

Then read each detected colony from the `Result`:

```cpp
ci::Surface8u surface(ci::loadImage(ci::app::loadAsset("tech-epixel-test-dish.jpg")));
Result result = countColonies(surface);

for (unsigned int i = 0; i < result.size(); i++) {
	const OneObjectRow &row = result.getRow(i);
	if (row.isValid()) {
		ci::vec2 center = ci::fromOcv((row.getPoint(0) + row.getPoint(2)) * 0.5);
		int radius = row.getRadius();
	}
}
```

TinderBox copies the two pre-trained classifier files, `trainedClassifier.xml` and `trainedClassifierPS.xml`, from the block's [assets](assets) into your project's `assets` folder. It doesn't copy the test image `tech-epixel-test-dish.jpg`, so copy it from the same folder yourself, or load your own image instead.

Anything in OpenCFU that loads images with OpenCV's imagecodecs won't work, since Cinder's OpenCV library does not include this dependency. You can get around this pretty easily by using Cinder's image loading and handling functions instead. (See the basic example.)

The OpenCFU classes used above:

| Method                                                                  | Description                                                                                          |
| ----------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------- |
| `ProcessingOptions::setImage(mat)`                                      | Set the image to analyze from a `cv::Mat`.                                                           |
| `ProcessingOptions::setThr(threshold)` / `setThrMode(mode)`             | Set the threshold (0 to 255) and mode: `OCFU_THR_NORM`, `OCFU_THR_INV`, or `OCFU_THR_BILAT`.         |
| `ProcessingOptions::setMinMaxRad(pair)`                                 | Set the minimum and maximum colony radius in pixels.                                                 |
| `ProcessingOptions::setTrainedClassifierPath(path)` / `...PSPath(path)` | Set the paths to the pre-trained classifier XML files.                                               |
| `Processor::runAll()`                                                   | Run the detection steps on the image.                                                                |
| `Processor::getNumResult()`                                             | Get the `Result`, with one `OneObjectRow` per detected object.                                       |
| `Processor::writeResult()`                                              | Print the results to `stdout` as comma-separated values.                                             |
| `Result::size()` / `getNValid()` / `getRow(i)`                          | Get the number of detected objects, the number that passed OpenCFU's filters, or one row.            |
| `OneObjectRow::isValid()`                                               | Whether the object passed OpenCFU's filters.                                                         |
| `OneObjectRow::getPoint(i)` / `getRadius()` / `getBGRMean()`            | Get a corner (0 to 3) of the object's rotated bounding rectangle, its radius, or its mean BGR color. |

## Examples

Both samples load a test image of a petri dish from their `assets` folder and draw a circle and a cross over each object OpenCFU found. All sample projects expect the OpenCV3 block in `blocks/OpenCV3`. The Xcode projects and the OpenCFUThreaded Visual Studio 2013 project also expect this block in `blocks/OpenCFU` and the Cinder directory itself to be named `Cinder`. The clone commands under [Installation](#installation) use these block directory names.

### OpenCFUBasic

The [basic sample](samples/OpenCFUBasic/src/OpenCFUBasicApp.cpp) runs OpenCFU once in `setup()` and prints the results to `stdout`. Open the Xcode project in [samples/OpenCFUBasic/xcode](samples/OpenCFUBasic/xcode), or the Visual Studio solution in [samples/OpenCFUBasic/vc2015](samples/OpenCFUBasic/vc2015) (Visual Studio 2015) or [samples/OpenCFUBasic/vc2013](samples/OpenCFUBasic/vc2013) (Visual Studio 2013).

### OpenCFUThreaded

The [threaded sample](samples/OpenCFUThreaded/src/OpenCFUThreadedApp.cpp) runs the analysis on a background thread when you press the space bar, logs the number of colonies found and how long it took, and draws a spinning bar to show that the main thread isn't blocked. Open the Xcode project in [samples/OpenCFUThreaded/xcode](samples/OpenCFUThreaded/xcode) or the Visual Studio 2013 solution in [samples/OpenCFUThreaded/vc2013](samples/OpenCFUThreaded/vc2013).

## Updating bundled libraries

OpenCFU is included as a git submodule at `src/OpenCFU`, pinned to commit [8235e76](https://github.com/kitschpatrol/OpenCFU/tree/8235e76ecadf440e782bbc59d789f42c5f5856eb) of the [kitschpatrol/OpenCFU](https://github.com/kitschpatrol/OpenCFU) fork of Quentin Geissmann's [OpenCFU](https://github.com/qgeissmann/OpenCFU) repository. The fork exposes the trained classifier paths in `ProcessingOptions`, adds the `HAVE_CV_IMAGECODECS` switch around OpenCV image loading, removes some unneeded `highgui` includes, and fixes compilation on Windows and without Cinder's precompiled header.

[src/config.h](src/config.h) is the header generated by OpenCFU's `configure` script for version 3.9.0 without the GUI. `INSTALLDIR` and a commented-out `HAVE_CV_IMAGECODECS` define were added at the end. Uncomment `HAVE_CV_IMAGECODECS` only if your OpenCV build includes imagecodecs.

## Background

### Other projects

- [OpenCFU](https://github.com/qgeissmann/OpenCFU) itself is a standalone application with a graphical interface and a command-line mode. It's described in the paper [OpenCFU, a New Free and Open-Source Software to Count Cell Colonies and Other Circular Objects](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0054072).

### Known issues

- The API isn't very Cinder-esque: you use OpenCFU's `ProcessingOptions`, `Processor`, and `Result` classes directly.
- Image processing is synchronous. See the OpenCFUThreaded sample for running it on a background thread.
- OpenCFU doesn't hook into Cinder's logging API. It writes errors to `std::cerr`, and `Processor::writeResult()` writes to `std::cout`.
- The OpenCFU library could use better encapsulation. Its classes are in the global namespace, and its headers include the block's `config.h`.
- If OpenCFU can't open a trained classifier at the path you set, it falls back to its desktop installation path (`../data/` on Windows, `/data/` elsewhere), and the assertion meant to catch a missing classifier never fires.
- Both samples draw every object in the `Result`, including ones OpenCFU's filters marked as invalid, and the threaded sample reports `Result::size()` rather than `Result::getNValid()` as the number of colonies.
- In the threaded sample, each space bar press starts another detached analysis thread, and nothing stops a running thread when the app quits.

## Maintainers

[kitschpatrol](https://github.com/kitschpatrol)

## Acknowledgments

OpenCFU was created by [Quentin Geissmann](https://github.com/qgeissmann), with improvements by [Nathanael Lampe](https://github.com/natl) and [Mac Cowell](https://github.com/100ideas).

<!-- contributing -->

## Contributing

[Issues](https://github.com/kitschpatrol/Cinder-OpenCFU/issues) are welcome and appreciated.

Please open an issue to discuss changes before submitting a pull request. Unsolicited PRs (especially AI-generated ones) are unlikely to be merged.

This repository uses [MDAT](https://github.com/kitschpatrol/mdat) for readme placeholder expansion.

<!-- /contributing -->

<!-- license -->

## License

[GPL-3.0-or-later](license.txt) © Eric Mika

<!-- /license -->

OpenCFU, included as a submodule in `src/OpenCFU`, is also licensed under the GPL, version 3 or later, and is copyright Quentin Geissmann and its contributors.
