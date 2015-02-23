#include "audio_draw.h"
#include "audio_nodes.h"

#include <cinder/audio/Utilities.h>
#include <cinder/audio/MonitorNode.h>
#include <cinder/Font.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/TextureFont.h>

namespace cieq
{

Plot::Plot()
    : mDrawBounds(true)
    , mBoundsColor(0.5f, 0.5f, 0.5f, 1)
{}

void Plot::draw(float shift, float shiftLength) //Added 'size_t shift' to hopefully allow for window shifting
{
    drawLocal(shift, shiftLength);

	if (mDrawBounds)
	{
		drawBounds();
	}

	if (mDrawLabels)
	{
		drawLabels();
	}
}

void Plot::drawBounds()
{
	ci::gl::color(mBoundsColor);
	ci::gl::drawStrokedRect(mBounds);
}

void Plot::drawLabels()
{
	ci::gl::color(0, 0.9f, 0.9f);

	// draw x-axis label
	mTextureFont->drawString(mHorzText, ci::Vec2f(mBounds.x1 + mBounds.getWidth() / 2.0f - mTextureFont->measureString(mHorzText).x / 2.0f, mBounds.y2 + 20.0f));
	
	// draw plot title
	mTextureFont->drawString(mPlotTitle, ci::Vec2f(mBounds.x1 + mBounds.getWidth() / 2.0f - mTextureFont->measureString(mPlotTitle).x / 2.0f, mBounds.y1 - 20.0f));

	// draw y-axis label
	ci::gl::pushModelView();
	ci::gl::translate(mBounds.x1 - 20.0f, mBounds.y1 + mBounds.getHeight() / 2.0f + mTextureFont->measureString(mVertText).x / 2.0f);
	ci::gl::rotate(-90.0f);
	mTextureFont->drawString(mVertText, ci::Vec2f::zero());
	ci::gl::popModelView();
}

void Plot::onHorzAxisTextChange()
{
	mHorzText = mHorzTitle + " (" + mHorzUnit +")";
}

void Plot::onVertAxisTextChange()
{
	mVertText = mVertTitle + " (" + mVertUnit + ")";
}

void Plot::setup()
{
	mTextureFont = ci::gl::TextureFont::create(ci::Font(ci::Font::getDefault().getName(), 16));
}

SpectrumPlot::SpectrumPlot(AudioNodes& nodes)
	: mScaleDecibels( true )
	, mAudioNodes(nodes)
{}

// original draw function is from Cinder examples _audio/common
void SpectrumPlot::drawLocal(float shift, float shiftLength) //Added 'size_t shift' to hopefully allow for window shifting
{
	auto& spectrum = mAudioNodes.getMonitorSpectralNode()->getMagSpectrum();
	
	if (spectrum.empty())
		return;

	ci::ColorA bottomColor(0, 0, 0.7f, 1);

	float width = mBounds.getWidth();
	float height = mBounds.getHeight();
	std::size_t numBins = spectrum.size();
	float padding = 0;
	float binWidth = ( width - padding * ( numBins - 1 ) ) / (float)numBins;

	std::size_t numVerts = spectrum.size() * 2 + 2;
	if( mVerts.size() < numVerts ) {
		mVerts.resize( numVerts );
		mColors.resize( numVerts );
	}

	std::size_t currVertex = 0;
	ci::Rectf bin(mBounds.x1, mBounds.y1, mBounds.x1 + binWidth, mBounds.y2);
	for( std::size_t i = 0; i < numBins; i++ ) {
		float m = spectrum[i];
		if( mScaleDecibels )
			m = ci::audio::linearToDecibel(m) / 100;

		bin.y1 = bin.y2 - m * height;

		mVerts[currVertex] = bin.getLowerLeft();
		mColors[currVertex] = bottomColor;
		mVerts[currVertex + 1] = bin.getUpperLeft();
		mColors[currVertex + 1] = ci::ColorA( 0, m, 0.7f, 1 );

		bin += ci::Vec2f(binWidth + padding, 0);
		currVertex += 2;
	}

	mVerts[currVertex] = bin.getLowerLeft();
	mColors[currVertex] = bottomColor;
	mVerts[currVertex + 1] = bin.getUpperLeft();
	mColors[currVertex + 1] = mColors[currVertex - 1];

	ci::gl::color(0, 0.9f, 0);

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, mVerts.data() );
	glColorPointer( 4, GL_FLOAT, 0, mColors.data() );  // note: on OpenGL ES v1.1, the 'size' param to glColorPointer can only be 4
	glDrawArrays( GL_TRIANGLE_STRIP, 0, (GLsizei)mVerts.size() );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

void SpectrumPlot::enableDecibelsScale(bool on /*= true*/)
{
	mScaleDecibels = on;
}


WaveformPlot::WaveformPlot(AudioNodes& nodes)
	: mGraphColor(0, 0.9f, 0, 1)
	, mAudioNodes(nodes)
{}

void WaveformPlot::setGraphColor(const ci::ColorA& color)
{
	mGraphColor = color;
}

// original draw function is from Cinder examples _audio/common
void WaveformPlot::drawLocal(float shift, float shiftLength) //Added 'size_t shift' to hopefully allow for window shifting
{
    auto& buffer = mAudioNodes.getMonitorNode()->getBuffer();
    size_t hardwareSampleRate = 0;
    //Get current sample rate of audio hardware:
    hardwareSampleRate = mAudioNodes.getInputDeviceNode()->getSampleRate();
    //Get total number of frames in raw audio buffer:
    size_t rawNumFrames = buffer.getNumFrames();
    //Create a variable to use for window size of shifted window in samples:
    size_t shiftLengthSamples = 0;
    //Check to make sure there was an input given for shiftLength:
    if (shiftLength <= 0 || shiftLength == NULL)
    {
        shiftLength = rawNumFrames;
    }
    else
    {
        //Convert window size of shifted window from seconds to nearest whole number of samples:
        shiftLengthSamples = floor(shiftLength * hardwareSampleRate);
    }
    //convert window shift from seconds to nearest whole number of samples:
    size_t shiftInt = floor(shift * hardwareSampleRate);
    //Calculate what number of samples needs to be subtracted from the raw number of samples:
    size_t shiftSub = rawNumFrames - (shiftLengthSamples + shiftInt);
    //Calculate what the new ending sample number should be:
    size_t shiftEnd = rawNumFrames - shiftSub;
    //Need to make sure the shifted window doesn't exceed the raw input window:
    if (shiftEnd > rawNumFrames || shiftEnd < 0)
    {
        shiftEnd = rawNumFrames;
        shiftInt = 0;
    }

	ci::gl::color(mGraphColor);

	const float waveHeight = mBounds.getHeight() / (float)buffer.getNumChannels();
    const float width = mBounds.getWidth();
    //Changed denominator to rescale xScale based on shift window so shifted signal "zooms" to full graph width:
    const float xScale = (mBounds.getWidth() / ((float)shiftEnd - shiftInt));

	float yOffset = mBounds.y1;
	for (std::size_t ch = 0; ch < buffer.getNumChannels(); ch++) 
    {
		ci::PolyLine2f waveform;
		const float *channel = buffer.getChannel(ch);
		float x = mBounds.x1;
        //Added the 'shift' portions below to allow for window shifting:
        for (std::size_t i = 0; i < (shiftEnd - shiftInt); i++) 
        {
			x += xScale;
            float y = (1 - (channel[i + shiftInt] * 0.5f + 0.5f)) * waveHeight + yOffset;
            waveform.push_back(ci::Vec2f(x, y));
		}

		if (!waveform.getPoints().empty())
			ci::gl::draw(waveform);

		yOffset += waveHeight;
	}
}

} //!namespace cieq