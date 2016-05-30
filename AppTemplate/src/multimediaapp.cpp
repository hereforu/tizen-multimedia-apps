/*
 * multimediaapp.cpp
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */
#include "base.h"
#include "apptemplate.h"
#include "multimediaapp.h"
#include "firstview.h"
#include "secondview.h"



MultimediaApp::MultimediaApp()
	:m_FrameWnd(NULL), m_Model(NULL)
{

}
MultimediaApp::~MultimediaApp()
{

}

void MultimediaApp::MoveNextView()
{
	baseassert(m_FrameWnd!= NULL);
	m_FrameWnd->MoveNextView();

}
void MultimediaApp::MovePrevView()
{
	baseassert(m_FrameWnd!= NULL);
	m_FrameWnd->MovePrevView();
}

void MultimediaApp::HandlerAppCreate()
{
	//create frame
	m_FrameWnd = new FrameWindow;
	m_FrameWnd->CreateBaseFrame();

	m_FrameWnd->AddView(ViewFactory<FirstView>::CreateInstance());
	m_FrameWnd->AddView(ViewFactory<SecondView>::CreateInstance());
	m_FrameWnd->ActivateFirstView();
	m_FrameWnd->Show();
	//create view
	//show frame
}

void MultimediaApp::HandlerAppTerminate()
{
	delete m_FrameWnd;
	m_FrameWnd = NULL;
	delete m_Model;
	m_Model = NULL;
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



