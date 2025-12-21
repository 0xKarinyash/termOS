#ifndef SERIAL_H
#define SERIAL_H

int serial_init();
void serial_writec(char chr);
void serial_write(const char *str);

#endif