/*
 * multimediaapp.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "frame.h"
#include "model.h"
#include "view.h"

#ifndef MULTIMEDIAAPP_H_
#define MULTIMEDIAAPP_H_

class MultimediaApp
{
public:
	MultimediaApp();
	~MultimediaApp();

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
	FrameWindow* m_FrameWnd;
	Model* m_Model;
	View* m_View;

};



#endif /* MULTIMEDIAAPP_H_ */
