/* SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-only */
/* SPDX-FileCopyrightText: Copyright Amazon.com, Inc. or its affiliates. All rights reserved. */

#include "efa_errno.h"

#include <stdio.h>

#define IO_COMP_STATUS_MESSAGES(code, suffix, ...)	[EFA_IO_COMP_STATUS_##suffix] = #__VA_ARGS__,
#define PROV_ERRNO_MESSAGES(code, suffix, ...)		[FI_EFA_ERR_##suffix - EFA_PROV_ERRNO_START] = #__VA_ARGS__,

static const char *efa_io_comp_strerror(enum efa_errno status)
{
	static const char *io_comp_status_str[] = {
		EFA_IO_COMP_STATUSES(IO_COMP_STATUS_MESSAGES)
	};

	return EFA_ERRNO_IS_IO_COMP_STATUS(status)
		? io_comp_status_str[status]
		: "Unknown error";
}

static const char *efa_prov_strerror(enum efa_errno err)
{
	static const char *prov_errno_str[] = {
		EFA_PROV_ERRNOS(PROV_ERRNO_MESSAGES)
	};

	return EFA_ERRNO_IS_PROV(err)
		? prov_errno_str[err - EFA_PROV_ERRNO_START]
		: "Unknown error";
}

/**
 * @brief Convert an EFA error code into a short, printable string
 *
 * Given a non-negative EFA-specific error code, this function returns a pointer
 * to a null-terminated string that corresponds to it; suitable for
 * interpolation in logging messages.
 *
 * @param[in]	err    An EFA-specific error code
 * @return	Null-terminated string with static storage duration (caller does
 *		not free).
 */
const char *efa_strerror(enum efa_errno err)
{
	return EFA_ERRNO_IS_PROV(err)
		? efa_prov_strerror(err)
		: efa_io_comp_strerror(err);
}

/**
 * @brief Report a long-form help message to the user
 *
 * Given a non-negative EFA-specific error code, this function prints a
 * long-form help message. These help messages are provided as a best effort
 * attempt at debugging known issues, especially for `EFA_IO_COMP_STATUS_*`
 * errors from RDMA Core.
 *
 * @param[in]	err    An EFA-specific error code
 *
 * @todo As this function grows, we should consider reading these long-form
 * messages from disk, instead of baking the strings in.
 */
void efa_show_help(enum efa_errno err) {
	char *help = "";
	switch (err) {
	case EFA_IO_COMP_STATUS_REMOTE_ERROR_BAD_DEST_QPN:
		help = "This error is detected remotely; "
		"typically encountered when the peer process is no longer present";
		break;
	case EFA_IO_COMP_STATUS_LOCAL_ERROR_UNRESP_REMOTE:
		help = "This error is detected locally; "
		"typically caused by a peer hardware failure or "
		"incorrect inbound/outbound rules in the security group - "
		"EFA requires \"All traffic\" type allowlisting. "
		"Please also verify the peer application has not "
		"terminated unexpectedly.";
		break;
	default:
		return;
	}
	fprintf(stderr, "%s\n", help);
}
