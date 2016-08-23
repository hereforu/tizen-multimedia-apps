/*
 * multimediaapp.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */
#include "common/base.h"
#include "common/multimediaapp.h"
#include <stdexcept>



MultimediaApp::MultimediaApp()
	:m_frame(NULL), m_model(NULL)
{

}
MultimediaApp::~MultimediaApp()
{

}

void MultimediaApp::MoveNextView()
{
	assert_ifnot(m_frame!= NULL);
	m_frame->MoveNextView();

}

void MultimediaApp::MovePrevView()
{
	assert_ifnot(m_frame!= NULL);
	m_frame->MovePrevView();
}

Model* MultimediaApp::GetModel()
{
	return m_model;
}

bool MultimediaApp::HandlerAppCreate(Model* model, const std::vector<View*>& views)
{
	try
	{
		m_model = model;
		m_model->Create();
		m_frame = new FrameWindow;
		m_frame->Create();
		for(unsigned int i = 0; i < views.size(); ++i)
		{
			m_frame->AddView(views[i]);
		}
		m_frame->ActivateFirstView();
		m_frame->Show();
	}
	catch(const std::runtime_error& e)
	{
		destroyframe();
		destroymodel();
		std::string msg = "fail to create App because ";
		msg += e.what();
		dlog_print(DLOG_FATAL, "FrameWindow", msg.c_str());
		return false;
	}
	return true;
}

void MultimediaApp::HandlerAppTerminate()
{
	destroyframe();
	destroymodel();
}
void MultimediaApp::HandlerAppPause()
{

}
void MultimediaApp::HandlerAppResume()
{

}
void MultimediaApp::HandlerAppControl()
{

}
void MultimediaApp::HandleLowBatteryEvent()
{

}
void MultimediaApp::HandleLowMemoryEvent()
{

}
void MultimediaApp::HandleDeviceOrientationChangedEvent()
{

}
void MultimediaApp::HandleLanguageChangedEvent()
{
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
}
void MultimediaApp::HandleRegionFormatChangedEvent()
{

}
void MultimediaApp::destroymodel()
{
	if(m_model)
	{
		m_model->Destroy();
		delete m_model;
		m_model = NULL;
	}
}
void MultimediaApp::destroyframe()
{
	if(m_frame)
	{
		m_frame->Destroy();
		delete m_frame;
		m_frame = NULL;
	}
}


