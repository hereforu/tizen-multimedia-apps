/*
 * srcSelectionView.cpp
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 */

#include "common/base.h"
#include "srcSelectionView.h"



SrcSelectionView::SrcSelectionView()
	:m_srcNum(0), m_maxSelection(5), m_list_usrData(NULL)
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
	m_srcNum = getModel()->GetNumAllMediaItems();
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
	getModel()->UpdateSource(m_selectedidxvec);

	/* destruction */
	delete[] m_list_usrData;
	m_list_usrData = NULL;
}

void SrcSelectionView::UpdateView()
{

}


/* source */
void SrcSelectionView::getSelectedSrc()
{
	m_selectedidxvec.clear();
	getModel()->GetSelectedSourceIdx(m_selectedidxvec);
}

AudioManagerModel* SrcSelectionView::getModel()
{
	AudioManagerModel* amm = static_cast<AudioManagerModel*>(MODEL);
	AppTool::Assert(amm != NULL);
	return amm;
}


/* list */
bool SrcSelectionView::delete_if_alreadyselected(int idx)
{
	std::vector<unsigned int>::iterator iter;
	for(iter = m_selectedidxvec.begin(); iter != m_selectedidxvec.end(); ++iter)
	{
		if(*iter == idx)
		{
			m_selectedidxvec.erase(iter);
			return true;
		}
	}
	return false;
}

bool SrcSelectionView::isalreadyselected(int idx)
{
	std::vector<unsigned int>::iterator iter;
	for(iter = m_selectedidxvec.begin(); iter != m_selectedidxvec.end(); ++iter)
	{
		if(*iter == idx)
		{
			return true;
		}
	}
	return false;
}

void SrcSelectionView::select_source(int idx, Evas_Object *obj)
{
	if(delete_if_alreadyselected(idx)==false)
	{
		if(m_selectedidxvec.size() >= m_maxSelection)
		{
			elm_check_state_set(obj, EINA_FALSE);
			popAlarm("You can select up to 5.");
		}
		else
		{
			m_selectedidxvec.push_back(idx);
		}
	}
}

void SrcSelectionView::src_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	UsrData *ud = (UsrData*)data;
	assert(ud != NULL);

	SrcSelectionView *ssv = (SrcSelectionView*) ud->ssv;
	ssv->select_source(ud->idx, obj);
}

const char* SrcSelectionView::getsourcedisplayname(int idx)
{
	return getModel()->GetMediaInfo(idx).name.c_str();
}

char* SrcSelectionView::genlist_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (strcmp(part, "elm.text") == 0)
	{
		UsrData *ud = (UsrData*)data;
		if(ud != NULL)
		{
			SrcSelectionView *ssv = (SrcSelectionView*) ud->ssv;
			return strdup(ssv->getsourcedisplayname(ud->idx));
		}
	}
	return NULL;
}
Evas_Object* SrcSelectionView::get_genlist_content(int idx, Evas_Object *obj, void *data)
{
	Evas_Object* checkbox = elm_check_add(obj);
	elm_object_style_set(checkbox, "on&off"); //"default");

	Eina_Bool checked = (isalreadyselected(idx))?EINA_TRUE:EINA_FALSE;
	elm_check_state_set(checkbox, checked);

	evas_object_smart_callback_add(checkbox, "changed", src_selected_cb, data);

	evas_object_size_hint_align_set(checkbox, 0.7, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(checkbox, 1, 1);

	return checkbox;
}

Evas_Object* SrcSelectionView::genlist_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	UsrData *ud = (UsrData*)data;

	if (!strcmp(part, "elm.swallow.icon.1") && ud!=NULL)
	{
		SrcSelectionView *ssv = (SrcSelectionView*) ud->ssv;
		return ssv->get_genlist_content(ud->idx, obj, data);
	}
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
	elm_popup_timeout_set(popup, 2.0); // 1 sec
	evas_object_smart_callback_add(popup, "timeout", popupTimeout_cb, NULL);

	elm_object_part_text_set(popup, "default", str);
	evas_object_show(popup);
}
