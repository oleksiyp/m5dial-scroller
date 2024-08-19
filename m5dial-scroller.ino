#include <M5Dial.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;

long oldPosition = -999;
bool pressed = false;
bool selectMode = false;
int touch = 0;

void draw() {
  M5Dial.Display.clear();
  M5Dial.Display.setTextColor(selectMode ? GREEN : RED);
  M5Dial.Display.setTextSize(touch == 1 ? 1.3 : 1);
  M5Dial.Display.drawString("Scroll",
                            M5Dial.Display.width() / 2,
                            M5Dial.Display.height() / 4);
  M5Dial.Display.setTextSize(touch == 2 ? 1.3 : 1);
  M5Dial.Display.setTextColor(selectMode ? RED : GREEN);
  M5Dial.Display.drawString("Select",
                            M5Dial.Display.width() / 2,
                            M5Dial.Display.height() * 2 / 4);
  M5Dial.Display.setTextSize(touch == 3 ? 2 : 1);
  M5Dial.Display.setTextColor(GREEN);
  M5Dial.Display.drawString("<",
                            M5Dial.Display.width() / 4,
                            M5Dial.Display.height() * 3 / 4);
  M5Dial.Display.setTextSize(touch == 4 ? 2 : 1);
  M5Dial.Display.drawString("F5",
                            M5Dial.Display.width() * 2 / 4,
                            M5Dial.Display.height() * 3 / 4);
  M5Dial.Display.setTextSize(touch == 5 ? 2 : 1);
  M5Dial.Display.drawString(">",
                            M5Dial.Display.width() * 3 / 4,
                            M5Dial.Display.height() * 3 / 4);
}

void setup() {
    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);
    M5Dial.Display.setTextDatum(middle_center);
    M5Dial.Display.setTextFont(&fonts::Orbitron_Light_32);
    M5Dial.Display.setTextSize(1);
    
    USB.begin();
    Keyboard.begin();
    Mouse.begin();

    draw();
}

void loop() {
  M5Dial.update();

  auto t = M5Dial.Touch.getDetail();
  if (t.wasPressed()) {
    if (t.y < M5Dial.Display.height() / 3) {
      touch = 1;
      selectMode = false;
      M5Dial.Speaker.tone(8000, 20);
      draw(); 
    } else if (t.y < M5Dial.Display.height() * 2 / 3) {
      touch = 2;
      selectMode = true;
      M5Dial.Speaker.tone(8000, 20);
      draw(); 
    } else if (t.x < M5Dial.Display.width() / 3) {
      touch = 3;
      draw(); 
      M5Dial.Speaker.tone(8000, 20);
      Keyboard.press(KEY_LEFT_ALT);
      M5.delay(50);
      Keyboard.press(KEY_LEFT_ARROW);
      M5.delay(300);
      Keyboard.releaseAll();
      Keyboard.press(KEY_ESC);
      M5.delay(300);
      Keyboard.releaseAll();
    } else if (t.x < M5Dial.Display.width() * 2 / 3) {
      touch = 4;
      draw(); 
      M5Dial.Speaker.tone(8000, 20);
      Keyboard.press(KEY_F5);
      M5.delay(300);
      Keyboard.releaseAll();
    } else {
      touch = 5;
      draw(); 
      M5Dial.Speaker.tone(8000, 20);
      Keyboard.press(KEY_LEFT_ALT);
      M5.delay(50);
      Keyboard.press(KEY_RIGHT_ARROW);
      M5.delay(300);
      Keyboard.releaseAll();
    }
  } else if (t.wasReleased()) {
    touch = 0;
    draw();
  }

  long newPosition = M5Dial.Encoder.read();
  if (selectMode) {
    while (newPosition != oldPosition) {
      if (newPosition < oldPosition) {
        Keyboard.press(KEY_LEFT_SHIFT);
        M5.delay(50);
        Keyboard.press(KEY_TAB);
        M5.delay(100);
        Keyboard.releaseAll();
        oldPosition--;
      } else {
        Keyboard.press(KEY_TAB);
        delay(100);
        Keyboard.releaseAll();
        oldPosition++;
      }
      if (newPosition != oldPosition) {
        M5.delay(50);
      }
    }
  } else {
    Mouse.move(0, 0, oldPosition - newPosition);
    oldPosition = newPosition;
  }

  if (M5Dial.BtnA.isPressed()) {
    Keyboard.press(KEY_RETURN);
    pressed = true;
  } else if (pressed) {
    Keyboard.release(KEY_RETURN);
    pressed = false;
  }
}
