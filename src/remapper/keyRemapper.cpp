/*
 *  keyRemapper.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "keyRemapper.h"
#include "midiConfig.h"

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;
extern ofColor blue;

void remapper::setup()
{
	// set the vertical displacement
	yDis=50;

	// fill the keyboard with 4 octave of keys
	kb.setup(ofGetWidth(), 4);

	//setup the band, with the file containing the physical instruments.
	band.setup(parseFile(cfg().instFile.c_str()));
	band.configureSize();

	// load the font used to draw the text on screen.
	dinc.loadFont("fonts/Din.ttf");
	dinc.setSize(35);
	dinc.setMode(OF_FONT_TOP);

	// make sure that the held instrument isn't accidently clicked in the first place.
	held.setHeld(false);
}

void remapper::update()
{
	// adjust how many bins of instruments there are in the band side bar
	band.configureSize();
	band.update();
}

void remapper::draw(double _x, double _y)
{
  bool band1st=false;
  
  ofPoint controlPad(20,50);
  ofPoint pad(30,30);
  
  ofRectangle controlBox(_x,_y,ofGetWidth(),band.h+controlPad.y*2);
  ofRectangle kbBox;
  ofRectangle bandBox;
  ofRectangle keyBox(kb.x,controlBox.y+controlBox.height,kb.w,kb.h+controlPad.y/4);
  
  if(!band1st){
    kbBox=ofRectangle(controlBox.x,controlBox.y,controlBox.width-(band.w+controlPad.x),controlBox.height);
    bandBox=ofRectangle(controlBox.x+kbBox.width,controlBox.y,band.w+40,controlBox.height);
  }
  else{
    bandBox=ofRectangle(controlBox.x,controlBox.y,band.w+40,controlBox.height);
    kbBox=ofRectangle(controlBox.x+bandBox.width,controlBox.y,controlBox.width-(bandBox.width),controlBox.height);
  }
  
  //_-_-_-_-_ draw the background for the control bar
  ofSetColor(black);
  ofRect(controlBox);
  
  //_-_-_-_-_ draw shadows
  drawBorder(kbBox);
  
  ofSetColor(gray);
  ofRect(bandBox);
  
  ofSetColor(white*.15);
  drawHatching(bandBox.x, bandBox.y, bandBox.width, bandBox.height, 75, 75);
  
  drawBorder(bandBox);
  
   
  //_-_-_-_-_ draw the band box contents
  dinc.setSize(23);
  dinc.setMode(OF_FONT_TOP);
  ofSetColor(yellow);
  dinc.drawString("Drag acoustic instruments to keys on the keyboard", bandBox.x+40, bandBox.y+20);
  band.draw(bandBox.x+controlPad.x,bandBox.y+pad.y*1.5);
	band.drawInstruments();
  
  //_-_-_-_-_ draw the keyboard
  kb.draw(0, controlBox.y+controlBox.height,ofGetWidth(),ofGetHeight()-(controlBox.y+controlBox.height));
  
  //_-_-_-_-_ draw the keyboard controls
  kb.drawKeyboardControls(kbBox.x+pad.x, kbBox.y+controlPad.y, kbBox.width-pad.x*2,kbBox.height-controlPad.y*2);
  
  //_-_-_-_-_ draw the held block
  held.drawBackground();
  
}


bool remapper::clickDown(int _x, int _y)
{
	bool ret=false;
	if(kb.clickDown(_x, _y)) ret=true;
	for (unsigned int i=0; i<kb.getActiveNotes().size(); i++) {
		/*if(kb.getActiveNotes()[i].over(_x,_y)){
			held=kb.getActiveNotes()[i];
			kb.getActiveNotes()[i].setDefault(true);
			held.clickDown(_x, _y);
		}*/
	}
	if(band.clickDown(_x, _y)){
		held=band.lastInstrument();
		held.clickDown(_x, _y);
	}
	kb.getKey().buttons.clickDown(_x, _y);
	return ret;
}

bool remapper::clickUp(int _x, int _y)
{
	kb.clickUp();
	/*if(band.clickUp(kb.getActiveNotes())){
		kb.selectButton(1);
	}*/
  if(band.clickUp()){
    kb.selectButton(1);
  }
	for (unsigned int i=0; i<kb.getActiveNotes().size(); i++) {
		kb.getActiveNotes()[i].clickUp();
	}
	if(held.isHeld()){
		bool ret=false;
		for (unsigned int i=0; i<kb.size(); i++) {
			if(kb[i].isSharp()&&!ret&&kb[i].over(_x, _y)){
				kb.getNotes(i)[0]=held;
				kb.getNotes(i)[0].setDefault(false);
				//kb.getNotes(i)[0].resizeByFont(40);
				kb.selectButton(i,1);
				ret=true;
				//kb[i].select(false);
			}
		}
		for (unsigned int i=0; i<kb.size(); i++) {
			if(!kb[i].isSharp()&&!ret&&kb[i].over(_x, _y)){
				kb.getNotes(i)[0]=held;
				kb.getNotes(i)[0].setDefault(false);
				kb.selectButton(i,1);
				//kb[i].select(false);
				ret=true;
			}
		}
		held.clickUp();
		held.clear();
	}
	//if(held.clickUp(kb.getActiveNotes()))
	//	kb.selectButton(1);
	kb.getKey().buttons.clickUp();
	return false;
}

void remapper::drag(int _x, int _y)
{
	band.mouseMotion(_x, _y);
	held.mouseMotion(_x, _y);
  kb.mouseMotion(_x, _y);
	if(held.isHeld()){
		bool tmp=false;
		for (unsigned int i=0; i<kb.size(); i++) {
			if(kb[i].isSharp()&&kb[i].over(_x, _y)&&!tmp){
				tmp=true;
				kb.pressKey(i);
			}
			else kb[i].select(false);
		}
		for (unsigned int i=0; i<kb.size(); i++) {
			if(!kb[i].isSharp()&&kb[i].over(_x, _y)&&!tmp){
				tmp=true;
				kb.pressKey(i);
			}
			else if(!kb[i].over(_x, _y)) kb[i].select(false);
		}
	}
}

void remapper::resize(int _w, int _h)
{
	kb.setup(15*ofGetWidth()/16., 4);
	band.configureSize();
	band.update();
}