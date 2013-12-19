/* This code has been adapted from the Zlib code
 * provided by Peter J Jones and Jack Lloyd
 */

#ifndef GZIP_H
#define GZIP_H

#include <botan/filter.h>

namespace i2pcpp {

using namespace Botan;

/**
* Gzip Compression Filter
*/
class Gzip_Compression : public Botan::Filter
   {
   public:
      std::string name() const { return "Gzip_Compression"; }

      void write(const byte input[], size_t length);
      void start_msg();
      void end_msg();

      /**
      * Flush the compressor
      */
      void flush();

      /**
      * @param level how much effort to use on compressing (0 to 9);
      *        higher levels are slower but tend to give better
      *        compression
      * @param raw_deflate if true no gzip header/trailer will be used
      */
      Gzip_Compression(size_t level = 6,
                       bool raw_deflate = false);

      ~Gzip_Compression() { clear(); }
   private:
      void clear();
      const size_t level;
      const bool raw_deflate;

			secure_vector<byte> buffer;
      class Gzip_Stream* gzip;
   };

/**
* Gzip Decompression Filter
*/
class Gzip_Decompression : public Filter
   {
   public:
      std::string name() const { return "Gzip_Decompression"; }

      void write(const byte input[], size_t length);
      void start_msg();
      void end_msg();

      Gzip_Decompression(bool raw_deflate = false);
      ~Gzip_Decompression() { clear(); }
   private:
      void clear();

      const bool raw_deflate;

			secure_vector<byte> buffer;
      class Gzip_Stream* gzip;
      bool no_writes;
   };

}

#endif
