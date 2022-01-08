#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <time.h>
#include <poll.h>

#define CONTACT_1 "+11234567890"
#define CONTACT_2 "+11234567890"

#define EMERGENCY_MESSAGE \
	"ALERT! User is in danger! Last location is:"

#define VOICE_MESSAGE \
	"ALERT! User has sent a voice recording! Voice message is:"

// Bluetooth connections on MacOS/Linux are represented as just a file in the /dev/ folder,
// which acts like a terminal interface to send and receive data. If we open this file as a
// terminal we can just start read()ing the data we need.
// e.g. this bluetooth module is located at /dev/tty.HC-05-DevB
int terminal_open(const char *port) {
	struct termios term;
	int fd;

	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	if (tcgetattr(fd, &term) < 0) {
		perror("tcgetattr");
		return -1;
	}

	speed_t brate = B9600;

	cfsetispeed(&term, brate);
	cfsetospeed(&term, brate);

	term.c_cflag &= ~PARENB;
	term.c_cflag &= ~CSTOPB;
	term.c_cflag &= ~CSIZE;
	term.c_cflag |= CS8;
	term.c_cflag &= ~CRTSCTS;
	term.c_cflag |= CREAD | CLOCAL;
    term.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    term.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    term.c_oflag &= ~OPOST; // make raw

    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 20;

    if (tcsetattr(fd, TCSANOW, &term) < 0) {
    	perror("tcsetattr");
    	return -1;
    }

    return fd;
}

// Reads a line of text from the file descriptor fd, and stores it into buf.
// Will not write more than bufsize bytes.
int terminal_readline(int fd, char *buf, int bufsize) {
	int numread = 0;

	do {
		int n = read(fd, buf + numread, bufsize);

		if (n == -1) {
			// read error
			perror("read");
			return -1;
		} else if (n == 0) {
			// got no bytes, wait 100ms then try again
			usleep(100 * 1000);
		} else {
			// we got some bytes, may not be the amount we need though
			// increase the number of bytes read
			// then, the loop condition will check if we read up to the buffer size,
			// or if the last char we read is a newline character

			numread += n;

			if (buf[numread-1] == '\n') {
				break;
			}
		}
	} while (numread < bufsize);

	return numread;
}

// Processes a message received from the bluetooth device
void process_message(char *message) {
	if (strncmp("TEXT ", message, 5) == 0) {
		// message starts with "TEXT "

		int contact = message[5] - '0';
		fprintf(stderr, "Messaging contact %d...\n", contact);
		const char *command;

		switch (contact) {

        // sendMessage.scpt is a script that sends a text message
        // using the "Messages" app on my mac

		case 1:
			command = "osascript sendMessage.scpt " CONTACT_1 " '" EMERGENCY_MESSAGE "'";
			break;

		case 2:
			command = "osascript sendMessage.scpt " CONTACT_2 " '" EMERGENCY_MESSAGE "'";
			break;

		default:
			fprintf(stderr, "Unknown contact %d\n", contact);
			break;
		}

		system(command);

	} else if (strncmp("VOICE ", message, 6) == 0) {
		int micval = atoi(message + 6);
		fprintf(stderr, "Messaging contact 1 with mic value %d...\n", micval);

		char cmdbuf[1024];
		snprintf(cmdbuf, 1024, "osascript sendMessage.scpt %s '%s %d'\n", CONTACT_1, VOICE_MESSAGE, micval);
		system(cmdbuf);

	} else {
		// unknown message
		fprintf(stderr, "Unknown message\n");
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s [file]\n", argv[0]);
		return 0;
	}

	int fd = terminal_open(argv[1]);
	if (fd == -1) {
		return 1;
	}

	printf("Reading...\n");

	char buf[1024];

	for (;;) {
		memset(buf, 0, sizeof buf);
		int n = terminal_readline(fd, buf, sizeof buf);
		if (n == -1) {
			return 1;
		}

		buf[n-1] = 0; // we want to replace the newline with a 0

		fprintf(stderr, "Message received: %d bytes, \"%s\"\n", n, buf);
		process_message(buf);
	}
}
