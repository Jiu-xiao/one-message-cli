
#include "om_cli.hpp"

#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

#include "om_com.h"
#include "om_fmt.h"
#include "om_msg.h"

om_topic_t *topic = NULL;
std::string *name = NULL;
uint32_t topic_len = 0;
om_com_t com;
om_com_map_item_t map[10];
uint8_t fifo_buff[4096], prase_buff[4096], read_buff[4096], write_buff[4096];
void *msg_pack = NULL;

void quit(int value) {
  if (topic) {
    om_msg_del_topic(topic);
  }

  if (name) {
    delete name;
  }

  if (msg_pack) {
    free(msg_pack);
  }

  exit(value);
}

void create_topic() {
  if (topic) {
    printf("Error: Invalid arguments.\n");
    quit(-1);
  }

  if (name && topic_len) {
    topic = om_config_topic(NULL, "CA", name->c_str(), topic_len);
    om_com_create_static(&com, fifo_buff, sizeof(fifo_buff), map, 10,
                         prase_buff, sizeof(prase_buff));
    om_com_add_topic_with_name(&com, name->c_str());
    msg_pack = malloc(sizeof(om_com_raw_type_t) + topic_len);
  }
}

int kbhit(void) {
  fd_set rfds;
  struct timeval tv;
  int retval = 0;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  /* Wait up to five seconds. */
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  retval = select(1, &rfds, NULL, NULL, &tv);
  /* Don't rely on the value of tv now! */

  if (retval == -1) {
    perror("select()");
    return 0;
  } else if (retval) {
    return 1;
    /* FD_ISSET(0, &rfds) will be true. */
  } else {
    return 0;
  }
  return 0;
}

int main(int argc, char **argv) {
  om_init();

  int arg_num = argc - 1;
  if (arg_num) {
    argv++;
  } else {
    printf("Usage: %s [options]\n", argv[0]);
    printf("Options:\n");
    printf(" -n <name>     topic name\n");
    printf(" -l <length>   length of topic data\n");
    printf(" -m <message>  message to send\n");
    printf(" -r <recvice>  start to receive\n");
    printf(" -t <transmit> start to transmit\n");
    quit(-1);
  }

  while (arg_num) {
    if (strlen(*argv) != 2 || (*argv)[0] != '-') {
      printf("Error: Invalid arguments.\n");
      quit(-1);
    }

    switch ((*argv)[1]) {
      case OM_CLI_ARG_TOPIC_NAME: {
        arg_num--;
        argv++;

        if (!arg_num) {
          printf("Error: Invalid arguments.\n");
          quit(-1);
        }

        name = new std::string(*argv);
        arg_num--;
        argv++;

        create_topic();
        break;
      }
      case OM_CLI_ARG_TOPIC_LENGTH:
        arg_num--;
        argv++;

        if (!arg_num) {
          printf("Error: Invalid arguments.\n");
          quit(-1);
        }

        topic_len = std::stoi(*argv);
        arg_num--;
        argv++;

        create_topic();
        break;
      case OM_CLI_ARG_MESSAGE:
        arg_num--;
        argv++;
        if (!topic || !arg_num) {
          printf("Error: Invalid arguments.\n");
          quit(-1);
        }
        strcpy(reinterpret_cast<char *>(write_buff), *argv);
        om_publish(topic, write_buff, topic_len, true, false);
        om_com_generate_pack(topic, msg_pack);
        for (int i = 0; i < sizeof(om_com_raw_type_t) + topic_len; i++) {
          (void)putc((static_cast<uint8_t *>(msg_pack))[i], stdout);
        }
        arg_num--;
        argv++;
        break;
      case OM_CLI_ARG_RECV:
        arg_num--;
        argv++;
        while (true) {
          usleep(1000);
          int len = getchar();
          if (len < 0) {
            quit(-1);
          }
          uint8_t data = len;
          auto ans = om_com_prase_recv(&com, &data, 1, true, false);
          if (ans == OM_COM_RECV_SUCESS) {
            for (int i = 0; i < topic_len; i++) {
              (void)putc((static_cast<uint8_t *>(topic->msg.buff))[i], stdout);
            }
            (void)fflush(stdout);
          }
        }

        break;

      case OM_CLI_ARG_TRANS: {
        arg_num--;
        argv++;
        uint32_t index = 0;
        system("stty -icanon");
        system("stty -echo");
        while (true) {
          usleep(1000);
          int len = getchar();
          if (len < 0) {
            continue;
          }
          read_buff[index++] = len;
          uint8_t data = len;
          if (index < topic_len) {
            continue;
          }
          index = 0;
          om_publish(topic, read_buff, topic_len, true, false);
          om_com_generate_pack(topic, msg_pack);
          for (int i = 0; i < sizeof(om_com_raw_type_t) + topic_len; i++) {
            (void)putc((static_cast<uint8_t *>(msg_pack))[i], stdout);
          }
          (void)fflush(stdout);
        }
        break;
      }
      default:
        printf("Error: Invalid arguments.\n");
        quit(-1);
    }
  }

  quit(0);
  return 0;
}
