/*
 * mainview.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */


#include "apptemplate.h"
#include "MainView.h"
#include "multimediaapp.h"
#include <stdexcept>

MainView::MainView()
	:m_label(NULL), m_room(NULL), m_pack(NULL)
{

}

MainView::~MainView()
{

}

const char* MainView::getedcfilename()
{
	return "3D Audio Room";
}

void MainView::destroyremains()
{
	m_pack->DestroyAll();
	delete m_pack;
	m_pack = NULL;

	m_room->Destroy();
	delete m_room;
	m_room = NULL;
}

void MainView::UpdateView()
{
	m_room->ShowObjectsIfinRoom();
}

void MainView::decorateview(Evas_Object* box)
{
	m_room = new AudioRoom();
	m_room->Create(box);

	m_pack = new ButtonPack();

	//to be linked with the model
	std::vector<BTPackParam> sourcebtn_params;
	sourcebtn_params.push_back(BTPackParam("S1", "images/source_1.png", MainView::clicked_source1_cb, (void*)this));
	sourcebtn_params.push_back(BTPackParam("S2", "images/source_2.png", MainView::clicked_source2_cb, (void*)this));
	sourcebtn_params.push_back(BTPackParam("S3", "images/source_3.png", MainView::clicked_source3_cb, (void*)this));
	sourcebtn_params.push_back(BTPackParam("S4", "images/source_4.png", MainView::clicked_source4_cb, (void*)this));
	sourcebtn_params.push_back(BTPackParam("S5", "images/source_5.png", MainView::clicked_source5_cb, (void*)this));
	m_pack->AddPacksHorizontally(box, sourcebtn_params);

	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam("Play", "images/play.png", MainView::clicked_play_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam("Sources", "images/plus.png", MainView::clicked_srcsel_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam("Exit", "images/exit.png", MainView::clicked_exit_cb, (void*)this));
	m_pack->AddPacksHorizontally(box, functionbtn_params);
}

void MainView::handlesourcebtn(int btnid)
{
	m_room->ToggleSource((RoomObjectType)btnid);
}
void MainView::handleplaybtn()
{
	//TODO
}
void MainView::handlesrcselbtn()
{
	m_room->HideObjects();
	MOVE_NEXTVIEW;
}

void MainView::clicked_source1_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesourcebtn(1);
}
void MainView::clicked_source2_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesourcebtn(2);
}
void MainView::clicked_source3_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesourcebtn(3);
}
void MainView::clicked_source4_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesourcebtn(4);
}
void MainView::clicked_source5_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesourcebtn(5);
}

void MainView::clicked_srcsel_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesrcselbtn();
}
void MainView::clicked_exit_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}
void MainView::clicked_play_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handleplaybtn();
}










