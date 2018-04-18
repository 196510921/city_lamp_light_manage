#include "app_common.h"

UINT8 text_2_hex(char d)
{
	if(d < '0' || d > 'f')
		return -1;

	if(d >= '0' && d <= '9')
	{
		return (d - '0');
	}
	else if (d >= 'a' && d <= 'f')
	{
		return (d- 'a' + 10);
	}

	return -1;
}


