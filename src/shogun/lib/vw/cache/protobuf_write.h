#ifndef _VW_PROTOBUF_WRITE_H__
#define _VW_PROTOBUF_WRITE_H__

#include <shogun/lib/vw/cache/cache_write.h>

#include <iostream>
#include <fstream>
#include <string>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "proto/example.pb.pb.h"

using namespace google::protobuf::io;

namespace shogun
{

class ProtobufCacheWriter: public VwCacheWriter
{
public:
	ProtobufCacheWriter(const char* fname, VwEnvironment* env_to_use);

	virtual ~ProtobufCacheWriter();

	virtual void cache_example(VwExample* &ex);

protected:
	/// FileOutputStream from the opened file
	FileOutputStream* file_stream;

	/// Coded stream which wraps the FileOutputStream
	CodedOutputStream* coded_stream;
};

}

#endif
