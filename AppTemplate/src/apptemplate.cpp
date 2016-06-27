#include "base.h"
#include "multimediaapp.h"



static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
	   Initialize UI resources and application's data
	   If this function returns true, the main loop of application starts
	   If this function returns false, the application is terminated */
	MultimediaApp *app = (MultimediaApp *)data;
	app->HandlerAppCreate();
	//create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	MultimediaApp *app = (MultimediaApp *)data;
	app->HandlerAppControl();
}

static void
app_pause(void *data)
{
	MultimediaApp *app = (MultimediaApp *)data;
	app->HandlerAppPause();
}

static void
app_resume(void *data)
{
	MultimediaApp *app = (MultimediaApp *)data;
	app->HandlerAppResume();
}

static void
app_terminate(void *data)
{
	MultimediaApp *app = (MultimediaApp *)data;
	app->HandlerAppTerminate();
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	MultimediaApp *app = (MultimediaApp *)user_data;
	app->HandleLanguageChangedEvent();
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	MultimediaApp *app = (MultimediaApp *)user_data;
	app->HandleDeviceOrientationChangedEvent();
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	MultimediaApp *app = (MultimediaApp *)user_data;
	app->HandleRegionFormatChangedEvent();
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	MultimediaApp *app = (MultimediaApp *)user_data;
	app->HandleLowBatteryEvent();
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	MultimediaApp *app = (MultimediaApp *)user_data;
	app->HandleLowMemoryEvent();
}

int
main(int argc, char *argv[])
{
	MultimediaApp* App = MultimediaApp::GetInstance();
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, App);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, App);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, App);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, App);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, App);

	ret = ui_app_main(argc, argv, &event_callback, App);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
	}
	return ret;
}
