deps_config := \
	/home/jonas/esp32/esp-idf/components/app_trace/Kconfig \
	/home/jonas/esp32/esp-idf/components/aws_iot/Kconfig \
	/home/jonas/esp32/esp-idf/components/bt/Kconfig \
	/home/jonas/esp32/esp-idf/components/driver/Kconfig \
	/home/jonas/esp32/esp-idf/components/esp32/Kconfig \
	/home/jonas/esp32/esp-idf/components/esp_adc_cal/Kconfig \
	/home/jonas/esp32/esp-idf/components/espmqtt/Kconfig \
	/home/jonas/esp32/esp-idf/components/ethernet/Kconfig \
	/home/jonas/esp32/esp-idf/components/fatfs/Kconfig \
	/home/jonas/esp32/esp-idf/components/freertos/Kconfig \
	/home/jonas/esp32/esp-idf/components/heap/Kconfig \
	/home/jonas/esp32/esp-idf/components/libsodium/Kconfig \
	/home/jonas/esp32/esp-idf/components/log/Kconfig \
	/home/jonas/esp32/esp-idf/components/lwip/Kconfig \
	/home/jonas/esp32/esp-idf/components/mbedtls/Kconfig \
	/home/jonas/esp32/esp-idf/components/openssl/Kconfig \
	/home/jonas/esp32/esp-idf/components/pthread/Kconfig \
	/home/jonas/esp32/esp-idf/components/spi_flash/Kconfig \
	/home/jonas/esp32/esp-idf/components/spiffs/Kconfig \
	/home/jonas/esp32/esp-idf/components/tcpip_adapter/Kconfig \
	/home/jonas/esp32/esp-idf/components/wear_levelling/Kconfig \
	/home/jonas/esp32/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/jonas/esp32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/jonas/esp32/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/jonas/esp32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
