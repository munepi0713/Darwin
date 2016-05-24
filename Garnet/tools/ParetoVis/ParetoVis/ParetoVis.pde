String[] lines;

class Individual {
  float[] fitness;
  int rank;
}

static String[] _args;

/**
 * Entry point.
 */
public static void main(String args[]) {
  _args = new String[args.length + 1];
  _args[0] = "ParetoVis";
  for (int i = 0; i < args.length; i++) {
    _args[i + 1] = args[i];
  }
  PApplet.main(_args);
}

String _filename = null;

void setup() {
  size(500, 500);
  
  if ( _args.length > 1 ) {
    _filename = _args[1];
  }
  else {
    _filename = "C:\\Users\\doi\\Documents\\Study\\program\\samples\\garnet\\ExtractFlower\\ExtractFlower_Test20130518r\\.garnet_cp.txt";
  }
}

boolean isShown = false;
int _counter = 0;

void keyPressed() {
  if ( key == ' ' ) isShown = false;
}

int step = 80;

void drawBackground() {
  background(255, 255, 255);
  stroke(128, 128, 128);
  for (int x = 0; x < width; x += step) {
    line(x, 0, x, (height - 1));
  }
  for (int y = height - 1; y >= 0; y -= step) {
    line(0, y, width, y);
  }
}

void drawIndividual(Individual ind) {
  int c =  (int)pow(2, 9 - ind.rank);
  noStroke();
  fill(c, c, c);
  ellipse(ind.fitness[0] * step, height - ind.fitness[1] * step, 3, 3);
}

void draw() {
  if ( !isShown ) {
    println("-----" + _counter);
    drawBackground();
    
    _counter++;
    //lines = loadStrings("C:\\Users\\Munehiro\\Documents\\Study\\Study\\program\\samples\\garnet\\ExtractFlower\\ExtractFlower_22a\\.garnet_cp.txt");
    //lines = loadStrings("D:\\Munehiro\\Study\\program\\samples\\garnet\\ExtractFlower\\ExtractFlower_22b\\.garnet_cp.txt");
    //lines = loadStrings("D:\\Munehiro\\Study\\program\\samples\\garnet\\ExtractFlower\\ExtractFlower_23b\\.garnet_cp.txt");
    lines = loadStrings(_filename);

    int counter = 0;
    Individual ind = new Individual();
    for (int i = 0; i < lines.length; i++) {
      println(lines[i]);
      if ( lines[i].startsWith("# FVector") ) {
        String[] tokens = split(lines[i], ',');
        float[] pos = new float[2];
        for (int j = 1; j < 3; j++) {
          float value;
          if ( j < tokens.length ) {
            value = Float.parseFloat(tokens[j]);
          }
          else {
            value = counter;
          }
          pos[j - 1] = value;
        }
        ind.fitness = pos;
      }
      else if ( lines[i].startsWith("Rank") ) {
        String[] tokens = split(lines[i], ',');
        ind.rank = Integer.parseInt(trim(tokens[1]));
        if ( ind.fitness != null ) {
          drawIndividual(ind);
        }
      }
    } 
    isShown = true;
  }
}

