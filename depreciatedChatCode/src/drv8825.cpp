// filename: drv8825.cpp

#include "focuser/drv8825.h"
#include <chrono>
#include <thread>
#include <iostream>

DRV8825::DRV8825(const std::string& chipName,
                 int dirPin,
                 int stepPin,
                 int enablePin,
                 std::array<int, 3> modePins)
{

  std::string chipPath = "/dev/" + chipName;
  gpioChip = gpiod_chip_open(chipPath.c_str());
  if (!gpioChip)
    {
      throw std::runtime_error("Failed to open GPIO chip: " + chipPath);
    }
  
    /*
     * We request ALL GPIO lines at once.
     * libgpiod v2 works this way by design.
     */
    std::array<unsigned int, 6> offsets = {
        static_cast<unsigned int>(dirPin),
        static_cast<unsigned int>(stepPin),
        static_cast<unsigned int>(enablePin),
        static_cast<unsigned int>(modePins[0]),
        static_cast<unsigned int>(modePins[1]),
        static_cast<unsigned int>(modePins[2])
    };

    dirOffset    = 0;
    stepOffset   = 1;
    enableOffset = 2;
    modeOffsets  = {3, 4, 5};

    gpiod_line_settings* settings = gpiod_line_settings_new();

    gpiod_line_settings_set_direction(settings,
                                      GPIOD_LINE_DIRECTION_OUTPUT);

    // *** THIS IS THE IMPORTANT LINE ***
    // Force push-pull output so STEP gets a real HIGH level
    gpiod_line_settings_set_drive(
        settings,
        GPIOD_LINE_DRIVE_PUSH_PULL
    );

    // Start all outputs LOW (safe default)
    gpiod_line_settings_set_output_value(settings,
					 GPIOD_LINE_VALUE_INACTIVE);

    gpiod_line_config* lineConfig = gpiod_line_config_new();
    
    gpiod_line_config_add_line_settings(
        lineConfig,
        offsets.data(),
        offsets.size(),
        settings
    );

    gpiod_request_config* requestConfig = gpiod_request_config_new();
    gpiod_request_config_set_consumer(requestConfig, "drv8825");

    lineRequest = gpiod_chip_request_lines(
        gpioChip,
        requestConfig,
        lineConfig
    );

    if (!lineRequest)
    {
        throw std::runtime_error("Failed to request GPIO lines");
    }

    // Start disabled for safety
    disable();
}

DRV8825::~DRV8825()
{
    if (lineRequest)
    {
        gpiod_line_request_release(lineRequest);
    }

    if (gpioChip)
    {
        gpiod_chip_close(gpioChip);
    }
}

void DRV8825::writeLine(int offset, bool value)
{
    gpiod_line_value gpioValue =
        value ? GPIOD_LINE_VALUE_ACTIVE
              : GPIOD_LINE_VALUE_INACTIVE;

    gpiod_line_request_set_value(lineRequest, offset, gpioValue);
}

void DRV8825::enable()
{
    writeLine(enableOffset, 1);   // Waveshare board: ENABLE is active HIGH
}

void DRV8825::disable()
{
    writeLine(enableOffset, 0);
}

void DRV8825::setMicrostep(Microstep mode)
{
    /*
     * DRV8825 microstep table:
     *
     * M0 M1 M2
     *  0  0  0  Full
     *  1  0  0  Half
     *  0  1  0  1/4
     *  1  1  0  1/8
     *  0  0  1  1/16
     */

    std::array<int, 3> values = {0, 0, 0};

    switch (mode)
    {
        case Microstep::Full:       values = {0, 0, 0}; break;
        case Microstep::Half:       values = {1, 0, 0}; break;
        case Microstep::Quarter:    values = {0, 1, 0}; break;
        case Microstep::Eighth:     values = {1, 1, 0}; break;
        case Microstep::Sixteenth:  values = {0, 0, 1}; break;
    }

    for (size_t i = 0; i < 3; ++i)
    {
        writeLine(modeOffsets[i], values[i]);
    }
}

void DRV8825::moveSteps(Direction direction,
                        int steps,
                        int stepDelayMicroSeconds)
{
    writeLine(dirOffset,
              direction == Direction::Forward ? 0 : 1);

    enable();

    double stepDelaySeconds = (double)stepDelayMicroSeconds/1000000.0;

    for (int i = 0; i < steps; ++i)
    {
      std::cout << "STEP HIGH\n"; // temp debugging
      writeLine(stepOffset, 1);
      std::this_thread::sleep_for(
	std::chrono::duration<double>(stepDelaySeconds));

      std::cout << "STEP LOW\n"; // temp debugging
      writeLine(stepOffset, 0);
      std::this_thread::sleep_for(
	std::chrono::duration<double>(stepDelaySeconds));
    }

    disable();
}
