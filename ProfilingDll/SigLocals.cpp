#include "pch.h"
#include "SigLocals.h"

SigLocals::SigLocals(sig_count local_count) : TypeInfo(SIG_LOCAL_SIG)
{
	locals = new TypeInfo *[local_count];
}