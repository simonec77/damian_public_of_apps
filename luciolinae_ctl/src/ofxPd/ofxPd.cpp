/*
 *  ofxPd.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 12/08/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofxPd.h"
#include <assert.h>

extern "C"
{
#include "m_pd.h"
#include "m_imp.h"
#include "s_main.h"
#include "s_stuff.h"

extern void sched_audio_callbackfn(void);
};


typedef uint32_t UInt32;

void ofxPd::setup( string _lib_dir )
{
	lib_dir = ofToDataPath(_lib_dir);
}

void ofxPd::addOpenFile( string file_path )
{
	open_files.push_back( ofToDataPath( file_path ) );
}

void ofxPd::addSearchPath( string path )
{
	search_path.push_back( ofToDataPath( path ) );
}




void ofxPd::start()
{
	this->startThread();
}


void ofxPd::stop()
{
	if ( this->isThreadRunning() )
		sys_exit();
}


void ofxPd::threadedFunction()
{
	
	// concatenate string vectors to single strings
	string externs_cat, open_files_cat, search_path_cat;
	for ( int i=0; i<externs.size(); i++ )
	{
		if ( i > 0 )
			externs_cat += ":";
		externs_cat += externs[i];
	}
	for ( int i=0; i<open_files.size(); i++ )
	{
		if ( i > 0 )
			open_files_cat += ":";
		open_files_cat += open_files[i];
	}
	for ( int i=0; i<search_path.size(); i++ )
	{
		if ( i > 0 )
			search_path_cat += ":";
		search_path_cat += search_path[i];
	}
	
	int sound_rate = 44100;
	int block_size = 64;
	int n_out_channels = 2;
	int n_in_channels = 0;
	sys_main( lib_dir.c_str(), externs_cat.c_str(), open_files_cat.c_str(), search_path_cat.c_str(),
			 sound_rate, block_size, n_out_channels, n_in_channels );

}

void ofxPd::audioRequested( float* output, int bufferSize, int nChannels )
{
    //assert(inBusNumber == 0);
    //assert(ioData->mNumberBuffers == 1);  // doing one callback, right?
    //assert(sizeof(t_sample) == 4);  // assume 32-bit floats here
	
    //AudioCallbackParams *params = (AudioCallbackParams*)inRefCon;
    
    // sys_schedblocksize is How many frames we have per PD dsp_tick
    // inNumberFrames is how many CoreAudio wants
	int inNumberFrames = bufferSize;
    
    // Make sure we're dealing with evenly divisible numbers between
    // the number of frames CoreAudio needs vs the block size for a given PD dsp tick.
    //Otherwise this looping scheme we're doing below doesn't make much sense.
    assert(inNumberFrames % sys_schedblocksize == 0);
	
    // How many times to generate a DSP event in PD
    UInt32 times = inNumberFrames / sys_schedblocksize;

    for(UInt32 i = 0; i < times; i++) {
        
		// do one DSP block
        sys_lock();
        //(*params->callback)(inTimeStamp);
		sched_audio_callbackfn();
        sys_unlock();
		
        // This should cover sys_noutchannels channels. Turns non-interleaved into 
        // interleaved audio.
        for (int n = 0; n < sys_schedblocksize; n++) {
            for(int ch = 0; ch < sys_noutchannels; ch++) {
                t_sample fsample = CLAMP(sys_soundout[n+sys_schedblocksize*ch],-1,1);
                output[(n*sys_noutchannels+ch) + // offset in iteration
					    i*sys_schedblocksize*sys_noutchannels] // iteration starting address
					= fsample;
            }        
        }
        
        // After loading the samples, we need to clear them for the next iteration
        memset(sys_soundout, 0, sizeof(t_sample)*sys_noutchannels*sys_schedblocksize);        
    }
	
}

