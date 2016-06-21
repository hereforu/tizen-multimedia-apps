/*
 * audioRoomView.cpp
 *
 *  Created on: June 21, 2016
 *      Author: Hyunjin
 * Description: 3D Audio play room.
 */

#include "audioRoomView.h"


AudioRoomView::AudioRoomView()
	:m_selectedNum(0), m_itemNum(0), m_list_item(NULL), m_list_play(NULL), m_toolbar(NULL), m_room(NULL)
{

}

AudioRoomView::~AudioRoomView()
{

}


/* view */
const char* AudioRoomView::getedcfilename()
{
	// view title
	return "3D Audio Room";
}

void AudioRoomView::decorateview(Evas_Object* box)
{
	createItemList();

	elm_box_padding_set(box, BOX_PAD, BOX_PAD);

	createAudioRoom(box);
	createToolbar(box);
	//createSelectionFrame(box);
	createButtons(box);
}

void AudioRoomView::destroyremains()
{
	deleteItemList();
	delete[] m_list_play;
}

void AudioRoomView::updateview()
{
	updateToolbarItems(m_toolbar);
	//TODO updatePlayItems();
}


/* source: used to make toolbar item list */
std::vector<unsigned int> AudioRoomView::getSelectedSrc()
{
	std::vector<unsigned int> selected;
	// = m_model->getSelectedList(); // TODO
	selected.push_back(1);
	selected.push_back(3);
	selected.push_back(5);
	selected.push_back(7);
	selected.push_back(9);

	return selected;
}

StrVec AudioRoomView::getSrcNameList()
{
	// temp, TODO
	StrVec list_srcName;
	for(int i=0; i<10; i++)
		list_srcName.push_back("audio"); // get source name listfrom model

	return list_srcName;
}


/* toolbar item */
void AudioRoomView::createItemList()
{
	/* get sources */
	std::vector<unsigned int> selected = getSelectedSrc();
	m_selectedNum = selected.size();
	m_itemNum = m_selectedNum + 2; // sources + clear + delete

	StrVec list_srcName = getSrcNameList();

	/* make item list */
	m_list_item = new ToolbarItem[m_itemNum];
	// sources
	for(int i=1; i<=m_selectedNum; i++)
	{
		m_list_item[i].id = selected[i-1];
		m_list_selectedSrcName.push_back(list_srcName[selected[i-1]-1]);
		m_list_item[i].name = (m_list_selectedSrcName[i-1]).c_str();
		m_list_item[i].icon_img = "images/audio.png";
		m_list_item[i].item = NULL;
	}
	// clear btn
	m_list_item[0].id = -1;
	m_list_item[0].name = "Clear All";
	m_list_item[0].icon_img = "images/new.png";
	m_list_item[0].item = NULL;
	// delete btn
	m_list_item[m_itemNum-1].id = -1;
	m_list_item[m_itemNum-1].name = "Delete";
	m_list_item[m_itemNum-1].icon_img = "images/remove.png";
	m_list_item[m_itemNum-1].item = NULL;

}


/* toolbar */
void AudioRoomView::createToolbar(Evas_Object* box)
{
	Evas_Object *toolbar = elm_toolbar_add(box);
	m_toolbar = toolbar;
	if (!toolbar)
	{
		throw std::runtime_error("fail to create a toolbar");
	}

	evas_object_size_hint_weight_set(toolbar, EVAS_HINT_EXPAND, 0.1);
	evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	elm_toolbar_homogeneous_set(toolbar, EINA_TRUE);
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_icon_size_set(toolbar, TOOLBAR_ICON_SIZE);

	addToolbarItems(toolbar);

	elm_box_pack_end(box, toolbar);
	evas_object_show(toolbar);
}

void AudioRoomView::deleteItemList()
{
	for(int i=0; i<=m_itemNum; i++)
		elm_object_item_del(m_list_item[i].item);

	delete[] m_list_item;
	m_list_item = NULL;
}

void AudioRoomView::updateToolbarItems(Evas_Object *toolbar)
{
	deleteItemList();
	createItemList();
	addToolbarItems(toolbar);

	if(!m_selectedNum)
		elm_object_disabled_set(m_playBtn, EINA_TRUE);
	else
		elm_object_disabled_set(m_playBtn, EINA_FALSE);
}

void AudioRoomView::addToolbarItems(Evas_Object *toolbar)
{
	// clear
	appendToolbarItem(toolbar, 0, NULL, NULL);
			//TODO __append_toolbar_item(toolbar, &ad->icon_data[0], __toolbar_clear_clicked_cb, (void *)ad);

	// sources
	for (int i=1; i<=m_selectedNum; i++)
		appendToolbarItem(toolbar, i, NULL, NULL);
				//TODO __append_toolbar_item(toolbar, &ad->icon_data[i], __tollbar_item_clicked_cb, (void *)ad->icon_data[i].mode);

	// delete
	appendToolbarItem(toolbar, m_itemNum-1, NULL, NULL);
			//TODO __append_toolbar_item(toolbar, &ad->icon_data[IMAGE_POOL_SIZE - 1], __toolbar_selected_item_del_cb, (void *)ad);
}

void AudioRoomView::appendToolbarItem(Evas_Object *toolbar, int idx, Evas_Smart_Cb func, void *data)
{
	/* set icon */
	char path[PATH_MAX];
	if(idx >= m_itemNum)
	{
		throw std::runtime_error("invalid item idx");
	}
	getResource(m_list_item[idx].icon_img, path, (int)PATH_MAX);

	/* add item */
	Elm_Object_Item *it = elm_toolbar_item_append(toolbar, path, m_list_item[idx].name, func, data);
	if (!it)
	{
		throw std::runtime_error("fail to append an item to the toolbar");
	}

	/* set tooltip */
	elm_object_item_tooltip_text_set(it, m_list_item[idx].name);

	m_list_item[idx].item = it;
}


/* audio room */
void AudioRoomView::createAudioRoom(Evas_Object *box)
{
	Evas_Object* room = evas_object_rectangle_add(box);
	m_room = room;
	if (!room) {
		throw std::runtime_error("fail to create audio room");
	}

	evas_object_size_hint_weight_set(room, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(room, EVAS_HINT_FILL, EVAS_HINT_FILL);

	/*
	evas_object_event_callback_add(draw_area, EVAS_CALLBACK_MOUSE_DOWN, __mouse_down_cb, NULL);
	evas_object_event_callback_add(draw_area, EVAS_CALLBACK_MOUSE_UP, __mouse_up_cb, NULL);
	evas_object_event_callback_add(draw_area, EVAS_CALLBACK_MOUSE_MOVE, __mouse_move_cb, NULL);
*/

	elm_box_pack_end(box, room);
	evas_object_show(room);
}

void AudioRoomView::createSelectionFrame(Evas_Object* box)
{
	char path[PATH_MAX] = {0,};

	Evas_Object* selection_frame = evas_object_image_filled_add(box);
	if (!selection_frame) {
		throw std::runtime_error("fail to create a selection frame");
	}

	getResource("images/frame.png", path, (int)PATH_MAX);

	evas_object_image_file_set(selection_frame, path, NULL);
	evas_object_image_border_set(selection_frame, FRAME_BORDER, FRAME_BORDER, FRAME_BORDER, FRAME_BORDER);
	evas_object_layer_set(selection_frame, EVAS_LAYER_MAX - 1);
	evas_object_repeat_events_set(selection_frame, EINA_TRUE);
}

void AudioRoomView::createPlayList()
{
	m_list_play = new PlayItem[m_selectedNum+1]; // sources + listener
	// listener



	m_list_play[0].id = 0;
	//TODO m_list_play[0].x =


	AudioManagerModel* amm = (AudioManagerModel*) m_model;
	//amm->LocateSource();
	//amm->LocateListener();
}

void AudioRoomView::setDefaultPos()
{
	for(int i=1; i<m_selectedNum; i++);


}


/* button */
void AudioRoomView::createButtons(Evas_Object* box)
{
	Evas_Object* btnBox = elm_box_add(box);
	if(!btnBox)
	{
		throw std::runtime_error("fail to create button box");
	}

	elm_box_horizontal_set(btnBox, EINA_TRUE);
	elm_box_padding_set(btnBox, BOX_PAD, BOX_PAD);

	// play btn
	m_playBtn = createBtn(btnBox, playBtnclicked_cb, "Play");
	if(!m_selectedNum)
		elm_object_disabled_set(m_playBtn, EINA_TRUE);

	// select source btn
	createBtn(btnBox, selectSrcBtnclicked_cb, "Select Audio");

	// close btn
	createBtn(btnBox, closeBtnclicked_cb, "Close");

	evas_object_size_hint_align_set(btnBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(box, btnBox);
	evas_object_show(btnBox);
}

Evas_Object* AudioRoomView::createBtn(Evas_Object* box, Evas_Smart_Cb click_func, const char* label)
{
	Evas_Object* btn = elm_button_add(box);
	if(btn == NULL)
	{
		throw std::runtime_error("fail to create button");
	}

	elm_object_text_set(btn, label);

	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", click_func, this);
	elm_box_pack_end(box, btn);
	evas_object_show(btn);

	return btn;
}

void AudioRoomView::closeBtnclicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

void AudioRoomView::selectSrcBtnclicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_NEXTVIEW;
}

void AudioRoomView::playBtnclicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	//  TODO: request model to play
}


/* resource */
void AudioRoomView::getResource(const char *file, char *path_out, int path_max)
{
	char *res_path = app_get_resource_path();
	if (res_path)
	{
		snprintf(path_out, path_max, "%s%s", res_path, file);
		free(res_path);
	}
}



// TODO

//Eina_Bool __is_cursor_in_selection_frame(void)

//__set_selected_item(object_t *selected)
/*void __unselect_item(void){
s_info.selected = NULL;
evas_object_hide(s_info.selection_frame);
}*/

// __object_clicked_cb
//__move_object(void)

//__create_circle_part

//__update_freehand(void)

// void __toolbar_selected_item_del_cb(void *data, Evas_Object *obj, void *event_info)
//__win_rotation_cb(void *data, Evas_Object *obj, void *event_info)
//__clear_draw_area(void)
//__create_object(mode_type_t type)
//__add_object_part(object_t *object, Evas_Object *part)??
// update selection frame?


