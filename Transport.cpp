#include "Transport.h"

namespace i2pcpp {
	boost::signals2::connection Transport::registerEstablishedHandler(EstablishedSignal::slot_type const &eh)
	{
		return m_establishedSignal.connect(eh);
	}

	boost::signals2::connection Transport::registerReceivedHandler(ReceivedSignal::slot_type const &rh)
	{
		return m_receivedSignal.connect(rh);
	}
}
