/*
 * multimediaapp.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "frame.h"
#include "model.h"
#include "templatesingleton.h"

#ifndef MULTIMEDIAAPP_H_
#define MULTIMEDIAAPP_H_

class MultimediaApp : public TemplateSingleton<MultimediaApp>
{
public:
	MultimediaApp();
	~MultimediaApp();

	//change view
	void MoveNextView();
	void MovePrevView();
	Model* GetModel();

	//event handler
	void HandlerAppCreate();
	void HandlerAppTerminate();
	void HandlerAppPause();
	void HandlerAppResume();
	void HandlerAppControl();
	void HandleLowBatteryEvent();
	void HandleLowMemoryEvent();
	void HandleDeviceOrientationChangedEvent();
	void HandleLanguageChangedEvent();
	void HandleRegionFormatChangedEvent();

private:
	FrameWindow* m_frame;
	Model* m_model;

};

#define MOVE_NEXTVIEW MultimediaApp::GetInstance()->MoveNextView()
#define MOVE_PREVVIEW MultimediaApp::GetInstance()->MovePrevView()
#define MODEL MultimediaApp::GetInstance()->GetModel()

#endif /* MULTIMEDIAAPP_H_ */
