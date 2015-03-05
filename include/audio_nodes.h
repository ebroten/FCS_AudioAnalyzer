#ifndef CIEQ_INCLUDE_AUDIO_NODES_H_
#define CIEQ_INCLUDE_AUDIO_NODES_H_

#include <memory>

namespace cinder 
{
namespace audio 
{
    class InputDeviceNode;
    class MonitorNode;
    class MonitorSpectralNode;
}
} //!ci::audio

namespace cieq
{

class AppGlobals;

/*!
 * \class AudioNodes
 * \brief holds and handles all Cinder audio nodes. Also takes care
 * of initialization of those nodes.
 * \note from my understanding, node in Cinder is a unit of audio
 * processing. For each operation (regardless of being input / output)
 * a node is required. I have three nodes here, one monitoring the raw
 * input and the other one performing FFT on it. The third is reading
 * the input.
 */
class AudioNodes
{
public:
	AudioNodes(AppGlobals&);

	// \brief initializes all nodes and connect them together
    void												setup(size_t userWinSize, bool auto_enable = true);
	// \brief enables reading from input
	void												enableInput();
	// \brief disables reading from input
    void												disableInput();
    // \brief disconnects node from all inputs and/or outputs it may be connected to
    void												disconnectAll();
	// \brief toggles reading from input
	void												toggleInput();
    //Get the number of frequency bins in the current monitorSpectralNode
    size_t                                              getNumBins();
    //Get the FFT Size of the current monitorSpectralNode
    size_t                                              getFftSize();
    //Get the frequency of the last frequency bin in the current monitorSpectralNode
    size_t                                              getMaxFreqDisp();

	// \brief returns a pointer to the node which is reading data from input
	cinder::audio::InputDeviceNode* const				getInputDeviceNode();
	// \brief returns a pointer to the node which is having raw data in it
	cinder::audio::MonitorNode* const					getMonitorNode();
	// \brief returns a pointer to the node which is performing FFT on data
	cinder::audio::MonitorSpectralNode* const			getMonitorSpectralNode();

private:
	std::shared_ptr<cinder::audio::InputDeviceNode>		mInputDeviceNode;
	std::shared_ptr<cinder::audio::MonitorNode>			mMonitorNode;
	std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode;

private:
	AppGlobals&											mGlobals;
	bool												mIsEnabled;
};

} //!cieq

#endif //!CIEQ_INCLUDE_AUDIO_NODES_H_