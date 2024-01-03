default:
  gcc \
    simulation_v2.c settingsWindow.c drawFunctions.c being_struct.c  helpfulFunctions.c dSFMT.c \
    -DDSFMT_MEXP=521 \
    -O3 -pg -lGL -lGLU -lglut -lpthread -lm \
    -o simulation_v2
  ./simulation_v2

install:
  sudo apt -y install freeglut3-dev

compare:
  ./draw.py

draw:
  ./draw_graph.py
