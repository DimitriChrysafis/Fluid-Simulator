int cols, rows;
int scl = 5;
int w = 1200;
int h = 800;
float[][] waterHeight;
float[][] prevWaterHeight;
float damping = 0.96;
float angleX = PI/3;
float angleY = 0;
float zoom = 1;
boolean dragging = false;

void setup() {
  size(800, 600, P3D);
  cols = w / scl;
  rows = h / scl;
  
  waterHeight = new float[cols][rows];
  prevWaterHeight = new float[cols][rows];
}

void draw() {
  background(0);
  
  translate(width/2, height/2);
  rotateX(angleX);
  rotateY(angleY);
  scale(zoom);
  translate(-w/2, -h/2);

  for (int x = 1; x < cols-1; x++) {
    for (int y = 1; y < rows-1; y++) {
      waterHeight[x][y] = (
        prevWaterHeight[x-1][y] + 
        prevWaterHeight[x+1][y] + 
        prevWaterHeight[x][y-1] + 
        prevWaterHeight[x][y+1]
      ) / 2 - waterHeight[x][y];
      
      waterHeight[x][y] *= damping;
    }
  }

  noStroke();
  for (int x = 0; x < cols-1; x++) {
    for (int y = 0; y < rows-1; y++) {
      float avgHeight = (waterHeight[x][y] + waterHeight[x+1][y] + waterHeight[x+1][y+1] + waterHeight[x][y+1]) / 4;
      color c = color(map(avgHeight, -100, 100, 0, 255), map(avgHeight, -100, 100, 0, 255), 255, 150);
      
      beginShape(QUADS);
      fill(c);
      vertex(x*scl, y*scl, waterHeight[x][y]);
      vertex((x+1)*scl, y*scl, waterHeight[x+1][y]);
      vertex((x+1)*scl, (y+1)*scl, waterHeight[x+1][y+1]);
      vertex(x*scl, (y+1)*scl, waterHeight[x][y+1]);
      endShape(CLOSE);
    }
  }

  float[][] temp = prevWaterHeight;
  prevWaterHeight = waterHeight;
  waterHeight = temp;
}

void mouseDragged() {
  int x = int(map(mouseX, 0, width, 0, cols));
  int y = int(map(mouseY, 0, height, 0, rows));
  if (x >= 0 && x < cols && y >= 0 && y < rows) {
    waterHeight[x][y] = 100;
  }
}

void mousePressed() {
  dragging = true;
}

void mouseReleased() {
  dragging = false;
}

void mouseWheel(MouseEvent event) {
  float e = event.getCount();
  zoom -= e * 0.05;
  zoom = constrain(zoom, 0.5, 3);
}

void keyPressed() {
  if (key == 'w') {
    angleX -= 0.1;
  } else if (key == 's') {
    angleX += 0.1;
  } else if (key == 'a') {
    angleY -= 0.1;
  } else if (key == 'd') {
    angleY += 0.1;
  }
}
