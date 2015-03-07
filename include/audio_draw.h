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

    virtual	void	drawLocal(float shift, float shiftLength) = 0;
    //Added float shift and float shiftLength parameters to allow for window shifting:
    void			draw(float shift, float shiftLength);
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
    void					drawLocal(float shift, float shiftLength);

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
    void drawLocal(float shift, float shiftLength);

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

    void							drawLocal(float shift, float shiftLength) override;
    void							setup(int duration);
    size_t                          getMaxDispBins();

private:
    AudioNodes&						mAudioNodes;
    std::array<ci::Surface32f, 2>	mSpectrals;
    ci::gl::Texture					mTexCache;
    std::size_t						mTexW, mTexH;
    std::size_t						mFrameCounter;
    std::size_t						maxDispBins;
    std::size_t						maxFreqDisp;
    int								mActiveSurface;
    int								mBackBufferSurface;
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
};

} //!namespace cieq

#endif //!CIEQ_INCLUDE_AUDIO_DRAW_H_