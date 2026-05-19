#define FLASH_CONFIG_ADDR 0x08020000

void save_config_to_flash(const char* ssid, const char* pass);
void load_config_from_flash(char* ssid, char* pass);
