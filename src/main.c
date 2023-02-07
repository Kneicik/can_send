/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int data = 0;

struct can_frame frame = {
        .id = 0x010,
        .dlc = 8,
};
const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
int ret;

void main(void)
{
  int reti;
	reti = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  can_start(can_dev);
  while(1){
  ret = can_send(can_dev, &frame, K_MSEC(100), NULL, NULL);
  reti = gpio_pin_toggle_dt(&led);
  if (ret != 0) {
          printk("Sending failed [%d]", ret);
  }
  frame.data[0] = data;
  data = data + 1;
  k_msleep(500);
  reti = gpio_pin_toggle_dt(&led);
  }

}