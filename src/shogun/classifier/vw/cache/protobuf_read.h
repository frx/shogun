#ifndef _VW_PROTOBUF_READ_H__
#define _VW_PROTOBUF_READ_H__

#include <shogun/classifier/vw/cache/cache_read.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "proto/example.pb.pb.h"

using namespace google::protobuf::io;

namespace shogun
{

class ProtobufCacheReader: public VwCacheReader
{
public:
	ProtobufCacheReader(const char* fname, VwEnvironment* env_to_use);

	ProtobufCacheReader(int f, VwEnvironment* env_to_use);

	virtual ~ProtobufCacheReader();

	virtual bool read_cached_example(VwExample* const ae);

protected:
	/// FileInputStream from the opened file
	FileInputStream* file_stream;

	/// Coded stream which wraps the FileInputStream
	CodedInputStream* coded_stream;
};

}
#endif

	
