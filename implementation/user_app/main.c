#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>

// Size fo the buffer
#define BUFFER_SIZE 4096

struct tee_message {
	unsigned char command;
	unsigned char task_id;
	unsigned char payload[4092];
	unsigned char send_init_ipi;
};

const char *argp_program_version = "TEECoreUser v0.1";
const char *argp_program_bug_address = "pascal.scholz@tu-dresden.de";
static char args_doc[] = "[-c COMMAND] [-t TASK_ID]...";
static char doc[] = "User space application to interact with TEECore.";
static struct argp_option options[] = { 
	{ "command", 'c', "COMMAND", 0, "Command to send to TEECore. \nSupporting: none, send "},
	{ "task", 't', "TASK_ID", 0, "TaskID To send to TEECore."},
	{ "payload", 'p', "PAYLOAD", 0, "Payload to send along command and TaskID. Formatted as Hex"},
	{ 0 } 
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct tee_message *message = state->input;
	switch (key) {
	case 'c': 
		if (0 == strcmp("none", arg))
			message->command = 0x0;
		else if (0 == strcmp("send", arg))
			message->command = 0x11;
		break;
	case 't': 
		if (0 == strcmp("unknown", arg))
			message->task_id = 0xff;
		else if (0 == strcmp("ping", arg))
			message->task_id = 0x01; 
		else if (0 == strcmp("read_mem", arg))
			message->task_id = 0x02; 
		else if (0 == strcmp("write_mem", arg))
			message->task_id = 0x03;
 		else if (0 == strcmp("nop_mem", arg))
		 	message->task_id = 0x04;
		else if (0 == strcmp("ipi_attack", arg))
		 	message->task_id = 0x05;
		break;
	case 'p':
		// Parse at most 4094 characters and copy them to the payload field in 
		// the message
		char parse_buffer[5] = {0};
		size_t parse_index = 0;
		size_t bytes_count = 0;
		size_t msg_pld_idx = 0;

		while(('\0' != arg[parse_index]) && (4 > bytes_count) && (4092 > msg_pld_idx)) {
			memset(&parse_buffer, '\0', 5);
			// Copy either 4 bytes or all bytes till the end of arg
			while(('\0' != arg[parse_index]) && (4 > bytes_count)) {
				parse_buffer[bytes_count++] = arg[parse_index++];
			}
			// Copy at most 4 bytes to the payload buffer.
			// Convert them from ASCII to HEX before copy
			long parsed = strtol(&parse_buffer[0], NULL, 16);
			memcpy(
				&(message->payload[msg_pld_idx]), 
				&parsed, 
				4 > bytes_count ? bytes_count : 4
			);
			msg_pld_idx += bytes_count;
			bytes_count = 0;
		}
		break;
	case 'i':
		// Parse at most 4094 characters and copy them to the payload field in 
		// the message
		message->command = 0xde;
		message->task_id = 0xad;
		message->send_init_ipi = 1;
		break;
	case ARGP_KEY_END:
		if ((message->command == 0xA0) || (message->task_id == 0xA0)) {
			argp_failure(state, 1, 0, "required -t and -c. See --help for more information");
			return ARGP_ERR_UNKNOWN;
		}
		return 0;
	default: 
		return ARGP_ERR_UNKNOWN;
	} 
	
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

/// Writes to /dev/tee_dev the contents of *buffer.
/// Writes at most max(len, BUFFER_SIZE) bytes 
int write_to_chardev(struct tee_message *msg, size_t len) {
	FILE *tee_dev  = fopen("/dev/tee_dev", "w");
	if (NULL == tee_dev) {
		return 0;
	}
	// Write not more then BUFFER_SIZE bytes
	size_t bytes_to_write = len > BUFFER_SIZE ? BUFFER_SIZE : len;
	printf("Write %ld bytes to TEECore!\n", bytes_to_write);
	size_t bytes_written = 
		fwrite(msg, sizeof(char), bytes_to_write, tee_dev);
	fclose(tee_dev);
	return 0 ;// bytes_written;
}

int main(int argc, char** argv) {
	struct tee_message message;
	message.command = 0xA0;
	message.task_id = 0xA0;
	memset(message.payload, 0, 4092);
	message.send_init_ipi = 0;

	printf("Hello form userspace!\n");
	int rc = argp_parse(&argp, argc, argv, 0, 0, &message);
	if (0 == rc) {
		printf("Command: %d, TaskID %d\n", message.command, message.task_id);
		write_to_chardev(&message, BUFFER_SIZE);
	}
	return rc;
}