/* This code has been adapted from the Zlib code
 * provided by Peter J Jones and Jack Lloyd
 */

#include <i2pcpp/util/gzip.h>

#include <botan/exceptn.h>

#include <cstring>
#include <cstdlib>
#include <map>
#include <zlib.h>

namespace i2pcpp {

namespace {

/*
* Allocation Information for Gzip
*/
class Gzip_Alloc_Info
   {
   public:
      std::map<void*, size_t> current_allocs;
   };

/*
* Allocation Function for Gzip
*/
void* gzip_malloc(void* info_ptr, unsigned int n, unsigned int size)
   {
   Gzip_Alloc_Info* info = static_cast<Gzip_Alloc_Info*>(info_ptr);

   const size_t total_sz = n * size;

   void* ptr = std::malloc(total_sz);
   info->current_allocs[ptr] = total_sz;
   return ptr;
   }

/*
* Allocation Function for Gzip
*/
void gzip_free(void* info_ptr, void* ptr)
   {
   Gzip_Alloc_Info* info = static_cast<Gzip_Alloc_Info*>(info_ptr);
   auto i = info->current_allocs.find(ptr);
   if(i == info->current_allocs.end())
      throw Invalid_Argument("gzip_free: Got pointer not allocated by us");

   std::memset(ptr, 0, i->second);
   std::free(ptr);
   }

}

/**
* Wrapper Type for Gzip z_stream
*/
class Gzip_Stream
   {
   public:
      /**
      * Underlying stream
      */
      z_stream stream;

      /**
      * Constructor
      */
      Gzip_Stream()
         {
         std::memset(&stream, 0, sizeof(z_stream));
         stream.zalloc = gzip_malloc;
         stream.zfree = gzip_free;
         stream.opaque = new Gzip_Alloc_Info;
         }

      /**
      * Destructor
      */
      ~Gzip_Stream()
         {
         Gzip_Alloc_Info* info = static_cast<Gzip_Alloc_Info*>(stream.opaque);
         delete info;
         std::memset(&stream, 0, sizeof(z_stream));
         }
   };

/*
* Gzip_Compression Constructor
*/
Gzip_Compression::Gzip_Compression(size_t l, bool raw_deflate) :
   level((l >= 9) ? 9 : l),
   raw_deflate(raw_deflate),
   buffer(DEFAULT_BUFFERSIZE),
   gzip(0)
   {
   }

/*
* Start Compressing with Gzip
*/
void Gzip_Compression::start_msg()
   {
   clear();
   gzip = new Gzip_Stream;

   int res = deflateInit2(&(gzip->stream),
                          level,
                          Z_DEFLATED,
                          (raw_deflate ? -15 : (MAX_WBITS + 16)),
                          8,
                          Z_DEFAULT_STRATEGY);

   if(res == Z_STREAM_ERROR)
      throw Invalid_Argument("Bad setting in deflateInit2");
   else if(res != Z_OK)
      throw Memory_Exhaustion();
   }

/*
* Compress Input with Gzip
*/
void Gzip_Compression::write(const byte input[], size_t length)
   {
   gzip->stream.next_in = static_cast<Bytef*>(const_cast<byte*>(input));
   gzip->stream.avail_in = length;

   while(gzip->stream.avail_in != 0)
      {
      gzip->stream.next_out = static_cast<Bytef*>(&buffer[0]);
      gzip->stream.avail_out = buffer.size();
      deflate(&(gzip->stream), Z_NO_FLUSH);
      send(&buffer[0], buffer.size() - gzip->stream.avail_out);
      }
   }

/*
* Finish Compressing with Gzip
*/
void Gzip_Compression::end_msg()
   {
   gzip->stream.next_in = 0;
   gzip->stream.avail_in = 0;

   int rc = Z_OK;
   while(rc != Z_STREAM_END)
      {
      gzip->stream.next_out = reinterpret_cast<Bytef*>(&buffer[0]);
      gzip->stream.avail_out = buffer.size();

      rc = deflate(&(gzip->stream), Z_FINISH);
      send(&buffer[0], buffer.size() - gzip->stream.avail_out);
      }

   clear();
   }

/*
* Flush the Gzip Compressor
*/
void Gzip_Compression::flush()
   {
   gzip->stream.next_in = 0;
   gzip->stream.avail_in = 0;

   while(true)
      {
      gzip->stream.avail_out = buffer.size();
      gzip->stream.next_out = reinterpret_cast<Bytef*>(&buffer[0]);

      deflate(&(gzip->stream), Z_FULL_FLUSH);
      send(&buffer[0], buffer.size() - gzip->stream.avail_out);

      if(gzip->stream.avail_out == buffer.size())
        break;
      }
   }

/*
* Clean up Compression Context
*/
void Gzip_Compression::clear()
   {
   zeroise(buffer);

   if(gzip)
      {
      deflateEnd(&(gzip->stream));
      delete gzip;
      gzip = 0;
      }
   }

/*
* Gzip_Decompression Constructor
*/
Gzip_Decompression::Gzip_Decompression(bool raw_deflate) :
   raw_deflate(raw_deflate),
   buffer(DEFAULT_BUFFERSIZE),
   gzip(0),
   no_writes(true)
   {
   }

/*
* Start Decompressing with Gzip
*/
void Gzip_Decompression::start_msg()
   {
   clear();
   gzip = new Gzip_Stream;

   if(inflateInit2(&(gzip->stream), (raw_deflate ? -15 : (MAX_WBITS + 32))) != Z_OK)
      throw Memory_Exhaustion();
   }

/*
* Decompress Input with Gzip
*/
void Gzip_Decompression::write(const byte input_arr[], size_t length)
   {
   if(length) no_writes = false;

   // non-const needed by gzip api :(
   Bytef* input = reinterpret_cast<Bytef*>(const_cast<byte*>(input_arr));

   gzip->stream.next_in = input;
   gzip->stream.avail_in = length;

   while(gzip->stream.avail_in != 0)
      {
      gzip->stream.next_out = reinterpret_cast<Bytef*>(&buffer[0]);
      gzip->stream.avail_out = buffer.size();

      int rc = inflate(&(gzip->stream), Z_SYNC_FLUSH);

      if(rc != Z_OK && rc != Z_STREAM_END)
         {
         clear();
         if(rc == Z_DATA_ERROR)
            throw Decoding_Error("Gzip_Decompression: Data integrity error");
         else if(rc == Z_NEED_DICT)
            throw Decoding_Error("Gzip_Decompression: Need preset dictionary");
         else if(rc == Z_MEM_ERROR)
            throw Memory_Exhaustion();
         else
            throw std::runtime_error("Gzip decompression: Unknown error");
         }

      send(&buffer[0], buffer.size() - gzip->stream.avail_out);

      if(rc == Z_STREAM_END)
         {
         size_t read_from_block = length - gzip->stream.avail_in;

                 clear();
                 gzip = new Gzip_Stream;
                 if(inflateInit2(&(gzip->stream), (raw_deflate ? -15 : (MAX_WBITS + 32))) != Z_OK) {
                     throw Memory_Exhaustion();
                 }

         gzip->stream.next_in = input + read_from_block;
         gzip->stream.avail_in = length - read_from_block;

         input += read_from_block;
         length -= read_from_block;
         }
      }
   }

/*
* Finish Decompressing with Gzip
*/
void Gzip_Decompression::end_msg()
   {
   if(no_writes) return;
   gzip->stream.next_in = 0;
   gzip->stream.avail_in = 0;

   int rc = Z_OK;

   while(rc != Z_STREAM_END)
      {
      gzip->stream.next_out = reinterpret_cast<Bytef*>(&buffer[0]);
      gzip->stream.avail_out = buffer.size();
      rc = inflate(&(gzip->stream), Z_SYNC_FLUSH);

      if(rc != Z_OK && rc != Z_STREAM_END)
         {
         clear();
         throw Decoding_Error("Gzip_Decompression: Error finalizing");
         }

      send(&buffer[0], buffer.size() - gzip->stream.avail_out);
      }

   clear();
   }

/*
* Clean up Decompression Context
*/
void Gzip_Decompression::clear()
   {
   zeroise(buffer);

   no_writes = true;

   if(gzip)
      {
      inflateEnd(&(gzip->stream));
      delete gzip;
      gzip = 0;
      }
   }

}
