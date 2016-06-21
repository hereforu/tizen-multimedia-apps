/*
 * srcSelectionView.cpp
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 */

#include "srcSelectionView.h"


int SrcSelectionView::m_selectedNum = 0;
int SrcSelectionView::m_maxSelection = 5;

SrcSelectionView::SrcSelectionView()
	:m_srcNum(0), m_list_selectedSrc(NULL), m_list_usrData(NULL)
{

}

SrcSelectionView::~SrcSelectionView()
{

}


/* view */
const char* SrcSelectionView::getedcfilename()
{
	// view title
	return "Select Audio Sources";
}

void SrcSelectionView::decorateview(Evas_Object* box)
{
	/* construction */
	m_list_srcName = getSrcNameList();
	m_srcNum = m_list_srcName.size();
	if(!m_srcNum)
		popAlarm("No audio files");
	m_list_usrData = new UsrData[m_srcNum];

	getSelectedSrc(); // get from model

	/* decoration */
	createList(box);
	createBackBtn(box);
}

void SrcSelectionView::destroyremains()
{
	/* send current selected list to model */
	std::vector<unsigned int> selected;
	for(int i=0; i<m_srcNum; i++)
		if(m_list_selectedSrc[i])
			selected.push_back(i+1);

	AudioManagerModel* amm = static_cast<AudioManagerModel*>(MODEL);
	if(amm == NULL)
	{
		throw std::runtime_error("fail to cast model");
	}
	amm->UpdateSource(selected);

	/* destruction */
	delete[] m_list_selectedSrc;
	m_list_selectedSrc = NULL;
	delete[] m_list_usrData;
	m_list_usrData = NULL;
}

void SrcSelectionView::updateview()
{

}


/* source */
void SrcSelectionView::getSelectedSrc()
{
	AudioManagerModel* amm = static_cast<AudioManagerModel*>(MODEL);
	if(amm == NULL)
	{
		throw std::runtime_error("fail to cast model");
	}
	std::vector<unsigned int> selected = amm->GetSelectedSourceIdx();

	m_list_selectedSrc = new Eina_Bool[m_srcNum];
	for(int i=0; i<m_srcNum; i++)
		m_list_selectedSrc[i] = 0;

	for(int i=0; i<selected.size(); i++)
		m_list_selectedSrc[selected[i]-1] = EINA_TRUE;
}

StrVec SrcSelectionView::getSrcNameList()
{
	AudioManagerModel* amm = static_cast<AudioManagerModel*>(MODEL);
	if(amm == NULL)
	{
		throw std::runtime_error("fail to cast model");
	}

	return amm->GetAudioList();
}


/* list */
void SrcSelectionView::src_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	UsrData *ud = (UsrData*)data;
	assert(ud != NULL);

	SrcSelectionView *ssv = (SrcSelectionView*) ud->ssv;
	Eina_Bool *check = ssv->m_list_selectedSrc + ud->idx;

	if(*check)
		m_selectedNum++; // off -> on
	else
		m_selectedNum--; // on -> off

	if(m_selectedNum > m_maxSelection)
	{
		ssv->popAlarm("You can select up to 5.");
		m_selectedNum--;
		elm_check_state_set(obj, EINA_FALSE);
	}
}

char* SrcSelectionView::genlist_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (strcmp(part, "elm.text") == 0)
	{
		UsrData *ud = (UsrData*)data;
		if(ud != NULL)
		{
			SrcSelectionView *ssv = (SrcSelectionView*) ud->ssv;

			return strdup((ssv->m_list_srcName[ud->idx]).c_str());
			/* strdup: After the text is set to the part,
			 * it is freed automatically (do not free it manually) */
		}
	}
	return NULL;
}

Evas_Object* SrcSelectionView::genlist_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *checkbox = NULL;
	UsrData *ud = (UsrData*)data;

	if (!strcmp(part, "elm.swallow.icon.1") && ud!=NULL)
	{
		SrcSelectionView *ssv = (SrcSelectionView*) ud->ssv;

		checkbox = elm_check_add(obj);
		if(checkbox == NULL)
		{
			throw std::runtime_error("fail to create checkbox");  // 이 throw를 받아주는 데는 어디?
		}
		elm_object_style_set(checkbox, "on&off"); //"default");

		if(ssv->m_list_selectedSrc[ud->idx]) // reload selected list
			elm_check_state_set(checkbox, EINA_TRUE);
		else elm_check_state_set(checkbox, EINA_FALSE);

		elm_check_state_pointer_set(checkbox, (ssv->m_list_selectedSrc)+ud->idx);
		evas_object_smart_callback_add(checkbox, "changed", src_selected_cb, data);

		evas_object_size_hint_align_set(checkbox, 0.7, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(checkbox, 1, 1);

		return checkbox;
	}
	/* TODO no icon!
	if (!strcmp(part, "elm.swallow.icon"))
	{
		Evas_Object *img = elm_image_add(obj);
		elm_image_file_set(img, "audio.png", NULL);
		evas_object_size_hint_aspect_set(img, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

		return img;
	}*/
	return NULL;
}

Elm_Genlist_Item_Class* SrcSelectionView::createItc() // manage list items
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	if(itc == NULL)
	{
		throw std::runtime_error("fail to create genlist item class");
	}
	itc->item_style = "type1";
	itc->func.text_get = genlist_text_get_cb;;
	itc->func.content_get = genlist_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	return itc;
}

void SrcSelectionView::createList(Evas_Object* box)
{
	Elm_Genlist_Item_Class *itc = createItc();

	Evas_Object * list = elm_genlist_add(box);
	if(list == NULL)
	{
		throw std::runtime_error("fail to create list");
	}
	evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_genlist_homogeneous_set(list, EINA_TRUE); // make all items the same height (no need to calculate)
	elm_genlist_select_mode_set(list, ELM_OBJECT_SELECT_MODE_NONE);
	elm_genlist_mode_set(list, ELM_LIST_SCROLL);

	for(int i=0; i<m_srcNum; i++)
	{
		m_list_usrData[i].idx = i;
		m_list_usrData[i].ssv = this;

		elm_genlist_item_append(list, itc, &(m_list_usrData[i]), NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}

	elm_box_pack_end(box, list);
	evas_object_show(list);
	elm_genlist_item_class_free(itc);
}


/* button */
void SrcSelectionView::backBtnclicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_PREVVIEW;
}

void SrcSelectionView::createBackBtn(Evas_Object* box)
{
	Evas_Object* btn = elm_button_add(box);
	if(btn == NULL)
	{
		throw std::runtime_error("fail to create button");
	}

	elm_object_text_set(btn, "Back to Audio Room");

	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", backBtnclicked_cb, NULL);
	elm_box_pack_end(box, btn);
	evas_object_show(btn);
}


/* alarm */
void SrcSelectionView::popupTimeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
}

void SrcSelectionView::popAlarm(const char* str)
{
	Evas_Object* popup = elm_popup_add(m_box);
	if(popup == NULL)
	{
		throw std::runtime_error("fail to create popup");  // 이 throw를 받아주는 데는 어디?
	}

	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	elm_object_style_set(popup, "toast");
	elm_popup_timeout_set(popup, 1.0); // 1 sec
	evas_object_smart_callback_add(popup, "timeout", popupTimeout_cb, NULL);

	elm_object_part_text_set(popup, "default", str);
	evas_object_show(popup);
}
