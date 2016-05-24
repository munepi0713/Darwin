/**
 *
 */
class Config {
  private String _dataDir = null;
  private String _name = "";
  public void load() {
    String[] lines = loadStrings("settings.txt");
    if ( lines != null ) {
      for (String line : lines) {
        String[] tokens = line.split("=");
        if ( tokens.length >= 2 ) {
          if ( tokens[0].equals("DataDir") ) {
            _dataDir = tokens[1].trim();
            if ( !_dataDir.endsWith(File.separator) ) {
              _dataDir = _dataDir + File.separator;
            }
          }
        }
      }
    }
  }
  public String makePath(String relativePath) {
    return _dataDir + relativePath;
  }
  
  public void setName(String name) {
    _name = name;
  }
  public String getName() {
    return _name;
  }
  
}

/**
 *
 */
public class Record {
  public HashMap<String, Integer> labels = new HashMap<String, Integer>(); // <K,V>=<Name,Label>
}
/**
 *
 */
class Records {
  private TreeMap<String, Record> _records = new TreeMap<String, Record>(); // <K,V>=<Image,Record>
  public Iterator getImageIterator() {
    return _records.keySet().iterator();
  }
  public boolean isLabelled(String name, String imageName) {
    Record r = _records.get(imageName);
    return r.labels.containsKey(name);
  }
  public void setLabel(String name, String imageName, Integer label) {
    if ( !_records.containsKey(imageName) ) {
      Record r = new Record();
      _records.put(imageName, r);
    }
    _records.get(imageName).labels.put(name, label);
  }
  public void loadImages(String dirname) {
    File dir = new File(dirname);
    for (File file : dir.listFiles()) {
      if ( file.getName().endsWith(".png") || file.getName().endsWith(".png") ) {
        System.out.println(file.getName());
        _records.put(file.getName(), new Record());
      }
    }
  }
  public void load(String filename) {
    File file = new File(filename);
    if ( file != null && file.exists() ) {
      try {
        BufferedReader br = new BufferedReader(new FileReader(file));
        String line;
        while ( (line = br.readLine()) != null ) {
          String[] tokens = line.split(",");
          String imageName = tokens[0];
          for (int i = 1; i < tokens.length; i++) {
            String[] names = tokens[i].split("\\|");
            for (int j = 0; j < names.length; j++) {
              if ( !names[j].isEmpty() ) {
                setLabel(names[j], imageName, i - 1);
              }
            }
          }
        }
      }
      catch ( FileNotFoundException e ) {
        e.printStackTrace();
      }
      catch ( IOException e ) {
        e.printStackTrace();
      }
    }
  }
  public void save(String filename) throws IOException {
    File file = new File(filename);
    FileWriter writer = new FileWriter(file);
    for (String image : _records.keySet()) {
      Record r = _records.get(image);
      TreeMap<Integer, String> tokens = new TreeMap<Integer, String>();
      int maxLabel = 0;
      for (String name : r.labels.keySet()) {
        Integer label = r.labels.get(name);
        if ( tokens.containsKey(label) ) {
          tokens.put(label, tokens.get(label) + "|" + name);
        }
        else {
          tokens.put(label, name);
        }
        if ( maxLabel < label ) maxLabel = label;
      }
      writer.write(image);
      for (int i = 0; i <= maxLabel; i++) {
        String token = tokens.get(i);
        if ( token == null ) token = "";
        writer.write("," + token);
      }
      writer.write("\n");
    }
    System.out.println(file.getAbsolutePath());
    writer.close();
  }
}

/**
 *
 */
Config  config = new Config();
Records _records = new Records();
Records removed = new Records();

int grad = 0;
int lastkey = 0;

final int NAME_INPUT = 0;
final int PN_SELECTION = 1;
int _view;
final int KEY_ENTER = 10;
final int KEY_ESC = 27;
final int KEY_BACKSPACE = 8;

/**
 *
 */
void setup() {
  // 0. Set up screen.
  size(screen.width / 2, screen.height / 2);

  // 0. Load configuration from the default directory.
  System.out.print("0. Loading configurations ... ");
  config.load();
  System.out.println("OK");

  // 1.
  System.out.print("1. Loading images ... ");
  _records.loadImages(config.makePath(""));
  System.out.println("OK");
  
  // 2. Load records and removed records.
  System.out.print("2. Loading records ... ");
  _records.load(config.makePath(".records.csv"));
  System.out.println("OK");
  
  // 2. Initializa states.
  setupNameInput();
}
//----------------------------------------------------------------
//----------------------------------------------------------------
/**
 *
 */
void draw() {
  switch ( _view ) {
  case NAME_INPUT :
    drawNameInput();
    break;
  case PN_SELECTION :
    drawPNSelection();
    break;
  }
}

/**
 *
 */
void closeApp() {
  System.out.print("5. Saving records ... ");
  try {
    _records.save(config.makePath(".records.csv"));
    removed.save(config.makePath(".removed.csv"));
    System.out.println("OK");
  }
  catch ( IOException e ) {
    System.out.println("Failed");
    e.printStackTrace();
  }
}

/**
 *
 */
void keyPressed() {
  boolean processed = false;
  switch ( _view ) {
  case NAME_INPUT :
    processed = keyPressedNameInput();
    break;
  case PN_SELECTION : 
    processed = keyPressedPNSelection();
    break;
  }
  
  if ( !processed ) {
    switch ( key ) {
    case 'q' :
    case 'Q' :
    case KEY_ESC :
      // Exit
      System.out.println("Exit");
      closeApp();
      exit();
      break;
    default :
      System.out.println(String.format("Push %d", (int)key));
      lastkey = key;
      grad = 30;
    }
  }
}

//----------------------------------------------------------------
//----------------------------------------------------------------
String _name = "";

/**
 *
 */
void setupNameInput() {
  _view = NAME_INPUT;
}
  
void drawNameInput() {
  // Background
  background(0);
  
  // Name box.
  int h = 20;
  int w = 200;
  fill(192, 192, 192);
  rect(width / 2 - w, height / 2 - h, w * 2, h * 2);
  fill(128, 128, 255);
  text(_name, width / 2 - w + 2, height / 2 + h - 2);
}

boolean keyPressedNameInput() {
  switch ( key ) {
  case KEY_ENTER :
    config.setName(_name);
    setupPNSelection();
    return true;
  case KEY_BACKSPACE :
    _name = _name.substring(0, _name.length() - 1);
    return true;
  default :
    if ( Character.isLetterOrDigit(key) ) {
      _name += key;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
Iterator it = null;
String _imageName = null;
PImage _image;
int _count = 0;

void nextImage() {
  _imageName = null;
  
  while ( it.hasNext() ) {
    String nextImage = (String)it.next();
    if ( !_records.isLabelled(config.getName(), nextImage) ) {
      _imageName = nextImage;
      break;
    }
    else {
      _count++;
    }
  }
  
  // Load image
  if ( _imageName != null ) {
    _image = loadImage(config.makePath(_imageName));
  }
  else {
    _image = null;
  }
}

/**
 *
 */
void setupPNSelection() {
  it = _records.getImageIterator();
  nextImage();
  
  _view = PN_SELECTION;
}

/**
 *
 */
void drawPNSelection() {
  point(50, 50);
  
  background(0);
  
  fill(255, 255, 255);
  if ( _image != null ) {
    image(_image, 0, 0);
    text("Positive <--    --> Negative", 100, 400);
  }
  text(config.getName(), 0, 50);
  text(String.format("%d images", _count), 400, 50);
  
  if ( grad > 0 ) {
    fill(255, 255, 255, 255 * grad / 30);
    text(String.format("%c (%d)", lastkey, (int)lastkey), 300, 300);
    grad--;
  }
}

boolean keyPressedPNSelection() {
  switch ( key ) {
  case 'P' :
  case 'p':
  case '1':
    if ( _image != null ) {
      _records.setLabel(config.getName(), _imageName, 1);
      _count++;
      nextImage();
      return true;
    }
    break;
  case 'N' :
  case 'n':
  case '0':
    if ( _image != null ) {
      _records.setLabel(config.getName(), _imageName, 0);
      _count++;
      nextImage();
      return true;
    }
    break;
  }
  return false;
}


