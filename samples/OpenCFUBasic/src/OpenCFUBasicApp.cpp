#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderOpenCV.h"

#include "ProcessingOptions.hpp"
#include "Processor.hpp"
#include "defines.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class OpenCFUBasicApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	gl::Texture2dRef mDishImage;
	Result mResult;
};

void OpenCFUBasicApp::setup() {
	// Load dish image
	ci::Surface8u surface(loadImage(loadAsset("tech-epixel-test-dish.jpg")));
	cv::Mat input(toOcv(surface));

	// Set up openCFU
	ProcessingOptions opts;
	opts.setImage(input);
	opts.setThr(3);
	opts.setThrMode(OCFU_THR_INV);
	opts.setMinMaxRad(std::make_pair(30, 200));

	opts.setTrainedClassifierPath(getAssetPath("trainedClassifier.xml").string());
	opts.setTrainedClassifierPSPath(getAssetPath("trainedClassifierPS.xml").string()); // Hmm...

	// Run openCFU
	Processor processor(opts);
	processor.runAll();
	processor.writeResult();
	mResult = processor.getNumResult();

	// Prep for drawing
	mDishImage = gl::Texture2d::create(surface);
}

void OpenCFUBasicApp::mouseDown(MouseEvent event) {
}

void OpenCFUBasicApp::update() {
}

void OpenCFUBasicApp::draw() {
	// Fit to window
	float scaleFactor = min((float)getWindowWidth() / (float)mDishImage->getWidth(), (float)getWindowHeight() / (float)mDishImage->getHeight());

	gl::clear(Color(0, 0, 0));
	gl::pushMatrices();
	gl::scale(scaleFactor, scaleFactor);
	gl::color(Color(1, 1, 1));

	gl::draw(mDishImage);

	for (unsigned int i = 0; i < mResult.size(); i++) {
		const OneObjectRow &row = mResult.getRow(i);
		vec2 colonyCenter = fromOcv((row.getPoint(0) + row.getPoint(2)) * 0.5);
		float colonyRadius = row.getRadius();
		Color colonyAverageColor = Color(row.getBGRMean()[2] / 255.0, row.getBGRMean()[1] / 255.0, row.getBGRMean()[0] / 255.0);
		gl::color(Color(1, 1, 1));
		gl::drawStrokedCircle(colonyCenter, colonyRadius);

		gl::drawLine(colonyCenter - vec2(15, 0), colonyCenter + vec2(15, 0));
		gl::drawLine(colonyCenter - vec2(0, 15), colonyCenter + vec2(0, 15));
	}

	gl::popMatrices();
}

CINDER_APP(OpenCFUBasicApp, RendererGl(RendererGl::Options().msaa(4)))
