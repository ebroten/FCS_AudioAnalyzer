#ifndef CIEQ_INCLUDE_APP_H_
#define CIEQ_INCLUDE_APP_H_

#include <cinder/app/AppNative.h>
#include <cinder/Timer.h>
#include <functional>

#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "app_globals.h"
#include "app_event.h"
#include "audio_nodes.h"
#include "audio_draw.h"

namespace cieq
{

/*!
 * \class InputAnalyzer
 * \brief The main application class. Handles window initialization
          window title, etc.
 */
class InputAnalyzer final : public ci::app::AppNative
{
public:
	InputAnalyzer();

	// \note Tells Cinder how to prepare the window
    void		prepareSettings(Settings *settings) override final;
    //! sets up the application, this gets fired after Cinder is fully initialized.
    void		setup() override final;
    //! called at the beginning of every render loop, used to refresh data.
    void		update() override final;
    //! called at the end of every render loop, used to draw the data.
    void		draw() override final;
    //! application window resize callback.
    void		resize() override final;
    //! draws the current refresh rate of the render loop.
    void		drawFps();

	void		shutdown() override final;

    //! gets fired on mouse click
    void		mouseDown(ci::app::MouseEvent event) override final;
    //! gets fired on mouse drag
    //void		mouseDrag(ci::app::MouseEvent event) override final;
    //! gets fired on keyboard click
	void		keyDown(ci::app::KeyEvent event) override final;
    // Toggles boolean flag used to pause display updating
    void        togglePauseDrawing();
    // Gets fired on press of Linear / dB Mode button in parameter menu
    void        linearDBModeButton();

	AppGlobals&	getGlobals() { return mGlobals; }

private:
    //! the event processor class instance
    AppEvent	                            	mEventProcessor;
    //! the globals class instance, used to store common data
    AppGlobals                          		mGlobals;
    //! audio I/O class instance
    AudioNodes	                            	mAudioNodes;
    //! magnitude spectrum plot of raw input
    SpectrumPlot                            	mSpectrumPlot;
    //! waveform plot of raw input
    WaveformPlot                            	mWaveformPlot, mWaveformPlotShifted;
    //! running spectrogram plot audio signal
    SpectrogramPlot	                            mSpectrogramPlot;
    //! cinder's param ref, for tweaking variables during runtime
    ci::params::InterfaceGlRef                  mParams;
    bool                                        linearDbMode;
    bool                                        pauseDrawing;
    size_t                                      userWinSize;
    size_t                                      userWinSizePrev;
    size_t                                      userSpecDuration;
    size_t                                      userSpecDurSeconds;
    size_t                                      userSpecDurPrev;
    size_t                                      userSpecMaxFreq;
    size_t                                      userSpecMaxFreqPrev;
    size_t                                      fftSize;
    size_t                                      fftSizePrev;
    size_t                                      dispBins;
    size_t                                      actualMaxFreq;
    size_t                                      nearestFft;
    size_t                                      numDispParams;
    float                                       hSR;
    float                                       numBins;
    float                                       shift;
    float                                       shiftLength;
    float                                       userHopSizePrev;
    float                                       userMaxMag;
    ci::Timer                                   mTimer;
    double                                      userHopSize;
    double                                      timeEnter;
    double                                      timeEnterPrev;
    double                                      timeReturn;
    double                                      timeExit;
    double                                      timeSec1Enter;
    double                                      timeSec1Exit;
    double                                      timeSec1Process;
    double                                      timeSec2Enter;
    double                                      timeSec2Exit;
    double                                      timeSec2Process;
    double                                      timeSec3Enter;
    double                                      timeSec3Exit;
    double                                      timeSec3Process;
    double                                      timeSec4Enter;
    double                                      timeSec4Exit;
    double                                      timeSec4Process;
    double                                      nearestPow2;
    double                                      userWinSizeMs;
};

} //!cieq

#endif //!CIEQ_INCLUDE_APP_H_