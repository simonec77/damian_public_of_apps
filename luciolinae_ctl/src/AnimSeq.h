/*
 *  AnimSeq.h
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimSeq: public Animation
{
public:
	AnimSeq( Lights* _lights );
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw() {};
	
	
protected:
	
	// 0..1
	float anim_pct;
	
	// pct/second
	float speed;
	
	// current light
	int current_active;
};

