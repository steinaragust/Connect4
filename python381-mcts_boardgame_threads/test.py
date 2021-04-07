import cppyy

cppyy.include('Connect4.cpp')

game = cppyy.gbl.Connect4.Connect4()
