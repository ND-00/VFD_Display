#include "main.h"
#include <string.h>
#include <stdio.h>
#include <FlashSetting.h>

// --- Работа с Flash ---
void save_config_to_flash(const char* ssid, const char* pass) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase;
    uint32_t page_error;
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_5;
    erase.NbSectors = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }

    uint32_t addr = FLASH_CONFIG_ADDR;
    const char* strings[2] = {ssid, pass};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j += 4) {
            uint32_t data = 0xFFFFFFFF;
            memcpy(&data, &strings[i][j], 4);
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data);
            addr += 4;
        }
    }

    HAL_FLASH_Lock();
}

void load_config_from_flash(char* ssid, char* pass) {
    const char* base = (const char*)FLASH_CONFIG_ADDR;
    memcpy(ssid, base + 0, 64);
    memcpy(pass, base + 64, 64);
    ssid[63] = pass[63] = '\0';
}
