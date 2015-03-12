#include "app.h"
#include "math.h"

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
    if (mTimer.isStopped())
    {
        mTimer.start();
    }
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
    userWinSize = 500;
    userWinSizePrev = userWinSize;
    userSpecDurSeconds = 5;
    userSpecDurPrev = userSpecDurSeconds;
    userSpecDuration = userSpecDurSeconds;
    userSpecDurPrev = userSpecDuration;
    userSpecMaxFreq = 10000; //Default maximum frequency display of 10kHz
    userSpecMaxFreqPrev = userSpecMaxFreq;
    userHopSize = 20; //in Hz
    userHopSizePrev = userHopSize;
    userSpecDurSeconds = 5;
    userSpecDurPrev = userSpecDurSeconds;
    userSpecDuration = userHopSize * userSpecDurSeconds;
    userMaxDB = 100.0f;
    fftSize = 131072;
    fftSizePrev = fftSize;
    linearMult = 25;
    linearDbMode = 0;
    pauseDrawing = 0;
    shift = ((float)userWinSize / 1000) / 4;
    shiftLength = ((float)userWinSize / 1000) / 2;
    mParams->addParam("Window Size (ms)", &userWinSize).min(10).max(500).step(1);
    mParams->addParam("Spectrogram Update Rate (Hz)", &userHopSize).min(10).max(60).step(1);
    mParams->addParam("Spectrogram Duration (2-20s)", &userSpecDurSeconds).min(2).max(20).step(1);
    mParams->addParam("Spectrogram Max Freq Display (100-20000Hz)", &userSpecMaxFreq).min(100).max(20000).step(1);
    mParams->addParam("Spectrogram Max Magnitude Display", &linearMult).min(1).max(1024).step(1);
    //mParams->addButton("Switch to Decibel Mode", &linearDbMode);
    mParams->addParam("Spectrogram Max Decibel Display (dB)", &userMaxDB).min(0).max(250).step(1);
    mParams->addParam("Search Shift (s)", &shift).min(0.0f).max(0.5f).precision(3).step(0.001f);
    mParams->addParam("Search Length (s)", &shiftLength).min(0.01f).max(0.5f).precision(3).step(0.001f);

    const auto window_size = ci::app::getWindowSize();
    const auto plot_size_width = 0.9f * window_size.x; // 90% of window width
    const auto plot_size_height = 0.8f * 1.0f * window_size.y; // half of 90% of window width; 0.5f
    ci::Vec2f top_left = 0.05f * window_size;

    fftSize = static_cast<size_t>((plot_size_width * 44100) / static_cast<float>(userSpecMaxFreq));
    nearestPow2 = ceil(log2(static_cast<double>(fftSize)));
    nearestFft = static_cast<size_t>(pow(2, nearestPow2));
    //if (nearestFft > fftSize)
    //{
    //    fftSize = static_cast<size_t>(pow(2, (nearestPow2 - 1.0)));
    //}

    //Window size can be entered in ms now for the mAudioNodes.setup call:
    mAudioNodes.setup(userWinSize, fftSize);

    fftSize = mAudioNodes.getFftSize();
    hSR = static_cast<float>(mAudioNodes.getHardwareSampleRate());
    numBins = static_cast<float>(mAudioNodes.getNumBins());
    actualMaxFreq = static_cast<size_t>(plot_size_width * ((hSR / 2) / numBins));

    mEventProcessor.addKeyboardEvent([this](char c){ if (c == 's' || c == 'S') mAudioNodes.toggleInput(); });
    mEventProcessor.addMouseEvent([this](float, float){ togglePauseDrawing(); });
	mEventProcessor.addMouseEvent([this](float, float){ mAudioNodes.toggleInput(); });

    mSpectrogramPlot.setPlotTitle("Spectrogram");
    mSpectrogramPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));

    dispBins = (fftSize * userSpecMaxFreq) / static_cast<size_t>(hSR);
    mSpectrogramPlot.setup(userSpecDuration, dispBins);
	//mSpectrumPlot.setup();
    mWaveformPlotShifted.setup();
    mWaveformPlot.setup();

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
    //timeSec1Enter = mTimer.getSeconds();
	const auto window_size = ci::app::getWindowSize();
	const auto plot_size_width = 0.9f * window_size.x; // 90% of window width
	const auto plot_size_height = 0.8f * 1.0f * window_size.y; // half of 90% of window width; 0.5f

	ci::Vec2f top_left = 0.05f * window_size;
	//mSpectrumPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    //mWaveformPlotShifted.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    mSpectrogramPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    mSpectrogramPlot.setup(userSpecDuration, dispBins);

	//top_left.y += 0.5f * window_size.y;
	//mWaveformPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    //timeSec1Exit = mTimer.getSeconds();
    //timeSec1Process = timeSec1Exit - timeSec1Enter;
}

void InputAnalyzer::update()
{
    //timeSec1Enter = mTimer.getSeconds();
    if (userWinSize != userWinSizePrev)
    {
        hSR = static_cast<float>(mAudioNodes.getHardwareSampleRate());
        float minNumBins = static_cast<size_t>((pow(2, ceil(log2((static_cast<double>(userWinSize) / 1000) * static_cast<double>(hSR))))) / 2);
        float plotWidth = static_cast<float>(mSpectrogramPlot.getPlotWidth());
        float maxDispBins = plotWidth; // static_cast<float>(mSpectrogramPlot.getMaxDispBins()); //ceil(static_cast<float>(plotWidth) / static_cast<float>(pixelsPerBin));
        //actualMaxFreq = static_cast<size_t>((maxDispBins * hSR) / (2 * minNumBins)); // working
        //size_t minDispFreq = static_cast<size_t>((256 * hSR) / (2 * numBins)); // working
        fftSize = static_cast<size_t>((maxDispBins * hSR) / static_cast<float>(userSpecMaxFreq));
        nearestPow2 = ceil(log2(static_cast<double>(fftSize)));
        nearestFft = static_cast<size_t>(pow(2, nearestPow2));
        if (nearestFft > fftSize)
        {
            fftSize = static_cast<size_t>(pow(2, (nearestPow2 - 1.0)));
        }
        mAudioNodes.disableInput();
        mAudioNodes.disconnectAll();
        mAudioNodes.setup(userWinSize, fftSize);
        mAudioNodes.enableInput();

        fftSize = mAudioNodes.getFftSize();
        dispBins = (fftSize * userSpecMaxFreq) / static_cast<size_t>(hSR);
        mSpectrogramPlot.setup(userSpecDuration, dispBins);
        //userSpecDuration = userHopSize * userSpecDurSeconds; 
        //mSpectrogramPlot.setup(userSpecDuration);
        userWinSizePrev = userWinSize;
    }
    if (userSpecMaxFreq != userSpecMaxFreqPrev)
    {
        hSR = static_cast<float>(mAudioNodes.getHardwareSampleRate());
        numBins = static_cast<float>(mAudioNodes.getNumBins());
        float plotWidth = static_cast<float>(mSpectrogramPlot.getPlotWidth());
        float pixelsPerBin = ceil(static_cast<float>(plotWidth) / static_cast<float>(mSpectrogramPlot.getMaxDispBins()));
        float maxDispBins = plotWidth / 2;// static_cast<float>(mSpectrogramPlot.getMaxDispBins()); //ceil(static_cast<float>(plotWidth) / static_cast<float>(pixelsPerBin));
        if (maxDispBins < 256)
        {
            maxDispBins = 256;
        }
		actualMaxFreq = static_cast<size_t>((maxDispBins * hSR) / (2 * numBins)); // working
        size_t minDispFreq = static_cast<size_t>((256 * hSR) / (2 * numBins)); // working
        fftSize = static_cast<size_t>((maxDispBins * hSR) / static_cast<float>(userSpecMaxFreq));
		nearestPow2 = ceil(log2(static_cast<double>(fftSize)));
		nearestFft = static_cast<size_t>(pow(2, nearestPow2));
		if (nearestFft > fftSize)
		{
			fftSize = static_cast<size_t>(pow(2, (nearestPow2 - 1.0)));
		}
        //if (userSpecMaxFreq < minDispFreq)
        //{
        //    maxDispBins = 256;
        //    actualMaxFreq = static_cast<size_t>(maxDispBins * ((hSR / 2) / numBins));
        //    fftSize = (maxDispBins * hSR) / userSpecMaxFreq;
        //}
        //else
        //{
        //    while (userSpecMaxFreq > actualMaxFreq)
        //    {
        //        fftSize = pow(2, log2(static_cast<double>(fftSize)) - 1.0);
        //        numBins = fftSize / 2;
        //        //plotWidth = static_cast<float>(mSpectrogramPlot.getPlotWidth());
        //        actualMaxFreq = static_cast<size_t>(maxDispBins * ((hSR / 2) / numBins));
        //    }
        //}
        if (fftSize != fftSizePrev)
        {
            mAudioNodes.disableInput();
            mAudioNodes.disconnectAll();
            mAudioNodes.setup(userWinSize, fftSize);
            mAudioNodes.enableInput();
            fftSizePrev = fftSize;
        }
        fftSize = mAudioNodes.getFftSize();
        dispBins = (fftSize * userSpecMaxFreq) / static_cast<size_t>(hSR);
        mSpectrogramPlot.setup(userSpecDuration, dispBins);
        userSpecMaxFreqPrev = userSpecMaxFreq;
    }
    if (userSpecDurSeconds != userSpecDurPrev)
    {
        userSpecDuration = userHopSize * userSpecDurSeconds; 
        mSpectrogramPlot.setup(userSpecDuration, dispBins);
        userSpecDurPrev = userSpecDurSeconds;
    }
    //timeSec1Exit = mTimer.getSeconds();
    //timeSec1Process = timeSec1Exit - timeSec1Enter;
}

void InputAnalyzer::draw()
{
    if (!pauseDrawing)
    {
        timeEnter = mTimer.getSeconds();
        timeReturn = timeEnter - timeEnterPrev;
        timeEnterPrev = timeEnter;
        timeSec1Enter = mTimer.getSeconds();
        //Set background color for main window
        ci::ColorA backgroundColor = ci::ColorA::gray(0.25f, 0);
        ci::gl::clear(backgroundColor);
        ci::gl::enableAlphaBlending();
        timeSec1Exit = mTimer.getSeconds();
        timeSec1Process = timeSec1Exit - timeSec1Enter;

        timeSec2Enter = mTimer.getSeconds();
        //mSpectrumPlot.draw(0, 0, 0);
        //mWaveformPlotShifted.draw(shift, shiftLength, userMaxDB);
        mSpectrogramPlot.draw(userHopSize, userSpecMaxFreq, userMaxDB);
        //mWaveformPlot.draw(0, 10, 0);
        timeSec2Exit = mTimer.getSeconds();
        timeSec2Process = timeSec2Exit - timeSec2Enter;

        //Draw parameter window:
        timeSec3Enter = mTimer.getSeconds();
        mParams->draw();
        timeSec3Exit = mTimer.getSeconds();
        timeSec3Process = timeSec3Exit - timeSec3Enter;

        // draw framerate in FPS
        timeSec4Enter = mTimer.getSeconds();
        drawFps();
        timeSec4Exit = mTimer.getSeconds();
        timeSec4Process = timeSec4Exit - timeSec4Enter;
    }
}

void InputAnalyzer::shutdown()
{
    //NA
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
    std::stringstream numBinsDisp;
    std::stringstream fftSize;
    std::stringstream maxFreqDisp;
    std::stringstream actualHopRate;
    buf << "FPS: " << ci::app::getFrameRate();
    ci::gl::drawStringRight(buf.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 10));
    numBins << "Number of Bins: " << mAudioNodes.getNumBins();
    ci::gl::drawStringRight(numBins.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 20));
    fftSize << "FFT Size: " << mAudioNodes.getFftSize();
    ci::gl::drawStringRight(fftSize.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 30));
    numBinsDisp << "Number of Bins Displayed: " << mSpectrogramPlot.getMaxDispBins();
    ci::gl::drawStringRight(numBinsDisp.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 40));
    maxFreqDisp << "Max Freq Displayed: " << mAudioNodes.getMaxFreqDisp(mSpectrogramPlot.getMaxDispBins());
    ci::gl::drawStringRight(maxFreqDisp.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 50));
    actualHopRate << "Hop Rate (Hz): " << mSpectrogramPlot.getActualHopRate();
    ci::gl::drawStringRight(actualHopRate.str(), ci::Vec2i(ci::app::getWindowWidth() - 25, 60));
}

void InputAnalyzer::togglePauseDrawing()
{
    if (!pauseDrawing)
    {
        pauseDrawing = 1;
    }
    else
    {
        pauseDrawing = 0;
    }
}


} //!namespace cieq

CINDER_APP_NATIVE(cieq::InputAnalyzer, ci::app::RendererGl)