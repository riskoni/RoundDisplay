#define LGFX_USE_V1

#include "display.h"
#include "LGFX.h"
#include "CST816D.h"
#include "display_config.h"

#define buf_size 100

/* ---------------------------------------------------------------------------
Local definitions
--------------------------------------------------------------------------- */

static LGFX tft;
static CST816D touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * buf_size];

/* ---------------------------------------------------------------------------
Forward declarations
--------------------------------------------------------------------------- */

static void onFlushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
static void onTouchPadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

/* ---------------------------------------------------------------------------
Public
--------------------------------------------------------------------------- */

void display_init()
{ 
  touch.begin();

  tft.init();
  tft.initDMA();
  tft.startWrite();

  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * buf_size);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = onFlushDisplay;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = onTouchPadRead;
  lv_indev_drv_register(&indev_drv); 

  lv_obj_t *label = lv_label_create( lv_scr_act() );
  lv_label_set_text( label, "Display test" );
  lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
}

void display_loop()
{
    lv_timer_handler();
}

/* ---------------------------------------------------------------------------
Private
--------------------------------------------------------------------------- */

void onFlushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);

  lv_disp_flush_ready(disp);
}

void onTouchPadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  bool touched;
  uint8_t gesture;
  uint16_t touchX, touchY;

  touched = touch.getTouch(&touchX, &touchY, &gesture);

  if (touched){
    data->state = LV_INDEV_STATE_PR;

    data->point.x = touchX;
    data->point.y = touchY;
  }else{
    data->state = LV_INDEV_STATE_REL;
  }
}