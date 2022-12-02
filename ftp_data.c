#include <stdlib.h>

#include "ftp_data.h"

struct server_status* create_server_status()
{
	struct server_status* status;
	status = (struct server_status*)malloc(sizeof(struct server_status));

	status->is_connected = 0;
	status->socketaddr = -1;

	return status;
}

