#include "card.h"
#include "input.h"
#include "ram.h"
#include <stdio.h>

Card card;


class Mem8Rom : public Mem8
{
public:
    uint8_t value;

    uint8_t get() const override
    {
        return value;
    }

    void setImpl(uint8_t value) override
    {
        if (id >= 0x2000 && id < 0x4000)
        {
            if (value == 0)
                value = 1;
            card.upper_bank = value;
        }
    }
};

static Mem8Rom bootrom[0x100];
static Mem8Rom* rom;
static Mem8Ram sram[0x8000];

void Card::init()
{
    for(uint32_t n=0; n<0x100; n++)
        bootrom[n].id = n | ID_ROM;

    FILE* f = fopen("dmg_boot.bin", "rb");
    if (f)
    {
        uint32_t n = 0;
        while(fread(&bootrom[n].value, sizeof(uint8_t), sizeof(uint8_t), f) > 0)
        {
            n += 1;
        }
        fclose(f);
    }

    //f = fopen("cpu_instrs/individual/02-interrupts.gb", "rb");
    //f = fopen("tetris.gb", "rb");
    f = fopen("zelda.gbc", "rb");
    //f = fopen("cpu_instrs/cpu_instrs.gb", "rb");
    //f = fopen("instr_timing/instr_timing.gb", "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        uint32_t size = ftell(f);
        rom = new Mem8Rom[size];
        fseek(f, 0, SEEK_SET);
        for(uint32_t n=0; n<size; n++)
            rom[n].id = n | ID_ROM;
        uint32_t n = 0;
        while(fread(&rom[n].value, sizeof(uint8_t), sizeof(uint8_t), f) > 0)
        {
            n += 1;
        }
        printf("Card type: %02x\n", rom[0x147].value);
        fclose(f);
        rom[0x03].value = 0x01;
    }
    else
    {
        uint32_t size = 0x8000;
        rom = new Mem8Rom[size];
        fseek(f, 0, SEEK_SET);
        for(uint32_t n=0; n<size; n++)
        {
            rom[n].id = n | ID_ROM;
            rom[n].value = 0xDD;
        }
        for(uint32_t n=0x104; n<0x134; n++)
            rom[n].value = bootrom[n-0x104+0xA8].value;
        uint8_t checksum = 0;
        for(uint32_t n=0x134; n<0x14D; n++)
            checksum -= rom[n].value + 1;
        rom[0x14D].value = checksum;
    }
}

Mem8& Card::getRom(uint16_t address)
{
    if (address >= 0x4000)
        return rom[upper_bank * 0x4000 + (address & 0x3FFF)];
    return rom[address];
}

Mem8& Card::getSRam(uint16_t address)
{
    return sram[address];
}

Mem8& Card::getBoot(uint16_t address)
{
    return bootrom[address];
}
