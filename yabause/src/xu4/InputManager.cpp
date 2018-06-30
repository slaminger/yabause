#include "InputManager.h"
#include "InputConfig.h"
//#include "Settings.h"
//#include "Window.h"
//#include "Log.h"
#include "pugixml/pugixml.hpp"
#include <boost/filesystem.hpp>
//#include "platform.h"
#include <iostream>

using namespace std;
#include "MenuScreen.h"
extern "C"{
#include "yabause.h"
#include "peripheral.h"
}

#define LOG

#define SDL_MAX_AXIS_VALUE 0x110000
#define SDL_MIN_AXIS_VALUE 0x100000
#define SDL_HAT_VALUE 0x200000
#define SDL_MEDIUM_AXIS_VALUE (int)(32768 / 2)
#define SDL_BUTTON_PRESSED 1
#define SDL_BUTTON_RELEASED 0
//unsigned int SDL_PERCORE_INITIALIZED = 0;
//unsigned int SDL_PERCORE_JOYSTICKS_INITIALIZED = 0;
static unsigned int SDL_HAT_VALUES[] = { SDL_HAT_UP, SDL_HAT_RIGHT, SDL_HAT_LEFT, SDL_HAT_DOWN };
static const unsigned int SDL_HAT_VALUES_NUM = sizeof(SDL_HAT_VALUES) / sizeof(SDL_HAT_VALUES[0]);

#define KEYBOARD_GUID_STRING "-1"

// SO HEY POTENTIAL POOR SAP WHO IS TRYING TO MAKE SENSE OF ALL THIS (by which I mean my future self)
// There are like four distinct IDs used for joysticks (crazy, right?)
// 1. Device index - this is the "lowest level" identifier, and is just the Nth joystick plugged in to the system (like /dev/js#).
//    It can change even if the device is the same, and is only used to open joysticks (required to receive SDL events).
// 2. SDL_JoystickID - this is an ID for each joystick that is supposed to remain consistent between plugging and unplugging.
//    ES doesn't care if it does, though.
// 3. "Device ID" - this is something I made up and is what InputConfig's getDeviceID() returns.  
//    This is actually just an SDL_JoystickID (also called instance ID), but -1 means "keyboard" instead of "error."
// 4. Joystick GUID - this is some squashed version of joystick vendor, version, and a bunch of other device-specific things.
//    It should remain the same across runs of the program/system restarts/device reordering and is what I use to identify which joystick to load.

namespace fs = boost::filesystem;

InputManager* InputManager::mInstance = NULL;

InputManager::InputManager() : mKeyboardInputConfig(NULL)
{
}

InputManager::~InputManager()
{
	deinit();
}

InputManager* InputManager::getInstance()
{
	if(!mInstance)
		mInstance = new InputManager();

	return mInstance;
}

#define MAKE_PAD(a,b) ((a<<24)|(b))

uint32_t genid( int user, int joyId, const Input & result ){
	if( result.type == TYPE_AXIS ){
		return MAKE_PAD(user,((joyId << 18)|SDL_MAX_AXIS_VALUE|result.id));
	}else{
		return MAKE_PAD(user,((joyId << 18)|result.id));
	}
}

void InputManager::setGamePadomode( int user, int mode ){

	void * padbits;
	Input result;	
	int numJoysticks = SDL_NumJoysticks();
	
	if( numJoysticks != 0 ){
		
		int joyId = mInputConfigs[0]->getDeviceId();

		if( mode == 0 ){
			LOG("Switch to dig mode\n");
			PerPortReset();
			if( user == 0 ){
				padbits = PerPadAdd(&PORTDATA1);		
			}else{
				padbits = PerPadAdd(&PORTDATA2);		
			}
		}else if( mode == 1 ){
			LOG("Switch to Analog mode\n");
      PerPortReset();

			if( user == 0 ){
      	padbits = Per3DPadAdd(&PORTDATA1);
			}else{
				padbits = Per3DPadAdd(&PORTDATA2);
			}
      PerSetKey(MAKE_PAD(user,((joyId << 18)|SDL_MEDIUM_AXIS_VALUE|0)), PERANALOG_AXIS1, padbits);
      PerSetKey(MAKE_PAD(user,((joyId << 18)|SDL_MEDIUM_AXIS_VALUE|1)), PERANALOG_AXIS2, padbits);
      PerSetKey(MAKE_PAD(user,((joyId << 18)|SDL_MEDIUM_AXIS_VALUE|2)), PERANALOG_AXIS3, padbits);
      PerSetKey(MAKE_PAD(user,((joyId << 18)|SDL_MEDIUM_AXIS_VALUE|3)), PERANALOG_AXIS4, padbits);
		}

		mInputConfigs[0]->getInputByName("up", &result);
		PerSetKey(genid(user,joyId,result),PERPAD_UP, padbits);
		mInputConfigs[0]->getInputByName("down", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_DOWN, padbits);
		mInputConfigs[0]->getInputByName("left", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_LEFT, padbits);
		mInputConfigs[0]->getInputByName("right", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_RIGHT, padbits);
		mInputConfigs[0]->getInputByName("start", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_START, padbits);
		mInputConfigs[0]->getInputByName("select", &result);
		select_button_ = result.id;		
		mInputConfigs[0]->getInputByName("a", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_A, padbits);
		mInputConfigs[0]->getInputByName("b", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_B, padbits);
		mInputConfigs[0]->getInputByName("x", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_X, padbits);
		mInputConfigs[0]->getInputByName("y", &result);
		PerSetKey(genid(user,joyId,result) , PERPAD_Y, padbits);
		mInputConfigs[0]->getInputByName("rightshoulder", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_C, padbits);
		mInputConfigs[0]->getInputByName("leftshoulder", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_Z, padbits);

		mInputConfigs[0]->getInputByName("lefttrigger", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_LEFT_TRIGGER, padbits);

		mInputConfigs[0]->getInputByName("righttrigger", &result);
		PerSetKey(genid(user,joyId,result), PERPAD_RIGHT_TRIGGER, padbits);
		
			
		// Force Hat
		PerSetKey( MAKE_PAD(user, (joyId << 18) | SDL_HAT_VALUE | (SDL_HAT_UP<<4)), PERPAD_UP, padbits);
		PerSetKey( MAKE_PAD(user,(joyId << 18) | SDL_HAT_VALUE | (SDL_HAT_DOWN<<4)), PERPAD_DOWN, padbits);
		PerSetKey( MAKE_PAD(user,(joyId << 18) | SDL_HAT_VALUE | (SDL_HAT_LEFT<<4)), PERPAD_LEFT, padbits);
		PerSetKey( MAKE_PAD(user,(joyId << 18) | SDL_HAT_VALUE | (SDL_HAT_RIGHT<<4)), PERPAD_RIGHT, padbits);
	}
}

void InputManager::init()
{
	void * padbits;
	Input result;	

	if(initialized())
		deinit();

	//SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, 
	//Settings::getInstance()->getBool("BackgroundJoystickInput") ? "1" : "0");
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);

	// first, open all currently present joysticks
	int numJoysticks = SDL_NumJoysticks();
	for(int i = 0; i < numJoysticks; i++)
	{
		addJoystickByDeviceIndex(i);
	}

	if( numJoysticks != 0 && mInputConfigs.size() != 0 ){
		setGamePadomode( 0, 0 );
		return;
	}

	mKeyboardInputConfig = new InputConfig(DEVICE_KEYBOARD, "Keyboard", KEYBOARD_GUID_STRING);
	loadInputConfig(mKeyboardInputConfig);

	PerPortReset();
	padbits = PerPadAdd(&PORTDATA1);
	mKeyboardInputConfig->getInputByName("up", &result);
	PerSetKey(result.id, PERPAD_UP, padbits);
	mKeyboardInputConfig->getInputByName("down", &result);
	PerSetKey(result.id, PERPAD_DOWN, padbits);
	mKeyboardInputConfig->getInputByName("left", &result);
	PerSetKey(result.id, PERPAD_LEFT, padbits);
	mKeyboardInputConfig->getInputByName("right", &result);
	PerSetKey(result.id, PERPAD_RIGHT, padbits);

	mKeyboardInputConfig->getInputByName("start", &result);
	//printf("start id %d\n",result.id);
	PerSetKey(result.id, PERPAD_START, padbits);

	mKeyboardInputConfig->getInputByName("select", &result);
	select_button_ = result.id;

	mKeyboardInputConfig->getInputByName("a", &result);
	PerSetKey(result.id, PERPAD_A, padbits);

	mKeyboardInputConfig->getInputByName("b", &result);
	PerSetKey(result.id, PERPAD_B, padbits);

	mKeyboardInputConfig->getInputByName("x", &result);
	PerSetKey(result.id, PERPAD_X, padbits);

	mKeyboardInputConfig->getInputByName("y", &result);
	PerSetKey(result.id, PERPAD_Y, padbits);

	mKeyboardInputConfig->getInputByName("rightshoulder", &result);
	PerSetKey(result.id, PERPAD_C, padbits);

	mKeyboardInputConfig->getInputByName("leftshoulder", &result);
	PerSetKey(result.id, PERPAD_Z, padbits);

	mKeyboardInputConfig->getInputByName("lefttrigger", &result);
	PerSetKey(result.id, PERPAD_LEFT_TRIGGER, padbits);

	mKeyboardInputConfig->getInputByName("righttrigger", &result);
	PerSetKey(result.id, PERPAD_RIGHT_TRIGGER, padbits);

}

void InputManager::addJoystickByDeviceIndex(int id)
{
	assert(id >= 0 && id < SDL_NumJoysticks());
	
	// open joystick & add to our list
	SDL_Joystick* joy = SDL_JoystickOpen(id);
	assert(joy);

	// add it to our list so we can close it again later
	SDL_JoystickID joyId = SDL_JoystickInstanceID(joy);
	mJoysticks[joyId] = joy;

	char guid[65];
	SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, 65);

	// create the InputConfig
	mInputConfigs[joyId] = new InputConfig(joyId, SDL_JoystickName(joy), guid);
	if(!loadInputConfig(mInputConfigs[joyId]))
	{
		cout << "Added unconfigured joystick " << SDL_JoystickName(joy) << " (GUID: " << guid << ", instance ID: " << joyId << ", device index: " << id << ").";
	}else{
		cout << "Added known joystick " << SDL_JoystickName(joy) << " (instance ID: " << joyId << ", device index: " << id << ")";
	}

	// set up the prevAxisValues
	int numAxes = SDL_JoystickNumAxes(joy);
	mPrevAxisValues[joyId] = new int[numAxes];
	std::fill(mPrevAxisValues[joyId], mPrevAxisValues[joyId] + numAxes, 0); //initialize array to 0

	int numHats = SDL_JoystickNumHats(joy);
	mHatValues[joyId] = new u8[numHats];
	std::fill(mHatValues[joyId], mHatValues[joyId] + numHats, 0); //initialize array to 0

}

void InputManager::removeJoystickByJoystickID(SDL_JoystickID joyId)
{
	assert(joyId != -1);

	// delete old prevAxisValues
	auto axisIt = mPrevAxisValues.find(joyId);
	delete[] axisIt->second;
	mPrevAxisValues.erase(axisIt);

	// delete old InputConfig
	auto it = mInputConfigs.find(joyId);
	delete it->second;
	mInputConfigs.erase(it);

	// close the joystick
	auto joyIt = mJoysticks.find(joyId);
	if(joyIt != mJoysticks.end())
	{
		SDL_JoystickClose(joyIt->second);
		mJoysticks.erase(joyIt);
	}else{
//		LOG(LogError) << "Could not find joystick to close (instance ID: " << joyId << ")";
	}
}

void InputManager::deinit()
{
	if(!initialized())
		return;

	for(auto iter = mJoysticks.begin(); iter != mJoysticks.end(); iter++)
	{
		SDL_JoystickClose(iter->second);
	}
	mJoysticks.clear();

	for(auto iter = mInputConfigs.begin(); iter != mInputConfigs.end(); iter++)
	{
		delete iter->second;
	}
	mInputConfigs.clear();

	for(auto iter = mPrevAxisValues.begin(); iter != mPrevAxisValues.end(); iter++)
	{
		delete[] iter->second;
	}
	mPrevAxisValues.clear();

	if(mKeyboardInputConfig != NULL)
	{
		delete mKeyboardInputConfig;
		mKeyboardInputConfig = NULL;
	}

	SDL_JoystickEventState(SDL_DISABLE);
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}




int InputManager::getNumJoysticks() { return mJoysticks.size(); }
int InputManager::getButtonCountByDevice(SDL_JoystickID id)
{
	if(id == DEVICE_KEYBOARD)
		return 120; //it's a lot, okay.
	else
		return SDL_JoystickNumButtons(mJoysticks[id]);
}

InputConfig* InputManager::getInputConfigByDevice(int device)
{
	if(device == DEVICE_KEYBOARD)
		return mKeyboardInputConfig;
	else
		return mInputConfigs[device];
}

int InputManager::handleJoyEventsMenu(void) {

}


int InputManager::handleJoyEvents(void) {
	int joyId;
	int i;
	int j;
	SDL_Joystick* joy;
	Sint16 cur;
	Uint8 buttonState;
	Uint8 newHatState;
	Uint8 oldHatState;
	int hatValue;
	
	// update joysticks states
	SDL_JoystickUpdate();

	if(this->menu_layer_) {
		return handleJoyEventsMenu();
	}
	
	// check each joysticks
	for ( joyId = 0; joyId < SDL_NumJoysticks() ; joyId++ )
	{
		joy = mJoysticks[ joyId ];
		
		if ( !joy )
		{
			continue;
		}
		
		// check axis
		for ( i = 0; i < SDL_JoystickNumAxes( joy ); i++ )
		{
			cur = SDL_JoystickGetAxis( joy, i );

			
			PerAxisValue((joyId << 18) | SDL_MEDIUM_AXIS_VALUE | i, (u8)(((int)cur+32768) >> 8));
			
			if ( cur < -SDL_MEDIUM_AXIS_VALUE )
			{
				PerKeyUp( (joyId << 18) | SDL_MAX_AXIS_VALUE | i );
				PerKeyDown( (joyId << 18) | SDL_MIN_AXIS_VALUE | i );
				//printf("PerAxisValue %d, %d\n",i,((u8)(((int)cur+32768) >> 8)));
			}
			else if ( cur > SDL_MEDIUM_AXIS_VALUE )
			{
				PerKeyUp( (joyId << 18) | SDL_MIN_AXIS_VALUE | i );
				PerKeyDown( (joyId << 18) | SDL_MAX_AXIS_VALUE | i );
				//printf("PerAxisValue %d, %d\n",i,((u8)(((int)cur+32768) >> 8)));
			}
			else
			{
				PerKeyUp( (joyId << 18) | SDL_MIN_AXIS_VALUE | i );
				PerKeyUp( (joyId << 18) | SDL_MAX_AXIS_VALUE | i );
			}
		}
		
		// check buttons
		for ( i = 0; i < SDL_JoystickNumButtons( joy ); i++ )
		{
			buttonState = SDL_JoystickGetButton( joy, i );
			
			if ( buttonState == SDL_BUTTON_PRESSED )
			{
				PerKeyDown( (joyId << 18) | (i) );
				//printf("SDL_BUTTON_PRESSED %d",(i));
			}
			else if ( buttonState == SDL_BUTTON_RELEASED )
			{
				PerKeyUp( (joyId << 18) | (i) );
				//printf("SDL_BUTTON_RELEASED %d",(i +1));
			}
		}

		// check hats
		for ( i = 0; i < SDL_JoystickNumHats( joy ); i++ )
		{
			newHatState = SDL_JoystickGetHat( joy, i );
			oldHatState = mHatValues[ joyId ][ i ];

			for ( j = 0 ; j < SDL_HAT_VALUES_NUM; j++ )
			{
				hatValue = SDL_HAT_VALUES[ j ];
				if ( oldHatState & hatValue && ~newHatState & hatValue )
				{
					PerKeyUp( (joyId << 18) | SDL_HAT_VALUE | (hatValue << 4) | i );
				}
			}
			for ( j = 0 ; j < SDL_HAT_VALUES_NUM; j++ )
			{
				hatValue = SDL_HAT_VALUES[ j ];
				if ( ~oldHatState & hatValue && newHatState & hatValue )
				{
					PerKeyDown( (joyId << 18) | SDL_HAT_VALUE | (hatValue << 4) | i);
				}
			}

			mHatValues[ joyId ][ i ] = newHatState;
		}
	}

	// return success
	return 0;
}

static const int DEADZONE = 23000;

bool InputManager::parseEventMenu(const SDL_Event& ev ){
	std::vector<std::string> evstr;
	switch(ev.type)
	{
	case SDL_JOYAXISMOTION:{
  	//if it switched boundaries
		bool causedEvent = false;
    if((abs(ev.jaxis.value) > DEADZONE) != (abs(mPrevAxisValues[ev.jaxis.which][ev.jaxis.axis]) > DEADZONE)) {
    	int normValue;
      if(abs(ev.jaxis.value) <= DEADZONE)
      	normValue = 0;
      else
        if(ev.jaxis.value > 0)
      	  normValue = 1;
        else
          normValue = -1;
      //window->input(getInputConfigByDevice(ev.jaxis.which), Input(ev.jaxis.which, TYPE_AXIS, ev.jaxis.axis, normValue, false));
			InputConfig* cfg = getInputConfigByDevice(ev.jbutton.which);
			evstr = cfg->getMappedTo(Input(ev.jbutton.which, TYPE_BUTTON, ev.jbutton.button, normValue, false));
			if( evstr.size() > 0 ){
				menu_layer_->keyboardEvent(evstr[0],0,normValue,0);
			}
      causedEvent = true;
	  }
    mPrevAxisValues[ev.jaxis.which][ev.jaxis.axis] = ev.jaxis.value;
    return causedEvent;		
	}
  case SDL_JOYBUTTONDOWN:
  case SDL_JOYBUTTONUP:{
			InputConfig* cfg = getInputConfigByDevice(ev.jbutton.which);
			evstr = cfg->getMappedTo(Input(ev.jbutton.which, TYPE_BUTTON, ev.jbutton.button, ev.jbutton.state == SDL_PRESSED, false));
			if( evstr.size() ) {
				menu_layer_->keyboardEvent(evstr[0],0,ev.jbutton.state == SDL_PRESSED,0);
			}
		if( SDL_JOYBUTTONUP == ev.type && (ev.jbutton.button == select_button_ ||evstr[0]=="b")  ){
			SDL_Event event = {};
			event.type = showmenu_;
			event.user.code = 0;
			event.user.data1 = 0;
			event.user.data2 = 0;
			SDL_PushEvent(&event);
		}
      return true;
	}
  case SDL_JOYHATMOTION:{
			InputConfig* cfg = getInputConfigByDevice(ev.jhat.which);
	    evstr = cfg->getMappedTo( Input(ev.jhat.which, TYPE_HAT, ev.jhat.hat, ev.jhat.value, false));
			if( evstr.size() != 0 ){
				menu_layer_->keyboardEvent(evstr[0],0, (ev.jhat.value != 0) ,0);
			}
      return true;
	}
	case SDL_KEYDOWN:
		if(ev.key.repeat)
			return false;	

		if( mKeyboardInputConfig )	{
			evstr = mKeyboardInputConfig->getMappedTo(Input(DEVICE_KEYBOARD, TYPE_KEY, ev.key.keysym.sym, 1, false));
			if( evstr.size() != 0 ){
				menu_layer_->keyboardEvent(evstr[0],0,1,0);
			}
			return true;
		}else{
			return false;
		}
		break;
  case SDL_KEYUP:
		if( mKeyboardInputConfig )	{
			evstr = mKeyboardInputConfig->getMappedTo(Input(DEVICE_KEYBOARD, TYPE_KEY, ev.key.keysym.sym, 0, false));
			if( evstr.size() != 0 ){
				menu_layer_->keyboardEvent(evstr[0],0,0,0);
			}
			return true;
		}else{
			return false;
		}
		break;
	}
	return false;
}

bool InputManager::parseEvent(const SDL_Event& ev)
{
	if( this->menu_layer_) {
		return parseEventMenu(ev);
	}
	bool causedEvent = false;
	switch(ev.type)
	{
  case SDL_JOYBUTTONUP:{
		if( ev.jbutton.button == select_button_ ){
			SDL_Event event = {};
			event.type = showmenu_;
			event.user.code = 0;
			event.user.data1 = 0;
			event.user.data2 = 0;
			SDL_PushEvent(&event);
		}
    return true;
	}		
	case SDL_KEYDOWN:
		if(ev.key.repeat)
			return false;
		if(ev.key.keysym.sym == SDLK_ESCAPE)
		{
			SDL_Event event = {};
			event.type = showmenu_;
			event.user.code = 0;
			event.user.data1 = 0;
			event.user.data2 = 0;
			SDL_PushEvent(&event);
			return false;
		}
		PerKeyDown( ev.key.keysym.sym );
		return true;
	case SDL_KEYUP:
	 	PerKeyUp( ev.key.keysym.sym );
		return true;
	case SDL_JOYDEVICEADDED:
		addJoystickByDeviceIndex(ev.jdevice.which); // ev.jdevice.which is a device index
		return true;
	case SDL_JOYDEVICEREMOVED:
		removeJoystickByJoystickID(ev.jdevice.which); // ev.jdevice.which is an SDL_JoystickID (instance ID)
		return false;
	}
	return false;
}

bool InputManager::loadInputConfig(InputConfig* config)
{
	std::string path = getConfigPath();
	if(!fs::exists(path)){

		LOG("loadInputConfig %s is not found!\n", path.c_str() );

		return false;
	}
		

		
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());

	if(!res)
	{
//		LOG(LogError) << "Error parsing input config: " << res.description();
		return false;
	}

	pugi::xml_node root = doc.child("inputList");
	if(!root)
		return false;

	pugi::xml_node configNode = root.find_child_by_attribute("inputConfig", "deviceGUID", config->getDeviceGUIDString().c_str());
	if(!configNode)
		configNode = root.find_child_by_attribute("inputConfig", "deviceName", config->getDeviceName().c_str());
	if(!configNode)
		return false;

	config->loadFromXML(configNode);
	return true;
}

//used in an "emergency" where no keyboard config could be loaded from the inputmanager config file
//allows the user to select to reconfigure in menus if this happens without having to delete es_input.cfg manually
void InputManager::loadDefaultKBConfig()
{
	InputConfig* cfg = getInputConfigByDevice(DEVICE_KEYBOARD);

	cfg->clear();
	cfg->mapInput("up", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_UP, 1, true));
	cfg->mapInput("down", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_DOWN, 1, true));
	cfg->mapInput("left", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_LEFT, 1, true));
	cfg->mapInput("right", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_RIGHT, 1, true));

	cfg->mapInput("a", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_RETURN, 1, true));
	cfg->mapInput("b", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_ESCAPE, 1, true));
	cfg->mapInput("start", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_F1, 1, true));
	cfg->mapInput("select", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_F2, 1, true));

	cfg->mapInput("pageup", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_RIGHTBRACKET, 1, true));
	cfg->mapInput("pagedown", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_LEFTBRACKET, 1, true));
}

void InputManager::writeDeviceConfig(InputConfig* config)
{
	assert(initialized());

	std::string path = getConfigPath();

	pugi::xml_document doc;

	if(fs::exists(path))
	{
		// merge files
		pugi::xml_parse_result result = doc.load_file(path.c_str());
		if(!result)
		{
//			LOG(LogError) << "Error parsing input config: " << result.description();
		}else{
			// successfully loaded, delete the old entry if it exists
			pugi::xml_node root = doc.child("inputList");
			if(root)
			{
				pugi::xml_node oldEntry = root.find_child_by_attribute("inputConfig", "deviceGUID", config->getDeviceGUIDString().c_str());
				if(oldEntry)
					root.remove_child(oldEntry);
				oldEntry = root.find_child_by_attribute("inputConfig", "deviceName", config->getDeviceName().c_str());
				if(oldEntry)
					root.remove_child(oldEntry);
			}
		}
	}

	pugi::xml_node root = doc.child("inputList");
	if(!root)
		root = doc.append_child("inputList");

	config->writeToXML(root);
	doc.save_file(path.c_str());
}

std::string InputManager::getConfigPath()
{
	std::string path = "/home/pigaming"; //getHomePath();
	path += "/.emulationstation/es_temporaryinput.cfg";
	return path;
}

bool InputManager::initialized() const
{
	return mKeyboardInputConfig != NULL;
}

int InputManager::getNumConfiguredDevices()
{
	int num = 0;
	for(auto it = mInputConfigs.begin(); it != mInputConfigs.end(); it++)
	{
		if(it->second->isConfigured())
			num++;
	}

	if(mKeyboardInputConfig->isConfigured())
		num++;

	return num;
}

std::string InputManager::getDeviceGUIDString(int deviceId)
{
	if(deviceId == DEVICE_KEYBOARD)
		return KEYBOARD_GUID_STRING;

	auto it = mJoysticks.find(deviceId);
	if(it == mJoysticks.end())
	{
//		LOG(LogError) << "getDeviceGUIDString - deviceId " << deviceId << " not found!";
		return "something went horribly wrong";
	}

	char guid[65];
	SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(it->second), guid, 65);
	return std::string(guid);
}
