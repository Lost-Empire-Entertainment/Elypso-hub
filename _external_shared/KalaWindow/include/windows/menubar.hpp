//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"

#include "graphics/window.hpp"

namespace KalaWindow::Windows
{
	using std::string;
	using std::function;

	using KalaWindow::Graphics::Window;

	enum class LabelType
	{
		LABEL_LEAF,  //Clickable with required function, can't have children
		LABEL_BRANCH //Not clickable, won't work if function is added, can have children
	};
	struct MenuBarEvent
	{
		string parentLabel{};        //Name of parent label, leave empty if root

		string label{};              //Name of this label
		u32 labelID{};               //ID assigned to leaves, used for interaction
		function<void()> function{}; //Function assigned to leaves

		uintptr_t hMenu{};           //Branch HMENU handle for fast lookup
	};

	//Windows-only native menu bar. All leaf and and branch interactions are handled by the message loop.
	//Attach a function in CreateLabel for leaves, leave empty for functions so that the message loop
	//calls your function so that the menu bar interactions call your chosen functions.
	class LIB_API MenuBar
	{
	public:
		//Create a new empty menu bar at the top of the window.
		//Only one menu bar can be added to a window
		static void CreateMenuBar(Window* window);
		static bool IsInitialized(Window* window);

		//If true, then menu bar is shown
		static void SetMenuBarState(
			bool state,
			Window* window);
		static bool IsEnabled(Window* window);

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//branch and leaf creation will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isMenuBarVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isMenuBarVerboseLoggingEnabled; }

		//Create a menu bar label. Leaves must have functions, branches can't.
		//Leave parentRef empty if you want this label to be root
		static void CreateLabel(
			Window* windowRef,
			LabelType type,
			const string& parentRef,
			const string& labelRef,
			const function<void()> func = nullptr);

		//Add a horizontal separator line to the menu label.
		//If itemLabel isn't empty and exists then the separator is placed after the item label,
		//otherwise it is placed at the end of the menu label
		static void AddSeparator(
			Window* windowRef,
			const string& parentRef,
			const string& labelRef = "");

		//Destroy the existing menu bar inside the window
		static void DestroyMenuBar(Window* window);
	private:
		static inline bool isEnabled{};
		static inline bool isMenuBarVerboseLoggingEnabled{};
	};
}