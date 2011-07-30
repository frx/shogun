#include <shogun/lib/vw/protobuf_read.h>

using namespace shogun;

ProtobufCacheReader::ProtobufCacheReader(const char* fname):
	VwCacheReader(fname)
{
	file_stream = new FileInputStream(fd);
	coded_stream = new CodedInputStream(file_stream);
	coded_stream->SetTotalBytesLimit(INT_MAX, -1);
}

ProtobufCacheReader::ProtobufCacheReader(int f):
	VwCacheReader(f)
{
	file_stream = new FileInputStream(fd);
	coded_stream = new CodedInputStream(file_stream);
	coded_stream->SetTotalBytesLimit(INT_MAX, -1);	
}

ProtobufCacheReader::~ProtobufCacheReader()
{
	delete coded_stream;
	delete file_stream;
	close(fd);
}

VwExample* ProtobufCacheReader::read_cached_example()
{
	vwcache::Example ex_cached;
	
	google::protobuf::uint32 read_size;
	if (!coded_stream->ReadVarint32(&read_size))
	{
		SG_SINFO("Failed to read object size from cached input!\n");
		return NULL;
	}
	
	CodedInputStream::Limit lim = coded_stream->PushLimit(read_size);
	if (!ex_cached.ParseFromCodedStream(coded_stream))
	{
		SG_SINFO("Failed to parse from cached input!\n");
		return NULL;
	}
	
	coded_stream->PopLimit(lim);

	VwExample* ex = new VwExample();

	// Read label
	ex->ld.label = ex_cached.ld().label();
	ex->ld.weight = ex_cached.ld().weight();
	ex->ld.initial = ex_cached.ld().initial();

	// Read tag
	ex->tag.push_many(ex_cached.mutable_tag()->c_str(), ex_cached.mutable_tag()->length());

	// Read namespaces
	for (int i = 0; i < ex_cached.channels_size(); i++)
	{
		google::protobuf::uint32 index = ex_cached.channels(i).index();
		ex->indices.push(index);
		v_array<VwFeature>* ours = ex->atomics+index;
		float64_t* our_sum_feat_sq = ex->sum_feat_sq+index;

		// Read feature object
		for (int j = 0; j < ex_cached.channels(i).x_size(); j++)
		{
			VwFeature f = {ex_cached.channels(i).x(j), ex_cached.channels(i).diff_index(j)};
			*our_sum_feat_sq += f.x*f.x;
			ours->push(f);
		}
	}

	return ex;
}
			
					 
