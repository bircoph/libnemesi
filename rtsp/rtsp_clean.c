#include <nemesi/rtsp.h>
#include <nemesi/etui.h>

#include <fcntl.h>

void rtsp_clean(void *rtsp_thread)
{
	// struct RTSP_Ctrl *rtsp_ctrl = (struct RTSP_Ctrl *)rtsp_control;
	struct RTSP_Thread *rtsp_th = (struct RTSP_Thread *)rtsp_thread;
	int n;
#ifdef USE_UIPRINTF
	char optstr[256];
#endif // USE_UIPRINTF
	struct command *comm = rtsp_th->comm;
	int command_fd = rtsp_th->pipefd[0];
	char ch[1];

	if ( (n = fcntl(command_fd, F_GETFL, 0)) < 0 )
		nmsprintf(2, "fcntl F_GETFL error\n");
	n |= O_NONBLOCK;
	 if (fcntl(command_fd, F_SETFL, n) < 0)
		 nmsprintf(2, "fcntl F_SETFL error\n");
#if 1 // We must read last teardown reply from server
	nmsprintf(2, "Waiting for last Teardown response\n");
	if((n=read(command_fd, ch, 1)) == 1)
		if(cmd[comm->opcode] (rtsp_th, comm->arg))
			return;
	if( (*(rtsp_th->waiting_for)) && (rtsp_th->fd != -1) ) {
		if ((n=rtsp_recv(rtsp_th)) < 0)
			nmsprintf(1, "No teardown response received\n");
		else if (n > 0){
			if (full_msg_rcvd(rtsp_th))
				if (handle_rtsp_pkt(rtsp_th))
					nmsprintf(1, "\nError!\n");
		} else {
			nmsprintf(1, "\nServer died prematurely!\n");
			rtsp_th->busy=0;
		}
	}
#endif
	reinit_rtsp(rtsp_th);
	nmsprintf(2, "RTSP Thread R.I.P.\n");
#ifdef USE_UIPRINTF
	fprintf(stderr, "\r"); /* TODO Da ottimizzare */
	while((n=read(UIINPUT_FILENO, optstr, 1)) > 0)
		write(STDERR_FILENO, optstr, n);
#endif // USE_UIPRINTF
}
