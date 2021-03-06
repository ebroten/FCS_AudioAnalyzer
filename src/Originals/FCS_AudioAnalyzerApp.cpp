#include "Resources.h"

namespace cieq
{

    InputAnalyzer::InputAnalyzer()
        : mGlobals(mEventProcessor)
        , mAudioNodes(mGlobals)
        , mSpectrumPlot(mAudioNodes)
        , mWaveformPlot(mAudioNodes)
    {}

    void InputAnalyzer::prepareSettings(Settings *settings)
    {
        // Enables the console window to show up beside the graphical window
        // We need the console for debugging and logging purposes
        settings->enableConsoleWindow(true);

        // It's ok if a user resizes the window, we'll respond accordingly
        settings->setResizable(true);

        // Title of the graphical window when it pops up
        settings->setTitle("Cinder Audio Equalizer");

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
        mAudioNodes.setup();
        mEventProcessor.addKeyboardEvent([this](char c){ if (c == 's' || c == 'S') mAudioNodes.toggleInput(); });
        mEventProcessor.addMouseEvent([this](float, float){ mAudioNodes.toggleInput(); });

        const auto window_size = ci::app::getWindowSize();
        const auto plot_size_width = 0.9f * window_size.x; // 90% of window width
        const auto plot_size_height = 0.8f * 0.5f * window_size.y; // half of 90% of window width

        mSpectrumPlot.setup();
        mWaveformPlot.setup();

        ci::Vec2f top_left = 0.05f * window_size;
        mSpectrumPlot.setPlotTitle("FFT Analysis of input data");
        mSpectrumPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
        mSpectrumPlot.setHorzAxisTitle("Frequency").setHorzAxisUnit("Hz");
        mSpectrumPlot.setVertAxisTitle("Magnitude").setVertAxisUnit("Db");

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
        mSpectrumPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));

        top_left.y += 0.5f * window_size.y;
        mWaveformPlot.setBounds(ci::Rectf(top_left, top_left + ci::Vec2f(plot_size_width, plot_size_height)));
    }

    void InputAnalyzer::update()
    {
        //noop
    }

    void InputAnalyzer::draw()
    {
        ci::gl::clear();
        ci::gl::enableAlphaBlending();

        mSpectrumPlot.draw();
        mWaveformPlot.draw();
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

} //!namespace cieq

CINDER_APP_NATIVE(cieq::InputAnalyzer, ci::app::RendererGl)