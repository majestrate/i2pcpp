#include <iostream>
#include <fstream>
#include <string>
#include <botan/botan.h>
#include <botan/elgamal.h>
#include <botan/rng.h>
#include <botan/pkcs8.h>

using namespace Botan;

#include <memory>
#include <array>

using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		if(argc != 1) {
			return 1;
		}

		ofstream priv("elgpriv.pem");
		ofstream pub("elgpub.pem");
		ofstream params("elgparameters.pem");

		if(!priv || !pub || !params)
		{
			cout << "Couldn't write output files" << std::endl;
			return 1;
		}

		Botan::LibraryInitializer init;

		AutoSeeded_RNG rng;

		DL_Group group("modp/ietf/2048");
		params << group.PEM_encode(DL_Group::DSA_PARAMETERS);

		ElGamal_PrivateKey key(rng, group);

		string v = X509::PEM_encode(key);
		pub << v;

		string s = PKCS8::PEM_encode(key);
		priv << s;

		DataSource_Memory dsm((byte *)s.data(), s.size());
		try {
			ElGamal_PrivateKey *newKey = dynamic_cast<ElGamal_PrivateKey *>(PKCS8::load_key(dsm, rng, ""));
		} catch(Botan::Decoding_Error &e) {
			cerr << e.what() << endl;
		}
	}	catch(Botan::Encoding_Error &e) {
		std::cout << "Exception caught: " << e.what() << std::endl;
	}

	return 0;
}
