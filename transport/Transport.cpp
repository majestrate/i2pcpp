#include "Transport.h"

namespace i2pcpp {
	Transport::Transport() {}
	Transport::~Transport() {}

	boost::signals2::connection Transport::registerEstablishedHandler(EstablishedSignal::slot_type const &eh)
	{
		return m_establishedSignal.connect(eh);
	}

	boost::signals2::connection Transport::registerReceivedHandler(ReceivedSignal::slot_type const &rh)
	{
		return m_receivedSignal.connect(rh);
	}

	boost::signals2::connection Transport::registerFailureSignal(FailureSignal::slot_type const &fs)
	{
		return m_failureSignal.connect(fs);
	}

	boost::signals2::connection Transport::registerDisconnectedSignal(DisconnectedSignal::slot_type const &ds)
	{
		return m_disconnectedSignal.connect(ds);
	}

	boost::signals2::connection Transport::registerTimeoutSignal(TimeoutSignal::slot_type const &ts)
	{
		return m_timeoutSignal.connect(ts);
	}

	void Transport::invokeTimeoutSignal(RouterHash const & rh) {}
}
