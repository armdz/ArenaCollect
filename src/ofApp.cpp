#include "ofApp.h"
//  lolo
//  armdz.com
//  github.com/armdz

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("Arena Collect");
    state = APP_IDLE;
    
    //  Areas
    float   area_size = 120;
    
    Button  area_file;
    area_file.id = COMPO;
    area_file.area = ofRectangle(10,10,area_size,area_size);
    area_file.label = "DROP COMPO";
    area_file.enabled = false;
    areas.push_back(area_file);
    
    Button  area_destination;
    area_destination.id = FOLDER;
    area_destination.area = ofRectangle(area_size+20,10,area_size,area_size);
    area_destination.label = "DROP FOLDER";
    areas.push_back(area_destination);
    
    //  Buttons setup
    current_button = NULL;
    
    Button  button_set_dest;
    button_set_dest.id = GO;
    button_set_dest.area = ofRectangle(area_size*2+30,10,area_size,area_size);
    button_set_dest.label = "GO!";
    button_set_dest.pressed = false;
    button_set_dest.enabled = true;
    buttons.push_back(button_set_dest);
    
    progress_bar.set(10,area_size+20,area_size*3+20,30);
    
    reset();
    ofSetLineWidth(1);


}

void    ofApp::reset()
{
   // progress_width_current = 0.0f;
    //progress_width = 0.0f;
    destination_folder = "";
    destination_folder_name = "";
    compo_name = "";
    current_button = NULL;
    state = APP_IDLE;
    buttons.at(0).enabled = false;
    composition.clear();
    current_copy_file = 0;
}

void    ofApp::load_file(string _file_path)
{
    state = APP_LOADING;
    if(composition.loadFile(_file_path))
    {
        files_to_copy.clear();
        process_composition();
    }
}

void ofApp::process_composition(bool    _write)
{
    int to_write = 0;
    state = APP_PROCESS;
    composition.pushTag("composition");
    int total_decks = composition.getNumTags("decks");
    composition.pushTag("decks");
    
    
    for(int deck_index=0;deck_index<total_decks;deck_index++)
    {
        composition.pushTag("deck",deck_index);
        
        int total_clips = composition.getNumTags("clip");
        if(_write)
        {
            total_clips = files_to_copy.size();
        }
        
        for(int clip_index=0;clip_index<total_clips;clip_index++)
        {
            int current_clip_index = _write ? files_to_copy.at(clip_index).clip_index : clip_index;
            
            int clip_unique_id = composition.getAttribute("clip", "uniqueId", -1,current_clip_index);

            composition.pushTag("clip",current_clip_index);
        
            composition.pushTag("videoClip");
            
            if(composition.getAttribute("source", "type", "") == "file"){
            
                if(!_write){
                    //  first instance only scan the xml
                    string short_name = composition.getAttribute("source", "shortName", "");
                    ResolumeVideoclip   video_clip;
                    video_clip.unique_id = clip_unique_id;
                    video_clip.destination_path = destination_folder+"/sources/"+short_name;
                    video_clip.source_path = composition.getAttribute("source", "name", "");
                    video_clip.clip_index = clip_index;
                    
                    ofFile  temp_file = ofFile(video_clip.source_path);
                    if(temp_file.exists()){
                        files_to_copy.push_back(video_clip);
                    }
                }else{
                    //  write the xml
                    if(composition.setAttribute("source", "name", files_to_copy.at(clip_index).destination_path,files_to_copy.at(clip_index).clip_index))
                    {
                        //  done !
                        to_write++;
                    }
                    
                }
                
            }
            
            composition.popTag();   //  video clip
            composition.popTag();   //  clip
            
        }
        
        composition.popTag();   //  deck
    }
    
    composition.popTag();   //  decks
    composition.popTag();   //  composition
    
    if(!_write)
    {
        start_copy();
    }else{
        //  save the compo
        if(to_write == files_to_copy.size()-1){
            composition.save(destination_folder+"/"+compo_name+"_collect.avc");
            ofSystemAlertDialog("Done ! New composition: " + compo_name+"_collect.avc");
        }else{
            ofSystemAlertDialog("An error occurred during the editing of the composition file.");
        }

    }
    
}

void    ofApp::start_copy()
{
    current_copy_file = 0;
    //  starts the copy loop
    copy_file();
}

void    ofApp::copy_file()
{
    state = APP_COPY;
    file_copy.set(files_to_copy.at(current_copy_file).source_path, files_to_copy.at(current_copy_file).destination_path);
    file_copy.start();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    if(state == APP_COPY){
        
        float current_copy_index = current_copy_file;
        float copy_count = files_to_copy.size()-1;
        
        progress_width_current+=((current_copy_index/copy_count)-progress_width_current)*.1f;
        
        
        if(file_copy.done())
        {
            state = APP_COPY_DONE;
            //  copy done , next file
            current_copy_file++;
            if(current_copy_file < files_to_copy.size())
            {
                copy_file();
            }else{
                //  done
                state = APP_COPY_DONE;
            }
        }
    }else if(state == APP_COPY_DONE){
        process_composition(true);
        progress_width_current = 1.0f;
        state = APP_IDLE;
        reset();
    }

    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(50);
    
    
    for(int i=0;i<areas.size();i++)
    {
      
        string current_area_label = areas.at(i).label;
        bool    has_content = false;
        if(areas.at(i).id == COMPO){
            if(compo_name != ""){
                current_area_label = compo_name;
                if(current_area_label.size() > 12)
                    current_area_label = compo_name.substr(0,9) + "...";
                has_content = true;
            }
        }else if(areas.at(i).id == FOLDER)
        {
            if(destination_folder_name != ""){
                current_area_label = destination_folder_name;
                if(current_area_label.size() > 12)
                    current_area_label = destination_folder_name.substr(0,9) + "...";
                has_content = true;
            }
        }
        ofPushStyle();
        ofNoFill();
        if(has_content){
            ofSetColor(200,0,100);
        }else{
            ofSetColor(100);
        }
        ofDrawRectangle(areas.at(i).area);
        ofPopStyle();
        
        int label_width = 8*current_area_label.size();
        ofSetColor(255);
        ofDrawBitmapString(current_area_label, areas.at(i).area.getCenter().x-label_width*.5, areas.at(i).area.getCenter().y+4);
        
    }
    for(int i=0;i<buttons.size();i++)
    {
        bool    is_pressed = false;
        if(!buttons.at(i).enabled){
            ofSetColor(100);
        }else{
            if(current_button != NULL && current_button->label == buttons.at(i).label){
                ofSetColor(180,0,100);
            }else{
                ofSetColor(200,0,100);
            }
        }
        ofDrawRectangle(buttons.at(i).area);
        ofSetColor(255);
        int label_width = 8*buttons.at(i).label.size();
        ofDrawBitmapString(buttons.at(i).label, buttons.at(i).area.getCenter().x-label_width*.5, buttons.at(i).area.getCenter().y+4);
    }
    
    ofPushStyle();
    
    ofFill();
    ofSetColor(200,0,100);
    ofRectangle bar(progress_bar);
    bar.width=progress_width_current*progress_bar.getWidth();
    ofDrawRectangle(bar);
    ofNoFill();
    ofSetColor(255);
    ofPushMatrix();
    ofDrawRectangle(progress_bar);
    ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    int i = 0;
    while(i < buttons.size() && current_button == NULL)
    {
        if(buttons.at(i).enabled && buttons.at(i).area.inside(x, y))
        {
            current_button = &buttons.at(i);
        }
        i++;
    }
    if(current_button != NULL)
    {
        current_button->pressed = true;
        if(current_button->id == GO)
        {
            progress_width_current = 0.0f;
            current_button->enabled = false;
            load_file(current_file.getAbsolutePath());
        }
    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    current_button = NULL;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    ofSetWindowShape(400, 180);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
    if(dragInfo.files.size() == 1)
    {
        int i = 0;
        Button  *area = NULL;
        while(i < areas.size() && area == NULL)
        {
            if(areas.at(i).area.inside(dragInfo.position.x, dragInfo.position.y))
            {
                area = &areas.at(i);
            }
            i++;
        }
        
        if(area != NULL)
        {
            
            ofFile  file = ofFile(dragInfo.files.at(0));
            if(area->id == COMPO)
            {
                if(file.getExtension() == "avc")
                {
                    current_file = file;
                    compo_name = current_file.getBaseName();
                }
            }else if(area->id == FOLDER)
            {
                if(file.isDirectory())
                {
                    destination_folder = file.getAbsolutePath();
                    destination_folder_name = file.getBaseName();
                }
            }
        }
        
        if(destination_folder_name != "" && compo_name != "")
        {
            buttons.at(0).enabled = true;
        }
        
    }

}
