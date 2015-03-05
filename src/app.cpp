#include "app.h"

namespace cieq
{
InputAnalyzer::InputAnalyzer()
	: mGlobals(mEventProcessor)
	, mAudioNodes(mGlobals)
	, mSpectrumPlot(mAudioNodes)
    , mWaveformPlot(mAudioNodes)
    , mSpectrogramPlot(mAudioNodes)
    , mWaveformPlotShifted(mAudioNodes)
{}

void InputAnalyzer::prepareSettings(Settings *settings)
{
	// Enables the console window to show up beside the graphical window
	// We need the console for debugging and logging purposes
	settings->enableConsoleWindow(true);

	// It's ok if a user resizes the window, we'll respond accordingly
	settings->setResizable(true);

	// Title of the graphical window when it pops up
	settings->setTitle("FCS Audio Analyzer");

	// Get the current display (monitor) dimensions
	const auto current_display_size = settings->getDisplay()->getSize();
	// calculate the window size to be 90% of the monitor size
	const auto window_size = (current_display_size * 9) / 10;
	// calculate the window position (top left) to be at 10% of monitor size
	const auto window_position = (current_display_size * 5) / 100;

	// set the window size to be 90% of the monitor size
	settings->setWindowSize(window_size);
	// set the window position (top left) to be at 10% of monitor size
	settings->setWindowPos(window_position);
}

void InputAnalyzer::setup()
{
    mParams = ci::params::InterfaceGl::create(cinder::app::getWindow(), "App parameters", ci::app::toPixels(ci::Vec2i(300, 100)));
    
    mEventProcessor.addKeyboardEvent([this](char c)
    { if (c == 'p' || c == 'P') 
        {
            if (mParams->isVisible())
                mParams->hide();
            else
                mParams->show();
        }
    });
    mGlobals.setParamsPtr(mParams.get());

    //Setup parameters:
    userWinSize = 250;
    userWinSizePrev = userWinSize;
    userSpecDurSeconds = 5;
    userSpecDurPrev = userSpecDurSeconds;
    userSpecDuration = userSpecDurSeconds; // / ((float)userWinSize / 1000);//Not this simple. need to look at this more...
    userSpecDurPrev = userSpecDuration;
    userSpecMaxFreq = 10000; //Default maximum frequency display of 10kHz
    userSpecMaxFreqPrev = userSpecMaxFreq;
    shift = ((float)userWinSize / 1000) / 4;
    shiftLength = ((float)userWinSize / 1000) / 2;
    mParams->addParam("Window Size (ms)", &userWinSize).min(10).max(500).step(1);
    mParams->addParam("Spectrogram Duration (2-20s)", &userSpecDurSeconds).min(2).max(20).step(1);
    mParams->addParam("Spectrogram Max Freq Display (100-10000Hz)", &userSpecMaxFreq).min(100).max(10000).step(1);
    mParams->addParam("Search Shift (s)", &shift).min(0.0f).max(0.5f).precision(3).step(0.001f);
    mParams->addParam("Search Length (s)", &shiftLength).min(0.01f).max(0.5f).precision(3).step(0.001f);

    //Window size can be entered in ms now for the mAudioNodes.setup call:
    mAudioNodes.setup(userWinSize);

	mEventProcessor.addKeyboardEvent([this](char c){ if (c == 's' || c == 'S') mAudioNodes.toggleInput(); });
	mEventProcessor.addMouseEvent([this](float, float){ mAudioNodes.toggleInput(); });

	const auto window_size = ci::app::getWindowSize();
	const auto plot_size_width = 0.9f * window_size.x; // 90% of window width
	const auto plot_size_height = 0.8f * 0.5f * window_size.y; // half of 90% of window width
    ci::Vec2f top_left = 0.05f * window_size;

    mSpectrogramPlot.setPlotTitle("Spectrogram");
    mSpectrogramPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
	//mSpectrumPlot.setup();
    mWaveformPlotShifted.setup();
    mWaveformPlot.setup();
    mSpectrogramPlot.setup(userSpecDuration);

	//mSpectrumPlot.setPlotTitle("FFT Analysis of input data");
	//mSpectrumPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
	//mSpectrumPlot.setHorzAxisTitle("Frequency").setHorzAxisUnit("Hz");
	//mSpectrumPlot.setVertAxisTitle("Magnitude").setVertAxisUnit("Db");
    //mWaveformPlotShifted.setPlotTitle("RAW input data - Search Display");
    //mWaveformPlotShifted.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    //mWaveformPlotShifted.setHorzAxisTitle("Time").setHorzAxisUnit("s");
    ////mWaveformPlotShifted.setVertAxisTitle("Amplitude").setVertAxisUnit("...");
    //mSpectrogramPlot.setPlotTitle("Spectrogram");
    //mSpectrogramPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    mSpectrogramPlot.setHorzAxisTitle("Frequency").setHorzAxisUnit("Hz");
    mSpectrogramPlot.setVertAxisTitle("Time").setVertAxisUnit("s");

	top_left.y += 0.5f * window_size.y;
	mWaveformPlot.setPlotTitle("RAW input data");
	mWaveformPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
	mWaveformPlot.setHorzAxisTitle("Time").setHorzAxisUnit("s");
	mWaveformPlot.setVertAxisTitle("Amplitude").setVertAxisUnit("...");
}

void InputAnalyzer::resize()
{
	const auto window_size = ci::app::getWindowSize();
	const auto plot_size_width = 0.9f * window_size.x; // 90% of window width
	const auto plot_size_height = 0.8f * 0.5f * window_size.y; // half of 90% of window width

	ci::Vec2f top_left = 0.05f * window_size;
	//mSpectrumPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    //mWaveformPlotShifted.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    mSpectrogramPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    mSpectrogramPlot.setup(userSpecDuration);

	top_left.y += 0.5f * window_size.y;
	mWaveformPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
}

void InputAnalyzer::update()
{
    if (userWinSize != userWinSizePrev)
    {
        mAudioNodes.disableInput();
        mAudioNodes.disconnectAll();
        mAudioNodes.setup(userWinSize);
        mAudioNodes.enableInput();
        userSpecDuration = userSpecDurSeconds; // / ((float)userWinSize / 1000);//Not this simple. need to look at this more...
        mSpectrogramPlot.setup(userSpecDuration);
        userWinSizePrev = userWinSize;
    }
    if (userSpecDurSeconds != userSpecDurPrev)
    {
        userSpecDuration = userSpecDurSeconds; // / ((float)userWinSize / 1000);//Not this simple. need to look at this more...
        mSpectrogramPlot.setup(userSpecDuration);
        userSpecDurPrev = userSpecDurSeconds;
    }
}

void InputAnalyzer::draw()
{
    //Set background color for main window
    ci::ColorA backgroundColor = ci::ColorA::gray(0.25f, 0);
    ci::gl::clear(backgroundColor);
	ci::gl::enableAlphaBlending();

	//mSpectrumPlot.draw(0);
    //mWaveformPlotShifted.draw(shift, shiftLength);
    mSpectrogramPlot.draw(0, userSpecMaxFreq);
    mWaveformPlot.draw(0, 10);

    //Draw parameter window:
    mParams->draw();

    // draw framerate in FPS
    drawFps();

}

void InputAnalyzer::shutdown()
{
	//noop
}

void InputAnalyzer::mouseDown(ci::app::MouseEvent event)
{
	mEventProcessor.processMouseEvents(
		static_cast<float>(event.getX()),
		static_cast<float>(event.getY()));
}

void InputAnalyzer::keyDown(ci::app::KeyEvent event)
{
	mEventProcessor.processKeybaordEvents(event.getChar());
}

void InputAnalyzer::drawFps()
{
    std::stringstream buf;
    std::stringstream numBins;
    std::stringstream fftSize;
    std::stringstream maxFreqDisp;
    buf << "FPS: " << ci::app::getFrameRate();
    ci::gl::drawStringRight(buf.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 10));
    numBins << "Number of Bins: " << mAudioNodes.getNumBins();
    ci::gl::drawStringRight(numBins.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 20));
    fftSize << "FFT Size: " << mAudioNodes.getFftSize();
    ci::gl::drawStringRight(fftSize.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 30));
    maxFreqDisp << "Max Freq Displayed: " << mAudioNodes.getMaxFreqDisp();
    ci::gl::drawStringRight(maxFreqDisp.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 40));
}

} //!namespace cieq

CINDER_APP_NATIVE(cieq::InputAnalyzer, ci::app::RendererGl)