/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#define LED0_NODE DT_ALIAS(led0)
#define STACKSIZE KB(2)
#define PRIORITY 7
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int data = 0;

struct can_frame frame = {
        .id = 0x010,
        .dlc = 8,
};
const struct can_filter my_filter = {
		    .flags = CAN_FILTER_DATA,
        .id = 0x123,

};
CAN_MSGQ_DEFINE(my_can_msgq, 2);
struct can_frame rx_frame;
int filter_id;
int rx;
const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
int ret;

void receive(){
while(1){
  filter_id = can_add_rx_filter_msgq(can_dev, &my_can_msgq, &my_filter);
  if (filter_id < 0) {
    printk("Unable to add rx msgq [%d]", filter_id);
    return;
  }
  k_msgq_get(&my_can_msgq, &rx_frame, K_FOREVER);
  printk("ramka: %d %d \n", rx_frame.data[0], rx_frame.data[1]);
}
}


void send(void)
{
  int reti;
  can_start(can_dev);
  while(1){
    ret = can_send(can_dev, &frame, K_MSEC(100), NULL, NULL);
    if (ret != 0) {
            printk("Sending failed [%d]", ret);
    }
    reti = gpio_pin_toggle_dt(&led);
    frame.data[0] = data;
    data = data + 1;
    k_msleep(150);
    reti = gpio_pin_toggle_dt(&led);
    k_msleep(150);
    
  }


}
K_THREAD_DEFINE(my_thread, STACKSIZE, receive, NULL, NULL, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(my_threadd, STACKSIZE, send, NULL, NULL, NULL,
                PRIORITY, 0, 0);