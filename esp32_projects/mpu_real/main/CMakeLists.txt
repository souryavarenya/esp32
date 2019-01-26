set(COMPONENT_SRCS "mpu_real.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(EXTRA_COMPONENT_DIRS $(IDF_PATH)/components/I2Cbus $(IDF_PATH)/components/SPIbus)

register_component()
