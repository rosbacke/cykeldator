#!/usr/bin/python3

import string
import numpy as np
from numpy import uint32

def main():
	print("Hello")
	f = open("cykeldump.txt")
	oldDd = (0,0,0,0)

	# Read out file, convert to a matrix with floating point type.
	for i in f:
		sd = i.split(' ')

		# Raw decimal array.
		#     count          TP:assert      TP:releaae     TickTime (ms)
		dd = np.array([float.fromhex(sd[0]), float.fromhex(sd[1]), float.fromhex(sd[2]), float.fromhex(sd[3])])
		try:
			#print(dd)
			raw = np.vstack((raw, dd))
		except NameError:	
			raw = np.array(dd)

	# Are we missing any count?
	last = raw[0][0] - 1;
	for i in raw:
		if (i[0] - last) != 1:
			print("last:{} now:{}", (last, i[0]))
			raise ValueError()
		last = i[0]

	# All accounted for.
	# Reconstruct a linear time from the modulo cutoff.
	toAdd = np.array([0.0, 0.0, 0.0, 0.0])
	last = raw[0]
	for index, line in enumerate(raw):
		# Basic idea: first, check the sysTick, if it overflows add.		
		if line[3] - last[3] < 0:
			raise
			toAdd[3] += float(0x100000000)
		raw[index, 3] = (line[3] + toAdd[3]) * 0.001

		#print(raw[index])
		last = line

	# Construct a new mtrix with systick time stamps and
	# delta times. 
	# (cnt) (systick), delta up/down (delta active), (delta released)
	deltaM = np.zeros((raw.shape[0], 5))
	last = raw[0]
	for index, line in enumerate(raw):
		delta = line - last
		row = deltaM[index]
		row[0] = line[0]
		row[1] = line[3]
		deltaActive = line[2] - line[1]
		if deltaActive < 0:
			deltaActive += 2^32
		deltaRelease = line[1] - last[2]
		if deltaRelease < 0:
			deltaRelease += 2^32
		row[2] = (deltaActive + deltaRelease) / 72000000
		row[3] = (deltaActive) / 72000000
		row[4] = (deltaRelease) / 72000000
		deltaM[index] = row
		last = line
		#print (index, row)

	# print (deltaM.shape)

	# Do a first calculation of speed
	speed = np.zeros((raw.shape[0], 3))
	for index, line in enumerate(deltaM):
		# 3.6 m/s->km/h : 2.2 circum wheel, 36 spokes on wheel.
		speed[index] = (line[0], line[1], 3.6 * 2.2/36 / line[2])
		print(speed[index])

	# Speed in km/h Manual check was max 35km/m during ride.
	# Seems to work well
	print (speed)





if __name__=='__main__':
	main()
