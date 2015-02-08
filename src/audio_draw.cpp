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

void Plot::draw(size_t shift) //Added 'size_t shift' to hopefully allow for window shifting
{
	drawLocal(shift);

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
void SpectrumPlot::drawLocal(size_t shift) //Added 'size_t shift' to hopefully allow for window shifting
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
void WaveformPlot::drawLocal(size_t shift) //Added 'size_t shift' to hopefully allow for window shifting
{
    auto& buffer = mAudioNodes.getMonitorNode()->getBuffer();
    size_t hardwareSampleRate = 0;
    //Get current sample rate of audio hardware:
    hardwareSampleRate = mAudioNodes.getInputDeviceNode()->getSampleRate();
    //convert window shift from seconds to number of samples:
    shift = shift * hardwareSampleRate;

	ci::gl::color(mGraphColor);

	const float waveHeight = mBounds.getHeight() / (float)buffer.getNumChannels();
	const float xScale = mBounds.getWidth() / (float)buffer.getNumFrames();

	float yOffset = mBounds.y1;
	for (std::size_t ch = 0; ch < buffer.getNumChannels(); ch++) {
		ci::PolyLine2f waveform;
		const float *channel = buffer.getChannel(ch);
		float x = mBounds.x1;
        //Added the '- shift' and '+ shift' portions below to allow for window shifting
        for (std::size_t i = 0; i < (buffer.getNumFrames() - shift); i++) {
			x += xScale;
            float y = (1 - (channel[i + shift] * 0.5f + 0.5f)) * waveHeight + yOffset;
            waveform.push_back(ci::Vec2f(x, y));
		}

		if (!waveform.getPoints().empty())
			ci::gl::draw(waveform);

		yOffset += waveHeight;
	}
}

} //!namespace cieq