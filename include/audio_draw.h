#ifndef CIEQ_INCLUDE_AUDIO_DRAW_H_
#define CIEQ_INCLUDE_AUDIO_DRAW_H_

#include <cinder/Color.h>
#include <cinder/Vector.h>
#include <cinder/Rect.h>
#include <cinder/Surface.h>
#include <cinder/Font.h>
#include <cinder/gl/Texture.h>
#include <cinder/PolyLine.h>
#include <cinder/Timer.h>


#include <vector>
#include <array>
#include <sstream>

using namespace ci;
using namespace std;

namespace cinder 
{
namespace gl 
{
    class TextureFont;
}
} //!ci::gl

namespace cieq
{

class AudioNodes;

class Plot
{
public:
	Plot();
	virtual ~Plot() {};
	void			setup();
    
	Plot&			setHorzAxisTitle(const std::string& title)	{ mHorzTitle = title; onHorzAxisTextChange(); return *this; }
	Plot&			setVertAxisTitle(const std::string& title)	{ mVertTitle = title; onVertAxisTextChange(); return *this; }
	Plot&			setVertAxisUnit(const std::string& unit)	{ mVertUnit = unit; onVertAxisTextChange(); return *this; }
	Plot&			setHorzAxisUnit(const std::string& unit)	{ mHorzUnit = unit; onHorzAxisTextChange(); return *this; }
	Plot&			setPlotTitle(const std::string& title)		{ mPlotTitle = title; return *this; }
	Plot&			setBounds(const ci::Rectf& bounds)			{ mBounds = bounds; return *this; }
	Plot&			setBoundsColor(const ci::ColorA& color)		{ mBoundsColor = color; return *this; }
	Plot&			setDrawBounds(bool on = true)				{ mDrawBounds = on; return *this; }
	Plot&			setDrawLabels(bool on = true)				{ mDrawLabels = on; return *this; }

    virtual	void	drawLocal(double winSizeMs, float shift, float shiftLength, float maxDB, bool linearDbMode) = 0;
    //Added float shift and float shiftLength parameters to allow for window shifting:
    void			draw(double winSizeMs, float shift, float shiftLength, float maxDB, bool linearDbMode);
	void			drawBounds();
	void			drawLabels();

protected:
	std::string	mHorzTitle, mVertTitle;
	std::string	mHorzUnit, mVertUnit;
	std::string	mHorzText, mVertText;
	std::string	mPlotTitle;
	ci::Rectf	mBounds;
	ci::ColorA	mBoundsColor;
	bool		mDrawBounds, mDrawLabels;
    ci::Font	mLabelFont;

private:
	void			onHorzAxisTextChange();
    void			onVertAxisTextChange();
};

class WaveformPlot final : public Plot
{
public:
	WaveformPlot(AudioNodes& nodes);

    void					setGraphColor(const ci::ColorA& color);
    //Added float shift and float shiftLength parameters to allow for window shifting:
    void					drawLocal(double winSizeMs, float shift, float shiftLength, float maxDB, bool linearDbMode);

private:
	ci::ColorA				mGraphColor;
	AudioNodes&				mAudioNodes;
};

class SpectrumPlot final : public Plot
{
public:
	SpectrumPlot(AudioNodes& nodes);

    void enableDecibelsScale(bool on = true);
    //Added float shift and float shiftLength parameters to allow for window shifting:
    void drawLocal(double winSizeMs, float shift, float shiftLength, float maxDB, bool linearDbMode);

private:
	bool					mScaleDecibels;
	std::vector<ci::Vec2f>	mVerts;
	std::vector<ci::ColorA>	mColors;
	AudioNodes&				mAudioNodes;
};

class SpectrogramPlot final : public Plot
{
public:
    SpectrogramPlot(AudioNodes& nodes);

    void							drawLocal(double winSizeMs, float shift, float shiftLength, float maxDB, bool linearDBMode) override;
    void							setup(int duration, size_t width);
    size_t                          getMaxDispBins();
    double                          getActualHopRate();
    size_t                          getPlotWidth();

private:
    AudioNodes&						mAudioNodes;
    std::vector<float>              spectrum;
    std::array<Surface32f, 2>   	mSpectrals;
    gl::Texture					    mTexCache;
    std::string                     tickLabelYOriginString;
    std::string                     tickLabelYCenterString;
    std::string                     tickLabelYEndString;
    std::size_t						mTexW, mTexH;
    std::size_t						mFrameCounter;
    std::size_t						nodeNumber;
    std::size_t						maxDispBins;
    std::size_t						maxFreqDisp;
    std::size_t                     flag;
    std::size_t                     pixelsPerBin;
    std::size_t                     binSkipMult;
    std::size_t                     plotWidth;
    std::size_t                     fftSize;
    std::size_t                     numBins;
    std::size_t                     hardwareSampleRate;
    int								mActiveSurface;
    int								mBackBufferSurface;
    float                           pixelSpecMag;
    float                           pixelDispMag;
    float                           tickLabelYOrigin;
    float                           tickLabelYCenter;
    float                           tickLabelYEnd;
    ci::Timer                       mTimer;
    double                          timeEnter;
    double                          timeEnterPrev;
    double                          timeReturn;
    double                          timeExit;
    double                          timeProcess;
    double                          timeHop;
    double                          timeSec1Enter;
    double                          timeSec1Exit;
    double                          timeSec1Process;
    double                          timeSec2Enter;
    double                          timeSec2Exit;
    double                          timeSec2Process;
    double                          timeSec3Enter;
    double                          timeSec3Exit;
    double                          timeSec3Process;
    double                          timeLine;
    double                          timeNode1Start;
    double                          timeNode1StartPrev;
    double                          timeCurrentNodeStart;
    double                          timeCurrentNodeCall;
    double                          timeCurrentNodeQuo;
    double                          timeCurrentNodeRem;
    double                          actualHopRate;
};

} //!namespace cieq

#endif //!CIEQ_INCLUDE_AUDIO_DRAW_H_