#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ThreadFileCopy.h"

struct ResolumeVideoclip{
    string  source_path;
    string  destination_path;
    int     type;
    int     unique_id;
    int     clip_index;
};

enum AppState
{
    APP_IDLE,
    APP_LOADING,
    APP_PROCESS,
    APP_COPY,
    APP_COPY_DONE
};

enum ButtonId
{
    GO,
    COMPO,
    FOLDER
};

struct Button{
    ofRectangle area;
    ButtonId    id;
    string      label;
    bool        pressed;
    bool        enabled;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void    load_file(string _file_path);
    void    process_composition(bool    _write = false);
    void    start_copy();
    void    copy_file();
    void    reset();
    
    ofxXmlSettings              composition;
    ThreadFileCopy              file_copy;
    vector<ResolumeVideoclip>   files_to_copy;
    vector<Button>              buttons;
    vector<Button>              areas;

    AppState state;
    ofFile  current_file;
    int current_copy_file;
    string  destination_folder;
    string  destination_folder_name;
    string  compo_name;
    ofRectangle progress_bar;
    float   progress_width;
    float   progress_width_current;
    Button  *current_button;

		
};
