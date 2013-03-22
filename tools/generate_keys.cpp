#include <iostream>
#include <fstream>
#include <string>
#include <botan/botan.h>
#include <botan/elgamal.h>
#include <botan/rng.h>
#include <botan/pkcs8.h>
#include <botan/dsa.h>

#include <memory>
#include <array>

using namespace std;
using namespace Botan;

int main(int argc, char* argv[])
{
	Botan::LibraryInitializer init;
	AutoSeeded_RNG rng;

	BigInt p("0x9C05B2AA960D9B97B8931963C9CC9E8C3026E9B8ED92FAD0A69CC886D5BF8015FCADAE31A0AD18FAB3F01B00A358DE237655C4964AFAA2B337E96AD316B9FB1CC564B5AEC5B69A9FF6C3E4548707FEF8503D91DD8602E867E6D35D2235C1869CE2479C3B9D5401DE04E0727FB33D6511285D4CF29538D9E3B6051F5B22CC1C93");
	BigInt q("0xA5DFC28FEF4CA1E286744CD8EED9D29D684046B7");
	BigInt g("0xC1F4D27D40093B429E962D7223824E0BBC47E7C832A39236FC683AF84889581075FF9082ED32353D4374D7301CDA1D23C431F4698599DDA02451824FF369752593647CC3DDC197DE985E43D136CDCFC6BD5409CD2F450821142A5E6F8EB1C3AB5D0484B8129FCF17BCE4F7F33321C3CB3DBB14A905E7B2B3E93BE4708CBCC82");
	DSA_PrivateKey dsa_key(rng, DL_Group(p, q, g));

	ElGamal_PrivateKey elg_key(rng, DL_Group("modp/ietf/2048"));

	std::cout << "DSA public key:\n" << X509::PEM_encode(dsa_key) << '\n';
	std::cout << "DSA private key:\n" << PKCS8::PEM_encode(dsa_key) << '\n';

	std::cout << "ElGamal public key:\n" << X509::PEM_encode(elg_key) << '\n';
	std::cout << "ElGamal private key:\n" << PKCS8::PEM_encode(elg_key);

	return 0;
}
