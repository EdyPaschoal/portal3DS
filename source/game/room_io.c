#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "game/room_io.h"
#include "game/camera.h"
#include "game/portal.h"
#include "utils/filesystem.h"

void readVect3Di(vect3Di_s* v, FILE* f)
{
	if(!v || !f)return;

	fread(v, sizeof(vect3Di_s), 1, f);
}

void readVect3Df(vect3Df_s* v, FILE* f, bool fp)
{
	if(!v || !f)return;

	if(fp)
	{
		s32 fpv[3];
		fread(fpv, sizeof(s32), 3, f);
		v->x=f32tofloat(fpv[0]);
		v->y=f32tofloat(fpv[1]);
		v->z=f32tofloat(fpv[2]);
	}else fread(v, sizeof(vect3Df_s), 1, f);
}

void readRectangle(rectangle_s* rec, FILE* f)
{
	if(!rec || !f)return;
	
	readVect3Di(&rec->position, f);
	readVect3Di(&rec->size, f);
	readVect3Df(&rec->normal, f, true);

	// printf("rec : %d %d %d\n", rec->position.x, rec->position.y, rec->position.z);

	fread(&rec->portalable, sizeof(bool), 1, f);
	
	u16 mid=0; fread(&mid,sizeof(u16),1,f);

	//TEMP ?
	if(rec->portalable)rec->material=getMaterial(1);
	else rec->material=getMaterial(2);
}

void readRectangles(room_s* r, FILE* f)
{
	if(!r || !f)return;
	int i;
	int k=r->rectangles.num;
	r->rectangles.num=0;
	for(i=0;i<k;i++)
	{
		rectangle_s rec;
		readRectangle(&rec, f);
		addRoomRectangle(r, rec);
	}
}

extern camera_s testCamera;
extern portal_s testPortal1, testPortal2;

void readEntity(u8 i, FILE* f)
{
	if(!f)return;
	u8 type=0, dir=0; vect3Di_s v;
	fread(&type, sizeof(u8), 1, f);
	readVect3Di(&v, f);
	fread(&dir, sizeof(u8), 1, f);
	// entityTargetArray[i]=-1;
	// entityActivatorArray[i]=NULL;
	// entityEntityArray[i]=NULL;
	switch(type)
	{
		case 0:
			//energy ball catcher
			{
				vect3Di_s p; readVect3Di(&p,f);
				s16 target=-1; fread(&target, sizeof(s16), 1, f);
				// energyDevice_struct* e=createEnergyDevice(NULL, p, dir, type);
				// if(e)entityActivatorArray[i]=&e->activator;
				// entityTargetArray[i]=target;
			}
			break;
		case 1:
			//energy ball launcher
			{
				vect3Di_s p; readVect3Di(&p,f);
				// createEnergyDevice(NULL, p, dir, type);
			}
			break;
		case 2:
			//timed button
			{
				vect3Di_s p; readVect3Di(&p,f);
				u8 d; fread(&d, sizeof(u8), 1, f);
				s16 target=-1; fread(&target, sizeof(s16), 1, f);
				// timedButton_struct* e=createTimedButton(NULL, p, (d+2)*8192);
				// if(e)entityActivatorArray[i]=&e->activator;
				// entityTargetArray[i]=target;
			}
			break;
		case 3:
			//pressure button
			{
				vect3Di_s p; readVect3Di(&p,f);
				s16 target=-1; fread(&target, sizeof(s16), 1, f);
				// bigButton_struct* e=createBigButton(NULL, p);
				// if(e)entityActivatorArray[i]=&e->activator;
				// entityTargetArray[i]=target;
			}
			break;
		case 4:
			//turret
			{
				vect3Di_s p; readVect3Di(&p,f);
				u8 d; fread(&d, sizeof(u8), 1, f);
				// createTurret(NULL, p, d);
			}
			break;
		case 5: case 6:
			//cubes
			{
				vect3Di_s p; readVect3Di(&p,f);
				s16 target=-1; fread(&target, sizeof(s16), 1, f);
			}
			break;
		case 7:
			//dispenser
			{
				vect3Di_s p; readVect3Di(&p,f);
				s16 target=-1; fread(&target, sizeof(s16), 1, f);
				// cubeDispenser_struct* e=createCubeDispenser(NULL, p, true);
				// entityEntityArray[i]=(void*)e;
				// entityTargetTypeArray[i]=DISPENSER_TARGET;
			}
			break;
		case 8:
			//emancipation grid
			{
				s32 l; fread(&l,sizeof(s32),1,f);
				vect3Di_s p; readVect3Di(&p,f);
				// createEmancipationGrid(NULL, p, (dir%2)?(-l):(l), !(dir<=1)); //TEMP ?
			}
			break;
		case 9:
			//platform
			{
				vect3Di_s p1, p2;
				readVect3Di(&p1,f);
				readVect3Di(&p2,f);
				s16 target=-1; fread(&target, sizeof(s16), 1, f);
				// platform_struct* e=createPlatform(NULL, p1, p2, true);
				// entityEntityArray[i]=(void*)e;
				// entityTargetTypeArray[i]=PLATFORM_TARGET;
			}
			return;
		case 10:
			//door
			{
				vect3Di_s p; readVect3Di(&p,f);
				u8 orientation; fread(&orientation, sizeof(u8), 1, f);
				// door_struct* e=createDoor(NULL, p, orientation%2);
				// entityEntityArray[i]=(void*)e;
				// entityTargetTypeArray[i]=DOOR_TARGET;
			}
			break;
		case 11:
			//light
			{
				vect3Di_s p; readVect3Di(&p,f);
				// createLight(p, TILESIZE*2*16);
			}
			break;
		case 12:
			//platform target
			{
				s16 target=-1;
				fread(&target, sizeof(s16), 1, f);
			}
			return;
		case 13:
			//wall door (start)
			{
				vect3Di_s p; readVect3Di(&p,f);
				u8 o; fread(&o,sizeof(u8),1,f);
				// printf("start : %d %d %d\n", p.x, p.y, p.z);
				setCameraPosition(&testCamera, vect3Df(p.x*TILESIZE_FLOAT*2, p.y*HEIGHTUNIT_FLOAT, p.z*TILESIZE_FLOAT*2));
				testPortal1.position=vect3Df(p.x*TILESIZE_FLOAT*2, p.y*HEIGHTUNIT_FLOAT, p.z*TILESIZE_FLOAT*2);
				testPortal2.position=vect3Df(p.x*TILESIZE_FLOAT*2, p.y*HEIGHTUNIT_FLOAT, p.z*TILESIZE_FLOAT*2);
				// setupWallDoor(NULL, &entryWallDoor, p, o);
				// if(entryWallDoor.used)
				// {
				// 	getPlayer()->object->position=addVect(entryWallDoor.elevator.realPosition,vect(0,PLAYERRADIUS*5,0));
				// 	switch(o)
				// 	{
				// 		case 0:
				// 			rotateCamera(NULL, vect(0,-8192,0));
				// 			break;
				// 		case 1:
				// 			rotateCamera(NULL, vect(0,8192,0));
				// 			break;
				// 		case 4:
				// 			rotateCamera(NULL, vect(0,8192*2,0));
				// 			break;
				// 	}
				// 	getPlayer()->object->speed=vect(0,0,0);
				// 	moveCamera(NULL, vect(0,0,PLAYERGROUNDSPEED*10));
				// 	getPlayer()->object->position=addVect(getPlayer()->object->position,getPlayer()->object->speed);
				// 	getPlayer()->object->speed=vect(0,0,0);
				// }
				// entityEntityArray[i]=(void*)&entryWallDoor;
				// entityTargetTypeArray[i]=WALLDOOR_TARGET;
			}
			return;
		case 14:
			//wall door (exit)
			{
				vect3Di_s p; readVect3Di(&p,f);
				u8 o; fread(&o,sizeof(u8),1,f);
				// setupWallDoor(NULL, &exitWallDoor, p, o);
				// entityEntityArray[i]=(void*)&exitWallDoor;
				// entityTargetTypeArray[i]=WALLDOOR_TARGET;
				// exitWallDoor.override=true;
			}
			return;
		default:
			break;
	}
}

void readEntities(FILE* f)
{
	if(!f)return;

	u16 cnt; fread(&cnt,sizeof(u16),1,f);
	int i; for(i=0;i<cnt;i++)readEntity(i,f);
	// for(i=0;i<cnt;i++)addEntityTarget(i,cnt,entityEntityArray[i],entityTargetTypeArray[i]);
}

void readHeader(mapHeader_s* h, FILE* f)
{
	if(!h || !f)return;
	fseek(f, 0, SEEK_SET);
	fread(h, MAPHEADER_SIZE, 1, f);
}

void readRoom(char* filename, room_s* r, u8 flags)
{
	if(!filename || !r)return;

	FILE* f=openFile(filename,"rb");
	if(!f)return;

	mapHeader_s h;
	readHeader(&h, f);

	//room data
	initRoom(r, 64*2, 64*2, vect3Df(-32*2,-32*2,0));

	fseek(f, h.rectanglesPosition, SEEK_SET);
	fread(&r->rectangles.num,sizeof(int),1,f);

	printf("RECTANGLE NUMBER %d", r->rectangles.num);
	
	readRectangles(r, f);

	// //lighting stuff
	// if(flags&MAP_READ_LIGHT)
	// {
	// 	fseek(f, h.lightPosition, SEEK_SET);
	// 	readLightingData(r, &r->lightingData, f);
	// }

	//entities
	if(flags&MAP_READ_ENTITIES)
	{
		fseek(f, h.entityPosition, SEEK_SET);
		readEntities(f);
	}

	// //sludge stuff
	// fseek(f, h.sludgePosition, SEEK_SET);
	// 	readSludgeRectangles(f);

	// //info
	// if(flags&MAP_READ_INFO)
	// {
	// 	int l=strlen(filename);
	// 	filename[l-1]='i';
	// 	filename[l-2]='n';
	// 	filename[l-3]='i';
	// 	readMapInfo(filename);
	// }

	generateRoomGeometry(r);
	
	fclose(f);
}