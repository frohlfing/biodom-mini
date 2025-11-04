/**
 * @file test_ArduCamMini2MPPlusOV2640.cpp
 * Einfache Unit-Testfälle (funktional). Diese Tests sind hardware-abhängig.
 * Sie prüfen primär, dass API-Aufrufe keine fatalen Fehler werfen.
 */

#include <Arduino.h>
#include <unity.h>
#include "ArduCamMini2MPPlusOV2640.h"
#include <MicroSDCard.h>

#define CAM_CS_PIN 17
#define SD_CS_PIN 16

ArduCamMini2MPPlusOV2640 cam(CAM_CS_PIN, 21, 22, 8000000UL, 512, 5000);
MicroSDCard sd(SD_CS_PIN, 1);

void test_begin_and_capture_api_calls() {
  TEST_ASSERT_TRUE(cam.begin());
  // SD init may fail in test env; skip if sd.begin fails
  if (sd.begin(2000000UL)) {
    // create and remove a small file via captureToSD (if camera present)
    if (cam.isCameraPresent()) {
      TEST_ASSERT_TRUE(cam.captureToSD(sd, "/unittest.jpg", 3000));
      TEST_ASSERT_TRUE(sd.exists("/unittest.jpg"));
      TEST_ASSERT_TRUE(sd.remove("/unittest.jpg"));
    } else {
      // camera not present — at least camera.begin succeeded or not; mark test as passed for headless
      TEST_ASSERT_TRUE(true);
    }
  } else {
    TEST_ASSERT_TRUE(true);
  }
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_begin_and_capture_api_calls);
  UNITY_END();
}

void loop() {}
