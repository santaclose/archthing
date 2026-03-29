#/usr/bin/python

import random
import math

def normVec(a):
	veclen = lenVec(a)
	return (a[0] / veclen, a[1] / veclen)
def negVec(a):
	return (-a[0], -a[1])
def dotVec(a, b):
	return a[0] * b[0] + a[1] * b[1]
def addVec(a, b):
	return (a[0] + b[0], a[1] + b[1])
def subVec(a, b):
	return (a[0] - b[0], a[1] - b[1])
def scaleVec(a, b):
	return (a[0] * b[0], a[1] * b[1])
def halfVec(a):
	return (a[0] / 2.0, a[1] / 2.0)
def lenVec(a):
	return math.sqrt(dotVec(a, a))

minRoomSideLength = 2.0
maxRoomSideLength = 4.0

cellSize = 5
roomCount = 20
windowSizes = [1.2, 5]
windowSeparation = 0.6
doorWidth = 0.75
floorCount = 1

windowSizes.sort()

import matplotlib.pyplot as plt
import matplotlib

# Agg for saving image as file
matplotlib.use('Agg')
def visualizeLines(wallLines, windowLines, doorLines, outputPath='output_preview.png'):
	plt.figure(figsize=(10, 10))
	
	# Definimos los estilos para cada categoría
	categories = [
		(wallLines, 'gray', 'Paredes', 2.0),
		(windowLines, 'blue', 'Ventanas', 2.0),
		(doorLines, 'red', 'Puertas', 3.0)
	]

	for lines, color, label, width in categories:
		first_plot = True
		for chain in lines:
			if not chain: continue
			# Extraemos X y Z (asumiendo que tus datos son [x, z])
			x = [v[0] for v in chain]
			z = [v[1] for v in chain]
			
			# Solo añadimos el label a la primera línea de cada grupo para la leyenda
			plt.plot(x, z, color=color, linewidth=width, label=label if first_plot else "")
			first_plot = False

	plt.axis('equal') # Mantener proporciones cuadradas
	plt.grid(True, linestyle='--', alpha=0.6)
	plt.legend()
	plt.title("Plano de Planta - Previsualización")
	
	# Guardar en archivo y cerrar para liberar memoria
	plt.savefig(outputPath, dpi=300, bbox_inches='tight')
	plt.close()
	print(f"Visualización guardada en: {outputPath}")

def writeObj(wallLines, windowLines, doorLines):
	vertexCounter = 1  # Los índices de OBJ empiezan en 1
	
	with open('assets/input/input.obj', 'w') as file:
		# Definimos los tipos de elementos a procesar para no repetir código
		categories = [
			('wall', wallLines),
			('window', windowLines),
			('door', doorLines)
		]

		for prefix, lines in categories:
			for idx, chain in enumerate(lines):
				# Escribir nombre del objeto
				file.write(f"o {prefix}.0.{idx:03d}\n")
				
				firstVertexIndex = vertexCounter
				
				# Escribir todos los vértices de esta cadena
				for v in chain:
					file.write(f"v {v[0]} 0.0 {v[1]}\n")
					vertexCounter += 1
				
				# Crear las líneas conectando los vértices
				# i va desde el primer vértice hasta el penúltimo de la cadena
				num_vertices = len(chain)
				for i in range(num_vertices - 1):
					v1 = firstVertexIndex + i
					v2 = firstVertexIndex + i + 1
					file.write(f"l {v1} {v2}\n")

	# 	# # Output walls (obj uses 1 based indices)
	# 	# file.write('o wall\n')
	# 	# for v in outline:
	# 	# 	file.write(f'v {v[0]} 0.0 {v[1]}\n')

	# 	# # OPEN POLYGON
	# 	# for i in range(1, len(outline)):
	# 	# 	file.write(f"l {i} {i + 1}\n")

def sameSizeBlocks():
	# Choose room size and fill cells
	takenSlots = set([(0,0)])
	for i in range(1, roomCount):
		while True:
			baseSlot = random.choice(list(takenSlots))
			newSlot = addVec(baseSlot, random.choice([(1,0), (-1,0), (0,1), (0,-1)]))
			if newSlot not in takenSlots:
				takenSlots.add(newSlot)
				break

	# Compute outline from cells
	outlineDict = dict()
	for slot in takenSlots:
		if addVec(slot, (-1, 0)) not in takenSlots:
			a = scaleVec(addVec(slot, (-0.5, 0.5)), (cellSize, cellSize))
			b = scaleVec(addVec(slot, (-0.5, -0.5)), (cellSize, cellSize))
			outlineDict[a] = b
		if addVec(slot, (1, 0)) not in takenSlots:
			a = scaleVec(addVec(slot, (0.5, -0.5)), (cellSize, cellSize))
			b = scaleVec(addVec(slot, (0.5, 0.5)), (cellSize, cellSize))
			outlineDict[a] = b
		if addVec(slot, (0, -1)) not in takenSlots:
			a = scaleVec(addVec(slot, (-0.5, -0.5)), (cellSize, cellSize))
			b = scaleVec(addVec(slot, (0.5, -0.5)), (cellSize, cellSize))
			outlineDict[a] = b
		if addVec(slot, (0, 1)) not in takenSlots:
			a = scaleVec(addVec(slot, (0.5, 0.5)), (cellSize, cellSize))
			b = scaleVec(addVec(slot, (-0.5, 0.5)), (cellSize, cellSize))
			outlineDict[a] = b

	outline = []
	current = list(outlineDict.keys())[0]
	outline.append(current)
	while True:
		current = outlineDict[current]
		if current == outline[0]:
			break
		outline.append(current)

	# Remove unnecessary vertices
	toRemove = []
	for i in reversed(range(len(outline))):
		a = i - 1 if i - 1 > -1 else len(outline) - 1
		b = i + 1 if i + 1 < len(outline) else 0
		disp = subVec(outline[i], outline[a])
		test = addVec(outline[i], disp)
		if lenVec(subVec(test, outline[b])) < 0.001:
			toRemove.append(i)
	for x in toRemove:
		outline.pop(x)

	# Final geometry will be stored in these
	wallLines = [[]]
	doorLines = []
	windowLines = []

	# Main door
	disp = subVec(outline[0], outline[-1])
	dispMag = lenVec(disp)
	dispNorm = normVec(disp)
	scaleFactor = (dispMag - doorWidth) / 2.0
	doorPosA = addVec(outline[-1], scaleVec(dispNorm, (scaleFactor, scaleFactor)))
	doorPosB = addVec(outline[0], scaleVec(dispNorm, (-scaleFactor, -scaleFactor)))
	outline.insert(0, doorPosB)
	outline.append(doorPosA)

	doorLines.append([doorPosA, doorPosB])

	# wallLines = [outline]
	# Windows and walls
	currentWallStart = 0
	for i in range(1, len(outline)):
		disp = subVec(outline[i], outline[i - 1])
		dispMag = lenVec(disp)

		for potentialWindowSize in reversed(windowSizes):
			windowSize = potentialWindowSize
			windowCountForWall = int((dispMag - windowSeparation) / (windowSeparation + windowSize))
			if windowCountForWall > 0:
				break

		# print(f"windowCountForWall: {windowCountForWall}")
		if windowCountForWall > 0:
			dispNorm = normVec(disp)
			extraSpace = dispMag - (windowCountForWall * (windowSize + windowSeparation) + windowSeparation)
			toWallSep = extraSpace / 2.0 + windowSeparation
			windowStart = addVec(outline[i - 1], scaleVec(dispNorm, (toWallSep, toWallSep)))
			windowEnd = subVec(outline[i], scaleVec(dispNorm, (toWallSep, toWallSep)))
			wallLines[-1].extend(outline[currentWallStart:i])
			wallLines[-1].append(windowStart)
			for wi in range(windowCountForWall):
				offset = (windowSize + windowSeparation) * wi;
				windowLines.append([addVec(windowStart, scaleVec(dispNorm, (offset, offset))), addVec(windowStart, scaleVec(dispNorm, (windowSize + offset, windowSize + offset)))])
				if wi < windowCountForWall - 1:
					wallLines.append([addVec(windowStart, scaleVec(dispNorm, (offset + windowSize, offset + windowSize))), addVec(windowStart, scaleVec(dispNorm, (offset + windowSize + windowSeparation, offset + windowSize + windowSeparation)))])
			wallLines.append([windowEnd])
			currentWallStart = i
	wallLines[-1].extend(outline[currentWallStart:])

	import pprint
	print('window lines:')
	pprint.pprint(windowLines)
	print('wall lines:')
	pprint.pprint(wallLines)

	visualizeLines(wallLines, windowLines, doorLines)
	writeObj(wallLines, windowLines, doorLines)

sameSizeBlocks()

def randomStacking():
	roomSizes = [(random.uniform(minRoomSideLength, maxRoomSideLength), random.uniform(minRoomSideLength, maxRoomSideLength)) for x in range(roomCount)]

	roomPositions = [(0.0, 0.0) for x in range(roomCount)]
	# furthestRoomToDirection = [0, 0, 0, 0]
	# directions = [(1.0, 0.0), (-1.0, 0.0), (0.0, 1.0), (0.0, -1.0)]
	furthestRoomToDirection = [0, 0]
	directions = [(1.0, 0.0), (-1.0, 0.0)]

	for i in range(1, roomCount):
		randomIndex = random.randint(0, len(directions) - 1)
		targetDir = directions[randomIndex]
		roomToAppendTo = furthestRoomToDirection[randomIndex]

		prevRoomPos = roomPositions[roomToAppendTo]
		prevRoomSize = roomSizes[roomToAppendTo]
		newRoomSize = roomSizes[i]
		newRoomPos = addVec(prevRoomPos, halfVec(scaleVec(targetDir, prevRoomSize)))
		newRoomPos = addVec(newRoomPos, halfVec(scaleVec(targetDir, newRoomSize)))
		roomPositions[i] = newRoomPos

		# furthestRoomToDir = furthestRoomToDirection[randomIndex]

		furthestRoomPoint = addVec(prevRoomPos, halfVec(scaleVec(targetDir, prevRoomSize)))
		newRoomPoint = addVec(newRoomPos, halfVec(scaleVec(targetDir, newRoomSize)))
		if lenVec(newRoomPoint) > lenVec(furthestRoomPoint):
			furthestRoomToDirection[randomIndex] = i


	for i in range(roomCount):
		print(f"room {i}")
		print(f"   width:  {roomSizes[i][0]}")
		print(f"   height: {roomSizes[i][1]}")
		print(f"   posX:   {roomPositions[i][0]}")
		print(f"   posY:   {roomPositions[i][1]}")
