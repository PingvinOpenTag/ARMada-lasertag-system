RU
Для сборки проекта Вам потребуется IDE CooCox version 1.7
http://www.coocox.org/download/Tools/CoIDE-1.7.8.exe
Скачайте содержимое репозитория, разархивируйте.
Установите и запустите CooCox 1.7.8
Откройте файл проекта по следующему пути  Firmware/DeveloperPreview/rtos_103_SDIO_FAT_complite/rtos.coproj

Параметры сборки
В файле /Firmware/DeveloperPreview/rtos_103_SDIO_FAT_complite/types.h
есть несколько важных макросов:
макрос #define DEVICE_ROLE определяет, для какого устройства должна быть собрана прошивка, возможны два значения макроса

#define DEVICE_ROLE TAG - прошивка будет собираться для оружия

#define DEVICE_ROLE BANDANA - прошивка будет собрана для повязки

ненужный макрос обязательно должен быть отключен (закомментирован).




EN
To build the project, you need a IDE CooCox version 1.7
http://www.coocox.org/download/Tools/CoIDE-1.7.8.exe
