//#define FLOPPY_144_SECTORS_PER_TRACK 18

void resetDisk(u8 drive) {
    __asm__ (
        "int $0x13":
        :"dl"(drive), "a"(0x00)
    );
}
/*
void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    *cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
    *head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
    *sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
}

void readSector(u8 sector_size, u8 track, u8 sector, u8 head, u8 drive,
    u8 *buffer, u8 *status, u8 *sectors_read)
{
    u16 result;

    __asm__ volatile("int $0x13"
        : "=a"(result)
        : "a"(0x200|sector_size), "b"(buffer),
          "c"(track<<8|sector), "d"(head<<8|drive)
        : "memory");

    *status = result >> 8;
    *sectors_read = result >> 0;
}
*/