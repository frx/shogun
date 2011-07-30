#include <shogun/lib/vw/parse_example.h>
#include <shogun/lib/vw/vw_parser.h>
#include <shogun/lib/vw/protobuf_write.h>

using namespace shogun;

int32_t VwParser::read_features(CIOBuffer* buf, VwExample*& ae)
{
	ae = new VwExample();

	char *line=NULL;
	int num_chars = buf->read_line(line);
	if (num_chars == 0)
		return num_chars;

	/* Mark begin and end of example in the buffer */
	substring example_string = {line, line + num_chars};

	/* Channels containing separate namespaces/label information*/
	v_array<substring> channels;
	v_array<substring> words;
	channels.erase();

	/* Split at '|' character */
	tokenize('|', example_string, channels);

	/* If first char is not '|', then the first channel contains label data */
	substring* feature_start = &channels[1];

	if (*line == '|')
		feature_start = &channels[0]; /* Unlabelled data */
	else
	{
		/* First channel has label info */
		substring label_space = channels[0];
		char* tab_location = safe_index(label_space.start, '\t', label_space.end);
		if (tab_location != label_space.end)
			label_space.start = tab_location+1;

		/* Split the label space on spaces */
		tokenize(' ',label_space,words);
		if (words.index() > 0 && words.last().end == label_space.end) //The last field is a tag, so record and strip it off
		{
			substring tag = words.pop();
			ae->tag.push_many(tag.start, tag.end - tag.start);
		}

		(ae->ld).parse_label(words);
		set_minmax((ae->ld).label);
	}

	size_t mask = env->mask;

	/* Now parse the individual channels, i.e., namespaces */
	for (substring* i = feature_start; i != channels.end; i++)
	{
		substring channel = *i;

		tokenize(' ',channel, words);
		if (words.begin == words.end)
			continue;

		/* Set default scale value for channel */
		float channel_v = 1.;
		size_t channel_hash;
		char* base=NULL;
		/* Index by which to refer to the namespace */
		size_t index = 0;
		bool new_index = false;
		size_t feature_offset = 0;

		v_array<substring> name;

		if (channel.start[0] != ' ')
		{
			/* Nonanonymous namespace specified */
			feature_offset++;
			feature_value(words[0], name, channel_v);

			if (name.index() > 0)
			{
				index = (unsigned char)(*name[0].start);
				if (ae->atomics[index].begin == ae->atomics[index].end)
					new_index = true;
			}
			channel_hash = hasher(name[0], hash_base);
		}
		else
		{
			/* Use default namespace with index below */
			index = (unsigned char)' ';
			if (ae->atomics[index].begin == ae->atomics[index].end)
				new_index = true;
			channel_hash = 0;
		}

		for (substring* i = words.begin+feature_offset; i != words.end; i++)
		{
			/* Get individual features and multiply by scale value */
			float v;
			feature_value(*i, name, v);
			v *= channel_v;

			/* Hash feature */
			size_t word_hash = (hasher(name[0], channel_hash)) & mask;
			VwFeature f = {v,word_hash};
			ae->sum_feat_sq[index] += v*v;
			ae->atomics[index].push(f);
		}

		/* Add index to list of indices if required */
		if (new_index && ae->atomics[index].begin != ae->atomics[index].end)
			ae->indices.push(index);


	}

	if (write_cache)
		cache_writer->cache_example(ae);
	
	return num_chars;
}
