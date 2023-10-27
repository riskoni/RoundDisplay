#include <lvgl.h>
#include <LovyanGFX.hpp>
#include "display_config.h"

class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 80000000;         // 80Mhz
      cfg.freq_read = 20000000;          // SPI time
      cfg.spi_3wire = true;              // MOSI pin - true
      cfg.use_lock = true;               
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 6;  
      cfg.pin_mosi = 7;  
      cfg.pin_miso = -1; //Disabled
      cfg.pin_dc = 2;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {                                     
      auto cfg = _panel_instance.config(); 

      cfg.pin_cs = CS_PIN;
      cfg.pin_rst = -1;  //Disabled
      cfg.pin_busy = -1; //Disabled

      cfg.memory_width = 240; 
      cfg.memory_height = 240;
      cfg.panel_width = 240;
      cfg.panel_height = 240;
      cfg.offset_x = 0; 
      cfg.offset_y = 0; 
      cfg.offset_rotation = 0; 
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true; 
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;

      _panel_instance.config(cfg);
    }

    {                                      // Set backlight control. (delete if not necessary)
      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = BACKLIGHT_PIN;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 1; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }

    setPanel(&_panel_instance);
  }
};