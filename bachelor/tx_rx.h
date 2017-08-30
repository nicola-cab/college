#ifndef __tx_rx_h__
#define __tx_rx_h__



int fullwrite(int fd, const void *buf, int count);
int fullread(int fd, void *buf, int count);


int send_data(int fd, const char *buffer, unsigned int num_bytes);
int receive_data(int fd, char *buffer);



#endif // __tx_rx_h__
