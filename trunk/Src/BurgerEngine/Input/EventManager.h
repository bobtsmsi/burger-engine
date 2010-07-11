/*************************************
*
*		BurgerEngine Project
*		
*		Created :	27/06/10
*		Authors :	Franck Letellier
*					Baptiste Malaga
*		Contact :   shadervalouf@googlegroups.com
*
**************************************/

#ifndef __EVENTMANAGER_H__
#define __EVENTMANAGER_H__

///	\name EventManager.h
///	\brief	Register and dispatch eventCallbacks
///			So far works for input but could work with other stuff
///			Here an exemple how to access control over keyboard
///			You create a function bool MyClass::Foo(unsigned char) in you class
///			Must return Fasle is want other callback to be not be called. (Pop Up menu)
///			Then you call the following
///			Engine::GetInstance().GetEventManager().RegisterCallbackKeyboardUpKey(CallBackKeyBoard<this,MyClass::Foo>);
///			To unregister
///			Engine::GetInstance().GetEventManager().UnRegisterCallbackKeyboardUpKey(CallBackKeyBoard<this,MyClass::Foo>);

#include <vector>
//#include "BurgerEngine/Base/Functor.h"
#include "BurgerEngine/External/Loki/Include/loki/Functor.h"


class EventManager
{
public:

	//Call back typedef
	typedef Loki::Functor<bool,LOKI_TYPELIST_1(unsigned char)> CallbackKeyboard;

public:

	/// \brief	Initialize method (set what input manager to use....)
	void Init();

	/// \brief	Clear all callback list
	void Clear();

	/// \brief	Has to be called each frame
	///			Will clear the event queue
	void ProcessEventList();

	/// \brief Register callback when a key is release
	void RegisterCallbackKeyboardUpKey(CallbackKeyboard a_oCallback);
	void UnRegisterCallbackKeyboardUpKey(CallbackKeyboard a_oCallback);

	/// \brief Register callback when key is pressed
	void RegisterCallbackKeyboardDownKey(CallbackKeyboard a_oCallback);
	void UnRegisterCallbackKeyboardDownKey(CallbackKeyboard a_oCallback);

	/// \brief Register callbakc when mouse is clicked
	void RegisterCallbackMouseDownClick(void* a_pObject,
		void (*a_pFunction)(int a_iButton, int a_iState, int a_iXCoordinates, int a_iYCoordinates));
	void UnRegisterCallbackMouseDownClick();
	
	/// \brief Register when mouse is moved without button pressed
	void RegisterCallbackMousePassiveMotion(void* a_pObject, void (*a_pFunction)(int a_iXCoordinates, int a_iYCoordinates));
	void UnRegisterCallbackMousePassiveMotion();

	/// \brief Register when mouse is moved with button pressed
	void RegisterCallbackMouseActiveMotion(void* a_pObject, void (*a_pFunction)(int a_iXCoordinates, int a_iYCoordinates));
	void UnRegisterCallbackMouseActiveMotion();

	///--------- Dispatch event ----------------
	///\brief	Send the key event to every register Callback
	void DispatchKeyboardUpKeyEvent(unsigned char a_cKey);

	///\brief	Send the key event to every register Callback
	void DispatchKeyboardDownKeyEvent(unsigned char a_cKey);

	///\brief	Send the click event to every register Callback
	void DispatchMouseDownClick(int a_iButton, int a_iState, int a_iXCoordinates, int a_iYCoordinates);

	///\brief	Send the mouse motion event to every register Callback
	void DispatchMousePassiveMotion(int a_iXCoordinates, int a_iYCoordinates);

	///\brief	Send the mouse motion to every register Callback
	void DispatchMouseActiveMotion(int a_iXCoordinates, int a_iYCoordinates);



private:

	//Vector containing the callbacks
	std::vector<CallbackKeyboard> m_vKeyboardUpKeyCallbacks;
	std::vector<CallbackKeyboard> m_vKeyboardDownKeyCallbacks;
	//std::vector<callback> m_vMouseDownClickCallbacks;
	//std::vector<callback> m_vMousePassiveMotionCallbacks;
	//std::vector<callback> m_vMouseActiveMotionCallbacks;
};

#endif //__EVENTMANAGER_H__
