#include <shogun/lib/vw/protobuf_write.h>

using namespace shogun;

CProtobufCacheWriter::CProtobufCacheWriter(const char* fname):
	CVwCacheWriter(fname)
{
	file_stream = new FileOutputStream(fd);
	coded_stream = new CodedOutputStream(file_stream);
}

CProtobufCacheWriter::~CProtobufCacheWriter()
{
	delete coded_stream;
	delete file_stream;
	close(fd);
}
       
void CProtobufCacheWriter::cache_example(const VwExample*& ex)
{
	vwcache::Example ex_cached;

	// Store the label
	vwcache::LabelData ld = ex_cached.ld();
	ld.set_label(ex->ld.label);
	ld.set_weight(ex->ld.weight);
	ld.set_initial(ex->ld.initial);

	// Store the tag
	if (ex->tag.index() > 0)
	{
		char* tag = new char[ex->tag.index()];
		memcpy(tag, ex->tag.begin, ex->tag.index());
			
		*(ex_cached.mutable_tag()) = tag;
	}

	// Loop over namespaces
	for (google::protobuf::uint32* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		vwcache::Channel* channel = ex_cached.add_channels();
		channel->set_index(*i);
		// Loop over features
		for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
		{
			channel->add_diff_index(f->weight_index);
			channel->add_x(f->x);
		}
	}

	google::protobuf::uint32 cached_size = ex_cached.ByteSize();
	coded_stream->WriteVarint32(cached_size);

	if (!ex_cached.SerializeToCodedStream(coded_stream))
		SG_SERROR("Error serializing to coded output stream!\n");

}
