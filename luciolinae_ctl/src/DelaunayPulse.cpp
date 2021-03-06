/*
 *  DelaunayPulse.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "DelaunayPulse.h"
#include "DelaunayPulseOut.h"
#include "DelaunayPulseIn.h"
#include "Lights.h"

DelaunayPulses* DelaunayPulses::instance = NULL;

void DelaunayPulses::addPulseOut( int target, float brightness, float decay, float speed )
{
	if ( pulses.size()> 8 )
		return;
	pulses.push_back( new DelaunayPulseOut( lights ) );
	pulses.back()->start( target, brightness, decay, speed );
}

void DelaunayPulses::addPulseIn( int target, float max_brightness, float start_radius, float speed )
{
	if ( pulses.size()> 8 )
		return;
	pulses.push_back( new DelaunayPulseIn( lights ) );
	pulses.back()->start( target, start_radius, max_brightness, speed );
}

void DelaunayPulses::update( float elapsed )
{
	lights->pushBlendState();
	lights->enableBlending();
	lights->setBlendMode( Lights::BLEND_ADD );
	lights->setBlendAlpha( 1.0f );
	for ( int i=0; i<pulses.size(); i++ )
	{
		pulses[i]->update( elapsed );
		if ( pulses[i]->isFinished() )
		{
			delete pulses[i];
			pulses.erase( pulses.begin()+i );
			i--;
		}
	}
	lights->popBlendState();
}