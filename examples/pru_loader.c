/*
 * Nixie Cape PRU Standalone Clock Application
 *
 * Copyright 2012, Matt Ranostay. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MATT RANOSTAY ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATT RANOSTAY OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 *or implied, of Matt Ranostay.
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <pruss/prussdrv.h>
#include <pruss/pruss_intc_mapping.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <time.h>

struct pru_data
{
	/* data section - copy to PRU */
	uint8_t *prumem;
};

int main(int argc, char **argv)
{
	//char *tlc5946_clock_pru = "tlc5946_clock.bin";

	if(argc<0)
	{
		printf("Usage: %s pru_program.bin\n",argv[0]);
		return -1;
	}

	printf("prussdrv_init()\n");
	prussdrv_init();

	printf("prussdrv_open()\n");
	if (prussdrv_open(PRU_EVTOUT_0))
	{
		fprintf(stderr, "Cannot setup PRU_EVTOUT_0.\n");
		return -EINVAL;
	}

	if (prussdrv_exec_program(0, argv[0]))
	{
		printf("Non-zero result from prussdrv_pruintc_init\n");
	}

	prussdrv_exit();

	return 0;
}
