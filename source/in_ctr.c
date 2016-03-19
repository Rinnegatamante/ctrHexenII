/*
Copyright (C) 2015 Felipe Izzo
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// in_ctr.c -- for the Nintendo 3DS

#include "quakedef.h"

#include <3ds.h>

circlePosition cstick;
circlePosition circlepad;
touchPosition oldtouch, touch;
extern u8 isN3DS;
extern qboolean inverted;
extern qboolean gyroscope;

void IN_Init (void)
{
  if ( COM_CheckParm ("-nomouse") )
    return;
}

void IN_Shutdown (void)
{
}

void IN_Commands (void)
{
}

void IN_Move (usercmd_t *cmd)
{
  if(hidKeysHeld() & KEY_TOUCH){
    hidTouchRead(&touch);
    touch.px =  (touch.px + oldtouch.px) / 2;
    touch.py =  (touch.py + oldtouch.py) / 2;
    cl.viewangles[YAW] -= (touch.px - oldtouch.px) * sensitivity.value/2;
    if(in_mlook.state & 1)
      cl.viewangles[PITCH] += (touch.py - oldtouch.py) * sensitivity.value/2;
    oldtouch = touch;
  }

  hidCircleRead(&circlepad);
  //CirclePad deadzone to fix ghost movements
  if(abs(circlepad.dy) > 15){
    cmd->forwardmove += m_forward.value * circlepad.dy * 2;
  }
  if(abs(circlepad.dx) > 15){
    if(isN3DS)
      cmd->sidemove += m_side.value * circlepad.dx * 2;
    else
      cl.viewangles[YAW] -= m_side.value * circlepad.dx * sensitivity.value * 0.01;
  }

  if(isN3DS){

    hidCstickRead(&cstick);

    if(m_pitch.value < 0)
      cstick.dy = -cstick.dy;

    cstick.dx = abs(cstick.dx) < 10 ? 0 : cstick.dx * sensitivity.value * 0.01;
    cstick.dy = abs(cstick.dy) < 10 ? 0 : cstick.dy * sensitivity.value * 0.01;

    cl.viewangles[YAW] -= cstick.dx;
	if (inverted) cl.viewangles[PITCH] += cstick.dy;
	else cl.viewangles[PITCH] -= cstick.dy;
	
  }
  
	// Gyroscope support
	if (gyroscope){ 
		angularRate cpos;
		hidGyroRead(&cpos);
		if (cpos.y > 200 || cpos.y < -200){
			cl.viewangles[YAW] += cpos.y / 250;
		}
		if (cpos.x > 200 || cpos.x < -200){
			if (inverted) cl.viewangles[PITCH] -= cpos.x / 200;
			else cl.viewangles[PITCH] += cpos.x / 200;
		}
	}
  
  V_StopPitchDrift ();

}
