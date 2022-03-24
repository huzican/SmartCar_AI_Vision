import pyb
import sensor, image, time, math
import os, nncu
from machine import UART
from pyb import LED

red = LED(1)    # 定义一个LED1   红灯
green = LED(2)  # 定义一个LED2   绿灯
blue = LED(3)   # 定义一个LED3   蓝灯

isGray = 1 #一开始采集灰度图片
isRGB  = 0 #采集彩色图片标志
last_img_type = 'g' #上一帧图像是灰度
# img_type = 'g' #这一帧图像为灰度
toggle_flag = 0 #图片采集类型翻转标志

uart = UART(1, baudrate=115200)
net_path = "_1a_simple_background_1.nncu"
labels = [line.rstrip() for line in open("/sd/labels_animal_fruits.txt")]   # 加载标签
net = nncu.load(net_path, load_to_fb=True)

#上电默认采集灰度图片
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)
sensor.set_brightness(1500)
sensor.skip_frames(time = 20)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()

while(True):
    #发送的数据，如果灰度没有框，彩图也没框
    output_str = chr(0x49) + chr(0x49) + chr(10) + chr(0) + chr(0) + chr(0) + chr(0) + chr(10)
    isRectangle = False #灰度图识别到框的标志
    isAniOrFru = False #彩色图识别到动物水果的标志
    #每次循环前记录图像类型
    if isGray:
        last_img_type = 'g'
    elif isRGB:
        last_img_type = 'r'

    input_str = uart.read(1).decode().strip() #读取是否发来切换标志，6为灰度切彩图，4为彩图切灰度
    #print(input_str)
    #if(input_str =='A'):
        #blue.on()
    #input_str = 6

    if input_str == 'A' and isGray: #接到数据，并且是灰度图，灰度切彩图
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
        sensor.set_brightness(1500)
        sensor.skip_frames(time = 10)
        sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
        sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
        clock = time.clock()
        isGray = 0
        isRGB = 1
        blue.on()
    elif input_str == 'B' and isRGB: #接到数据，并且是彩色图，彩图切灰度
        sensor.reset()
        sensor.set_pixformat(sensor.GRAYSCALE)
        sensor.set_framesize(sensor.QQVGA)
        sensor.set_brightness(1500)
        sensor.skip_frames(time = 20)
        sensor.set_auto_gain(False)
        sensor.set_auto_whitebal(False)
        #clock = time.clock()
        isRGB = 0
        isGray = 1

    img = sensor.snapshot() #采集一张图片

    #灰度图，传回的数据格式
    #  0        1       2          3                 4       5       6       7
    #0x49      0x49   img_type     x_(categroy)      x_ge    w_      w_ge    check
    #帧头      帧头   灰度0彩色1

    #如果是灰度，
    if isGray:
        for r in img.find_rects(threshold = 20000):
            if (r.rect()[2]>40) and (r.rect()[2]<70) and (r.rect()[3]>40) and (r.rect()[3]<70):
                image_type = 0
                img_x = int(r.rect()[0]+r.rect()[2]/2)
                print('img_x: ',img_x)
                rec_x_ = img_x//10
                rec_x_ge = img_x%10
                rec_width_ = r.rect()[2]//10
                rec_width_ge = r.rect()[2]%10
                check = image_type + rec_x_ + rec_x_ge + rec_width_ + rec_width_ge
                output_str = chr(0x49) + chr(0x49)+chr(image_type) + chr(rec_x_) + chr(rec_x_ge) + chr(rec_width_) + chr(rec_width_ge) + chr(check)
                isRectangle = True
                img.draw_rectangle(r.rect(), color = (255, 0, 0))
                for p in r.corners():
                    img.draw_circle(p[0], p[1], 5, color = (255, 0, 0))
                print(r)

    #如果是彩色图像
    elif isRGB:
        for r in img.find_rects(threshold = 20000):
            # img.draw_rectangle(r.rect(), color = (255, 0, 0))
            img1 = img.copy(r.rect())
            for obj in nncu.classify(net , img1, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):
                # print("**********\nTop 1 Detections at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
                if (obj.rect()[2]>20) and (obj.rect()[2]<70) and (obj.rect()[3]>20) and (obj.rect()[3]<70):
                    sorted_list = sorted(zip(labels, obj.output()), key = lambda x: x[1], reverse = True)
                    ## 打印准确率最高的结果
                    #for i in range(1):
                        #print("%s = %f" % (sorted_list[i][0], sorted_list[i][1]))

                    isAniOrFru = True
                    image_type = 1
                    categroy = labels.index(sorted_list[0][0])
                    img_rgb_x = int(r.rect()[0]+r.rect()[2]/2)
                    rec_x_rgb = img_rgb_x//10
                    rec_x_ge_rgb = img_rgb_x%10
                    rec_width_rgb = r.rect()[2]
                    check =image_type + categroy + rec_x_rgb + rec_x_ge_rgb + rec_width_rgb
                    output_str = chr(0x49) + chr(0x49)+chr(image_type) + chr(categroy) + chr(rec_x_rgb) + chr(rec_x_ge_rgb) + chr(rec_width_rgb) + chr(check)

    uart.write(output_str)
    green.on()
