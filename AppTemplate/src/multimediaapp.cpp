/*
 * multimediaapp.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */
#include "base.h"
#include "apptemplate.h"
#include "multimediaapp.h"
#include "mainview.h"
#include "srcSelectionView.h"
#include "samplemodel.h"
#include "audioManagerModel.h"
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
	AppTool::Assert(m_frame!= NULL);
	m_frame->MoveNextView();

}

void MultimediaApp::MovePrevView()
{
	AppTool::Assert(m_frame!= NULL);
	m_frame->MovePrevView();
}

Model* MultimediaApp::GetModel()
{
	return m_model;
}

void MultimediaApp::HandlerAppCreate()
{
	//todo: add exception handling
	try
	{
		m_model = (Model*)new AudioManagerModel();
		m_model->Create();
		//create frame
		m_frame = new FrameWindow;
		m_frame->CreateBaseFrame();
		m_frame->AddView(AppTool::ObjectFactory<MainView>::CreateInstance());
		m_frame->AddView(AppTool::ObjectFactory<SrcSelectionView>::CreateInstance());
		m_frame->ActivateFirstView();
		m_frame->Show();
	}
	catch(const std::runtime_error& e)
	{
		std::string msg = "fail to create App because ";
		msg += e.what();
		dlog_print(DLOG_FATAL, "FrameWindow", msg.c_str());
	}
}

void MultimediaApp::HandlerAppTerminate()
{
	delete m_frame;
	m_frame = NULL;
	delete m_model;
	m_model = NULL;
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



