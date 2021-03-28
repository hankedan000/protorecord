#pragma once

#include "ProtorecordTypes.pb.h"

namespace protorecord
{
	inline
	unsigned int
	major_version()
	{
		// TODO get this from CMake
		return 0;
	}

	inline
	unsigned int
	minor_version()
	{
		// TODO get this from CMake
		return 1;
	}

	inline
	unsigned int
	patch_version()
	{
		// TODO get this from CMake
		return 0;
	}

	inline
	Version
	this_version()
	{
		Version v;
		v.set_major(major_version());
		v.set_minor(minor_version());
		v.set_patch(patch_version());
		return v;
	}

}// protorecord