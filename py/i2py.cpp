#include <Python.h>
#include <botan/botan.h>
#include <botan/elgamal.h>
#include <botan/rng.h>
#include <botan/pkcs8.h>
#include <botan/dsa.h>

static PyObject * i2py_gen_keys(PyObject * mod)
{
  Botan::AutoSeeded_RNG rng;
  
  Botan::BigInt p("0x9C05B2AA960D9B97B8931963C9CC9E8C3026E9B8ED92FAD0A69CC886D5BF8015FCADAE31A0AD18FAB3F01B00A358DE237655C4964AFAA2B337E96AD316B9FB1CC564B5AEC5B69A9FF6C3E4548707FEF8503D91DD8602E867E6D35D2235C1869CE2479C3B9D5401DE04E0727FB33D6511285D4CF29538D9E3B6051F5B22CC1C93");
  Botan::BigInt q("0xA5DFC28FEF4CA1E286744CD8EED9D29D684046B7");
  Botan::BigInt g("0xC1F4D27D40093B429E962D7223824E0BBC47E7C832A39236FC683AF84889581075FF9082ED32353D4374D7301CDA1D23C431F4698599DDA02451824FF369752593647CC3DDC197DE985E43D136CDCFC6BD5409CD2F450821142A5E6F8EB1C3AB5D0484B8129FCF17BCE4F7F33321C3CB3DBB14A905E7B2B3E93BE4708CBCC82");
  Botan::DSA_PrivateKey dsa_key(rng, Botan::DL_Group(p, q, g));
  
  Botan::ElGamal_PrivateKey elg_key(rng, Botan::DL_Group("modp/ietf/2048"));
  
  PyObject * ret = Py_BuildValue(
				 "(ssss)", 
				 Botan::X509::PEM_encode(dsa_key).c_str(), 
				 Botan::PKCS8::PEM_encode(dsa_key).c_str(),
				 Botan::X509::PEM_encode(elg_key).c_str(),
				 Botan::PKCS8::PEM_encode(elg_key).c_str()
				 );

  ret = ret ? ret : Py_None;
  Py_INCREF(ret); return ret;
}

static PyMethodDef i2py_methods[] = {

  {"gen_keys", (PyCFunction) i2py_gen_keys, METH_NOARGS, "generate keys"},

  {NULL,NULL,0,NULL}
};

static struct PyModuleDef i2py_module = {
  PyModuleDef_HEAD_INIT,
  "i2py",
  NULL,
  -1,
  i2py_methods
};

PyMODINIT_FUNC
PyInit_i2py(void)
{
  Botan::LibraryInitializer init;
  return PyModule_Create(&i2py_module);
}
