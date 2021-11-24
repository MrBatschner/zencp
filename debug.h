#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef DEBUG
	#define DEBUGF printf
#else
	void debugs (void *data, ...) {}
	#define DEBUGF debugs
#endif

#endif
