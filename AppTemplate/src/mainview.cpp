/*
 * mainview.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include "apptemplate.h"
#include "MainView.h"
#include "multimediaapp.h"
#include <stdexcept>

#define CB_NAME_MACRO(i) MainView::clicked_source##i_cb

MainView::MainView()
	:m_room(NULL), m_pack(NULL)
{

	m_srcbtnindexmap[0].cbfunc = MainView::clicked_source0_cb;
	m_srcbtnindexmap[0].index = -1;
	m_srcbtnindexmap[1].cbfunc = MainView::clicked_source1_cb;
	m_srcbtnindexmap[1].index = -1;
	m_srcbtnindexmap[2].cbfunc = MainView::clicked_source2_cb;
	m_srcbtnindexmap[2].index = -1;
	m_srcbtnindexmap[3].cbfunc = MainView::clicked_source3_cb;
	m_srcbtnindexmap[3].index = -1;
	m_srcbtnindexmap[4].cbfunc = MainView::clicked_source4_cb;
	m_srcbtnindexmap[4].index = -1;
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
	handlestopbtn();
	ecore_timer_del(m_timer);

	m_pack->Destroy();
	delete m_pack;
	m_pack = NULL;

	m_room->Destroy();
	delete m_room;
	m_room = NULL;
}

void MainView::UpdateView()
{
	std::vector<unsigned int> selectedsourceindex;
	std::vector<RoomSourceProperty> sources;
	getModel()->GetSelectedSourceIdx(selectedsourceindex);
	for(int i = 0; i < selectedsourceindex.size(); ++i)
	{
		MediaContentItem item = getModel()->GetMediaInfo(selectedsourceindex[i]);
		sources.push_back(RoomSourceProperty(selectedsourceindex[i], item.name.c_str()));
	}
	configure_room(sources);
	reset_allbtns();
	add_sourcebtns(sources);
	add_defaultbtns();
}

AudioManagerModel* MainView::getModel()
{
	AudioManagerModel* amm = static_cast<AudioManagerModel*>(MODEL);
	AppTool::Assert(amm != NULL);
	return amm;
}

void MainView::decorateview(Evas_Object* box)
{
	m_room = new AudioRoom();
	m_room->Create(box);

	m_pack = new ButtonPack();
	m_pack->Create(box);

	add_defaultbtns();

	m_timer = ecore_timer_add(0.5, MainView::timer_cb, (void*)this);
	ecore_timer_freeze(m_timer);
}

void MainView::configure_room(const std::vector<RoomSourceProperty>& sources)
{
	m_room->ConfigureSources(sources);
	m_room->ShowObjectsIfinRoom();
}
void MainView::reset_allbtns()
{
	m_pack->ResetAllButtons();
}

void MainView::add_sourcebtns(const std::vector<RoomSourceProperty>& sources)
{
	std::vector<BTPackParam> sourcebtn_params;
	for(unsigned int i = 0; i < sources.size(); ++i)
	{
		m_srcbtnindexmap[i].index = sources[i].sourceidx;
		const char* name = sources[i].name.c_str();
		const char* icon_path = m_room->GetSourceIconPath(sources[i].sourceidx);
		Evas_Smart_Cb cbfunc = m_srcbtnindexmap[i].cbfunc;
		sourcebtn_params.push_back(BTPackParam(name, icon_path, cbfunc, (void*)this));
	}
	m_pack->AddPacksHorizontally(sourcebtn_params);
}

void MainView::add_defaultbtns()
{
	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam(NULL, "images/play.png", MainView::clicked_play_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam(NULL, "images/stop.png", MainView::clicked_stop_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam(NULL, "images/plus.png", MainView::clicked_srcsel_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam(NULL, "images/exit.png", MainView::clicked_exit_cb, (void*)this));
	m_pack->AddPacksHorizontally(functionbtn_params);
}

Posf MainView::convertrelativepos(const EvasCoordRect& rect, const Pos& pos)
{
	Posf posf;
	const float max_x = 5.0f;
	const float max_y = 5.0f*(float)rect.h / (float)rect.w;

	posf.x = (float)(pos.x-rect.x);
	posf.y = (float)(pos.y-rect.y);

	posf.x = posf.x * max_x / (float)rect.w;
	posf.y = posf.y * max_y / (float)rect.h;
	posf.z = 0.0f;

	return posf;
}

void MainView::locateobjects()
{
	EvasCoordRect rect = m_room->GetRect();
	Pos pos = m_room->GetListenerPos();
	Posf posf = convertrelativepos(rect, pos);
	getModel()->LocateListener(posf.x, posf.y, posf.z);

	getModel()->ResetSource();
	std::vector<ObjectPos> sources_pos;
	m_room->GetPosofSourcesinRoom(sources_pos);
	for(int i = 0; i < sources_pos.size(); ++i)
	{
		posf = convertrelativepos(rect, sources_pos[i].pos);
		getModel()->LocateSource(sources_pos[i].objidx, posf.x , posf.y, posf.z);
		getModel()->PushSource(sources_pos[i].objidx);
	}
}

void MainView::handletimer()
{
	locateobjects();
}

void MainView::handlesourcebtn(int btnid)
{
	handlestopbtn();
	m_room->ToggleSource(m_srcbtnindexmap[btnid].index);
}

void MainView::handleplaybtn()
{
	locateobjects();
	getModel()->PlaySources();
	ecore_timer_thaw(m_timer);
}

void MainView::handlestopbtn()
{
	ecore_timer_freeze(m_timer);
	getModel()->StopSources();
}

void MainView::handlesrcselbtn()
{
	handlestopbtn();
	m_room->HideObjects();
	MOVE_NEXTVIEW;
}
Eina_Bool MainView::timer_cb(void* data)
{
	MainView* view = (MainView*)data;
	view->handletimer();
	return EINA_TRUE;
}

void MainView::clicked_source0_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlesourcebtn(0);
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
void MainView::clicked_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
	MainView* view = (MainView*)data;
	view->handlestopbtn();
}










