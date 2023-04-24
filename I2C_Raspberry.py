import serial
from board import SCL, SDA
import busio
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306
import time
i2c = busio.I2C(SCL, SDA)
disp = adafruit_ssd1306.SSD1306_I2C(128, 64, i2c)

disp.fill(0)
disp.show()
#image = Image.new('1', (128, 64))
width = disp.width
height = disp.height
image = Image.new('1', (width, height))

draw = ImageDraw.Draw(image)    
font = ImageFont.load_default()

draw.line((5, 0, 123, 0), fill=255)
draw.text((18, 7), '==== EQUIPO ====', font=font, fill=255)
draw.text((20, 17), 'Jesus Alejandro', font=font, fill=255)
draw.text((26,27), 'Abraham Ortiz', font=font, fill=255)
draw.line((5, 49, 123, 49), fill=255)
disp.image(image)
disp.show()
time.sleep(3)


disp.fill(0)
disp.show()
image1 = Image.new('1', (width, height))
draw1 = ImageDraw.Draw(image1)
font = ImageFont.load_default()

s = serial.Serial('/dev/ttyUSB0',9600)
#print(s.name)
x=0

for i in range(7):
	lectura = s.readline()
	draw1.text((2, x), lectura, font=font, fill=255)
	x=x+7

	

s.close()
# Muestra Texto
disp.image(image1)
disp.show()


