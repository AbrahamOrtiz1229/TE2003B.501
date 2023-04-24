import serial
s = serial.Serial('/dev/ttyUSB0',9600)
print(s.name)
for i in range(7):
	lectura = s.readline()
	print(lectura)

s.close()
