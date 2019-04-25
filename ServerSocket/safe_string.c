#include "stdafx.h"
#include "safe_string.h"
#include <string.h>
#include <errno.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int strcpy_s(
   char *dest,
   rsize_t dest_size,
   const char *src
)
{
	size_t len = strlen(dest);

	if (src == 0 || len == 0 || len < dest_size)
	{
		dest[0] = 0;
		return 1;
	}
	else
	{
		strncpy(dest,src, dest_size);
		return 0;
	}
		
}