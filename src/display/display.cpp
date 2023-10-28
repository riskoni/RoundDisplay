#define LGFX_USE_V1

#include "display.h"
#include "display_config.h"
#include "LGFX.h"
#include "CST816D.h"

#define buf_size 100

/* ---------------------------------------------------------------------------
Local definitions
--------------------------------------------------------------------------- */

static LGFX tft;
static CST816D touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * buf_size];

static lv_style_t background_style;
static lv_style_t foreground_style;
static lv_style_t accent_style;
/* ---------------------------------------------------------------------------
Forward declarations
--------------------------------------------------------------------------- */

static void drawMainLayout();
static void initStyles();
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

  tft.setBrightness(10);

  initStyles();
  drawMainLayout();
}

void display_loop()
{
    lv_timer_handler();
}

/* ---------------------------------------------------------------------------
Private
--------------------------------------------------------------------------- */

void configureScreen1(lv_obj_t *parent)
{
  lv_obj_t * label = lv_label_create(parent);
  lv_obj_add_style(label, &foreground_style, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(label, "Screen 1");
  lv_obj_center(label);
}

static void value_changed_event_cb(lv_event_t * e)
{
  lv_obj_t * arc = lv_event_get_target(e);
  int value = lv_arc_get_value(arc);
  tft.setBrightness(value);
}

void configureScreen2(lv_obj_t *parent)
{
  uint8_t arcSize = 230;

  lv_obj_t * arc = lv_arc_create(parent);
  lv_obj_set_size(arc, arcSize, arcSize);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, tft.getBrightness());
  lv_obj_center(arc);
  lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);
  lv_obj_add_style(arc, &foreground_style, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_style(arc, &accent_style, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_add_style(arc, &accent_style, LV_PART_KNOB | LV_STATE_DEFAULT);

  /* Dummy rect to fix scrolling of tiles */
  lv_obj_t* rect = lv_obj_create(parent);
  lv_obj_set_size(rect, arcSize-100, arcSize-100);
  lv_obj_center(rect);
  lv_obj_add_style(rect, &background_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_t * label = lv_label_create(parent);
  lv_obj_add_style(label, &foreground_style, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(label, "brightness");
  lv_obj_center(label);
}

void drawMainLayout()
{
  lv_obj_t *tileView = lv_tileview_create(lv_scr_act() );
  lv_obj_add_style(tileView, &background_style, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_scrollbar_mode(tileView, LV_SCROLLBAR_MODE_OFF);

  lv_obj_t * tile1 = lv_tileview_add_tile(tileView, 0, 0, LV_DIR_BOTTOM);
  lv_obj_t * tile2 = lv_tileview_add_tile(tileView, 0, 1, LV_DIR_TOP );

  configureScreen1(tile1);
  configureScreen2(tile2);
}

void initStyles()
{
  lv_style_init(&background_style);
  lv_style_set_bg_color(&background_style, lv_color_hex(BACKGROUND_COLOR));
  lv_style_set_border_color(&background_style, lv_color_hex(BACKGROUND_COLOR));

  lv_style_init(&foreground_style);
  lv_style_set_text_color(&foreground_style, lv_color_hex(TEXT_COLOR));
  lv_style_set_arc_color(&foreground_style, lv_color_hex(POPUP_COLOR));
  lv_style_set_text_font(&foreground_style, &lv_font_montserrat_20);

  lv_style_init(&accent_style);
  lv_style_set_bg_color(&accent_style, lv_color_hex(ACCENT_COLOR));
  lv_style_set_text_color(&accent_style, lv_color_hex(ACCENT_COLOR));
  lv_style_set_arc_color(&accent_style, lv_color_hex(ACCENT_COLOR));
}

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