Для работы с библиотекой необходимо подключить файл "SSD1306.h" - является основным.

Для выбора типа интерфейса подключения экрана до подключения основного файла или непосредственно в файле "SSD1306.c"

Подключение экрана к контроллеру STM32F103:

SPI1: APB2

VDD = +3.3V
GND = GND

SCK = PA5	(SCK)
SDA = PA7	(MOSI)
___ = PA6	(MISO)

RES = PA2	(самостоятельное назначение)
D/C = PA3	(самостоятельное назначение)
CS  = PA4	(NSS)
