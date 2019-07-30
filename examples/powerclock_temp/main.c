// \file
// This program waits for a button press on imix board. If the button is pressed,
// the application will read bunch of measurements from the temperature sensors.
// It will write the results to flash.
#include <stdio.h>
#include <button.h>
#include <tock.h>
#include <temperature.h>
#include <internal/nonvolatile_storage.h>
#include <gpio.h>

static bool button_pressed;

// Callback for button presses.
//   btn_num: The index of the button associated with the callback
//   val: 1 if pressed, 0 if depressed
static void button_callback(int btn_num,
                            int val,
                            __attribute__ ((unused)) int arg2,
                            __attribute__ ((unused)) void *ud) {
  if (val == 1) {
    button_pressed = true;
  }
}

int main(void) {
  printf("Starting temp application\n"
          "This app waits for a button press. If the button is pressed,\n"
          "the app will start reading from sensors. Then it will write\n"
          "the results to flash.\n");


  unsigned num_measurements = 5;
  int temperatures[num_measurements];

  // setup for flash reads/writes
  size_t buf_size = num_measurements;
  uint8_t writebuf[buf_size];
  
  int ret = nonvolatile_storage_internal_write_buffer(writebuf, buf_size);
  int offset = 0;
  int len = 512;

  button_subscribe(button_callback, NULL);

  button_enable_interrupt(0);  

  while (1) {
    printf("waiting for a gpio interrupt\n");
    // waiting for a gpio interrupt
    yield_for(&button_pressed);
    button_pressed = false;

    int temp = 0;
    // starts reading sensors
    for (unsigned i = 0; i < num_measurements; ++i) {
      temperature_read_sync(&temp);
      temperatures[i] = temp;
    }

    // prints out all the readings
    for (unsigned i = 0; i < num_measurements; ++i) {
      /* code */
      printf("%d\n", temperatures[i]);
    }

    ret = nonvolatile_storage_internal_write(offset, len);
    if (ret != 0) {
      printf("\tError writing to flash.\n");
      return ret;
    }
  }
  
  return 0;
}
