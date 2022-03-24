import pyb
import sensor, image, time
import os, nncu
from machine import UART
from pyb import LED

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.set_brightness(1500)
sensor.skip_frames(time = 10)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()
uart = UART(1, baudrate=115200)

net_path = "model_0524.nncu"                         # 定义模型的路径
labels = [line.rstrip() for line in open("/sd/labels_number.txt")]  # 加载标签
net = nncu.load(net_path, load_to_fb=True)                          # 加载模型


while(True):
    isAprilTag = False
    isNum = False
    #AprilTagID = -1
    img = sensor.snapshot()
    for tag in img.find_apriltags(families=image.TAG25H9):
        AprilTagID = tag.id()
        isAprilTag = True
    if(isAprilTag):
        #isAprilTag = False
        img_type = 2
        categroy = AprilTagID
        check = img_type + categroy + 1
        output_str = chr(0x49) + chr(0x49)+chr(img_type) + chr(categroy) + chr(1) +chr(0) + chr(0) + chr(check)
        uart.write(output_str)
        #print(AprilTagID)
        #img.draw_rectangle(tag.rect(), color = (255, 0, 0))
        #print(tag.rect())
        #continue
    if not isAprilTag:
        for r in img.find_rects(threshold = 50000):             # 在图像中搜索矩形
            #img.draw_rectangle(r.rect(), color = (255, 0, 0))   # 绘制矩形外框，便于在IDE上查看识别到的矩形位置
            img1 = img.copy(r.rect())                           # 拷贝矩形框内的图像
            for obj in nncu.classify(net , img1, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):
                #print("**********\nTop 1 Detections at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
                #print(obj.rect()[2])
                #print(obj.rect()[3])
                if (obj.rect()[2]>0) and (obj.rect()[2]<60) and (obj.rect()[3]>0) and (obj.rect()[3]<60):
                    sorted_list = sorted(zip(labels, obj.output()), key = lambda x: x[1], reverse = True)
                    img_type = 1
                    categroy = labels.index(sorted_list[0][0])
                    if categroy >= 0 and categroy <= 9:
                        isNum = True
                    p1 = int(sorted_list[0][1])
                    p2 = int(sorted_list[0][1]*10)%10
                    p3 = int(sorted_list[0][1]*100)%10
                    check =img_type + p1 + p2 + p3 + categroy
                    output_str = chr(0x49) + chr(0x49)+chr(img_type) + chr(categroy) + chr(p1) + chr(p2) + chr(p3) + chr(check)
                    uart.write(output_str)
                    # 打印准确率最高的结果
                    #for i in range(1):
                        #print("%s = %f" % (sorted_list[i][0], sorted_list[i][1]))

    if(not isAprilTag) and (not isNum):
        output_str = chr(0x49) + chr(0x49)+chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0)
        uart.write(output_str)
