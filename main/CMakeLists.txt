# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         		# list the source files of this component
    INCLUDE_DIRS "."    				# optional, add here public include directories
    PRIV_INCLUDE_DIRS   				# optional, add here private include directories
    REQUIRES           esp_adc esp_wifi nvs_flash WiFi_Fn MQTT_Fn# optional, list the public requirements (component names)
   )
