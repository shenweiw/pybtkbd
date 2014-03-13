

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#define	PSMHIDCTL	  0x11
#define	PSMHIDINT  0x13

struct bthidd_t {
	int serv_ctrl;
	int serv_intr;
	int sock_ctrl;
	int sock_intr;
	int b_shutdown;
};

// Wrapper for bind, caring for all the surrounding variables
static int bth_bind(int sockfd, unsigned short port) {
	struct sockaddr_l2 l2a;
	int i;
	memset(&l2a, 0, sizeof(l2a));
	l2a.l2_family = AF_BLUETOOTH;
	bacpy(&l2a.l2_bdaddr, BDADDR_ANY);
	l2a.l2_psm = htobs(port);
	i = bind(sockfd, (struct sockaddr *)&l2a, sizeof(l2a));
	if (0 > i) {
		fprintf(stderr, "Bind error (PSM %d): %s.\n", port, strerror(errno));
	}
	return i;
}

struct bthidd_t * bthidd_init(void) {
	struct bthidd_t * p_hidd;
	int ret;

	p_hidd = malloc(sizeof(struct bthidd_t));
	if (!p_hidd) { goto _err; }
	memset(p_hidd, 0, sizeof(struct bthidd_t));

	// Open, bind and listen socket for HID-Control.
	p_hidd->serv_ctrl = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (p_hidd->serv_ctrl < 0) {
		fprintf(stderr, "Failed to generate bluetooth socket for HID-Control.\n");
		goto _err;
	}
	ret = bth_bind(p_hidd->serv_ctrl, PSMHIDCTL);
	if (ret) {
		fprintf(stderr, "Failed to bind sockets (%d) to PSM (%d).\n",
			p_hidd->serv_ctrl, PSMHIDCTL);
		goto _err;
	}
	ret = listen(p_hidd->serv_ctrl, 1);
	if (ret) {
		fprintf(stderr, "Failed to listen on HID-Control BT socket.\n");
		goto _err_with_ctrl;
	}

	// Open, bind and listen socket for HID-Interrupt.
	p_hidd->serv_intr = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (p_hidd->serv_intr < 0) {
		fprintf(stderr, "Failed to generate bluetooth socket for HID-Interrupt.\n");
		goto _err_with_ctrl;
	}
	ret = bth_bind(p_hidd->serv_intr, PSMHIDINT);
	if (ret) {
		fprintf(stderr, "Failed to bind sockets (%d) to PSM (%d).\n",
			p_hidd->serv_intr, PSMHIDINT);
		goto _err_with_ctrl;
	}
	ret = listen(p_hidd->serv_intr, 1);
	if (ret) {
		fprintf(stderr, "Failed to listen on HID-Interrupt BT socket.\n");
		goto _err_with_intr;
	}
	
	fprintf(stdout, "%s() %d: The End. handle=%p\n", __FUNCTION__, __LINE__, p_hidd);
	fprintf(stdout, "%s() %d: contol=%x interrupt=%x\n", __FUNCTION__, __LINE__, p_hidd->serv_ctrl, p_hidd->serv_intr);
	return p_hidd;

_err_with_intr:
	close(p_hidd->serv_intr);
_err_with_ctrl:
	close(p_hidd->serv_ctrl);
_err:
	fprintf(stdout, "%s() %d: Error.\n", __FUNCTION__, __LINE__);
	return NULL;
}

void bthidd_exit(struct bthidd_t *p_hidd) {
	if (p_hidd->sock_intr) {
		close(p_hidd->sock_intr);
		p_hidd->sock_intr = 0;
	}
	if (p_hidd->sock_ctrl) {
		close(p_hidd->sock_ctrl);
		p_hidd->sock_ctrl = 0;
	}
	if (p_hidd->serv_intr) {
		close(p_hidd->serv_intr);
		p_hidd->serv_intr = 0;
	}
	if (p_hidd->serv_ctrl) {
		close(p_hidd->serv_ctrl);
		p_hidd->serv_ctrl = 0;
	}
}

// only set the b_shutdown field.
void bthidd_shutdown(struct bthidd_t *p_hidd, int shutdown) {
	p_hidd->b_shutdown = !!shutdown;
}

// return: True on success.
int bthidd_accept(struct bthidd_t *p_hidd) {
	struct sockaddr_l2	addr_l2_remote_ctrl;
	struct sockaddr_l2	addr_l2_remote_intr;
	socklen_t		addr_l2_len;
	char			bthaddrstr[40];
	int			j, ok=1;
	struct timeval		tv;
	fd_set			fds;

	addr_l2_len = sizeof(addr_l2_remote_ctrl);

	// Wait for connection request from remote to our HID-Control socket.
	while (ok && !p_hidd->b_shutdown) {
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(p_hidd->serv_ctrl, &fds);

		j = select(p_hidd->serv_ctrl + 1, &fds, NULL, NULL, &tv);
		if (j < 0) {
			fprintf(stderr, "select() error on HID-Control BT socket: "
				"%s! Aborting.\n", strerror(errno));
			ok = 0;
			break;
		}
		if (j == 0) {
			// Nothing happend. should goto the beginning.
			continue;
		}

		p_hidd->sock_ctrl = accept(p_hidd->serv_ctrl,
			(struct sockaddr *)&addr_l2_remote_ctrl, &addr_l2_len);
		if (p_hidd->sock_ctrl < 0) {
			fprintf(stderr, "Failed to get a HID-Control connection: "
				"%s.\n", strerror(errno));
			continue;
		}

		ba2str(&addr_l2_remote_ctrl.l2_bdaddr, bthaddrstr);
		bthaddrstr[39] = 0;
		fprintf(stdout, "Incoming connection from node [%s, %d, %d] for HID-Control.\n",
			bthaddrstr, addr_l2_remote_ctrl.l2_psm, addr_l2_remote_ctrl.l2_cid);
		break;
	}

	// Wait for connection request to our HID-Interrupt socket.
	while (ok && !p_hidd->b_shutdown) {
		tv.tv_sec  = 3;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(p_hidd->serv_intr, &fds);
		j = select(p_hidd->serv_intr + 1, &fds, NULL, NULL, &tv);
		if (j < 0) {
			fprintf(stderr, "select() error on HID-Interrupt BT socket: "
				"%s! Aborting.\n", strerror(errno));
			ok = 0;
			break;
		}
		if (j == 0) {
			close(p_hidd->sock_ctrl);
			fprintf(stderr, "Interrupt connection failed to "
				"establish (HID-Control connection already"
				" there), timeout!\n");
			ok = 0;
			break;
		}

		p_hidd->sock_intr = accept(p_hidd->serv_intr,
			(struct sockaddr *)&addr_l2_remote_intr, &addr_l2_len);
		if (p_hidd->sock_intr < 0) {
			close(p_hidd->sock_intr);
			fprintf(stderr, "Failed to get an interrupt connection: "
				"%s.\n", strerror(errno));
			continue;
		}

		ba2str(&addr_l2_remote_intr.l2_bdaddr, bthaddrstr);
		bthaddrstr[39] = 0;
		fprintf(stdout, "Incoming connection from node [%s, %d, %d] for HID-Interrupt.\n",
			bthaddrstr, addr_l2_remote_intr.l2_psm, addr_l2_remote_intr.l2_cid);
		break;
	}

	return ok;
}

int bthidd_ctrl_send(struct bthidd_t *p_hidd, char *data, int len) {
	return send(p_hidd->sock_ctrl, data, len, MSG_NOSIGNAL);
}

int bthidd_intr_send(struct bthidd_t *p_hidd, char *data, int len) {
	return send(p_hidd->sock_intr, data, len, MSG_NOSIGNAL);
}


#define	__PACKED__	__attribute((packed))
#define	REPORTID_MOUSE	1
#define	REPORTID_KEYBD	2

// Mouse HID report, as sent over the wire:
struct hidrep_mouse_t {
	unsigned char	btcode;	// Fixed value for "Data Frame": 0xA1
	unsigned char	rep_id;	// Will be set to REPORTID_MOUSE for "mouse"
	unsigned char	button;	// bits 0..2 for left,right,middle, others 0
	signed   char	axis_x;	// relative movement in pixels, left/right
	signed   char	axis_y;	// dito, up/down
	signed   char	axis_z;	// Used for the scroll wheel (?)
} __PACKED__;

// Keyboard HID report, as sent over the wire:
struct hidrep_keyb_t {
	unsigned char	btcode;	// Fixed value for "Data Frame": 0xA1
	unsigned char	rep_id;	// Will be set to REPORTID_KEYBD for "keyboard"
	unsigned char	modify;	// Modifier keys (shift, alt, the like)
	unsigned char	key[8];	// Currently pressed keys, max 8 at once
} __PACKED__;

void test_send_hid_mouse_report(int sockintr) {
	struct hidrep_mouse_t evmouse;
	int n;

	memset(&evmouse, 0, sizeof(evmouse));
	evmouse.btcode = 0xA1; // always this constant value.
	evmouse.rep_id = REPORTID_MOUSE;
	evmouse.button = 0 & 0x07;
	evmouse.axis_x = 10;
	evmouse.axis_y = 10;
	evmouse.axis_z = 0;

	n = send(sockintr, &evmouse, sizeof(evmouse), MSG_NOSIGNAL);
	if (n != sizeof(evmouse)) {
		fprintf(stderr, "send() failed. n=%d[%d]\n", n, (int)sizeof(evmouse));
	} else {
		fprintf(stdout, "send mouse event, OK.\n");
	}
}

int main(int argc, char *argv[]) {
	int i;
	struct bthidd_t *p;

	p = bthidd_init();
	if (p == NULL) { return -1; }

	i = bthidd_accept(p);
	if (!i) { return -1; }

	for (i=0; i<100; i++) {
		test_send_hid_mouse_report(p);
		sleep(1);
	}

	bthidd_exit(p);
	return 0;
}



