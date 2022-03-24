from machine import UART
import pyb
import struct
from pyb import LED
import sensor, image, time, math, tf
import os, nncu

red = LED(1)
green = LED(2)
blue = LED(3)


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.set_brightness(1000)
sensor.skip_frames(time = 20)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()



save_img_num = 152;
while(True):
    img = sensor.snapshot()
    blue.toggle()
    for r in img.find_rects(threshold = 50000):
        img.draw_rectangle(r.rect(), color = (255, 0, 0))


        img1 = img.copy(r.rect())
        #print(r.rect())
        if r.rect()[2]<150 and r.rect()[3]<150:
            image_pat = "/num2_small_B_"+str(save_img_num)+".jpg"
            img1.save(image_pat,quality=100)
            save_img_num += 1;
