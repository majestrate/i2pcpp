#include "i2p.h"

#include <string>
#include <iostream>

#include <botan/botan.h>
#include <botan/pkcs8.h>
#include <botan/auto_rng.h>

#include "ssu/UDPTransport.h"
#include "datatypes/Endpoint.h"

#include <thread>
#include <chrono>

using namespace i2pcpp;
using namespace i2pcpp::SSU;

const string local_ip = "127.0.0.1";
const short local_port = 27335;

namespace i2pcpp {
	I2PContext::I2PContext(string const &database) : m_db(database)
	{
		AutoSeeded_RNG rng;

		string encryptingKey = m_db.getConfigValue("private_encryption_key");
		DataSource_Memory dsm((unsigned char *)encryptingKey.data(), encryptingKey.size());
		m_encryptionKey = dynamic_cast<ElGamal_PrivateKey *>(PKCS8::load_key(dsm, rng, ""));

		string signingKey = m_db.getConfigValue("private_signing_key");
		DataSource_Memory dsm2((unsigned char *)signingKey.data(), signingKey.size());
		m_signingKey = dynamic_cast<DSA_PrivateKey *>(PKCS8::load_key(dsm2, rng, ""));

		string dsaParameters = m_db.getConfigValue("dsa_parameters");
		m_dsaParameters.PEM_decode(dsaParameters);

		BigInt encryptionKeyPublic, signingKeyPublic;
		encryptionKeyPublic = m_encryptionKey->get_y();
		signingKeyPublic = m_signingKey->get_y();

		ByteArray encryptionKeyBytes = BigInt::encode(encryptionKeyPublic), signingKeyBytes = BigInt::encode(signingKeyPublic);
		m_routerIdentity = RouterIdentity(encryptionKeyBytes, signingKeyBytes, Certificate());

		cerr << "My router hash: " << m_routerIdentity.getHash() << "\n";
	}
}

int main()
{
	try {
		Botan::LibraryInitializer init;
		I2PContext ctx("i2p.db");

		UDPTransport u(ctx, Endpoint(local_ip, local_port));
		u.begin();

		/*		RouterInfo ri = ctx.getDatabase().getRouterInfo("zhPja0k1cboGnHbhqO50hNPTVHIRE8b4GMwi7Htey~E=");
					ByteArray rib = ri.getBytes();
					for(auto c: rib)
					cout << c;*/

		this_thread::sleep_for(chrono::seconds(4));
		u.shutdown();
		u.join();
	} catch (runtime_error &e) {
		cerr << "main thread exception: " << e.what() << "\n";
	}

	return 0;
}
