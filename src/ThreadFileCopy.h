//
//  ThreadFileCopy.h
//  ArenaCollect
//
//  Created by Lolo on 10/05/2018.
//

#ifndef ThreadFileCopy_h
#define ThreadFileCopy_h
#include "ofMain.h"

class ThreadFileCopy : public ofThread {
    
public:
    

    ThreadFileCopy(): count(0), shouldThrowTestException(false)
    {
        moment = -1;
    }
    
    void    set(string _source_path,string _dest_path)
    {
        source_path = _source_path;
        dest_path = _dest_path;
        source_file = ofFile(source_path);
        moment = -1;
        copy_done = false;
    }
    
    void start()
    {
        startThread();
    }
   
    void stop()
    {
        stopThread();
    }
    
    /// Our implementation of threadedFunction.
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            if(moment == -1)
            {
                source_file.copyTo(dest_path,false,true);
                moment = 0;
            }else if(moment == 0)
            {
                ofFile  dest_file = ofFile(dest_path);
                if(source_file.getSize() == dest_file.getSize())
                {
                    copy_done = true;
                    moment = 1;
                    stop();
                }
            }
        }
    }
    
    bool    done()
    {
        bool    bkp = copy_done;
        copy_done = false;
        return bkp;
    }
    
    void throwTestException()
    {
        shouldThrowTestException = 1;
    }
    
protected:
    Poco::AtomicCounter shouldThrowTestException;
    
    
    string  source_path;
    string  dest_path;
    ofFile    source_file;
    int moment;
    int count;
    bool    copy_done;
    
    
    
};

#endif /* ThreadFileCopy_h */
