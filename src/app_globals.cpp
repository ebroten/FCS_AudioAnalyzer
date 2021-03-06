#include "app_globals.h"

#include <cinder/audio/Context.h>

namespace cieq
{

AppGlobals::AppGlobals(AppEvent& event_processor)
    :mEventProcessor(event_processor)
{}

AppEvent& AppGlobals::getEventProcessor()
{
	return mEventProcessor;
}

cinder::audio::Context& AppGlobals::getAudioContext()
{
	return *ci::audio::Context::master();
}

void AppGlobals::setParamsPtr(ci::params::InterfaceGl* const params)
{
    mParamsPtr = params;
}

ci::params::InterfaceGl* const AppGlobals::getParamsPtr()
{
    return mParamsPtr;
}

} // !namespace cieq