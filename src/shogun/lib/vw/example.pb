package vwcache;

message LabelData
{
	optional float label = 1;
	optional float weight = 2 [default = 1.0];
	optional float initial = 3 [default = 0.0];
}

message Feature
{
	optional int32 diff_index = 1;
	optional float x = 2;
}

message Channel
{
	optional uint32 index = 1;
	optional uint32 first_index = 2 [default = 0];
	repeated int32 diff_index = 3 [packed = true];
	repeated float x = 4 [packed = true];	
}

message Example
{
	optional LabelData ld = 1;
	optional string tag = 2;
	repeated Channel channels = 3;
}
	