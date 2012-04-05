#ifndef DRIVE_H
#define DRIVE_H

struct partition {
    struct drive *drive;
    unsigned char fs_type;
    unsigned char no;
    struct partition *next;
};

struct drive {
    unsigned char port;
    unsigned char type;
    struct partition *parts;
    struct drive *next;
};

struct drive* find_drives();
void list_drives(struct drive *d);

#endif /* DRIVE_H */