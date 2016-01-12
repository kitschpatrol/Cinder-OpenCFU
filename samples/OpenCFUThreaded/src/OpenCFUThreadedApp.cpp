// Uncomment to deal with vsprintf linkage errors on Win x64.
//#include <Windows.h>
//#include <stdio.h>
// int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

#include "cinder/Log.h"
#include "cinder/Timer.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderOpenCV.h"

#include "ProcessingOptions.hpp"
#include "Processor.hpp"
#include "defines.hpp"
#include <thread>

using namespace ci;
using namespace ci::app;
using namespace std;

class OpenCFUThreadedApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
	void analyze(Surface8uRef surface);

private:
	ci::Surface8uRef mDishSurface;
	gl::Texture2dRef mDishTexture;
	Result mResult;
};

void OpenCFUThreadedApp::setup() {
	CI_LOG_V("Press the space bar to find colonies in the image.");
	mDishSurface = Surface8u::create(loadImage(loadAsset("tech-epixel-test-dish.jpg")));
	mDishTexture = gl::Texture2d::create(*mDishSurface);
}

void OpenCFUThreadedApp::keyDown(KeyEvent event) {
	if (event.getCode() == KeyEvent::KEY_SPACE) {
		analyze(mDishSurface);
	}
}

void OpenCFUThreadedApp::mouseDown(MouseEvent event) {
}

void OpenCFUThreadedApp::update() {
}

void OpenCFUThreadedApp::analyze(Surface8uRef surface) {
	// Load dish image

	CI_LOG_V("Starting dish analysis on background thread.");
	Timer analysisTimer = Timer(true); // Start immediately

	std::thread t([this, surface, analysisTimer]() {
		// todo lock surface
		cv::Mat input(toOcv(*surface));

		// Set OpenCFU options
		ProcessingOptions opts;
		opts.setImage(input);
		opts.setThr(3);
		opts.setThrMode(OCFU_THR_INV);
		opts.setMinMaxRad(std::make_pair(30, 200));
		opts.setTrainedClassifierPath(getAssetPath("trainedClassifier.xml").string());
		opts.setTrainedClassifierPSPath(getAssetPath("trainedClassifierPS.xml").string());

		// Run OpenCFU
		Processor processor(opts);
		processor.runAll();
		Result result = processor.getNumResult();

		// Update results on the main thread
		ci::app::App::dispatchAsync([this, result, analysisTimer]() {
			CI_LOG_V("Finished dish analysis.");
			CI_LOG_V("OpenCFU found " << result.size() << " colonies in " << analysisTimer.getSeconds() << " seconds.");
			mResult = result;
		});
	});

	t.detach();
}

void OpenCFUThreadedApp::draw() {
	// Fit to window
	float scaleFactor = min((float)getWindowWidth() / (float)mDishTexture->getWidth(), (float)getWindowHeight() / (float)mDishTexture->getHeight());

	gl::clear(Color(0, 0, 0));

	// Draw the dish image
	gl::pushMatrices();
	gl::scale(scaleFactor, scaleFactor);
	gl::color(Color(1, 1, 1));
	gl::draw(mDishTexture);

	// Draw a circle around each found colony
	for (unsigned int i = 0; i < mResult.size(); i++) {
		const OneObjectRow &row = mResult.getRow(i);
		vec2 colonyCenter = fromOcv((row.getPoint(0) + row.getPoint(2)) * 0.5);
		float colonyRadius = row.getRadius();
		// Color colonyAverageColor = Color(row.getBGRMean()[2] / 255.0, row.getBGRMean()[1] / 255.0, row.getBGRMean()[0] / 255.0);
		gl::color(Color(1, 1, 1));
		gl::drawStrokedCircle(colonyCenter, colonyRadius);
		gl::drawLine(colonyCenter - vec2(15, 0), colonyCenter + vec2(15, 0));
		gl::drawLine(colonyCenter - vec2(0, 15), colonyCenter + vec2(0, 15));
	}

	gl::popMatrices();

	// Draw a spinner to prove non-blockage
	gl::pushMatrices();
	gl::color(Color(1, 1, 0));
	gl::translate(30, 30);
	gl::rotate(ci::app::getElapsedSeconds() * 10.0);

	gl::drawSolidRect(Rectf(-15, -4, 15, 4));
	gl::popMatrices();
}

CINDER_APP(OpenCFUThreadedApp, RendererGl(RendererGl::Options().msaa(4)))